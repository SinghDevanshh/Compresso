/*
Decompression is implemented by reversing the compression steps:
    > Entropy Decode: Decode the compressed data.
    > Dequantize: Multiply the quantized values by the quantization table.
    > Apply IDCT: Convert frequency domain back to spatial domain.
    > Merge Blocks: Combine 8x8 blocks to form the image.
    > Upsample: Expand the Cb and Cr channels.
    > Convert to RGB: Convert from YCbCr back to RGB.
*/

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdint>
#include <queue>
#include <map>
#include <string>
#include <fstream>

#define STB_IMAGE_IMPLEMENTATION
#include "lib/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "lib/stb_image_write.h"

// Define Node data structure for Huffman tree
struct Node {
    int value;
    Node* left;
    Node* right;

    Node(int v) : value(v), left(nullptr), right(nullptr) {}
};

// Helper clamp function
int clamp(int value, int minVal, int maxVal) {
    return (value < minVal) ? minVal : (value > maxVal ? maxVal : value);
}


// Function to load the Huffman tree from the file
Node* loadHuffmanTree(std::ifstream& file) {
    if (!file.good()) return nullptr; // Ensure the file stream is valid

    char isLeaf;
    if (!file.get(isLeaf)) { // Read leaf indicator and check for success
        return nullptr;
    }

    if (isLeaf == 1) { // Leaf node
        int value;
        if (!file.read(reinterpret_cast<char*>(&value), sizeof(value))) { // Read the value
            throw std::runtime_error("Failed to read leaf node value.");
        }
        return new Node(value);
    }

    // Internal node
    Node* node = new Node(-1); // -1 indicates internal node

    // Recursively load left and right children
    node->left = loadHuffmanTree(file);
    node->right = loadHuffmanTree(file);

    return node;
}



// Function to decode the Huffman encoded data
std::vector<int> decodeHuffmanData( std::ifstream& file, Node* root,size_t totalCoefficients) {
    std::vector<int> decodedData;
    Node* currentNode = root;

    uint64_t buffer = 0;
    int bitsRemaining = 0;

    while (decodedData.size() < totalCoefficients) {
        if (bitsRemaining == 0) {
            file.read(reinterpret_cast<char*>(&buffer), sizeof(buffer));
            bitsRemaining = 64;
        }

        int bit = (buffer >> (bitsRemaining - 1)) & 1;
        bitsRemaining--;

        currentNode = (bit == 0) ? currentNode->left : currentNode->right;

        if (!currentNode->left && !currentNode->right) { // Leaf node
            decodedData.push_back(currentNode->value);
            currentNode = root;
        }
    }
    return decodedData;
}


// Function to dequantize the coefficients
void dequantize(std::vector<int>& block, const std::vector<int>& quantTable) {
    for (size_t i = 0; i < block.size(); ++i) {
        block[i] = round(block[i] * static_cast<double>(quantTable[i]));
    }
}


// Function to apply IDCT to an 8x8 block
void applyIDCT(std::vector<int>& block) {
    std::vector<double> temp(64);
    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            double sum = 0;
            for (int u = 0; u < 8; ++u) {
                for (int v = 0; v < 8; ++v) {
                    double cu = (u == 0) ? 1 / sqrt(2.0) : 1.0;
                    double cv = (v == 0) ? 1 / sqrt(2.0) : 1.0;
                    sum += cu * cv * block[u * 8 + v] *
                           cos(((2 * x + 1) * u * M_PI) / 16.0) *
                           cos(((2 * y + 1) * v * M_PI) / 16.0);
                }
            }
            temp[y * 8 + x] = 0.25 * sum;
        }
    }

    for (int i = 0; i < 64; ++i) {
        block[i] = std::round(temp[i]);
    }
}

