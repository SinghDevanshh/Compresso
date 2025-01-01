// main.cpp
#include "FileTypeValidator.h"
#include "CompressJpeg.h"
#include <iostream>

int main(int argc, char* argv[]) {
    // Check if the correct number of arguments is provided
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <file_path> <file_type>\n";
        std::cerr << "Supported file types:\n";
        std::cerr << "  jpeg\n";
        // TODO -> List more supported types here
        return 1;
    }

    std::string filePath = argv[1];
    std::string fileTypeStr = argv[2];
    std::string outputFilePath = "output.jc";
    int quality;

    FileType expectedType;

    // Map string input to FileType enum
    if (fileTypeStr == "jpeg" || fileTypeStr == "JPEG") {
        expectedType = FileType::JPEG;
    }

    //TODO -> Add more elif for more file types
    else {
        std::cerr << "Unsupported file type: " << fileTypeStr << "\n";
        return 1;
    }

    // In a try catch block to handle exceptions thrown by the FileTypeValidator
    try {
        bool isValid = validateFileType(filePath, expectedType);
        if (isValid) {
            std::cout << "The file is a valid " << fileTypeStr << " file.\n";
        } else {
            std::cout << "The file is NOT a valid " << fileTypeStr << " file.\n";
        }
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    bool success = compressJpeg(filePath, outputFilePath , quality);

    if (success) {
        std::cout << "The file has been compressed" << std::endl;
    }
    else{
        std::cout << "There was an error " << std::endl;
    }


    return 0;
}



// Commands to run file on bash (can pick either one)
//  g++ -std=c++11 -o main main.cpp FileTypeValidator.cpp CompressJpeg.cpp
//  g++ -std=c++14 -o main main.cpp FileTypeValidator.cpp CompressJpeg.cpp

// ./main /Users/devansh/File-Compressor/Images/test4.jpeg jpeg

// ./main /Users/devansh/File-Compressor/Images/test3.test jpeg [SHOULD BE A VAILD JPEG as it is renamed from test2]