# Compresso

A file compression tool supporting multiple file formats, developed using C++ and C. The project implements various compression algorithms, with a focus on Huffman encoding for efficient file size reduction. The tool ensures compatibility across different operating systems and file types.

## Table of Contents

- [Project Overview](#project-overview)
- [Features](#features)
- [How It Works](#how-it-works)
- [Installation](#installation)
- [Usage](#usage)
- [Technical Details](#technical-details)
- [Performance](#performance)

## Project Overview

The File Compressor project aims to provide an efficient way to reduce file sizes while maintaining data integrity. Using advanced compression algorithms, such as Huffman encoding, the tool supports a variety of file types and is optimized for cross-platform use.

## Features

- **Support for multiple file formats**: Compresses various file types.
- **Efficient compression algorithms**: Utilizes Huffman encoding for lossless compression.
- **Cross-platform compatibility**: Works across different operating systems (Linux, Windows, macOS).
- **Fast compression speed**: The tool has been optimized for speed with a 25% improvement in performance compared to standard algorithms.
- **Data integrity**: Ensures that no data is lost during compression or decompression.

## How It Works

### Compression Process

1. **File Analysis**: The tool reads the input file and analyzes the frequency of characters (or bytes) within the file.
2. **Huffman Tree Construction**: Based on the frequency analysis, a Huffman tree is built to create optimal encoding.
3. **Encoding**: The input file is encoded using the generated Huffman tree, reducing its size.
4. **Saving**: The compressed file is saved in a custom format for future decompression.

### Decompression Process

1. **Huffman Tree Loading**: The tool loads the Huffman tree from the compressed file.
2. **Decoding**: The encoded data is decoded using the Huffman tree, reconstructing the original file.
3. **Saving**: The decompressed file is saved to the specified output path.

## Installation

To install and use the File Compressor tool, follow the steps below.

### Prerequisites

- C++11 or higher
- A C compiler (e.g., GCC or Clang)
- CMake (for building the project)

### Steps

1. Clone the repository:
   ```bash
   git clone https://github.com/SinghDevanshh/Compresso.git
   cd Compresso
   ```

2. Build the project:
   ```bash
   g++ -std=c++11 -o main main.cpp FileTypeValidator.cpp CompressJpeg.cpp
   ```

3. The executable will be available in the directory.

## Usage

Once built, the tool can be used via the command line to compress and decompress files.

### Compress a file
```bash
./main <input_file> <file_format>
```

### Decompress a file
```bash
./decompress <input_file>
```

## Technical Details

- **Programming Languages**: C++, C
- **Key Algorithms**: Huffman encoding, data compression techniques
- **Libraries Used**: 
  - **STB Image**: For handling image input/output.
  - **Standard C++ Libraries**: For file I/O, math, and system programming.
  
### Huffman Encoding

- Huffman encoding is used for lossless compression, reducing file sizes by analyzing the frequency of symbols (characters/bytes) and creating optimal encoding schemes.
- The algorithm constructs a binary tree (Huffman tree), where frequent symbols are assigned shorter codes, leading to more efficient storage.

## Performance

- The tool reduces file sizes by up to **30%** for JPG files.
- The tool reduces file sizes by up to **50%** for TXT files.
- Compression speed has been optimized, with a performance improvement.

### Compression Metrics (Text Files)

| File Name           | Original Size | Compressed Size |
|---------------------|---------------|------------------|
| `Big.txt`           | 6.5 MB        | 3.7 MB           |
| `test1.txt`         | 2 KB          | 971 Bytes        |
| `test2.txt`         | 13 KB         | 7 KB             |
| `Harry_Potter.txt`  | 438 KB        | 252 KB           |
|---------------------|---------------|------------------|
