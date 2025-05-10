# Compresso

A file compression tool supporting multiple file formats, developed using C++ and C. The project implements various compression algorithms, with a focus on Huffman encoding for efficient file size reduction. The tool also supports lossy JPEG compression using `libjpeg`. It is optimized for speed and designed to work across major operating systems.

## Table of Contents

- [Project Overview](#project-overview)
- [Features](#features)
- [How It Works](#how-it-works)
- [Installation](#installation)
- [Usage](#usage)
- [Technical Details](#technical-details)
- [Performance](#performance)
- [JPEG Compression (Lossy)](#jpeg-compression-lossy)

## Project Overview

**Compresso** provides a lightweight and extensible way to compress and decompress files. The project uses Huffman encoding for text files and lossy quality-adjustable recompression for JPEGs. It is written in C++ and C and works on Linux, macOS, and Windows with minimal setup.

## Features

- 🗂️ **Multi-format support**: Compresses JPEG and text files (more coming soon).
- ⚡ **Fast & efficient**: Optimized Huffman implementation and JPEG quality control.
- 🔁 **Bidirectional support**: Compress and decompress `.txt` files.
- 🛠️ **Cross-platform**: Compatible with major operating systems.
- 🧩 **Modular code**: Easy to extend with new file types and algorithms.

---

## How It Works

### Text File Compression

1. **Frequency Analysis**: Counts character frequencies.
2. **Huffman Tree Construction**: Builds tree using a min-heap.
3. **Encoding**: Encodes input text with Huffman codes.
4. **Output**: Saves both tree and bitstream in binary format.

### Text File Decompression

1. **Tree Reconstruction**: Loads tree from compressed file.
2. **Bitstream Decoding**: Decodes Huffman-encoded stream.
3. **Reconstruction**: Writes back the original file.

### JPEG Compression

1. **Read JPEG**: Using `libjpeg`.
2. **Recompress**: Apply lossy compression with quality factor.
3. **Write New JPEG**: Save the recompressed version.

---

## Installation

### Prerequisites

- A C++11-compatible compiler (e.g., GCC or Clang)
- `libjpeg` installed (`brew install jpeg` on macOS)
- `make` (recommended for building)

### Build Instructions

1. Clone the repository:

```bash
git clone https://github.com/SinghDevanshh/Compresso.git
cd Compresso
````

2. Build using the provided `Makefile`:

```bash
make
```

3. The `main` executable will be generated in the root directory.

> To clean up build artifacts:

```bash
make clean
```

---

## Usage

Run the executable with the following format:

```bash
./main <input_file_path> <file_type> [quality or --decompress]
```

### 📦 Compress a JPEG file

```bash
./main test_images/test.jpeg jpeg 85
```

### 📦 Compress a text file

```bash
./main test_files/sample.txt txt
```

### 📥 Decompress a text file

```bash
./main compressed.bin txt --decompress
```

---

## Technical Details

* **Languages**: C++, C
* **Algorithms**: Huffman Encoding (for `.txt`), JPEG lossy recompression
* **Libraries**:

  * `libjpeg`: For JPEG decoding and recompression
  * Standard C++ STL: For containers, file I/O, etc.

### Huffman Encoding

* Huffman encoding assigns shorter codes to frequent characters.
* Our implementation builds the Huffman tree, encodes the input, and stores the tree in the output for decompression.

---

## Performance

* Reduces JPEG file sizes by up to **40%**
* Reduces text file sizes by up to **50%**
* Optimized compression routines with fast I/O handling

### 📊 Compression Results (Text)

| File Name          | Original Size | Compressed Size |
| ------------------ | ------------- | --------------- |
| `Big.txt`          | 6.5 MB        | 3.7 MB          |
| `test1.txt`        | 2 KB          | 971 Bytes       |
| `test2.txt`        | 13 KB         | 7 KB            |
| `Harry_Potter.txt` | 438 KB        | 252 KB          |

---

## JPEG Compression (Lossy)

Lossy JPEG compression works by adjusting the image quality factor using `libjpeg`. This is ideal for large JPEGs where some quality loss is acceptable in exchange for space savings.

### Why Not Huffman for JPEG?

While Huffman encoding is ideal for **text and binary data**, it's not well-suited for JPEG images because:
- JPEG files are **already compressed**, often with built-in Huffman coding.
- Applying Huffman compression again typically yields **negligible or even increased size**.
- Lossy recompression allows **far more significant size reduction** by adjusting image quality.

### Libraries Used

- **libjpeg**: Used for reading and writing JPEG files with control over quality (used in `CompressJpeg.cpp`).
- **Standard C/C++ Libraries**: For file handling and memory operations.

> ⚠️ Ensure `libjpeg` is installed before compiling JPEG-related code:
> ```bash
> homebrew install jpeg
> ```

### JPEG Compression Results

Here are compression results using lossy JPEG recompression with quality factor adjustment:

| File Name        | Original Size | Compressed Size |
|------------------|----------------|------------------|
| `test2.jpeg`     | 1.6 MB         | 694 KB           |
| `test4.jpeg`     | 2.0 MB         | 557 KB           |
| `testsmall.jpeg` | 878 Bytes      | 999 Bytes        |

> 📌 Note: In rare cases like `testsmall.jpeg`, the recompression might slightly **increase** the file size due to format overhead when quality can't be further reduced.
