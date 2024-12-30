/*
JPEG compression is a lossy compression technique. The main steps in JPEG compression include:
    > Color Space Conversion: Convert the image from RGB to YCbCr (luminance and chrominance channels).
    > Subsampling: Reduce the resolution of the chrominance channels (Cb and Cr) to save space.
    > Block Splitting: Divide the image into 8x8 pixel blocks.
    > Discrete Cosine Transform (DCT): Transform each block from spatial domain to frequency domain.
    > Quantization: Reduce the precision of less significant frequencies.
    > Entropy Encoding: Compress the quantized data using Huffman coding or similar techniques.
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
    std::vector<double> temp(64);
    for (int u = 0; u < 8; ++u) {
        for (int v = 0; v < 8; ++v) {
            double sum = 0;
            for (int x = 0; x < 8; x++) {
                for (int y = 0; y < 8; y++) {
                    double coeff = block[y * 8 + x];
                    sum += coeff * cos(((2 * x + 1) * u * M_PI) / 16.0) *
                           cos(((2 * y + 1) * v * M_PI) / 16.0);
                }
            }
            double cu = (u == 0) ? 1 / sqrt(2.0) : 1.0;
            double cv = (v == 0) ? 1 / sqrt(2.0) : 1.0;
            temp[u * 8 + v] = 0.25 * cu * cv * sum;
        }
    }
    for (int i = 0; i < 64; ++i) {
        block[i] = round(temp[i]);
    }
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

void freeHuffmanTree(Node* root) {
    if (root) {
        freeHuffmanTree(root->left);
        freeHuffmanTree(root->right);
        delete root;
    }
}

void generateCodes(Node* root, const std::string& code, std::map<int, std::string>& huffmanCodes) {
    if (!root) return;

    if (root->value != -1) { // Leaf node
        huffmanCodes[root->value] = code;
    }
    else{
        // Internal node reached, skipping
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
                // throw std::runtime_error("Missing Huffman code for coefficient: " + std::to_string(coeff));
                // std::cout << "Missing Huffman code for coefficient: " + std::to_string(coeff) << std::endl;
                // Do nothing for now
            }
            else{
                encodedData += huffmanCodes.at(coeff);
            }
        }
    }


    return encodedData;
}

void saveHuffmanTree(Node* root, std::ofstream& file) {
    if (!root) return;

    if (root->value != -1) { // Leaf node
        file.put(1); // Leaf indicator
        file.write(reinterpret_cast<char*>(&root->value), sizeof(root->value));
    } else { // Internal node
        file.put(0); // Internal node indicator
    }

    saveHuffmanTree(root->left, file);
    saveHuffmanTree(root->right, file);
}


void saveEncodedData(const std::string& encodedData, Node* huffmanTree, const std::string& outputFile) {
    std::ofstream file(outputFile, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open output file.");
    }

    // Step 1: Save Huffman tree in compact format
    saveHuffmanTree(huffmanTree, file);

    // Step 2: Save encoded data with optimized bit packing
    uint64_t buffer = 0;
    int bitCount = 0;

    for (char bit : encodedData) {
        buffer = (buffer << 1) | (bit - '0');
        ++bitCount;

        if (bitCount == 64) {
            file.write(reinterpret_cast<char*>(&buffer), sizeof(buffer));
            buffer = 0;
            bitCount = 0;
        }
    }

    if (bitCount > 0) {
        buffer <<= (64 - bitCount);
        file.write(reinterpret_cast<char*>(&buffer), sizeof(buffer));
    }

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
    saveEncodedData(encodedData, huffmanTree, outputFile);

    std::cout << "Compression completed. Output saved to " << outputFile << std::endl;
}

bool compressJpeg(const std::string& inputPath, const std::string& outputPath, int quality){


    std::cout << "Starting JPEG compression..." << std::endl;

    // Input and output file paths
    std::string inputFile = "Images/test4.jpeg";  
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

        std::vector<int> quantTable = { 16,11,12,14,12,10,16,14,
                13,14,18,17,16,19,24,40,
                26,24,22,22,24,49,35,37,
                29,40,58,51,61,60,57,51,
                56,55,64,72,92,78,64,68,
                87,69,55,56,80,109,81,87,
                95,98,103,104,103,62,77,113,
                121,112,100,120,92,101,103,99 };

        // std::vector<int> quantTableAggressive = {
        //         32, 22, 20, 32, 48, 80, 102, 122,
        //         24, 24, 28, 38, 52, 116, 120, 110,
        //         28, 27, 32, 46, 72, 114, 130, 112,
        //         28, 34, 44, 58, 92, 174, 160, 124,
        //         36, 44, 74, 112, 136, 218, 206, 154,
        //         48, 70, 110, 128, 162, 208, 226, 184,
        //         98, 128, 156, 174, 206, 242, 240, 202,
        //         144, 184, 190, 196, 224, 200, 206, 198
        //         };

        // std::vector<int> quantTableExtremelyAggressive = {
        //         64, 48, 40, 64, 96, 160, 204, 244,
        //         48, 48, 56, 76, 104, 232, 240, 220,
        //         56, 52, 64, 92, 144, 228, 260, 224,
        //         56, 68, 88, 116, 184, 348, 320, 248,
        //         72, 88, 148, 224, 272, 436, 412, 308,
        //         96, 140, 220, 256, 324, 416, 452, 368,
        //         196, 256, 312, 348, 412, 484, 480, 404,
        //         288, 368, 380, 392, 448, 400, 412, 396
        //     };

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

        return true;
    } catch (const std::exception& e) {
        std::cerr << "An error occurred: " << e.what() << std::endl;
        return false;
    }

}
