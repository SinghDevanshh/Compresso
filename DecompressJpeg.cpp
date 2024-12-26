/*
Decompression is implemented by reversing the compression steps:
    > Entropy Decode: Decode the compressed data.
    > Dequantize: Multiply the quantized values by the quantization table.
    > Apply IDCT: Convert frequency domain back to spatial domain.
    > Merge Blocks: Combine 8x8 blocks to form the image.
    > Upsample: Expand the Cb and Cr channels.
    > Convert to RGB: Convert from YCbCr back to RGB.
*/