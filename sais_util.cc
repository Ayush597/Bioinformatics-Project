#include "sais_util.h"

#include <algorithm>
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
    int c1 = text.at(i);
    int c2 = text.at(i + 1);

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

vector<int> FindBucketSizes(const vector<int> &text, int alphabet_size) {
  vector<int> result(alphabet_size, 0);

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
  if (offset_a < 0) return false;
  int text_len = text.size();
  int i = 0;
  while (true) {
    char char_a;
    if (offset_a + i == text_len) {
      char_a = '$';
    } else {
      char_a = text.at(offset_a + i);
    }
    char char_b;
    if (offset_b + i == text_len) {
      char_b = '$';
    } else {
      char_b = text.at(offset_b + i);
    }
    char type_a = typemap.at(offset_a + i);
    char type_b = typemap.at(offset_b + i);
    if (char_a != char_b ||
        (type_a == kLType && type_b != kLType) ||
        (type_b == kLType && type_a != kLType)) {
      return false;
    }

    if (i > 0 && (IsLMSChar(offset_a + i, typemap) ||
                  IsLMSChar(offset_b + i, typemap))) {
      return true;
    }
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

// void EncodeSeam(const vector<int> &bucket_heads,
//                 const vector<int> &bucket_tails, const vector<int> &ls_seam,
//                 vector<char> *typemap) {
//   for (int i = 0, m = bucket_heads.size(); i < m; i++) {
//     int head_index = bucket_heads.at(i);
//     if (head_index >= (int)(*typemap).size()) {
//       return;
//     }
//     switch ((*typemap).at(head_index)) {
//       case 'L':
//         (*typemap).at(head_index) = '(';
//         break;
//       case 'S':
//         (*typemap).at(head_index) = 's';
//         break;
//       case '*':
//         (*typemap).at(head_index) = '{';
//         break;
//       case '-':
//         (*typemap).at(head_index) = '[';
//         break;
//     }

//     int tail_index = bucket_tails.at(i);
//     if (tail_index >= (int)(*typemap).size()) {
//       return;
//     }
//     switch ((*typemap).at(tail_index)) {
//       case 'L':
//         (*typemap).at(tail_index) = 'l';
//         break;
//       case 'S':
//         (*typemap).at(tail_index) = ')';
//         break;
//       case '*':
//         (*typemap).at(tail_index) = '}';
//         break;
//       case '-':
//         (*typemap).at(tail_index) = ']';
//         break;
//     }

//     int seam_index = ls_seam.at(i);
//     if (seam_index >= (int)(*typemap).size()) {
//       return;
//     }
//     switch ((*typemap).at(seam_index)) {
//       case 'L':
//         (*typemap).at(seam_index) = '<';
//         break;
//       case 'S':
//         (*typemap).at(seam_index) = '>';
//         break;
//       case '*':
//         (*typemap).at(seam_index) = '+';
//         break;
//       case '-':
//         (*typemap).at(seam_index) = '|';
//         break;
//     }
//   }
// }

// vector<int> TypeCount(vector<char> &typemap) {
//   vector<int> type_counts(3, 0);

//   for (int i = 0, n = typemap.size(); i < n; i++) {
//     switch (typemap.at(i)) {
//       case 'L':
//       case '<':
//       case '(':
//       case 'l':
//         type_counts.at(0)++;
//         break;
//       case 'S':
//       case '>':
//       case ')':
//       case 's':
//         type_counts.at(1)++;
//         break;
//       case '*':
//       case '+':
//       case '{':
//       case '}':
//         type_counts.at(2)++;
//         break;
//     }
//   }

//   return type_counts;
// }

// void PrintPerBucket(const vector<int> &values, const vector<char> &typemap,
//                     const vector<int> &bucket_heads,
//                     const vector<int> &bucket_tails, int cell_size,
//                     int debug_depth) {
//   int n = values.size();
//   int m = bucket_heads.size();
//   for (int i = 0; i < m; i++) {
//     int bucket_start = bucket_heads.at(i);
//     int bucket_end = bucket_tails.at(i);
//     if (bucket_end < bucket_start || bucket_end >= n) {
//       break;
//     }
//     vector<int> SA_subvector(values.begin() + bucket_start,
//                              values.begin() + bucket_end + 1);
//     vector<char> types_subvector(typemap.begin() + bucket_start,
//                                  typemap.begin() + bucket_end + 1);
//     string s = to_string(i);
//     PrintVector(SA_subvector, s, cell_size, debug_depth);
//     PrintVector(types_subvector, "", cell_size, debug_depth);
//   }
// }

// bool CheckUnique(vector<int> vec) {
//   vector<int> copy(vec);
//   std::sort(copy.begin(), copy.end());
//   for (int i = 1, n = copy.size(); i < n; i++) {
//     if (copy.at(i) >= 0 && copy.at(i - 1) == copy.at(i)) {
//       PrintVector(copy, "sorted: ", 4);
//       int prev_value = -1;
//       for (int i = 0, n = copy.size(); i < n; i++) {
//         int curr_value = copy.at(i);
//         if (curr_value == -1) {
//           continue;
//         }
//         if (curr_value != prev_value + 1) {
//           for (int j = prev_value + 1; j < curr_value; j++) {
//             cout << j << endl;
//           }
//         }
//         prev_value = curr_value;
//       }
//       return false;
//     }
//   }
//   return true;
// }