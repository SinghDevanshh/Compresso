// FileTypeValidator.h
#ifndef FILETYPEVALIDATOR_H
#define FILETYPEVALIDATOR_H

#include <string>
#include <vector>

// Enumeration for supported file types
enum class FileType {
    JPEG,
    // TO add more functionality later on
};

// Function to check if a file exists
bool fileExists(const std::string &filePath);

// Function to read the header bytes of a file
std::vector<unsigned char> readHeaderBytes(const std::string &filePath, size_t numBytes);

// Function to validate the file type
bool validateFileType(const std::string &filePath, FileType expectedType);

#endif // FILETYPEVALIDATOR_H