std::vector<uint8_t> reconstructImage(const std::vector<std::vector<int>>& yBlocks, 
                                      const std::vector<std::vector<int>>& cbBlocks, 
                                      const std::vector<std::vector<int>>& crBlocks, 
                                      int width, int height) {
    std::vector<uint8_t> reconstructed(width * height * 3); // RGB data

    int blockIndex = 0;
    for (int yBlock = 0; yBlock < height; yBlock += 8) {
        for (int xBlock = 0; xBlock < width; xBlock += 8) {
            if (blockIndex >= yBlocks.size() || blockIndex / 4 >= cbBlocks.size() || blockIndex / 4 >= crBlocks.size()) {
                std::cerr << "Block index out of bounds: " << blockIndex << std::endl;
                std::cerr << "yBlocks size: " << yBlocks.size() << ", cbBlocks size: " << cbBlocks.size()
                          << ", crBlocks size: " << crBlocks.size() << std::endl;

            }

            const auto& yBlockData = yBlocks[blockIndex];
            const auto& cbBlockData = cbBlocks[blockIndex / 4]; // 4:2:0 subsampling
            const auto& crBlockData = crBlocks[blockIndex / 4];

            for (int dy = 0; dy < 8; ++dy) {
                for (int dx = 0; dx < 8; ++dx) {
                    int x = xBlock + dx;
                    int y = yBlock + dy;

                    if (x < width && y < height) {
                        int pixelIndex = (y * width + x) * 3;

                        if ((dy / 2) * 4 + (dx / 2) >= cbBlockData.size() || (dy / 2) * 4 + (dx / 2) >= crBlockData.size()) {
                            std::cerr << "Subsampling index out of bounds for block index: " << blockIndex << std::endl;
                            continue; // Skip this pixel
                        }

                        int Y = yBlockData[dy * 8 + dx];
                        int Cb = cbBlockData[(dy / 2) * 4 + (dx / 2)]; // Subsampled
                        int Cr = crBlockData[(dy / 2) * 4 + (dx / 2)]; // Subsampled

                        // Convert YCbCr to RGB
                        // https://web.archive.org/web/20180421030430/http://www.equasys.de/colorconversion.html

                        int R = clamp(Y + 1.400 * (Cr - 128), 0.0, 255.0);
                        int G = clamp(Y - 0.343 * (Cb - 128) - 0.711 * (Cr - 128), 0.0, 255.0);
                        int B = clamp(Y + 1.765 * (Cb - 128), 0.0, 255.0);

                        reconstructed[pixelIndex] = static_cast<uint8_t>(R);
                        reconstructed[pixelIndex + 1] = static_cast<uint8_t>(G);
                        reconstructed[pixelIndex + 2] = static_cast<uint8_t>(B);
                    }
                }
            }
            
            ++blockIndex;
        }
    }

    return reconstructed;
}



void saveImage(const std::string& outputPath, const std::vector<uint8_t>& imageData, int width, int height) {
    if (!stbi_write_png(outputPath.c_str(), width, height, 3, imageData.data(), width * 3)) {
        throw std::runtime_error("Failed to save the image.");
    }
}


// Main decompression function
void decompressJPEG(const std::string& inputFile, const std::string& outputFile) {
    std::ifstream file(inputFile, std::ios::binary);
    if (!file) throw std::runtime_error("Failed to open input file.");


    // Input the image size in pixels
    int width = 200;
    int height = 200;

    // Step 1: Load Huffman tree
    Node* huffmanTree = loadHuffmanTree(file);
    if (!huffmanTree) throw std::runtime_error("Failed to load Huffman tree.");

    // Step 2: Decode Huffman data
    int expected_size = 59968;
    auto coefficients = decodeHuffmanData(file, huffmanTree, expected_size);

    // Step 3: Dequantize and apply IDCT
    std::vector<int> quantTable = { 16,11,12,14,12,10,16,14,
                13,14,18,17,16,19,24,40,
                26,24,22,22,24,49,35,37,
                29,40,58,51,61,60,57,51,
                56,55,64,72,92,78,64,68,
                87,69,55,56,80,109,81,87,
                95,98,103,104,103,62,77,113,
                121,112,100,120,92,101,103,99 };

    std::vector<std::vector<int>> yBlocks, cbBlocks, crBlocks;

    // Total number of blocks
    size_t totalBlocks = coefficients.size() / 64;

    // Calculate the number of blocks for each channel
    size_t yBlockCount = (width * height) / 64;
    size_t cbCrBlockCount = (totalBlocks-yBlockCount)/2;

    // Split coefficients into blocks and assign to Y, Cb, and Cr channels
    for (size_t i = 0; i < totalBlocks; ++i) {
        std::vector<int> block(coefficients.begin() + i * 64, coefficients.begin() + (i + 1) * 64);
        dequantize(block, quantTable);
        applyIDCT(block);

        if (i < yBlockCount) {
            yBlocks.push_back(block); // Assign to Y channel
        } else if (i < yBlockCount + cbCrBlockCount) {
            cbBlocks.push_back(block); // Assign to Cb channel
        } else {
            crBlocks.push_back(block); // Assign to Cr channel
        }
    }
    // Adding an additional block same as the last block to manage extreme end pixels
    yBlocks.push_back(yBlocks[yBlockCount-1]);
    cbBlocks.push_back(cbBlocks[cbCrBlockCount-1]);
    crBlocks.push_back(crBlocks[cbCrBlockCount-1]);

    // Step 4: Reconstruct the image
    auto reconstructedImage = reconstructImage(yBlocks, cbBlocks, crBlocks, width, height);

    // Step 5: Save the decompressed image
    saveImage(outputFile, reconstructedImage, width, height);

    file.close();
    std::cout << "Decompression completed. Output saved to " << outputFile << std::endl;
}

// Main function
int main() {
    std::string inputFile = "output.jc";
    std::string outputFile = "decompressed.jpeg";

    try {
        decompressJPEG(inputFile, outputFile);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    // verifyCompressedFile(inputFile);

    return 0;
}