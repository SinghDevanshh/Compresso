#include <iostream>
#include <fstream>
#include <unordered_map>
#include <queue>
#include <bitset>
using namespace std;


/*
------------------------------------------------------------------------------------------------------------------------------------
Structure for tree nodes
------------------------------------------------------------------------------------------------------------------------------------
*/

struct Node {
    char character;
    int freq;
    Node *l, *r;

    Node(char c, int f)
        : character(c), freq(f), l(nullptr), r(nullptr) {}

    // For non-leaf internal node
    Node(Node* left, Node* right)
        : character('$'), freq(left->freq + right->freq), l(left), r(right) {}
};

/*
------------------------------------------------------------------------------------------------------------------------------------
Function to rebuild Huffman tree from code map
------------------------------------------------------------------------------------------------------------------------------------
*/

Node* buildTreeFromCodes(const unordered_map<char, string>& codes) {
    Node* root = new Node('$', 0);
    for (const auto& pair : codes) {
        Node* curr = root;
        const string& code = pair.second;
        for (char bit : code) {
            if (bit == '0') {
                if (!curr->l) curr->l = new Node('$', 0);
                curr = curr->l;
            } else {
                if (!curr->r) curr->r = new Node('$', 0);
                curr = curr->r;
            }
        }
        curr->character = pair.first;
    }
    return root;
}


/*
------------------------------------------------------------------------------------------------------------------------------------
Function to read the Huffman tree
------------------------------------------------------------------------------------------------------------------------------------
*/

Node* loadTree(ifstream& inFile) {
    char bit;
    if (!inFile.get(bit))
        return nullptr;

    if (bit == '1') {
        char ch;
        inFile.get(ch);
        return new Node(ch, 0);
    } else if (bit == '0') {
        Node* internal = new Node('$', 0);
        internal->l = loadTree(inFile);
        internal->r = loadTree(inFile);
        return internal;
    }
    return nullptr;
}


/*
------------------------------------------------------------------------------------------------------------------------------------
Function to decode bitstream using Huffman tree
------------------------------------------------------------------------------------------------------------------------------------
*/

void decompressFile(const string& compressedFile, const string& outputFile) {
    ifstream inFile(compressedFile, ios::binary);
    ofstream outFile(outputFile);

    if (!inFile.is_open() || !outFile.is_open()) {
        cerr << "Error opening input/output files." << endl;
        return;
    }

    // Step 1: Rebuild the Huffman Tree
    Node* root = loadTree(inFile);

    // Step 2: Read until '#' (end of tree marker)
    char marker;
    inFile.get(marker);
    if (marker != '#') {
        cerr << "Tree marker not found, corrupted file!" << endl;
        return;
    }

    // Step 3: Read actual number of bits
    int totalBits = 0;
    inFile.read(reinterpret_cast<char*>(&totalBits), sizeof(int));

    // Step 4: Now decode only 'totalBits' from the stream
    Node* curr = root;
    char byte;
    int bitsRead = 0;
    while (inFile.read(&byte, 1) && bitsRead < totalBits) {
        bitset<8> bits(byte);
        for (int i = 7; i >= 0 && bitsRead < totalBits; --i, ++bitsRead) {
            curr = bits[i] ? curr->r : curr->l;
            if (!curr->l && !curr->r) {
                outFile.put(curr->character);
                curr = root;
            }
        }
    }

    inFile.close();
    outFile.close();
    cout << "Decompression complete. Output written to " << outputFile << endl;
}



/*
------------------------------------------------------------------------------------------------------------------------------------
Example :
------------------------------------------------------------------------------------------------------------------------------------
*/

int main() {
    string compressed = "compressed.bin";
    string output = "output.txt";

    decompressFile(compressed, output);
    return 0;
}


/*
------------------------------------------------------------------------------------------------------------------------------------
END
------------------------------------------------------------------------------------------------------------------------------------
*/