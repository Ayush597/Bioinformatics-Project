#include <string>
#include <vector>

std::vector<int> BuildSuffixArray(const std::vector<int> &text);

// The declarations below are temporarily here for debugging purposes.
// They should be removed after the BuildSuffixArray function is implemented.

std::vector<char> BuildTypeMap(const std::vector<int> &text);
std::vector<int> FindBucketSizes(const std::vector<int> &text, int alphabet_size=256);
std::vector<int> FindBucketTails(const std::vector<int> &bucket_sizes);

std::vector<int> guessLMSSort(const std::vector<int> &text, std::vector<int> &bucket_sizes, const std::vector<char> &type_map);