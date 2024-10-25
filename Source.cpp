#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <queue>

using namespace std;

class HuffmanTree {
public:
    struct Node {
        Node* left;
        Node* right;
        int frequency;
        char symbol;

        Node(char s, int f) : left(nullptr), right(nullptr), frequency(f), symbol(s) {}
        Node(Node* l, Node* r) : left(l), right(r), frequency(l->frequency + r->frequency), symbol('\0') {}
    };

    HuffmanTree() : root(nullptr) {}
    ~HuffmanTree() { clear(root); }

    void build(const string& text);
    void encode(const string& text, vector<bool>& encodedBits);
    bool decode(const vector<bool>& encodedBits, string& decodedText);

private:
    Node* root;
    map<char, vector<bool>> codes;

    void clear(Node* node);
    void generateCodes(Node* node, vector<bool>& code);
};

void HuffmanTree::clear(Node* node) {
    if (node) {
        clear(node->left);
        clear(node->right);
        delete node;
    }
}

void HuffmanTree::build(const string& text) {
    map<char, int> freq;
    for (char c : text) {
        freq[c]++;
    }

    auto compare = [](Node* a, Node* b) { return a->frequency > b->frequency; };
    priority_queue<Node*, vector<Node*>, decltype(compare)> pq(compare);

    for (auto& pair : freq) {
        pq.push(new Node(pair.first, pair.second));
    }

    while (pq.size() > 1) {
        Node* left = pq.top(); pq.pop();
        Node* right = pq.top(); pq.pop();
        pq.push(new Node(left, right));
    }

    root = pq.top();
    pq.pop();

    vector<bool> code;
    generateCodes(root, code);
}

void HuffmanTree::generateCodes(Node* node, vector<bool>& code) {
    if (node->left) {
        code.push_back(false);
        generateCodes(node->left, code);
        code.pop_back();
    }
    if (node->right) {
        code.push_back(true);
        generateCodes(node->right, code);
        code.pop_back();
    }
    if (!node->left && !node->right) {
        codes[node->symbol] = code;
    }
}

void HuffmanTree::encode(const string& text, vector<bool>& encodedBits) {
    for (char c : text) {
        const vector<bool>& code = codes[c];
        encodedBits.insert(encodedBits.end(), code.begin(), code.end());
    }
}

bool HuffmanTree::decode(const vector<bool>& encodedBits, string& decodedText) {
    Node* current = root;
    for (bool bit : encodedBits) {
        if (bit) {
            current = current->right;
        }
        else {
            current = current->left;
        }
        if (!current->left && !current->right) {
            decodedText += current->symbol;
            current = root;
        }
    }
    return current == root;
}

void to_encode(HuffmanTree& huffmanTree) {
    ifstream inputFile("исходный текст.txt");
    string text((istreambuf_iterator<char>(inputFile)), istreambuf_iterator<char>());
    inputFile.close();

    huffmanTree.build(text);

    vector<bool> encodedBits;
    huffmanTree.encode(text, encodedBits);

    ofstream outputFile("код.txt", ios::binary);
    for (bool bit : encodedBits) {
        outputFile.put(bit ? '1' : '0');
    }
    outputFile.close();
}

void to_decode(HuffmanTree& huffmanTree) {
    ifstream encodedFile("код.txt", ios::binary);
    vector<bool> encodedBits;
    char bit;
    while (encodedFile.get(bit)) {
        encodedBits.push_back(bit == '1');
    }
    encodedFile.close();

    string decodedText;
    huffmanTree.decode(encodedBits, decodedText);

    ofstream decodedFile("итог.txt");
    decodedFile << decodedText;
    decodedFile.close();
}

bool proof() {
    ifstream originalFile("исходный текст.txt");
    ifstream decodedFile("итог.txt");
    string originalText((istreambuf_iterator<char>(originalFile)), istreambuf_iterator<char>());
    string decodedText((istreambuf_iterator<char>(decodedFile)), istreambuf_iterator<char>());
    originalFile.close();
    decodedFile.close();

    return originalText == decodedText;
}

int main() {
    setlocale(LC_ALL, "Rus");

    HuffmanTree huffmanTree;

    to_encode(huffmanTree);
    to_decode(huffmanTree);

    if (proof()) {
        cout << "Декодирование успешно завершено." << endl;
    }
    else {
        cout << "Ошибка при декодировании." << endl;
    }

    return 0;
}