/*
JPEG compression is a lossy compression technique. The main steps in JPEG compression include:
    > Color Space Conversion: Convert the image from RGB to YCbCr (luminance and chrominance channels).
    > Subsampling: Reduce the resolution of the chrominance channels (Cb and Cr) to save space.
    > Block Splitting: Divide the image into 8x8 pixel blocks.
    > Discrete Cosine Transform (DCT): Transform each block from spatial domain to frequency domain.
    > Quantization: Reduce the precision of less significant frequencies.
    > Entropy Encoding: Compress the quantized data using Huffman coding or similar techniques.
*/

#include "CompressJpeg.h"

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
    int value;  // Coefficient value
    int frequency;  // Frequency of the value
    Node* left;  // Left child
    Node* right; // Right child

    Node(int v, int f) : value(v), frequency(f), left(nullptr), right(nullptr) {}
};

// Comparator for the priority queue
struct Compare {
    bool operator()(Node* a, Node* b) {
        return a->frequency > b->frequency;
    }
};

// Load the Image
struct Image {
    int width, height, channels;
    std::vector<uint8_t> data;
};

Image loadImage(const std::string& path) {
    int width, height, channels;
    uint8_t* imgData = stbi_load(path.c_str(), &width, &height, &channels, 3); // Force RGB
    if (!imgData) {
        throw std::runtime_error("Failed to load image");
    }

    Image img = {width, height, 3, std::vector<uint8_t>(imgData, imgData + (width * height * 3))};
    stbi_image_free(imgData);
    return img;
}

/*
Color Space Conversion:

RGB to YCbCr conversion using the following formulas:
Y = 0.299R + 0.587G + 0.114B
Cb = 128 - 0.168736R - 0.331264G + 0.5B
Cr = 128 + 0.5R - 0.418688G - 0.081312B
*/
struct YCbCr {
    std::vector<uint8_t> Y, Cb, Cr;
};

YCbCr rgbToYCbCr(const Image& img) {
    YCbCr ycbcr = {std::vector<uint8_t>(img.width * img.height),
                   std::vector<uint8_t>(img.width * img.height),
                   std::vector<uint8_t>(img.width * img.height)};

    for (size_t i = 0; i < img.data.size(); i += 3) {
        uint8_t R = img.data[i];
        uint8_t G = img.data[i + 1];
        uint8_t B = img.data[i + 2];

        ycbcr.Y[i / 3] = static_cast<uint8_t>(0.299 * R + 0.587 * G + 0.114 * B);
        ycbcr.Cb[i / 3] = static_cast<uint8_t>(128 - 0.168736 * R - 0.331264 * G + 0.5 * B);
        ycbcr.Cr[i / 3] = static_cast<uint8_t>(128 + 0.5 * R - 0.418688 * G - 0.081312 * B);
    }

    return ycbcr;
}

/*
Subsampling:

Reduce the resolution of the chrominance channels (Cb and Cr) by 
taking every alternate pixel (4:2:0 subsampling)
*/

std::vector<uint8_t> subsampleChannel(const std::vector<uint8_t>& channel, int width, int height) {
    std::vector<uint8_t> subsampled((width / 2) * (height / 2));

    for (int y = 0; y < height; y += 2) {
        for (int x = 0; x < width; x += 2) {
            subsampled[(y / 2) * (width / 2) + (x / 2)] = channel[y * width + x];
        }
    }
    return subsampled;
}

/*
Block Splitting

Divide the image into 8x8 blocks for DCT processing
*/
std::vector<std::vector<int>> splitIntoBlocks(const std::vector<uint8_t>& channel, int width, int height) {
    std::vector<std::vector<int>> blocks;

    for (int y = 0; y < height; y += 8) {
        for (int x = 0; x < width; x += 8) {
            std::vector<int> block(64);
            for (int dy = 0; dy < 8; ++dy) {
                for (int dx = 0; dx < 8; ++dx) {
                    block[dy * 8 + dx] = (y + dy < height && x + dx < width) ? 
                        channel[(y + dy) * width + (x + dx)] : 0;
                }
            }
            blocks.push_back(block);
        }
    }

    return blocks;
}

