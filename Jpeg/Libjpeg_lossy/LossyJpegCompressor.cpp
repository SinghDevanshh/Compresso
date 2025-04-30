#include <iostream>
#include <stdio.h>
#include <jpeglib.h>
#include <stdlib.h>

void compress_jpeg(const char* input_filename, const char* output_filename, int quality) {
    FILE* infile = fopen(input_filename, "rb");
    if (!infile) {
        std::cerr << "Error opening input file " << input_filename << std::endl;
        return;
    }

    // Read JPEG image
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, infile);
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);

    int width = cinfo.output_width;
    int height = cinfo.output_height;
    int num_channels = cinfo.output_components;

    JSAMPARRAY buffer = (*cinfo.mem->alloc_sarray)
        ((j_common_ptr)&cinfo, JPOOL_IMAGE, width * num_channels, 1);

    // Allocate memory for full image
    unsigned char* image_buffer = new unsigned char[width * height * num_channels];
    unsigned char* p = image_buffer;

    while (cinfo.output_scanline < height) {
        jpeg_read_scanlines(&cinfo, buffer, 1);
        memcpy(p, buffer[0], width * num_channels);
        p += width * num_channels;
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);

    // Write compressed image
    FILE* outfile = fopen(output_filename, "wb");
    if (!outfile) {
        std::cerr << "Error opening output file " << output_filename << std::endl;
        delete[] image_buffer;
        return;
    }

    struct jpeg_compress_struct cinfo_out;
    struct jpeg_error_mgr jerr_out;
    cinfo_out.err = jpeg_std_error(&jerr_out);
    jpeg_create_compress(&cinfo_out);
    jpeg_stdio_dest(&cinfo_out, outfile);

    cinfo_out.image_width = width;
    cinfo_out.image_height = height;
    cinfo_out.input_components = num_channels;
    cinfo_out.in_color_space = (num_channels == 3) ? JCS_RGB : JCS_GRAYSCALE;

    jpeg_set_defaults(&cinfo_out);
    jpeg_set_quality(&cinfo_out, quality, TRUE);

    jpeg_start_compress(&cinfo_out, TRUE);

    JSAMPROW row_pointer;
    while (cinfo_out.next_scanline < height) {
        row_pointer = (JSAMPROW)&image_buffer[cinfo_out.next_scanline * width * num_channels];
        jpeg_write_scanlines(&cinfo_out, &row_pointer, 1);
    }

    jpeg_finish_compress(&cinfo_out);
    jpeg_destroy_compress(&cinfo_out);
    fclose(outfile);
    delete[] image_buffer;

    std::cout << "Compression complete: " << output_filename << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " <input.jpg> <output.jpg> <quality (0-100)>\n";
        return 1;
    }

    const char* input = argv[1];
    const char* output = argv[2];
    int quality = atoi(argv[3]);
    compress_jpeg(input, output, quality);
    return 0;
}

/*

To run : 

Install jpeg using homebrew 

run g++ LossyJpegCompressor.cpp -o jpeg_optimize   -I/opt/homebrew/opt/jpeg/include   -L/opt/homebrew/opt/jpeg/lib   -ljpeg

./jpeg_optimize <input.jpg> <output.jpg> <quality (0-100)>


*/