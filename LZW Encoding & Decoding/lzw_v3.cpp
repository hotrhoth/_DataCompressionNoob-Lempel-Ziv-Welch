#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <limits>
#include <cstdint>
#include <algorithm>

using namespace std;
using CodeType = uint16_t;
const CodeType dms = numeric_limits<CodeType>::max();

enum class Mode {
    Compress,
    Decompress
};

void compress(istream &is, ostream &os) {
    map<pair<CodeType, char>, CodeType> dictionary;
    auto reset_dictionary = [&dictionary] {
        dictionary.clear();

        const long int minc = numeric_limits<char>::min();
        const long int maxc = numeric_limits<char>::max();

        for(long int c=minc; c<=maxc; c++) {
            const CodeType dictionary_size = dictionary.size();
            dictionary[{dms, static_cast<char> (c)}] = dictionary_size;
        }
    };

    reset_dictionary();

    CodeType i {dms}; // Index
    char c;
    while(is.get(c)) {
        if(dictionary.size()==dms)
            reset_dictionary();
        
        if(dictionary.count({i, c}) == 0) {
            const CodeType dictionary_size = dictionary.size();
            dictionary[{i, c}] = dictionary_size;
            os.write(reinterpret_cast<const char *> (&i), sizeof (CodeType));
            i = dictionary.at({dms, c});
        }
        else i = dictionary.at({i, c});
    }

    if(i != dms)
        os.write(reinterpret_cast<const char *> (&i), sizeof (CodeType));
}

void decompress(istream &is, ostream &os) {
    vector<std::pair<CodeType, char>> dictionary;

    auto reset_dictionary = [&dictionary] {
        dictionary.clear();
        dictionary.reserve(dms);

        const long int minc = std::numeric_limits<char>::min();
        const long int maxc = std::numeric_limits<char>::max();

        for (long int c = minc; c <= maxc; ++c)
            dictionary.push_back({dms, static_cast<char> (c)});
    };

    auto rebuild_string = [&dictionary](CodeType k) -> std::vector<char> {
        std::vector<char> s; // String

        while (k != dms)
        {
            s.push_back(dictionary.at(k).second);
            k = dictionary.at(k).first;
        }

        std::reverse(s.begin(), s.end());
        return s;
    };

    reset_dictionary();

    CodeType i {dms}; // Index
    CodeType k; // Key

    while (is.read(reinterpret_cast<char *> (&k), sizeof (CodeType)))
    {
        // dictionary's maximum size was reached
        if (dictionary.size() == dms)
            reset_dictionary();

        if (k > dictionary.size()) {
            cout << "invalid compressed code"; return;
        }

        std::vector<char> s; // String

        if (k == dictionary.size())
        {
            dictionary.push_back({i, rebuild_string(i).front()});
            s = rebuild_string(k);
        }
        else
        {
            s = rebuild_string(k);

            if (i != dms)
                dictionary.push_back({i, s.front()});
        }

        os.write(&s.front(), s.size());
        i = k;
    }

    if (!is.eof() || is.gcount() != 0)
        cout << "corrupted compressed file";
}

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