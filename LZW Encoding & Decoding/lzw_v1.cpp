#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;
using CodeType = uint16_t;
const CodeType dms = numeric_limits<CodeType>::max();

vector<char> operator+(vector<char> vc, char c) {
    vc.push_back(c);
    return vc;
}

enum class Mode {
    Compress,
    Decompress
};

void print_usage(const string &s="", bool su=true) {
    if(!s.empty()) cout << "\nERROR: " << s << "\n";
    if(su) {
        cout << "\nUsage:\n";
        cout << "\tprogram -flag input_file output_file\n\n";
        cout << "Where `flag' is either `c' for compressing, or `d' for decompressing, and\n";
        cout << "`input_file' and `output_file' are distinct files.\n\n";
        cout << "Examples:\n";
        cout << "\tlzw_v1.exe -c license.txt license.lzw\n";
        cout << "\tlzw_v1.exe -d license.lzw new_license.txt\n";
    }
    cout << "\n";
}

void compress(istream &is, ostream &os) {
    map<vector<char>, CodeType> dictionary;

    auto reset_dictionary = [&dictionary] {
        dictionary.clear();

        const long int minc = numeric_limits<char>::min();
        const long int maxc = numeric_limits<char>::max();

        for(long int c=minc; c<=maxc; c++) {
            const CodeType dictionary_size = dictionary.size();
            dictionary[{static_cast<char> (c)}] = dictionary_size;
        }
    };

    reset_dictionary();

    vector<char> s;
    char c;
    while(is.get(c)) {
        if(dictionary.size()==dms)
            reset_dictionary();
        
        s.push_back(c);
        if(dictionary.count(s)==0) {
            const CodeType dictionary_size = dictionary.size();

            dictionary[s] = dictionary_size;
            s.pop_back();
            os.write(reinterpret_cast<const char*>(&dictionary.at(s)), sizeof(CodeType));
            s = {c};
        }
    }

    if(!s.empty()) 
        os.write(reinterpret_cast<const char*>(&dictionary.at(s)), sizeof(CodeType));

}

void decompress(istream &is, ostream &os) {
    vector<vector<char>> dictionary;

    auto reset_dictionary = [&dictionary] {
        dictionary.clear();
        dictionary.reserve(dms);

        const long int minc = numeric_limits<char>::min();
        const long int maxc = numeric_limits<char>::max();

        for(long int c=minc; c<=maxc; c++)
            dictionary.push_back({static_cast<char>(c)});
    };

    reset_dictionary();

    vector<char> s;
    CodeType k; // key
    
    while(is.read(reinterpret_cast<char *>(&k), sizeof(CodeType))) {
        if(dictionary.size()==dms)
            reset_dictionary();
        if(k>dictionary.size()) {
            cout << "invalid compressed code"; return;
        }
        if(k==dictionary.size()) 
            dictionary.push_back(s + s.front());
        else if(!s.empty()) 
            dictionary.push_back(s + dictionary.at(k).front());
        
        os.write(&dictionary.at(k).front(), dictionary.at(k).size());
        s = dictionary.at(k);
    }

    if (!is.eof() || is.gcount() != 0)
        cout << "corrupted compressed file";
}

int main(int argc, char *argv[]) {
    if(argc != 4) {
        print_usage("Wrong number of arguments.");
        return EXIT_FAILURE;
    }

    Mode m;
    if(string(argv[1]) == "-c") 
        m = Mode::Compress;
    else if(string(argv[1]) == "-d") 
        m = Mode::Decompress;
    else {
        print_usage(std::string("flag `") + argv[1] + "' is not recognized.");
        return EXIT_FAILURE;
    }

    ifstream input_file(argv[2], ios_base::binary);
    if(!input_file.is_open()) {
        print_usage(std::string("input_file `") + argv[2] + "' could not be opened.");
        return EXIT_FAILURE;
    }

    ofstream output_file(argv[3], ios_base::binary);
    if(!output_file.is_open()) {
        print_usage(std::string("output_file`") + argv[3] + "' could not be opened.");
        return EXIT_FAILURE;
    }

    if(m == Mode::Compress)
        compress(input_file, output_file);
    else if(m == Mode::Decompress)
        decompress(input_file, output_file);

    return EXIT_SUCCESS;
}