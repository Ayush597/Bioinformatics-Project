#ifndef SAIS_UTIL_H_
#define SAIS_UTIL_H_

#include <vector>

extern const char kLType;
extern const char kSType;
extern const char kSStarType;

std::vector<char> BuildTypeMap(const std::vector<int> &text);

std::vector<int> FindBucketSizes(const std::vector<int> &text,
                                 int alphabet_size);

void FindBucketHeads(const std::vector<int> &bucket_sizes,
                     std::vector<int> *bucket_heads);
void FindBucketTails(const std::vector<int> &bucket_sizes,
                     std::vector<int> *bucket_tails);

std::vector<int> FindSeam(const std::vector<int> &text,
                          const std::vector<char> &typemap,
                          const std::vector<int> &bucket_sizes);

bool IsLMSChar(int offset, const std::vector<char> &typemap);

bool LMSSubstringsAreEqual(const std::vector<int> &text,
                           const std::vector<char> &typemap, int offset_a,
                           int offset_b);

int count_same_chars(const std::vector<int> &text, int first_pos_in_text,
                     int second_pos_in_text);

int FindMinInRange(const std::vector<int> array, int start_index,
                   int end_index);

#endif