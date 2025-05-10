// main.cpp
#include "File_Validate/FileTypeValidator.h"
#include "Jpeg/Libjpeg_lossy/LossyJpegCompressor.h"
#include "Txt/Compress_txt.h"
#include "Txt/Decompress_txt.h"
#include <algorithm>
#include <cctype>
#include <string>
#include <iostream>

int main(int argc, char* argv[]) {
    // Check if the correct number of arguments is provided
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <file_path> <file_type> <[Quality for jpeg] or [--decompress for txt] (optional)>\n";
        std::cerr << "Supported file types:\n";
        std::cerr << "  jpeg\n";
        std::cerr << "  txt\n";
        // TODO -> List more supported types here
        return 1;
    }

    // Default quality set to 75 if not provided
    int quality = 75;

    if (argc == 4) {
        quality = atoi(argv[3]);
    }

    const char * filePath = argv[1];
    std::string fileTypeStr = argv[2];
    std::transform(fileTypeStr.begin(), fileTypeStr.end(), fileTypeStr.begin(),[](unsigned char c){ return std::tolower(c); });

    FileType expectedType;

    // Map string input to FileType enum
    if (fileTypeStr == "jpeg") {
        expectedType = FileType::JPEG;
    }
    else if (fileTypeStr == "txt") {
        expectedType = FileType::TXT;
    }
    //TODO -> Add more elif for more file types
    else {
        std::cerr << "Unsupported file type: " << fileTypeStr << "\n";
        return 1;
    }

    // In a try catch block to handle exceptions thrown by the FileTypeValidator
    try {
        bool isValid = validateFileType(filePath, expectedType);
        if (isValid || std::string(argv[3]) == "--decompress") {
            std::cout << "The file is a valid " << fileTypeStr << " file.\n";
        } else {
            std::cout << "The file is NOT a valid " << fileTypeStr << " file.\n";
        }
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    if (fileTypeStr == "txt"){
        // Compress txt file
        const std::string inputFile = filePath;

        if (argc == 4 && std::string(argv[3]) == "--decompress") {
            const std::string outputFile = "output.txt";
            decompress_txt_file(inputFile, outputFile);
        } else {
            const std::string outputFile = "compressed.bin";
            compress_txt_file(inputFile, outputFile);
        }
    }

    else if (fileTypeStr == "jpeg"){
        // Compress jpeg file
        const char * outputFilePath = "compressed.jpeg";
        compress_jpeg(filePath, outputFilePath , quality);
    }

    else{
        std::cerr << "Error" << "\n";
        return 1;
    }

    return 0;
}


/* Usage :

Commands to run file on bash (can pick either one)

g++ -std=c++11 -o main main.cpp File_Validate/FileTypeValidator.cpp Jpeg/Libjpeg_lossy/LossyJpegCompressor.cpp -I/opt/homebrew/opt/jpeg/include   -L/opt/homebrew/opt/jpeg/lib   -ljpeg

OR 

g++ -std=c++14 -o main main.cpp File_Validate/FileTypeValidator.cpp Jpeg/Libjpeg_lossy/LossyJpegCompressor.cpp -I/opt/homebrew/opt/jpeg/include   -L/opt/homebrew/opt/jpeg/lib   -ljpeg

--------------------------------------------------------------------------------------------------------------------------------------------

./main test.jpeg jpeg 90

OR 

./main test.jpeg jpeg 

*/


/*
Todo :

Thinking about adding a frontend to this for project showcase
*/