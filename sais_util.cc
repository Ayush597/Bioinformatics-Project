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

void FindBucketHeads(const vector<int> &bucket_sizes,
                     vector<int> *bucket_heads) {
  int n = bucket_sizes.size();

  int offset = 1;
  for (int i = 0; i < n; i++) {
    (*bucket_heads)[i] = offset;
    offset += bucket_sizes[i];
  }
}

void FindBucketTails(const vector<int> &bucket_sizes,
                     vector<int> *bucket_tails) {
  int n = bucket_sizes.size();

  int offset = 1;
  for (int i = 0; i < n; i++) {
    offset += bucket_sizes[i];
    (*bucket_tails)[i] = offset - 1;
  }
}

vector<int> FindSeam(const vector<int> &text, const vector<char> &typemap,
                     const vector<int> &bucket_sizes) {
  int m = bucket_sizes.size();
  vector<int> seam_locations(m, 0);

  for (int i = 0, n = text.size(); i < n; i++) {
    if (typemap[i] != kLType) continue;
    int c = text[i];
    seam_locations[c]++;
  }

  int sum_bucket_size = 1;
  for (int i = 0; i < m; i++) {
    seam_locations[i] += sum_bucket_size;
    sum_bucket_size += bucket_sizes[i];
  }

  return seam_locations;
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
    bool is_a_lms = IsLMSChar(offset_a + i, typemap);
    bool is_b_lms = IsLMSChar(offset_b + i, typemap);

    // if we found our way to the next LMS substring
    // then there's no difference between the original
    // LMS substrings
    if (i > 0 && is_a_lms && is_b_lms) return true;

    if (is_a_lms != is_b_lms) return false;
    if (text[offset_a + i] != text[offset_b + i]) return false;

    i++;
  }
}

int CountSameChars(const vector<int> &text, int first_pos_in_text,
                     int second_pos_in_text) {
  if ((((unsigned int)first_pos_in_text) >= text.size()) ||
      (((unsigned int)second_pos_in_text) >= text.size())) {
    return 0;
  }

  int num_same_chars = 0;
  while (true) {
    if (text[first_pos_in_text] != text[second_pos_in_text]) {
      break;
    }
    num_same_chars++;
    first_pos_in_text++;
    second_pos_in_text++;
  }
  return num_same_chars;
}

int FindMinInRange(const vector<int> array, int start_index, int end_index) {
  int min = INT_MAX;
  for (int i = start_index; i <= end_index; i++) {
    if (array[i] != -1 && array[i] < min) {
      min = array[i];
    }
  }
  return min;
}