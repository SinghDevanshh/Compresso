#include <iostream>
#include <vector>
#include <unordered_map>
#include <fstream>
#include "Compress_txt.h"

using namespace std;


// To run g++ -std=c++11 -o compress Compress_txt.cpp

/*
------------------------------------------------------------------------------------------------------------------------------------
Function for a Min Heap:
------------------------------------------------------------------------------------------------------------------------------------
*/

// Structure for tree nodes
struct Node {
    char character;
    int freq;
    Node *l, *r;

    Node(char c, int f)
        : character(c)
        , freq(f)
        , l(nullptr)
        , r(nullptr)
    {
    }
};

// Structure for min heap
struct Min_Heap {
    int size;
    vector<Node*> array;

    Min_Heap(int s)
        : size(s)
        , array(s)
    {
    }
};

// Swap two Node pointers
void swapNode(Node*& a, Node*& b) {
    Node* temp = a;
    a = b;
    b = temp;
}

// Heapify the subtree rooted at idx
void Heapify(Min_Heap* minHeap, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < minHeap->size && minHeap->array[left]->freq < minHeap->array[smallest]->freq)
        smallest = left;

    if (right < minHeap->size && minHeap->array[right]->freq < minHeap->array[smallest]->freq)
        smallest = right;

    if (smallest != idx) {
        swapNode(minHeap->array[smallest], minHeap->array[idx]);
        Heapify(minHeap, smallest);
    }
}

// Function to create and build a min heap
Min_Heap* createAndBuildMin_Heap(char arr[], int freq[], int unique_size) {
    int i;

    // Initializing heap
    Min_Heap* minHeap = new Min_Heap(unique_size);

    // Initializing the array of pointers in minheap.
    for (i = 0; i < unique_size; ++i) {
        minHeap->array[i] = new Node(arr[i], freq[i]);
    }

    int n = minHeap->size - 1;
    for (i = (n - 1) / 2; i >= 0; --i) {
        Heapify(minHeap, i);
    }

    return minHeap;
}

// Function to print the heap (for testing)
void printMinHeap(Min_Heap* minHeap) {
    cout << "Character | Frequency" << endl;
    for (int i = 0; i < minHeap->size; ++i) {
        cout << minHeap->array[i]->character << "         | " << minHeap->array[i]->freq << endl;
    }
}

// Extract min node from heap
Node* extractMin(Min_Heap* minHeap) {
    Node* temp = minHeap->array[0];
    minHeap->array[0] = minHeap->array[minHeap->size - 1];
    minHeap->size--;
    Heapify(minHeap, 0);
    return temp;
}

