#ifndef SAIS_UTIL_H_
#define SAIS_UTIL_H_

#include <iostream>
#include <string>
#include <vector>

extern const char kLType;
extern const char kSType;
extern const char kSStarType;

template <typename T>
int NumDigits(T number) {
  if (std::is_same<T, char>::value) {
    return 1;
  }
  int digits = 0;
  if (number <= 0) digits = 1;
  while (number) {
    number /= 10;
    digits++;
  }
  return digits;
}

extern bool allow_printing;

template <typename T>
void PrintVector(const std::vector<T> &v, const std::string &name = "",
                 int cell_size = 0, int depth = 0) {
  if (!allow_printing) {
    return;
  }
  for (int i = 0; i < depth; i++) {
    std::cout << "   ";
  }
  std::cout << name;
  int padding = 20 - name.length();
  for (int i = 0; i < padding; i++) {
    std::cout << ' ';
  }
  for (auto i = v.begin(); i != v.end(); ++i) {
    int number_padding = cell_size - NumDigits(*i);
    for (int i = 0; i < number_padding; i++) {
      std::cout << ' ';
    }
    std::cout << *i << "  ";
  }
  std::cout << std::endl;
}

std::vector<char> BuildTypeMap(const std::vector<int> &text);

std::vector<int> TypeCount(std::vector<char> &typemap);

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

int CountSameChars(const std::vector<int> &text, int first_pos_in_text,
                   int second_pos_in_text);

int FindMinInRange(const std::vector<int> &array, int start_index,
                   int end_index);

void EncodeSeam(const std::vector<int> &bucket_heads,
                const std::vector<int> &bucket_tails,
                const std::vector<int> &ls_seam, std::vector<char> *typemap);

void PrintPerBucket(const std::vector<int> &values,
                    const std::vector<char> &typemap,
                    const std::vector<int> &bucket_heads,
                    const std::vector<int> &bucket_tails, int cell_size,
                    int debug_depth);

bool CheckUnique(std::vector<int> vec);

#endif