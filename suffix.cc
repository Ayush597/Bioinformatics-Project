#include "suffix.h"

#include <algorithm>

using namespace std;

const char kLType = 'L';
const char kSType = 'S';
const char kSStarType = '*';

vector<char> BuildTypeMap(const vector<int> &text) {
    int length = text.size();

    vector<char> suffix_types(length + 1);
    suffix_types[length] = kSStarType;
    suffix_types[length - 1] = kLType;

    for (int i = length - 2; i >= 0; i--) {
        char c1 = text[i];
        char c2 = text[i + 1];

        if (c1 < c2 || (c1 == c2 && suffix_types[i+1] == kSType)) {
            suffix_types[i] = kSType;
        } else {
            suffix_types[i] = kLType;

            if (suffix_types[i+1] == kSType) {
                suffix_types[i+1] = kSStarType;
            }
        }
    }

    return suffix_types;
}

vector<int> FindBucketSizes(const vector<int> &text, int alphabet_size) {
    vector<int> result(alphabet_size);

    for (int c : text) {
        result[c]++;
    }

    return result;
}

vector<int> FindBucketHeads(const vector<int> &bucket_sizes) {
    int n = bucket_sizes.size();
    vector<int> tails(n);

    int offset = 1;
    for (int i = 0; i < n; i++) {
        tails[i] = offset - 1;
        offset += bucket_sizes[i];
    }

    return tails;
}

vector<int> FindBucketTails(const vector<int> &bucket_sizes) {
    int n = bucket_sizes.size();
    vector<int> tails(n);

    int offset = 1;
    for (int i = 0; i < n; i++) {
        offset += bucket_sizes[i];
        tails[i] = offset - 1;
    }

    return tails;
}

vector<int> GuessLMSSort(const vector<int> &text, vector<int> &bucket_sizes, const vector<char> &type_map) {
    vector<int> guess(text.size() + 1, -1);
    vector<int> bucket_tails = FindBucketTails(bucket_sizes);

    int n = text.size();
    for (int i = 0; i < n; i++) {
        // not the start of an LMS suffix
        if (type_map[i] != kSStarType) continue;

        int bucket_index = text[i];
        guess[bucket_tails[bucket_index]] = i;
        bucket_tails[bucket_index] -= 1;
    }

    guess[0] = n;
    return guess;
}

void InduceSortL(const vector<int> &text, const vector<int> &bucket_sizes,
                 const vector<char> &typemap, vector<int>* guessed_suffix_array) {

    vector<int> bucket_heads = FindBucketHeads(bucket_sizes);

    int n = guessed_suffix_array->size();
    for (int i = 0; i < n; i++) {
        if ((*guessed_suffix_array)[i] == -1) continue;

        int j = (*guessed_suffix_array)[i] - 1;
        if (j < 0) continue;
        if (typemap[j] != kLType) continue;

        int bucket_index = text[j];
        (*guessed_suffix_array)[bucket_heads[bucket_index] + 1] = j;
        bucket_heads[bucket_index] += 1;
    }
}

void InduceSortS(const vector<int> &text, const vector<int> &bucket_sizes,
                 const vector<char> &typemap, vector<int>* guessed_suffix_array) {

    vector<int> bucket_tails = FindBucketTails(bucket_sizes);

    int n = guessed_suffix_array->size();
    for (int i = n - 1; i >= 0; i--) {
        int j = (*guessed_suffix_array)[i] - 1;

        if (j < 0) continue;
        if (typemap[j] != kSType && typemap[j] != kSStarType) continue;

        int bucket_index = text[j];
        (*guessed_suffix_array)[bucket_tails[bucket_index]] = j;
        bucket_tails[bucket_index] -= 1;
    }
}

vector<int> BuildSuffixArray(const vector<int> &text, int alphabet_size) {
    vector<char> suffix_types = BuildTypeMap(text);
    // cout << string(suffix_types.data()) << endl;

    vector<int> bucket_sizes = FindBucketSizes(text, alphabet_size);
    // PrintVector(bucket_sizes, "Bucket sizes: ");

    vector<int> guessed_suffix_array = GuessLMSSort(text, bucket_sizes, suffix_types);
    // PrintVector(guessed_suffix_array, "Guessed suffix array: ");

    InduceSortL(text, bucket_sizes, suffix_types, &guessed_suffix_array);
    // PrintVector(guessed_suffix_array, "After L induced sorting: ");

    InduceSortS(text, bucket_sizes, suffix_types, &guessed_suffix_array);
    // PrintVector(guessed_suffix_array, "After S induced sorting: ");

    return guessed_suffix_array;
}