#include <iostream>
#include <vector>
#include <unordered_map>
#include <fstream>

using namespace std;

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

// Map to store Huffman codes
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


// Example usage
int main() {
    char arr[] = { 'a', 'b', 'c', 'd', 'e', 'f' };
    int freq[] = { 5, 9, 12, 13, 16, 45 };

    int size = sizeof(arr) / sizeof(arr[0]);

    Min_Heap* minHeap = createAndBuildMin_Heap(arr, freq, size);

    cout << "Min Heap built from given characters and frequencies:" << endl;
    printMinHeap(minHeap);

    // Clean up memory
    for (int i = 0; i < size; ++i) {
        delete minHeap->array[i];
    }
    delete minHeap;

    return 0;
}