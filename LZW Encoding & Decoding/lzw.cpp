#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string>
#include <cstdint>
#include <exception>

using CodeType = uint16_t;

namespace globals {
    const CodeType dms {std::numeric_limits<CodeType>::max()};
}

extern "C" {

void compress(const char *input_file, const char *output_file) {
    std::ifstream is(input_file, std::ios::binary);
    std::ofstream os(output_file, std::ios::binary);

    std::map<std::pair<CodeType, char>, CodeType> dictionary;
    const auto reset_dictionary = [&dictionary] {
        dictionary.clear();
        for (long int c = std::numeric_limits<char>::min(); c <= std::numeric_limits<char>::max(); ++c) {
            dictionary[{globals::dms, static_cast<char>(c)}] = dictionary.size();
        }
    };

    reset_dictionary();

    CodeType i {globals::dms};
    char c;

    while (is.get(c)) {
        if (dictionary.size() == globals::dms) {
            reset_dictionary();
        }

        if (dictionary.count({i, c}) == 0) {
            os.write(reinterpret_cast<const char *>(&i), sizeof(CodeType));
            dictionary[{i, c}] = dictionary.size();
            i = dictionary.at({globals::dms, c});
        } else {
            i = dictionary.at({i, c});
        }
    }

    if (i != globals::dms) {
        os.write(reinterpret_cast<const char *>(&i), sizeof(CodeType));
    }
}

void decompress(const char *input_file, const char *output_file) {
    std::ifstream is(input_file, std::ios::binary);
    std::ofstream os(output_file, std::ios::binary);

    std::vector<std::pair<CodeType, char>> dictionary;

    const auto reset_dictionary = [&dictionary] {
        dictionary.clear();
        for (long int c = std::numeric_limits<char>::min(); c <= std::numeric_limits<char>::max(); ++c) {
            dictionary.push_back({globals::dms, static_cast<char>(c)});
        }
    };

    const auto rebuild_string = [&dictionary](CodeType k) -> std::vector<char> {
        std::vector<char> s;
        while (k != globals::dms) {
            s.push_back(dictionary.at(k).second);
            k = dictionary.at(k).first;
        }
        std::reverse(s.begin(), s.end());
        return s;
    };

    reset_dictionary();

    CodeType i {globals::dms};
    CodeType k;

    while (is.read(reinterpret_cast<char*>(&k), sizeof(CodeType))) {
        if (dictionary.size() == globals::dms) {
            reset_dictionary();
        }

        if (k > dictionary.size()) {
            throw std::runtime_error("Invalid compressed code");
        }

        std::vector<char> s;
        if (k == dictionary.size()) {
            dictionary.push_back({i, rebuild_string(i).front()});
            s = rebuild_string(k);
        } else {
            s = rebuild_string(k);
            if (i != globals::dms) {
                dictionary.push_back({i, s.front()});
            }
        }
        os.write(&s.front(), s.size());
        i = k;
    }
}
}
