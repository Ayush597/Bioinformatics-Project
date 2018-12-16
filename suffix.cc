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

vector<int> guessLMSSort(const vector<int> &text, vector<int> &bucket_sizes, const vector<char> &type_map) {
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


// Structure to store information of a suffix
struct suffix
{
    int index; // To store original index
    int rank[2]; // To store ranks and next rank pair
};

// A comparison function used by sort() to compare two suffixes
// Compares two pairs, returns 1 if first pair is smaller
int cmp(struct suffix a, struct suffix b)
{
    return (a.rank[0] == b.rank[0]) ? (a.rank[1] < b.rank[1] ? 1 : 0):
               (a.rank[0] < b.rank[0] ? 1: 0);
}

// This is the main function that takes a string 'txt' of size n as an
// argument, builds and return the suffix array for the given string
vector<int> BuildSuffixArray(const vector<int> &text) {
    int n = text.size();

    // A structure to store suffixes and their indexes
    struct suffix suffixes[n];

    // Store suffixes and their indexes in an array of structures.
    // The structure is needed to sort the suffixes alphabatically
    // and maintain their old indexes while sorting
    for (int i = 0; i < n; i++) {
        suffixes[i].index = i;
        suffixes[i].rank[0] = text[i] - 'a';
        suffixes[i].rank[1] = ((i+1) < n)? (text[i + 1] - 'a'): -1;
    }

    // Sort the suffixes using the comparison function
    // defined above.
    sort(suffixes, suffixes+n, cmp);

    // At this point, all suffixes are sorted according to first
    // 2 characters.  Let us sort suffixes according to first 4
    // characters, then first 8 and so on
    int ind[n];  // This array is needed to get the index in suffixes[]
                 // from original index.  This mapping is needed to get
                 // next suffix.
    for (int k = 4; k < 2*n; k = k*2) {
        // Assigning rank and index values to first suffix
        int rank = 0;
        int prev_rank = suffixes[0].rank[0];
        suffixes[0].rank[0] = rank;
        ind[suffixes[0].index] = 0;

        // Assigning rank to suffixes
        for (int i = 1; i < n; i++) {
            // If first rank and next ranks are same as that of previous
            // suffix in array, assign the same new rank to this suffix
            if (suffixes[i].rank[0] == prev_rank &&
                    suffixes[i].rank[1] == suffixes[i-1].rank[1]) {
                prev_rank = suffixes[i].rank[0];
                suffixes[i].rank[0] = rank;
            } else {
                // Otherwise increment rank and assign
                prev_rank = suffixes[i].rank[0];
                suffixes[i].rank[0] = ++rank;
            }
            ind[suffixes[i].index] = i;
        }

        // Assign next rank to every suffix
        for (int i = 0; i < n; i++) {
            int nextindex = suffixes[i].index + k/2;
            suffixes[i].rank[1] = (nextindex < n)?
                                  suffixes[ind[nextindex]].rank[0]: -1;
        }

        // Sort the suffixes according to first k characters
        sort(suffixes, suffixes+n, cmp);
    }

    // Store indexes of all sorted suffixes in the suffix array
    vector<int> suffix_arr(n);
    for (int i = 0; i < n; i++) {
        suffix_arr[i] = suffixes[i].index;
    }

    // Return the suffix array
    return  suffix_arr;
}