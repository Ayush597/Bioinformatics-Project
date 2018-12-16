#include "suffix.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>

using namespace std;

string LoadFromFile(const string &filename) {
    ifstream input_stream(filename);

    stringstream ss;
    ss << input_stream.rdbuf();

    return ss.str();
}

int main(int argc, char* argv[]) {
    // Check the number of parameters
    if (argc < 2) {
        // Tell the user how to run the program
        cerr << "Usage: " << argv[0] << " DATA_PATH" << endl;
        /* "Usage messages" are a conventional way of telling the user
         * how to run a program if they enter the command incorrectly.
         */
        return 1;
    }

    string filename = argv[1];
    string text = LoadFromFile(filename);
    text.append("$");

    cout << text << endl;
    int length = text.length();

    // build suffix array
    vector<int> suffix_array = BuildSuffixArray(text);
    for (auto i = suffix_array.begin(); i != suffix_array.end(); ++i) {
        cout << *i << ' ';
    }
    cout << endl;

    // create array of suffix types
    char suffix_types[length];
    suffix_types[length - 1] = '*';
    suffix_types[length - 2] = 'L';

    for (int i = length - 3; i >= 0; i--) {
        char c1 = text[i];
        char c2 = text[i + 1];

        if (c1 < c2 || (c1 == c2 && suffix_types[i+1] == 'S')) {
            suffix_types[i] = 'S';
        } else {
            suffix_types[i] = 'L';

            if (suffix_types[i+1] == 'S') {
                suffix_types[i+1] = '*';
            }
        }
    }

    cout << string(suffix_types).substr(0, length) << endl;

    return 0;
}