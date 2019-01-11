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
  suffix_types[length] = kSStarType;
  suffix_types[length - 1] = kLType;

  for (int i = length - 2; i >= 0; i--) {
    int c1 = text[i];
    int c2 = text[i + 1];

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
  vector<int> result(alphabet_size, 0);

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
    seam_locations[text[i]]++;
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
  if (offset_a < 0) return false;
  int text_len = text.size();
  int i = 0;
  while (true) {
    char char_a = (offset_a + i == text_len) ? '$' : text[offset_a + i];
    char char_b = (offset_b + i == text_len) ? '$' : text[offset_b + i];

    char type_a = typemap[offset_a + i];
    char type_b = typemap[offset_b + i];
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
  int n = text.size();
  while (true) {
    if (first_pos_in_text >= n ||
        second_pos_in_text >= n ||
        text[first_pos_in_text] != text[second_pos_in_text]) {
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
    if (array[i] != -1 && array[i] < min) {
      min = array[i];
    }
  }
  return min;
}

// void EncodeSeam(const vector<int> &bucket_heads,
//                 const vector<int> &bucket_tails, const vector<int> &ls_seam,
//                 vector<char> *typemap) {
//   for (int i = 0, m = bucket_heads.size(); i < m; i++) {
//     int head_index = bucket_heads[i];
//     if (head_index >= (int)(*typemap).size()) {
//       return;
//     }
//     switch ((*typemap)[head_index]) {
//       case 'L':
//         (*typemap)[head_index] = '(';
//         break;
//       case 'S':
//         (*typemap)[head_index] = 's';
//         break;
//       case '*':
//         (*typemap)[head_index] = '{';
//         break;
//       case '-':
//         (*typemap)[head_index] = '[';
//         break;
//     }

//     int tail_index = bucket_tails[i];
//     if (tail_index >= (int)(*typemap).size()) {
//       return;
//     }
//     switch ((*typemap)[tail_index]) {
//       case 'L':
//         (*typemap)[tail_index] = 'l';
//         break;
//       case 'S':
//         (*typemap)[tail_index] = ')';
//         break;
//       case '*':
//         (*typemap)[tail_index] = '}';
//         break;
//       case '-':
//         (*typemap)[tail_index] = ']';
//         break;
//     }

//     int seam_index = ls_seam[i];
//     if (seam_index >= (int)(*typemap).size()) {
//       return;
//     }
//     switch ((*typemap)[seam_index]) {
//       case 'L':
//         (*typemap)[seam_index] = '<';
//         break;
//       case 'S':
//         (*typemap)[seam_index] = '>';
//         break;
//       case '*':
//         (*typemap)[seam_index] = '+';
//         break;
//       case '-':
//         (*typemap)[seam_index] = '|';
//         break;
//     }
//   }
// }

// vector<int> TypeCount(vector<char> &typemap) {
//   vector<int> type_counts(3, 0);

//   for (int i = 0, n = typemap.size(); i < n; i++) {
//     switch (typemap[i]) {
//       case 'L':
//       case '<':
//       case '(':
//       case 'l':
//         type_counts[0]++;
//         break;
//       case 'S':
//       case '>':
//       case ')':
//       case 's':
//         type_counts[1]++;
//         break;
//       case '*':
//       case '+':
//       case '{':
//       case '}':
//         type_counts[2]++;
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
//     int bucket_start = bucket_heads[i];
//     int bucket_end = bucket_tails[i];
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
//     if (copy[i] >= 0 && copy[i - 1] == copy[i]) {
//       PrintVector(copy, "sorted: ", 4);
//       int prev_value = -1;
//       for (int i = 0, n = copy.size(); i < n; i++) {
//         int curr_value = copy[i];
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
