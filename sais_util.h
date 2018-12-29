#ifndef SAIS_UTIL_H_
#define SAIS_UTIL_H_

#include <vector>

extern const char kLType;
extern const char kSType;
extern const char kSStarType;

std::vector<char> BuildTypeMap(const std::vector<int> &text);

std::vector<int> FindBucketSizes(const std::vector<int> &text,
                                 int alphabet_size);

std::vector<int> FindBucketHeads(const std::vector<int> &bucket_sizes);

std::vector<int> FindBucketTails(const std::vector<int> &bucket_sizes);

bool IsLMSChar(int offset, const std::vector<char> &typemap);

bool LMSSubstringsAreEqual(const std::vector<int> &text,
                           const std::vector<char> &typemap, int offset_a,
                           int offset_b);

#endif