/*
Discrete Cosine Transform (DCT):

Implementing the DCT using the standard formula.
*/
void applyDCT(std::vector<int>& block) {
    double c[8][8];
    double pi = acos(-1);

    for (int u = 0; u < 8; ++u) {
        for (int v = 0; v < 8; ++v) {
            c[u][v] = (u == 0 ? 1 / sqrt(2) : 1.0) * (v == 0 ? 1 / sqrt(2) : 1.0);
            c[u][v] *= cos((2 * u + 1) * pi / 16.0) * cos((2 * v + 1) * pi / 16.0);
        }
    }

    std::vector<int> temp(64);
    for (int u = 0; u < 8; ++u) {
        for (int v = 0; v < 8; ++v) {
            double sum = 0;
            for (int x = 0; x < 8; ++x) {
                for (int y = 0; y < 8; ++y) {
                    sum += block[x * 8 + y] * c[u][x] * c[v][y];
                }
            }
            temp[u * 8 + v] = static_cast<int>(round(sum));
        }
    }

    block = temp;
}

/*
Quantization:

Divide the transformed coefficients by a quantization table and round them.
*/

void quantize(std::vector<int>& block, const std::vector<int>& quantTable) {
    for (size_t i = 0; i < block.size(); ++i) {
        block[i] = round(block[i] / static_cast<double>(quantTable[i]));
    }
}



/*
Entropy Encoding:

Using Huffman encoding
*/

std::map<int, int> buildFrequencyTable(const std::vector<std::vector<int>>& blocks) {
    std::map<int, int> frequencyTable;

    // Initialize all possible values with frequency 0
    for (int i = -255; i <= 255; ++i) {
        frequencyTable[i] = 0;
    }

    for (const auto& block : blocks) {
        for (int coeff : block) {
            frequencyTable[coeff]++;
        }
    }

    return frequencyTable;
}


Node* buildHuffmanTree(const std::map<int, int>& frequencyTable) {
    std::priority_queue<Node*, std::vector<Node*>, Compare> pq;

    for (const auto& pair : frequencyTable) {
        pq.push(new Node(pair.first, pair.second));
    }

    while (pq.size() > 1) {
        Node* left = pq.top(); pq.pop();
        Node* right = pq.top(); pq.pop();

        Node* parent = new Node(-1, left->frequency + right->frequency); // -1 indicates internal node
        parent->left = left;
        parent->right = right;

        pq.push(parent);
    }

    return pq.top(); // Root of the Huffman tree
}

void generateCodes(Node* root, const std::string& code, std::map<int, std::string>& huffmanCodes) {
    if (!root) return;

    if (root->value != -1) { // Leaf node
        huffmanCodes[root->value] = code;
    }

    generateCodes(root->left, code + "0", huffmanCodes);
    generateCodes(root->right, code + "1", huffmanCodes);
}

std::map<int, std::string> getHuffmanCodes(Node* root) {
    std::map<int, std::string> huffmanCodes;
    generateCodes(root, "", huffmanCodes);
    return huffmanCodes;
}

std::string encodeBlocks(const std::vector<std::vector<int>>& blocks, const std::map<int, std::string>& huffmanCodes) {
    std::string encodedData;

    for (const auto& block : blocks) {
        for (int coeff : block) {
            if (huffmanCodes.find(coeff) == huffmanCodes.end()) {
                throw std::runtime_error("Missing Huffman code for coefficient: " + std::to_string(coeff));
            }
            encodedData += huffmanCodes.at(coeff);
        }
    }


    return encodedData;
}

void saveEncodedData(const std::string& encodedData, const std::map<int, std::string>& huffmanCodes, const std::string& outputFile) {
    std::ofstream file(outputFile, std::ios::binary);

    if (!file) {
        throw std::runtime_error("Failed to open output file.");
    }

    // Save the Huffman codes (metadata)
    file << huffmanCodes.size() << "\n";
    for (const auto& pair : huffmanCodes) {
        file << pair.first << " " << pair.second << "\n";
    }

    // Save the encoded data
    file << encodedData;

    file.close();
}


