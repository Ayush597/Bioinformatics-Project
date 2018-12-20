#include <iostream>
#include <string>
#include <vector>

std::vector<int> BuildSuffixArray(const std::vector<int> &text, int alphabet_size);

// The declarations below are temporarily here for debugging purposes.
// They should be removed after the BuildSuffixArray function is implemented.

std::vector<char> BuildTypeMap(const std::vector<int> &text);
std::vector<int> FindBucketSizes(const std::vector<int> &text, int alphabet_size);
std::vector<int> FindBucketTails(const std::vector<int> &bucket_sizes);

std::vector<int> GuessLMSSort(const std::vector<int> &text, std::vector<int> &bucket_sizes,
                              const std::vector<char> &type_map);
void InduceSortL(const std::vector<int> &text, const std::vector<int> &bucket_sizes,
                 const std::vector<char> &typemap, std::vector<int>* guessed_suffix_array);
void InduceSortS(const std::vector<int> &text, const std::vector<int> &guessed_suffix_array,
                 const std::vector<char> &typemap, std::vector<int>* bucket_sizes);

template<typename T>
void PrintVector(const std::vector<T> &v, const std::string& name="") {
    std::cout << name;
    for (auto i = v.begin(); i != v.end(); ++i) {
        std::cout << *i << ' ';
    }
    std::cout << std::endl;
}