// Insert new node into heap
void insertMinHeap(Min_Heap* minHeap, Node* node) {
    ++minHeap->size;
    int i = minHeap->size - 1;

    minHeap->array[i] = node;

    while (i && minHeap->array[i]->freq < minHeap->array[(i - 1) / 2]->freq) {
        swapNode(minHeap->array[i], minHeap->array[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

// Check if heap has only one node
bool isSizeOne(Min_Heap* minHeap) {
    return (minHeap->size == 1);
}


/*
------------------------------------------------------------------------------------------------------------------------------------
Function to build a Huffman Tree :
------------------------------------------------------------------------------------------------------------------------------------
*/

// Function to build Huffman Tree
Node* buildHuffmanTree(char arr[], int freq[], int unique_size) {
    Node *l, *r, *top;

    // Step 1: Create initial heap
    Min_Heap* minHeap = createAndBuildMin_Heap(arr, freq, unique_size);

    // Step 2: Iterate while heap has more than one node
    while (!isSizeOne(minHeap)) {
        // Step 3: Extract two minimum freq nodes
        l = extractMin(minHeap);
        r = extractMin(minHeap);

        // Step 4: Create new node with these two as children
        top = new Node('$', l->freq + r->freq);
        top->l = l;
        top->r = r;

        // Step 5: Insert the new node into heap
        insertMinHeap(minHeap, top);
    }

    // Step 6: The remaining node is the root
    return extractMin(minHeap);
}


/*
------------------------------------------------------------------------------------------------------------------------------------
Function to Build Huffman Codes from the Tree :
------------------------------------------------------------------------------------------------------------------------------------
*/

// Map to store Huffman codes (Global)
unordered_map<char, string> huffmanCodes;

// Recursive function to generate codes
void generateCodes(Node* root, string str) {
    if (!root)
        return;

    // Found a leaf node
    if (!root->l && !root->r) {
        huffmanCodes[root->character] = str;
    }

    generateCodes(root->l, str + "0");
    generateCodes(root->r, str + "1");
}

/*
------------------------------------------------------------------------------------------------------------------------------------
Function to write the Huffman tree into file :
------------------------------------------------------------------------------------------------------------------------------------
*/

void saveTree(Node* root, ofstream& outFile) {
    if (!root) return;

    if (!root->l && !root->r) {
        // Leaf node: write '1' and the character
        outFile.put('1');
        outFile.put(root->character);
    } else {
        // Internal node: write '0'
        outFile.put('0');
        saveTree(root->l, outFile);
        saveTree(root->r, outFile);
    }
}

/*
------------------------------------------------------------------------------------------------------------------------------------
Function to Compress the Text File :
------------------------------------------------------------------------------------------------------------------------------------
*/

void compressFile(const string& inputFileName, const string& outputFileName, Node* root) {
    ifstream inFile(inputFileName, ios::in);
    ofstream outFile(outputFileName, ios::out | ios::binary);

    if (!inFile.is_open() || !outFile.is_open()) {
        cerr << "Error opening files!" << endl;
        return;
    }

    // Step 1: Save the Huffman Tree structure first
    saveTree(root, outFile);

    // Step 2: Mark end of tree with a special marker
    outFile.put('#'); // '#' as end of tree marker

    // Step 3: Build buffer first
    string buffer;
    char ch;
    while (inFile.get(ch)) {
        buffer += huffmanCodes[ch];
    }

    // Step 4: Save total number of actual bits
    int totalBits = buffer.size();
    outFile.write(reinterpret_cast<const char*>(&totalBits), sizeof(int));

    // Step 5: Now write compressed data
    int count = 0;
    unsigned char byte = 0;
    for (char bit : buffer) {
        byte = byte << 1 | (bit - '0');
        count++;

        if (count == 8) {
            outFile.put(byte);
            count = 0;
            byte = 0;
        }
    }

    // If leftover bits
    if (count > 0) {
        byte = byte << (8 - count);
        outFile.put(byte);
    }

    inFile.close();
    outFile.close();
    
    cout << "Compression complete. Output written to " << outputFileName << endl;
}



/*
------------------------------------------------------------------------------------------------------------------------------------
Function to Convert map to arrays for tree building :
------------------------------------------------------------------------------------------------------------------------------------
*/

void convertMapToArrays(const unordered_map<char, int>& freqMap, vector<char>& chars, vector<int>& freqs) {
    for (const auto& entry : freqMap) {
        chars.push_back(entry.first);
        freqs.push_back(entry.second);
    }
}

#include <unordered_map>

/*
------------------------------------------------------------------------------------------------------------------------------------
Function to Count character frequencies from the file :
------------------------------------------------------------------------------------------------------------------------------------
*/

unordered_map<char, int> countFrequencies(const string& filename) {
    unordered_map<char, int> freqMap;
    ifstream file(filename, ios::in);

    if (!file.is_open()) {
        cerr << "Error opening input file!" << endl;
        return freqMap;
    }

    char ch;
    while (file.get(ch)) {
        freqMap[ch]++;
    }

    file.close();
    return freqMap;
}

/*
------------------------------------------------------------------------------------------------------------------------------------
Export Function :
------------------------------------------------------------------------------------------------------------------------------------
*/

// Usage
void compress_txt_file(const string& inputFile, const string& outputFile) {
    unordered_map<char, int> freqMap = countFrequencies(inputFile);
    if (freqMap.empty()) {
        cerr << "Input file is empty or error reading!" << endl;
        return;
    }

    vector<char> chars;
    vector<int> freqs;
    convertMapToArrays(freqMap, chars, freqs);

    Node* root = buildHuffmanTree(chars.data(), freqs.data(), chars.size());
    huffmanCodes.clear();
    generateCodes(root, "");
    compressFile(inputFile, outputFile, root);
}

/*
------------------------------------------------------------------------------------------------------------------------------------
END
------------------------------------------------------------------------------------------------------------------------------------
*/