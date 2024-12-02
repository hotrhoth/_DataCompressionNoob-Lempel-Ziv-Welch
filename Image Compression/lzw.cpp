#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <map>
#include <string>
#include <tuple>
#include <stdexcept>

using namespace std;

vector<string> split(const string &s, char delimiter) {
    vector<string> tokens;
    string token;
    istringstream tokenStream(s);
    while (getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

tuple<int, int, int, vector<int>> load_image_data(const string &file_path) {
    ifstream file(file_path);
    if (!file.is_open()) {
        throw runtime_error("Failed to open file");
    }

    string shape_line, data_line;
    getline(file, shape_line);
    getline(file, data_line);

    vector<string> shape_tokens = split(shape_line, ',');
    int height = stoi(shape_tokens[0]);
    int width = stoi(shape_tokens[1]);
    int channels = stoi(shape_tokens[2]);

    vector<string> data_tokens = split(data_line, ',');
    vector<int> pixels;
    for (const string &token : data_tokens) {
        pixels.push_back(stoi(token));
    }

    return {height, width, channels, pixels};
}

void save_compressed_data(const vector<int> &compressed, const string &file_path) {
    ofstream file(file_path, ios::binary);
    if (!file.is_open()) {
        throw runtime_error("Failed to open file for writing compressed data");
    }

    for (int code : compressed) {
        file.write(reinterpret_cast<const char *>(&code), sizeof(code));
    }
    file.close();
    cout << "Compressed data saved to " << file_path << endl;
}

vector<int> load_compressed_data(const string &file_path) {
    ifstream file(file_path, ios::binary);
    if (!file.is_open()) {
        throw runtime_error("Failed to open file for reading compressed data");
    }

    vector<int> compressed;
    int code;
    while (file.read(reinterpret_cast<char *>(&code), sizeof(code))) {
        compressed.push_back(code);
    }
    file.close();
    return compressed;
}

void save_decompressed_image(const vector<int> &pixels, int height, int width, int channels, const string &file_path) {
    ofstream file(file_path);
    if (!file.is_open()) {
        throw runtime_error("Failed to open file for saving decompressed image");
    }

    file << height << "," << width << "," << channels << "\n";
    for (size_t i = 0; i < pixels.size(); ++i) {
        file << pixels[i];
        if (i < pixels.size() - 1) {
            file << ",";
        }
    }
    file.close();
    cout << "Decompressed image data saved to " << file_path << endl;
}

// Compress a vector of integers using LZW
vector<int> lzw_compress(const vector<int> &data) {
    map<string, int> dictionary;
    for (int i = 0; i < 256; ++i) {
        dictionary[string(1, i)] = i;
    }

    string current = "";
    vector<int> compressed;
    int code = 256;

    for (int value : data) {
        string next = current + char(value);
        if (dictionary.count(next)) {
            current = next;
        } else {
            compressed.push_back(dictionary[current]);
            dictionary[next] = code++;
            current = string(1, char(value));
        }
    }
    if (!current.empty()) {
        compressed.push_back(dictionary[current]);
    }

    return compressed;
}

// Decompress a vector of integers using LZW
vector<int> lzw_decompress(const vector<int> &compressed) {
    map<int, string> dictionary;
    for (int i = 0; i < 256; ++i) {
        dictionary[i] = string(1, i);
    }

    string current = dictionary[compressed[0]];
    string result = current;
    int code = 256;

    for (size_t i = 1; i < compressed.size(); ++i) {
        string entry;
        if (dictionary.count(compressed[i])) {
            entry = dictionary[compressed[i]];
        } else if (compressed[i] == code) {
            entry = current + current[0];
        }

        result += entry;
        dictionary[code++] = current + entry[0];
        current = entry;
    }

    vector<int> decompressed;
    for (char c : result) {
        decompressed.push_back(static_cast<unsigned char>(c));
    }

    return decompressed;
}

int main() {

    string input_file = "leaves.txt";
    string compressed_file = "compressed_leaves.lzw";
    string decompressed_file = "decompressed_leaves.txt";

    int height, width, channels;
    vector<int> pixels;
    tie(height, width, channels, pixels) = load_image_data(input_file);

    cout << "Compressing image data..." << endl;
    vector<int> compressed = lzw_compress(pixels);

    save_compressed_data(compressed, compressed_file);

    vector<int> loaded_compressed = load_compressed_data(compressed_file);

    cout << "Decompressing image data..." << endl;
    vector<int> decompressed = lzw_decompress(loaded_compressed);

    save_decompressed_image(decompressed, height, width, channels, decompressed_file);

    if (pixels == decompressed) {
        cout << "Decompression successful!" << endl;
    } else {
        cout << "Decompression failed!" << endl;
    }

    return 0;
}