void compressJPEG(const std::vector<std::vector<int>>& quantizedBlocks, const std::string& outputFile) {
    // Step 1: Build frequency table
    auto frequencyTable = buildFrequencyTable(quantizedBlocks);

    // Step 2: Build Huffman tree
    Node* huffmanTree = buildHuffmanTree(frequencyTable);

    // Step 3: Generate Huffman codes
    auto huffmanCodes = getHuffmanCodes(huffmanTree);

    // Step 4: Encode the blocks
    std::string encodedData = encodeBlocks(quantizedBlocks, huffmanCodes);
    std::cout << "encodedData success" << std::endl;

    // Step 5: Save the encoded data
    saveEncodedData(encodedData, huffmanCodes, outputFile);

    std::cout << "Compression completed. Output saved to " << outputFile << std::endl;
}


int main() {
    std::cout << "Starting JPEG compression..." << std::endl;

    // Input and output file paths
    std::string inputFile = "Images/test2.jpeg";  
    std::string outputFile = "output.jc"; 

    try {
        // Step 1: Load the image
        Image img = loadImage(inputFile);
        std::cout << "Image loaded successfully: " << img.width << "x" << img.height << std::endl;

        // Step 2: Convert to YCbCr color space
        YCbCr ycbcr = rgbToYCbCr(img);
        std::cout << "Color space conversion completed." << std::endl;

        // Step 3: Subsample the chrominance channels
        std::vector<uint8_t> subsampledCb = subsampleChannel(ycbcr.Cb, img.width, img.height);
        std::vector<uint8_t> subsampledCr = subsampleChannel(ycbcr.Cr, img.width, img.height);
        std::cout << "Subsampling completed." << std::endl;

        // Step 4: Split channels into 8x8 blocks
        auto yBlocks = splitIntoBlocks(ycbcr.Y, img.width, img.height);
        auto cbBlocks = splitIntoBlocks(subsampledCb, img.width / 2, img.height / 2);
        auto crBlocks = splitIntoBlocks(subsampledCr, img.width / 2, img.height / 2);
        std::cout << "Block splitting completed." << std::endl;

        // Step 5: Apply DCT to each block
        for (auto& block : yBlocks) applyDCT(block);
        for (auto& block : cbBlocks) applyDCT(block);
        for (auto& block : crBlocks) applyDCT(block);
        std::cout << "Discrete Cosine Transform (DCT) applied to all blocks." << std::endl;

        // Step 6: Quantize the blocks
        std::vector<int> quantTable = { 
            16, 11, 10, 16, 24, 40, 51, 61,
            12, 12, 14, 19, 26, 58, 60, 55,
            14, 13, 16, 24, 40, 57, 69, 56,
            14, 17, 22, 29, 51, 87, 80, 62,
            18, 22, 37, 56, 68, 109, 103, 77,
            24, 35, 55, 64, 81, 104, 113, 92,
            49, 64, 78, 87, 103, 121, 120, 101,
            72, 92, 95, 98, 112, 100, 103, 99
        };
        for (auto& block : yBlocks) quantize(block, quantTable);
        for (auto& block : cbBlocks) quantize(block, quantTable);
        for (auto& block : crBlocks) quantize(block, quantTable);
        std::cout << "Quantization completed." << std::endl;

        // Combine all blocks for compression
        std::vector<std::vector<int>> allBlocks = yBlocks;
        allBlocks.insert(allBlocks.end(), cbBlocks.begin(), cbBlocks.end());
        allBlocks.insert(allBlocks.end(), crBlocks.begin(), crBlocks.end());

        std::cout << "Combined successfully" << std::endl;

        // Step 7: Compress the blocks
        compressJPEG(allBlocks, outputFile);

        std::cout << "JPEG compression completed successfully." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "An error occurred: " << e.what() << std::endl;
    }

    return 0;
}