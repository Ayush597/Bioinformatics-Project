#include "sais_lcp.h"

#include <assert.h>
#include <algorithm>

#include "sa_to_lcp.h"
#include "sais_util.h"

using namespace std;

// void GuessLMSSort(const vector<int> &text, const vector<char> &typemap,
//                   int cell_size, int debug_depth, vector<int> *bucket_tails,
//                   vector<int> *suffix_array, vector<char> *debug_type_array,
//                   vector<int> *debug_input_array) {
void GuessLMSSort(const vector<int> &text, const vector<char> &typemap,
                  vector<int> *bucket_tails, vector<int> *suffix_array) {
  int n = text.size();
  for (int i = 0; i < n; i++) {
    // not the start of an LMS suffix
    if (!(IsLMSChar(i, typemap))) continue;

    int c = text[i];
    int k = (*bucket_tails)[c];
    (*suffix_array)[k] = i;
    // (*debug_input_array)[k] = text.at((*suffix_array)[k]);
    // (*debug_type_array)[k] = typemap[i];
    (*bucket_tails)[c] -= 1;

    // if (debug_depth == 1) {
    //   cout << i << " at " << k << endl;
    //   PrintVector((*suffix_array), "", cell_size, debug_depth);
    // }
  }

  (*suffix_array)[0] = n;
  // (*debug_input_array)[0] = -1;
  // (*debug_type_array)[0] = typemap[n];

  // assert(CheckUnique((*suffix_array)));
}

// void InduceSortL(const vector<int> &text, const vector<char> &typemap,
//                  int cell_size, int debug_depth, const vector<int> &ls_seam,
//                  vector<int> *bucket_heads, vector<int> *suffix_array,
//                  vector<char> *debug_type_array, vector<int>
//                  *debug_input_array, vector<int> *lcp_array = NULL,
//                  vector<int> *first_lms = NULL) {
void InduceSortL(const vector<int> &text, const vector<char> &typemap,
                 const vector<int> &ls_seam, vector<int> *bucket_heads,
                 vector<int> *suffix_array, vector<int> *lcp_array = NULL,
                 vector<int> *first_lms = NULL) {
  int n = suffix_array->size();
  int m = (*bucket_heads).size();

  vector<int> iteration(n);
  vector<int> is_first_in_bucket(n, 0);

  vector<int> last_l_suffix(m, -1);

  for (int i = 0; i < m; i++) {
    if ((*bucket_heads)[i] >= n) {
      break;
    }
    is_first_in_bucket[(*bucket_heads)[i]] = 1;
  }

  for (int i = 0; i < n; i++) {
    if ((*suffix_array)[i] == -1) continue;

    int j = (*suffix_array)[i] - 1;
    if (j < 0) continue;
    if (typemap[j] != kLType) continue;

    int c = text[j];

    int k = (*bucket_heads)[c];
    // assert((*suffix_array)[k] == -1);
    (*suffix_array)[k] = j;
    // (*debug_input_array)[k] = text.at((*suffix_array)[k]);
    // (*debug_type_array)[k] = typemap[j];
    last_l_suffix[c] = k;
    (*bucket_heads)[c] += 1;

    if (lcp_array == NULL) {
      continue;
    }

    // PrintVector((*suffix_array), "SA: ");
    // PrintVector((*lcp_array), "LCP: ");

    if (k + 1 == ls_seam[c]) {
      int pos_of_first_lms_suffix = (*first_lms)[c];
      if (pos_of_first_lms_suffix != -1) {
        int ls_seam_same_chars =
            CountSameChars(text, (*suffix_array)[k],
                           (*suffix_array)[pos_of_first_lms_suffix]);
        (*lcp_array)[pos_of_first_lms_suffix] = ls_seam_same_chars;
      }
    }

    iteration[k] = i;

    if (is_first_in_bucket[k]) {
      (*lcp_array)[k] = 0;
    }

    if (is_first_in_bucket[k]) {
      continue;
    }

    int prev_iter = iteration[k - 1];

    if ((*suffix_array)[prev_iter] < ((int)text.size()) &&
        text[(*suffix_array)[i]] !=
            text[(*suffix_array)[prev_iter]]) {
      (*lcp_array)[k] = 1;
      continue;
    }

    (*lcp_array)[k] = FindMinInRange(*lcp_array, prev_iter + 1, i) + 1;
  }

  // for (int i = 0; i < m; i++) {
  //   assert((*bucket_heads)[i] == ls_seam[i]);
  // }

  // assert(CheckUnique((*suffix_array)));
}

