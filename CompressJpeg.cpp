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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <iostream>
#include <vector>
#include <cmath>
#include <cstdint>



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


/*
Block Splitting

Divide the image into 8x8 blocks for DCT processing
*/

/*
Discrete Cosine Transform (DCT):

Implementing the DCT using the standard formula.
*/
/*
Quantization:

Divide the transformed coefficients by a quantization table and round them.
*/


/*
Entropy Encoding:

Using Huffman encoding--
*/




// namespace jpeg {


//     static void* jpeg_malloc(size_t size){
//         return malloc(size);
//     }

//     static void jpeg_free(void* pointer){
//         return free(pointer);
//     }





// }