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

vector<int> EncodeAlphabetically(const string& text) {
    int n = text.length();
    vector<int> encoded(n);

    for (int i = 0; i < n; i++) {
        encoded[i] = text[i] - 'a';
    }

    return encoded;
}

template<typename T>
void PrintVector(const vector<T> &v) {
    for (auto i = v.begin(); i != v.end(); ++i) {
        cout << *i << ' ';
    }
    cout << endl;
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
    cout << text << endl;

    vector<int> encoded = EncodeAlphabetically(text);
    PrintVector(encoded);

    // create array of suffix types
    vector<char> suffix_types = BuildTypeMap(encoded);
    cout << string(suffix_types.data()) << endl;

    vector<int> bucket_sizes = FindBucketSizes(encoded, 3);
    PrintVector(bucket_sizes);

    PrintVector(FindBucketTails(bucket_sizes));

    vector<int> guessed_suffix_array = guessLMSSort(encoded, bucket_sizes, suffix_types);
    PrintVector(guessed_suffix_array);

    // build suffix array
    // text.append("$");
    // vector<int> suffix_array = BuildSuffixArray(encoded);
    // PrintVector(suffix_array);

    // return 0;
}