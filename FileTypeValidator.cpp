// FileTypeValidator.cpp
#include "FileTypeValidator.h"
#include <fstream>
#include <stdexcept>
#include <algorithm>

/*
How does it work:
To ensure a file is a valid JPEG, verify its first few bytes (magic number), not just the file extension.
The first two bytes must be 0xFFD8, which is part of the JPEG file signature.
*/

// Function to check if a file exists
bool fileExists(const std::string &filePath) {
    std::ifstream file(filePath, std::ios::binary);
    return file.good();
}

// Function to read the header bytes of a file
std::vector<unsigned char> readHeaderBytes(const std::string &filePath, size_t numBytes) {
    std::ifstream file(filePath, std::ios::binary);
    std::vector<unsigned char> headerBytes(numBytes);

    if (!file.read(reinterpret_cast<char *>(headerBytes.data()), numBytes)) {
        throw std::runtime_error("Failed to read file header.");
    }

    return headerBytes;
}

// Function to validate the file type based on the expected type's header bytes
bool validateFileType(const std::string &filePath, FileType expectedType) {
    if (!fileExists(filePath)) {
        throw std::invalid_argument("File does not exist.");
    }

    std::vector<unsigned char> headerBytes;
    size_t requiredBytes = 0;
    std::vector<unsigned char> expectedMagic;

    switch (expectedType) {
        case FileType::JPEG:
            expectedMagic = {0xFF, 0xD8, 0xFF};
            requiredBytes = expectedMagic.size();
            break;
        // Add more cases here for different file types
        default:
            throw std::invalid_argument("Unsupported file type.");
    }

    headerBytes = readHeaderBytes(filePath, requiredBytes);

    return std::equal(expectedMagic.begin(), expectedMagic.end(), headerBytes.begin());
}