// void InduceSortS(const vector<int> &text, const vector<char> &typemap,
//                  int cell_size, int debug_depth, const vector<int> &ls_seam,
//                  vector<int> *bucket_tails, vector<int> *suffix_array,
//                  vector<char> *debug_type_array, vector<int>
//                  *debug_input_array, vector<int> *lcp_array = NULL) {
void InduceSortS(const vector<int> &text, const vector<char> &typemap,
                 const vector<int> &ls_seam, vector<int> *bucket_tails,
                 vector<int> *suffix_array, vector<int> *lcp_array = NULL) {
  int n = suffix_array->size();
  int m = (*bucket_tails).size();

  vector<int> iteration(n);
  vector<int> is_first_in_bucket(n, 0);

  for (int i = 0; i < m; i++) {
    if ((*bucket_tails)[i] >= n) {
      break;
    }
    is_first_in_bucket[(*bucket_tails)[i]] = 1;
  }

  for (int i = n - 1; i >= 0; i--) {
    int j = (*suffix_array)[i] - 1;

    if (j < 0) continue;
    if (typemap[j] != kSType && typemap[j] != kSStarType) continue;

    int c = text[j];
    int k = (*bucket_tails)[c];
    (*suffix_array)[k] = j;
    // (*debug_input_array)[k] = text.at((*suffix_array)[k]);
    // (*debug_type_array)[k] = typemap[j];
    (*bucket_tails)[c] -= 1;

    if (lcp_array == NULL) {
      continue;
    }

    iteration[k] = i;

    if (is_first_in_bucket[k]) {
      (*lcp_array)[k] = 0;
    }

    if (k > 0 && k == ls_seam[c]) {
      int first_pos = (*suffix_array)[k - 1];
      if (first_pos < 0) {
        (*lcp_array)[k] = 0;
      } else {
        int second_pos = (*suffix_array)[k];
        int ls_seam_same_chars = CountSameChars(text, first_pos, second_pos);
        (*lcp_array)[k] = ls_seam_same_chars;
      }
    }

    if (is_first_in_bucket[k]) {
      continue;
    }

    int prev_iter = iteration[k + 1];

    if ((*suffix_array)[prev_iter] < ((int)text.size()) &&
        text[(*suffix_array)[i]] !=
            text[(*suffix_array)[prev_iter]]) {
      (*lcp_array)[k + 1] = 1;
      continue;
    }

    (*lcp_array)[k + 1] = FindMinInRange(*lcp_array, i + 1, prev_iter) + 1;
  }
  // assert(CheckUnique((*suffix_array)));
}

int SummarizeSuffixArray(const std::vector<int> &text,
                         const std::vector<int> &suffix_array,
                         const std::vector<char> &typemap,
                         std::vector<int> *summary_string,
                         std::vector<int> *summary_suffix_offsets) {
  // this array will store the names of LMS substrings in the positions
  // that they appear in the original string
  vector<int> lms_names(text.size() + 1, -1);

  int current_name = -1;
  int last_lms_suffix_offset = -1;

  // the first LMS substring is always the empty string
  // lms_names[suffix_array[0]] = current_name;

  for (int i = 0, n = suffix_array.size(); i < n; i++) {
    // where does this suffix appear in the original string?
    int suffix_offset = suffix_array[i];

    if (!(IsLMSChar(suffix_offset, typemap))) continue;

    // if this LMS suffix starts with a different LMS substring from the last
    // one, then it gets a new name
    if (!LMSSubstringsAreEqual(text, typemap, last_lms_suffix_offset,
                               suffix_offset)) {
      current_name++;
    }

    // record the last LMS suffix we looked at
    last_lms_suffix_offset = suffix_offset;

    // store the name of this suffix in lms_names, in the same place
    // the suffix occurs in the original string
    lms_names[suffix_offset] = current_name;
  }

  for (int i = 0, n = lms_names.size() - 1; i < n; i++) {
    if (lms_names[i] == -1) continue;

    summary_suffix_offsets->push_back(i);
    summary_string->push_back(lms_names[i]);
  }

  int summary_alphabet_size = current_name + 1;
  return summary_alphabet_size;
}

