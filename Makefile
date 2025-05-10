# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++11 -Wall

# JPEG library paths (adjust if different)
JPEG_INC = /opt/homebrew/opt/jpeg/include
JPEG_LIB = /opt/homebrew/opt/jpeg/lib

INCLUDES = -I$(JPEG_INC)
LDFLAGS = -L$(JPEG_LIB) -ljpeg

# Source files with correct relative paths
SRC = main.cpp \
      File_Validate/FileTypeValidator.cpp \
      Jpeg/Libjpeg_lossy/LossyJpegCompressor.cpp \
      Txt/Compress_txt.cpp \
      Txt/Decompress_txt.cpp

# Object files will be placed in the same structure
OBJ = $(SRC:.cpp=.o)

# Target executable
TARGET = main

# Default target
all: $(TARGET)

# Link object files into the executable
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(INCLUDES) $(LDFLAGS)

# Compile .cpp files into .o files
%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCLUDES)

# Clean up build artifacts
clean:
	rm -f $(OBJ) $(TARGET)


# Clean outputs
outputs:
	rm -f compressed.bin compressed.jpeg output.txt