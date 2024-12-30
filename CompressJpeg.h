#ifndef COMPRESS_JPEG_H
#define COMPRESS_JPEG_H

#include <string>
#include <vector>

// Function to compress a JPEG image file.
// Parameters:
// - inputPath: Path to the input JPEG image file.
// - outputPath: Path to save the compressed JPEG image file.
// - quality: Compression quality (1-100, where 100 is the highest quality).
// Returns:
// - true if compression was successful, false otherwise.
bool compressJpeg(const std::string& inputPath, const std::string& outputPath, int quality);


#endif // COMPRESS_JPEG_H