void MakeSummarySuffixArray(const vector<int> &summary_string,
                            int summary_alphabet_size,
                            vector<int> *suffix_array, vector<int> *lcp_array) {
  int summary_len = summary_string.size();
  if (summary_alphabet_size == summary_len + 1) {
    // assert(CheckUnique(summary_string));

    (*suffix_array)[0] = summary_len;

    for (int x = 0; x < summary_len; x++) {
      int y = summary_string[x];
      (*suffix_array)[y] = x;
    }

    // for (int i = 0, n = (*suffix_array).size(); i < n; i++) {
    //   if ((*suffix_array)[i] < 0) {
    //     cout << "wadafak" << endl;
    //   }
    // }

    if (lcp_array != NULL) {
      SuffixArrayToLCP(summary_string, *suffix_array, lcp_array);
    }
  } else {
    BuildSuffixArray(summary_string, summary_alphabet_size, suffix_array,
                     lcp_array);
  }
}

vector<int> ComputeLCPOfLMS(const vector<int> &text,
                            const vector<int> &summary_suffix_offsets,
                            const vector<int> &summary_suffix_array,
                            const vector<int> &summary_lcp_array) {
  vector<int> lms_lcp_values(summary_lcp_array.size(), 0);
  int n = summary_suffix_offsets.size();
  if (n == 0) return lms_lcp_values;
  int sum = 0;
  vector<int> cumulative_lms_lengths(n, -1);
  cumulative_lms_lengths[0] = 0;
  for (int i = 1; i < n; i++) {
    sum += summary_suffix_offsets[i] - summary_suffix_offsets[i - 1];
    cumulative_lms_lengths[i] = sum;
  }

  for (int k = 2, n = summary_lcp_array.size(); k < n; k++) {
    int j = summary_suffix_array[k];
    int common_lsm_sum =
        cumulative_lms_lengths[j + summary_lcp_array[k]] -
        cumulative_lms_lengths[j];

    int first_pos_in_text =
        summary_suffix_offsets[summary_suffix_array[k - 1]] +
        common_lsm_sum;
    int second_pos_in_text = summary_suffix_offsets[j] + common_lsm_sum;

    int num_same_chars_after_lsm =
        CountSameChars(text, first_pos_in_text, second_pos_in_text);

    lms_lcp_values[k] = common_lsm_sum + num_same_chars_after_lsm;
  }

  return lms_lcp_values;
}

void AccurateLMSSort(const vector<int> &text, const vector<char> &typemap,
                     const vector<int> &summary_suffix_array,
                     const vector<int> &summary_lcp_array,
                     const vector<int> &summary_suffix_offsets,
                     const vector<int> &lms_lcp_values,
                     vector<int> *bucket_tails, vector<int> *suffix_array,
                     vector<int> *lcp_array, vector<int> *first_lms) {
  for (int i = summary_suffix_array.size() - 1; i > 0; i--) {
    int string_index = summary_suffix_offsets[summary_suffix_array[i]];

    int c = text[string_index];
    (*suffix_array)[(*bucket_tails)[c]] = string_index;
    (*first_lms)[c] = (*bucket_tails)[c];
    if (lcp_array != NULL) {
      (*lcp_array)[(*bucket_tails)[c]] = lms_lcp_values[i];
    }
    (*bucket_tails)[c] -= 1;
  }

  (*suffix_array)[0] = text.size();
  if (lcp_array != NULL) {
    (*lcp_array)[0] = 0;
  }

  // assert(CheckUnique((*suffix_array)));
}

/**
 * Calculates the suffix array of the input text using induced sorting in O(n).
 * Simultaneously calculates the LCP (longest common prefix) array of the
 * suffix array which only adds O(n) time complexity. Input text doesn't have
 * to contain the special sentinel character ('$') as its presence is implied.
 */
