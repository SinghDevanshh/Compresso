// FileTypeValidator.cpp
#include "FileTypeValidator.h"
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <cctype>

/*
How does it work:
To ensure a file is a valid JPEG, verify its first few bytes (magic number), not just the file extension.
The first two bytes must be 0xFFD8, which is part of the JPEG file signature.

For TXT, we simply check if the file exists and contains readable text characters.
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

    switch (expectedType) {
        case FileType::JPEG: {
            std::vector<uint8_t> expectedHeader = {0xFF, 0xD8, 0xFF};
            size_t requiredBytes = expectedHeader.size();
            std::vector<unsigned char> headerBytes = readHeaderBytes(filePath, requiredBytes);
            return std::equal(expectedHeader.begin(), expectedHeader.end(), headerBytes.begin());
        }

        case FileType::TXT: {
            std::ifstream file(filePath);
            if (!file) return false;

            // Check the first 512 characters to see if they are printable or whitespace
            char ch;
            int count = 0;
            while (file.get(ch) && count < 512) {
                if (!std::isprint(static_cast<unsigned char>(ch)) && !std::isspace(static_cast<unsigned char>(ch))) {
                    return false;
                }
                count++;
            }
            return true;
        }

        default:
            throw std::invalid_argument("Unsupported file type.");
    }
}
