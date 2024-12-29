/*
Decompression is implemented by reversing the compression steps:
    > Entropy Decode: Decode the compressed data.
    > Dequantize: Multiply the quantized values by the quantization table.
    > Apply IDCT: Convert frequency domain back to spatial domain.
    > Merge Blocks: Combine 8x8 blocks to form the image.
    > Upsample: Expand the Cb and Cr channels.
    > Convert to RGB: Convert from YCbCr back to RGB.
*/


// Some other iimplementations for DCT and similarly IDCT

// float **calloc_mat(int dimX, int dimY){
//     float **m = (float**) calloc(dimX, sizeof(float*));
//     float *p = (float*) calloc(dimX*dimY, sizeof(float));
//     int i;
//     for(i=0; i <dimX;i++){
//     m[i] = &p[i*dimY];

//     }
//    return m;
// }

// void applyDCT(float **DCTMatrix , float **blocks){
    
//     int i, j, u, v;
//     for (u = 0; u < 8; ++u) {
//         for (v = 0; v < 8; ++v) {
//         DCTMatrix[u][v] = 0;
//             for (i = 0; i < 8; i++) {
//                 for (j = 0; j < 8; j++) {
//                     DCTMatrix[u][v] += round(blocks[i][j] * cos(M_PI/((float)8)*(i+1./2.)*u)*cos(M_PI/((float)8)*(j+1./2.)*v));
//                 }               
//             }
//         }
//     }  
// }

// void idct(float **Matrix, float **DCTMatrix, int N, int M){
//     int i, j, u, v;

//     for (u = 0; u < N; ++u) {
//         for (v = 0; v < M; ++v) {
//             Matrix[u][v] = 1/4.*DCTMatrix[0][0];
//             for(i = 1; i < N; i++){
//                 Matrix[u][v] += 1/2.*DCTMatrix[i][0];
//             }
//             for(j = 1; j < M; j++){
//                 Matrix[u][v] += 1/2.*DCTMatrix[0][j];
//             }

//             for (i = 1; i < N; i++) {
//                 for (j = 1; j < M; j++) {
//                     Matrix[u][v] += DCTMatrix[i][j] * cos(M_PI/((float)N)*(u+1./2.)*i)*cos(M_PI/((float)M)*(v+1./2.)*j);
//                     }               
//                 }
//             Matrix[u][v] *= 2./((float)N)*2./((float)M);
//             }
//         }  
//     }

// Test for DCT 


        // float **DCTMatrix = calloc_mat(8, 8);
        
        // float    
        // testBlockA[8][8] = { {255, 255, 255, 255, 255, 255, 255, 255},
        //                  {255, 255, 255, 255, 255, 255, 255, 255},
        //                  {255, 255, 255, 255, 255, 255, 255, 255},
        //                  {255, 255, 255, 255, 255, 255, 255, 255},
        //                  {255, 255, 255, 255, 255, 255, 255, 255},
        //                  {255, 255, 255, 255, 255, 255, 255, 255},
        //                  {255, 255, 255, 255, 255, 255, 255, 255},
        //                  {255, 255, 255, 255, 255, 255, 255, 255} };
        // float **testBlock = calloc_mat(8, 8);
        // for(int i = 0; i<8; i++){
        //     for(int j = 0; j<8; j++){
        //     testBlock[i][j] = yBlocks[i][j];
        //     }
        // }
        // applyDCT(DCTMatrix,testBlock);

        // for (int i = 0; i < 8; ++i) {
        //     for (int j = 0; j < 8; ++j) {
        //         std::cout << DCTMatrix[i][j] << " ";
        //     }
        //     std::cout << std::endl;
        // }

        // std::cout << "Original Block:" << std::endl;
        // auto block = yBlocks[0];
        // for (int i = 0; i < 8; ++i) {
        //     for (int j = 0; j < 8; ++j) {
        //         std::cout << block[i * 8 + j] << " ";
        //     }
        //     std::cout << std::endl;
        // }
        // applyDCT2(block);

        // std::cout << "DCT Transformed Block:" << std::endl;
        // for (int i = 0; i < 8; ++i) {
        //     for (int j = 0; j < 8; ++j) {
        //         std::cout << block[i * 8 + j] << " ";
        //     }
        //     std::cout << std::endl;
        // }

        // std::vector<int> testBlock(64, 10);
        // applyDCT2(testBlock);

        // std::cout << "DCT Coefficients for Uniform Block:" << std::endl;
        // for (int i = 0; i < 8; ++i) {
        //     for (int j = 0; j < 8; ++j) {
        //         std::cout << testBlock[i * 8 + j] << " ";
        //     }
        //     std::cout << std::endl;
        // }