void BuildSuffixArray(const vector<int> &text, int alphabet_size,
                      vector<int> *suffix_array, vector<int> *lcp_array) {
  // vector<int> debug_input_array((*suffix_array).size(), -1);
  // vector<char> debug_type_array((*suffix_array).size(), '-');
  // bool debug_types = false;
  // bool print_input = false;
  // bool print_types = false;
  // bool print_per_bucket = false;
  // int cell_size = NumDigits(text.size()) + 1;

  // cout << endl;
  // PrintVector(text, "Input: ", cell_size, debug_depth);

  vector<char> typemap = BuildTypeMap(text);
  // PrintVector(typemap, "Suffix types: ", cell_size, debug_depth);

  // vector<int> start_type_counts = TypeCount(typemap);
  // PrintVector(start_type_counts, "Type counts: ", cell_size, debug_depth);

  vector<int> bucket_sizes = FindBucketSizes(text, alphabet_size);
  // PrintVector(bucket_sizes, "Bucket sizes: ", cell_size, debug_depth);
  // PrintVector(FindBucketHeads(bucket_sizes), "Bucket heads: ", cell_size,
  //             debug_depth);
  // PrintVector(FindBucketTails(bucket_sizes), "Bucket tails: ", cell_size,
  //             debug_depth);

  int m = bucket_sizes.size();
  // vector<int> default_bucket_heads(m);
  // vector<int> default_bucket_tails(m);
  // FindBucketHeads(bucket_sizes, &default_bucket_heads);
  // FindBucketTails(bucket_sizes, &default_bucket_tails);

  vector<int> bucket_heads(m);
  FindBucketHeads(bucket_sizes, &bucket_heads);
  vector<int> bucket_tails(m);
  FindBucketTails(bucket_sizes, &bucket_tails);
  vector<int> ls_seam = FindSeam(text, typemap, bucket_sizes);

  GuessLMSSort(text, typemap, &bucket_tails, suffix_array);
  // PrintVector(*suffix_array, "Guessed SA: ", cell_size, debug_depth);
  // if (debug_types) {
  //   if (print_input) {
  //     PrintVector(debug_input_array, "Guessed input: ", cell_size,
  //     debug_depth);
  //   }
  //   if (print_types) {
  //     EncodeSeam(default_bucket_heads, default_bucket_tails, ls_seam,
  //                &debug_type_array);
  //     PrintVector(debug_type_array, "Guessed types: ", cell_size,
  //     debug_depth);
  //   }
  //   if (print_per_bucket) {
  //     PrintPerBucket(*suffix_array, debug_type_array, default_bucket_heads,
  //                    default_bucket_tails, cell_size, debug_depth);
  //   }
  //   vector<int> type_counts = TypeCount(debug_type_array);
  //   PrintVector(type_counts, "Type counts: ", cell_size, debug_depth);
  //   if (type_counts[2] != start_type_counts[2]) {
  //     cout << "Invalid SStar types count" << endl;
  //   }
  //   cout << endl;
  // }

  InduceSortL(text, typemap, ls_seam, &bucket_heads, suffix_array);
  // PrintVector(*suffix_array, "After L induced: ", cell_size, debug_depth);
  // if (debug_types) {
  //   if (print_input) {
  //     PrintVector(debug_input_array, "L input: ", cell_size, debug_depth);
  //   }
  //   if (print_types) {
  //     EncodeSeam(default_bucket_heads, default_bucket_tails, ls_seam,
  //                &debug_type_array);
  //     PrintVector(debug_type_array, "L types: ", cell_size, debug_depth);
  //   }
  //   if (print_per_bucket) {
  //     PrintPerBucket(*suffix_array, debug_type_array, default_bucket_heads,
  //                    default_bucket_tails, cell_size, debug_depth);
  //   }
  //   vector<int> type_counts = TypeCount(debug_type_array);
  //   PrintVector(type_counts, "Type counts: ", cell_size, debug_depth);
  //   if (type_counts[0] != start_type_counts[0]) {
  //     cout << "Invalid L types count" << endl;
  //   }
  //   cout << endl;
  // }

  FindBucketTails(bucket_sizes, &bucket_tails);

  InduceSortS(text, typemap, ls_seam, &bucket_tails, suffix_array);
  // PrintVector(*suffix_array, "After S induced: ", cell_size, debug_depth);
  // if (debug_types) {
  //   if (print_input) {
  //     PrintVector(debug_input_array, "S input: ", cell_size, debug_depth);
  //   }
  //   if (print_types) {
  //     EncodeSeam(default_bucket_heads, default_bucket_tails, ls_seam,
  //                &debug_type_array);
  //     PrintVector(debug_type_array, "S types: ", cell_size, debug_depth);
  //   }
  //   if (print_per_bucket) {
  //     PrintPerBucket(*suffix_array, debug_type_array, default_bucket_heads,
  //                    default_bucket_tails, cell_size, debug_depth);
  //   }
  //   vector<int> type_counts = TypeCount(debug_type_array);
  //   PrintVector(type_counts, "Type counts: ", cell_size, debug_depth);
  //   if (type_counts[1] != start_type_counts[1]) {
  //     cout << "Invalid S types count" << endl;
  //   }
  //   cout << endl;
  // }

  vector<int> summary_string;
  vector<int> summary_suffix_offsets;
  int summary_alphabet_size = SummarizeSuffixArray(
      text, *suffix_array, typemap, &summary_string, &summary_suffix_offsets);
  // PrintVector(summary_string, "Summary string: ", cell_size, debug_depth);
  // PrintVector(summary_suffix_offsets, "Suffix offsets: ", cell_size,
  //             debug_depth);

  vector<int> summary_suffix_array(summary_string.size() + 1, -1);
  vector<int> summary_lcp_array(summary_string.size() + 1, -1);
  if (lcp_array != NULL) {
    MakeSummarySuffixArray(summary_string, summary_alphabet_size,
                           &summary_suffix_array, &summary_lcp_array);
  } else {
    MakeSummarySuffixArray(summary_string, summary_alphabet_size,
                           &summary_suffix_array, NULL);
  }
  // PrintVector(summary_suffix_array, "Summary SA: ", cell_size, debug_depth);
  // if (lcp_array != NULL) {
  //   PrintVector(summary_lcp_array, "Summary LCP: ", cell_size, debug_depth);
  // }

  vector<int> lms_lcp_values;
  if (lcp_array != NULL) {
    lms_lcp_values = ComputeLCPOfLMS(text, summary_suffix_offsets,
                                     summary_suffix_array, summary_lcp_array);
    // PrintVector(lms_lcp_values, "LMS LCP: ", cell_size, debug_depth);
    fill((*lcp_array).begin(), (*lcp_array).end(), -1);
  }

  fill((*suffix_array).begin(), (*suffix_array).end(), -1);
  FindBucketHeads(bucket_sizes, &bucket_heads);
  FindBucketTails(bucket_sizes, &bucket_tails);

  vector<int> first_lms(bucket_sizes.size(), -1);
  AccurateLMSSort(text, typemap, summary_suffix_array, summary_lcp_array,
                  summary_suffix_offsets, lms_lcp_values, &bucket_tails,
                  suffix_array, lcp_array, &first_lms);

  // cout << endl;
  // PrintVector(*suffix_array, "Accurate SA: ", cell_size, debug_depth);
  // if (lcp_array != NULL) {
  //   PrintVector(*lcp_array, "Accurate LCP: ", cell_size, debug_depth);
  //   cout << endl;
  // }

  InduceSortL(text, typemap, ls_seam, &bucket_heads, suffix_array, lcp_array,
              &first_lms);
  // PrintVector(*suffix_array, "Acc after L: ", cell_size, debug_depth);
  // if (lcp_array != NULL) {
  //   PrintVector(*lcp_array, "LCP after L: ", cell_size, debug_depth);
  //   cout << endl;
  // }

  FindBucketTails(bucket_sizes, &bucket_tails);

  InduceSortS(text, typemap, ls_seam, &bucket_tails, suffix_array, lcp_array);
  // PrintVector(*suffix_array, "Acc after S: ", cell_size, debug_depth);
  // if (lcp_array != NULL) {
  //   PrintVector(*lcp_array, "LCP after S: ", cell_size, debug_depth);
  //   cout << endl;
  // }

  // if (lcp_array != NULL) {
  //   vector<int> correct_lcp_array(text.size() + 1, -1);
  //   SuffixArrayToLCP(text, (*suffix_array), &correct_lcp_array);
  //   PrintVector(correct_lcp_array, "LCP correct: ", cell_size, debug_depth);
  // }

  // cout << endl;
}
