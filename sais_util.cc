#include "sais_util.h"

#include <climits>
#include <vector>

using namespace std;

const char kLType = 'L';
const char kSType = 'S';
const char kSStarType = '*';

vector<char> BuildTypeMap(const vector<int> &text) {
  int length = text.size();

  vector<char> suffix_types(length + 1);
  suffix_types.at(length) = kSStarType;
  suffix_types.at(length - 1) = kLType;

  for (int i = length - 2; i >= 0; i--) {
    char c1 = text.at(i);
    char c2 = text.at(i + 1);

    if (c1 < c2 || (c1 == c2 && suffix_types.at(i + 1) == kSType)) {
      suffix_types.at(i) = kSType;
    } else {
      suffix_types.at(i) = kLType;

      if (suffix_types.at(i + 1) == kSType) {
        suffix_types.at(i + 1) = kSStarType;
      }
    }
  }

  return suffix_types;
}

vector<int> TypeCount(vector<char> &typemap) {
  vector<int> type_counts(3, 0);
  for (int i = 0, n = typemap.size(); i < n; i++) {
    switch (typemap.at(i)) {
      case 'L':
        type_counts.at(0)++;
        break;
      case 'S':
        type_counts.at(1)++;
        break;
      case '*':
        type_counts.at(2)++;
        break;
      case '<':
        type_counts.at(0)++;
        break;
      case '>':
        type_counts.at(1)++;
        break;
      case '+':
        type_counts.at(2)++;
        break;
    }
  }
  return type_counts;
}

vector<int> FindBucketSizes(const vector<int> &text, int alphabet_size) {
  vector<int> result(alphabet_size);

  for (int c : text) {
    result.at(c)++;
  }

  return result;
}

void FindBucketHeads(const vector<int> &bucket_sizes,
                     vector<int> *bucket_heads) {
  int n = bucket_sizes.size();

  int offset = 1;
  for (int i = 0; i < n; i++) {
    (*bucket_heads).at(i) = offset;
    offset += bucket_sizes.at(i);
  }
}

void FindBucketTails(const vector<int> &bucket_sizes,
                     vector<int> *bucket_tails) {
  int n = bucket_sizes.size();

  int offset = 1;
  for (int i = 0; i < n; i++) {
    offset += bucket_sizes.at(i);
    (*bucket_tails).at(i) = offset - 1;
  }
}

vector<int> FindSeam(const vector<int> &text, const vector<char> &typemap,
                     const vector<int> &bucket_sizes) {
  int m = bucket_sizes.size();
  vector<int> seam_locations(m, 0);

  for (int i = 0, n = text.size(); i < n; i++) {
    if (typemap.at(i) != kLType) continue;
    int c = text.at(i);
    seam_locations.at(c)++;
  }

  int sum_bucket_size = 1;
  for (int i = 0; i < m; i++) {
    seam_locations.at(i) += sum_bucket_size;
    sum_bucket_size += bucket_sizes.at(i);
  }

  return seam_locations;
}

bool IsLMSChar(int offset, const vector<char> &typemap) {
  return typemap.at(offset) == kSStarType;
}

bool LMSSubstringsAreEqual(const std::vector<int> &text,
                           const std::vector<char> &typemap, int offset_a,
                           int offset_b) {
  int len = text.size();
  if (offset_a == len || offset_b == len) return false;

  int i = 0;
  while (true) {
    bool is_a_lms = IsLMSChar(offset_a + i, typemap);
    bool is_b_lms = IsLMSChar(offset_b + i, typemap);

    // if we found our way to the next LMS substring
    // then there's no difference between the original
    // LMS substrings
    if (i > 0 && is_a_lms && is_b_lms) return true;

    if (is_a_lms != is_b_lms) return false;
    if (text.at(offset_a + i) != text.at(offset_b + i)) return false;

    i++;
  }
}

int CountSameChars(const vector<int> &text, int first_pos_in_text,
                   int second_pos_in_text) {
  int num_same_chars = 0;
  while (true) {
    if (first_pos_in_text >= (int)text.size() ||
        second_pos_in_text >= (int)text.size() ||
        text.at(first_pos_in_text) != text.at(second_pos_in_text)) {
      break;
    }
    num_same_chars++;
    first_pos_in_text++;
    second_pos_in_text++;
  }
  return num_same_chars;
}

int FindMinInRange(const vector<int> &array, int start_index, int end_index) {
  int min = INT_MAX;
  for (int i = start_index; i <= end_index; i++) {
    if (array.at(i) != -1 && array.at(i) < min) {
      min = array.at(i);
    }
  }
  return min;
}

void EncodeSeam(const vector<int> &ls_seam, vector<char> *typemap) {
  for (int i = 0, m = ls_seam.size(); i < m; i++) {
    int index = ls_seam.at(i);
    if (index >= (int )(*typemap).size()) {
      return;
    }
    switch ((*typemap).at(index)) {
      case 'L':
        (*typemap).at(index) = '<';
        break;
      case 'S':
        (*typemap).at(index) = '>';
        break;
      case '*':
        (*typemap).at(index) = '+';
        break;
      case '-':
        (*typemap).at(index) = '|';
        break;
    }
  }
}