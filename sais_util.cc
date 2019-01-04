#include "sais_util.h"

#include <vector>

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

    if (c1 < c2 || (c1 == c2 && suffix_types[i + 1] == kSType)) {
      suffix_types[i] = kSType;
    } else {
      suffix_types[i] = kLType;

      if (suffix_types[i + 1] == kSType) {
        suffix_types[i + 1] = kSStarType;
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
    tails[i] = offset;
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

bool IsLMSChar(int offset, const vector<char> &typemap) {
  return typemap[offset] == kSStarType;
}

bool LMSSubstringsAreEqual(const std::vector<int> &text,
                           const std::vector<char> &typemap, int offset_a,
                           int offset_b) {
  int len = text.size();
  if (offset_a == len || offset_b == len) return false;

  int i = 0;
  while (true) {
    bool is_a_lms = IsLMSChar(i + offset_a, typemap);
    bool is_b_lms = IsLMSChar(i + offset_b, typemap);

    // if we found our way to the next LMS substring
    // then there's no difference between the original
    // LMS substrings
    if (i > 0 && is_a_lms && is_b_lms) return true;

    if (is_a_lms != is_b_lms) return false;
    if (text[i + offset_a] != text[i + offset_b]) return false;

    i++;
  }
}

int count_same_chars(const vector<int> &text, int first_pos_in_text,
                     int second_pos_in_text) {
  int num_same_chars = 0;
  while (true) {
    if (text[first_pos_in_text] == text[second_pos_in_text]) {
      num_same_chars++;
    } else {
      break;
    }
    first_pos_in_text++;
    second_pos_in_text++;
  }
  return num_same_chars;
}