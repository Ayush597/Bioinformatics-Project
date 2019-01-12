#include "sais_lcp.h"

#include <assert.h>
#include <algorithm>

#include "sa_to_lcp.h"
#include "sais_util.h"

using namespace std;

/******************
        UTILS
 ******************/

/* Authors: Juraj, Leon, Luka
 * Calculates the suffix types with a right-to-left scan.
 */
template <typename T>
vector<char> BuildTypeMap(const vector<T> &text) {
  int length = text.size();

  vector<char> suffix_types(length + 1);
  suffix_types[length] = kSStarType;
  suffix_types[length - 1] = kLType;

  for (int i = length - 2; i >= 0; i--) {
    T c1 = text[i];
    T c2 = text[i + 1];

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

/* Author: Leon
 * Calculates the sizes of all c-buckets by manually counting the characetrs in
 * the input text.
 */
template <typename T>
vector<int> FindBucketSizes(const vector<T> &text, int alphabet_size) {
  vector<int> result(alphabet_size, 0);

  for (T c : text) {
    result[c]++;
  }

  return result;
}

/* Author: Luka
 * Calculates the positions of the L/S seams in each bucket by manually counting
 * the number of L-suffixes in each bucket. The position points to where the
 * first S-suffix (which is located right after the last L-suffix) should be.
 */
template <typename T>
vector<int> FindSeam(const vector<T> &text, const vector<char> &typemap,
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

/* Author: Luka
 * Naively calculates the LCP of two suffixes (locations in the text) by
 * counting the common consecutive characetrs. This naive calculation is
 * sufficient at the L/S seam and keeps O(n) time complexity.
 */
template <typename T>
int CountSameChars(const vector<T> &text, int first_pos_in_text,
                   int second_pos_in_text) {
  int num_same_chars = 0;
  int n = text.size();
  while (true) {
    if (first_pos_in_text >= n || second_pos_in_text >= n ||
        text[first_pos_in_text] != text[second_pos_in_text]) {
      break;
    }
    num_same_chars++;
    first_pos_in_text++;
    second_pos_in_text++;
  }
  return num_same_chars;
}

/* Author: Luka
 * Checks if the two LMS-substrings (given by their location in the text) are
 * the same. They must have the same characeters, types and lengths to be equal.
 * Used in summary generation where same LMS-substrings give their respective
 * LMS-suffixes the same name in the reduced alphabet.
 */
template <typename T>
bool LMSSubstringsAreEqual(const std::vector<T> &text,
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
    if (char_a != char_b || (type_a == kLType && type_b != kLType) ||
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

/*************
  UTILS end
*************/

/* Author: Leon
 * Initial guess for where the LMS suffixes should go. LMS-suffixes are bucket
 * sorted into their respective c-buckets. Their order within buckets is
 * determined later (hence the guess).
 */
template <typename T>
void GuessLMSSort(const vector<T> &text, const vector<char> &typemap,
                  vector<int> *bucket_tails, vector<int> *suffix_array) {
  int n = text.size();
  for (int i = 0; i < n; i++) {
    // not the start of an LMS suffix
    if (!(IsLMSChar(i, typemap))) continue;

    int c = text[i];
    int k = (*bucket_tails)[c];
    (*suffix_array)[k] = i;
    (*bucket_tails)[c] -= 1;
  }

  (*suffix_array)[0] = n;
}

/* Authors: Leon, Luka
 * Induces the order of L-suffixes with a left-to-right scan of SA. LCP array
 * calculations are skipped if no argument is provided (before LMS-suffixes are
 * sorted).
 */
template <typename T>
void InduceSortL(const vector<T> &text, const vector<char> &typemap,
                 const vector<int> &ls_seam, vector<int> *bucket_heads,
                 vector<int> *suffix_array,
                 vector<LCP_ARRAY_TYPE> *lcp_array = NULL,
                 vector<int> *first_lms = NULL) {
  int n = suffix_array->size();
  int m = (*bucket_heads).size();

  // stores i' which are used to induce LCP values
  vector<int> iteration(m, -1);

  for (int i = 0; i < n; i++) {
    if ((*suffix_array)[i] == -1) continue;

    int j = (*suffix_array)[i] - 1;
    if (j < 0) continue;
    if (typemap[j] != kLType) continue;

    T c = text[j];

    int k = (*bucket_heads)[c];
    (*suffix_array)[k] = j;
    (*bucket_heads)[c] += 1;

    if (lcp_array == NULL) {
      continue;
    }

    // if this is the last L in its bucket then we need to recalculate the LCP
    // value of the first LMS-suffix in the same bucket (if any)
    if (k + 1 == ls_seam[c]) {
      int pos_of_first_lms_suffix = (*first_lms)[c];
      if (pos_of_first_lms_suffix != -1) {
        int ls_seam_same_chars = CountSameChars(
            text, (*suffix_array)[k], (*suffix_array)[pos_of_first_lms_suffix]);
        (*lcp_array)[pos_of_first_lms_suffix] = ls_seam_same_chars;
      }
    }

    int prev_iter = iteration[c];
    iteration[c] = i;

    // if this is the first L-suffix in its bucket then LCP=0 (suffix to the
    // left is in a different bucket and has a different starting character)
    if (prev_iter == -1) {
      (*lcp_array)[k] = 0;
      continue;
    }

    // if the "parent" suffixes which induced the sorting of this L-suffix (at
    // position k) and the L-suffix left of this (k - 1) have different starting
    // characters then LCP=1
    if ((*suffix_array)[prev_iter] < ((int)text.size()) &&
        text[(*suffix_array)[i]] != text[(*suffix_array)[prev_iter]]) {
      (*lcp_array)[k] = 1;
      continue;
    }

    // induce the LCP value of "parent" suffixes and add 1
    (*lcp_array)[k] = FindMinInRange(*lcp_array, prev_iter + 1, i) + 1;
  }
}

/* Authors: Leon, Luka
 * Induces the order of L-suffixes with a right-to-left scan of SA. LCP array
 * calculations are skipped if no argument is provided (before LMS-suffixes are
 * sorted).
 */
template <typename T>
void InduceSortS(const vector<T> &text, const vector<char> &typemap,
                 const vector<int> &ls_seam, vector<int> *bucket_tails,
                 vector<int> *suffix_array,
                 vector<LCP_ARRAY_TYPE> *lcp_array = NULL) {
  int n = suffix_array->size();
  int m = (*bucket_tails).size();

  vector<int> iteration(m, -1);

  for (int i = n - 1; i >= 0; i--) {
    int j = (*suffix_array)[i] - 1;

    if (j < 0) continue;
    if (typemap[j] != kSType && typemap[j] != kSStarType) continue;

    T c = text[j];
    int k = (*bucket_tails)[c];
    (*suffix_array)[k] = j;
    (*bucket_tails)[c] -= 1;

    if (lcp_array == NULL) {
      continue;
    }

    int prev_iter = iteration[c];
    iteration[c] = i;

    if (prev_iter == -1) {
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

    if (prev_iter == -1) {
      continue;
    }

    if ((*suffix_array)[prev_iter] < ((int)text.size()) &&
        text[(*suffix_array)[i]] != text[(*suffix_array)[prev_iter]]) {
      (*lcp_array)[k + 1] = 1;
      continue;
    }

    (*lcp_array)[k + 1] = FindMinInRange(*lcp_array, i + 1, prev_iter) + 1;
  }
}

/* Authors: Leon, Luka
 * Calculates the summary of the input text by giving names to the
 * LMS-substrings and using those names to summarize the text.
 */
template <typename T>
int SummarizeSuffixArray(const std::vector<T> &text,
                         const std::vector<int> &suffix_array,
                         const std::vector<char> &typemap,
                         std::vector<int> *summary_string,
                         std::vector<int> *summary_suffix_offsets) {
  // this array will store the names of LMS substrings in the positions
  // that they appear in the original string
  vector<int> lms_names(text.size() + 1, -1);

  int current_name = -1;
  int last_lms_suffix_offset = -1;

  for (int i = 0, n = suffix_array.size(); i < n; i++) {
    // where does this suffix appear in the original string?
    int suffix_offset = suffix_array[i];

    // only LMS-suffixes are considered for the summary
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

  // fill the calculated names into a new vector and also store offsets which
  // tell us the connection between names and their original suffixes
  for (int i = 0, n = lms_names.size() - 1; i < n; i++) {
    if (lms_names[i] == -1) continue;

    summary_suffix_offsets->push_back(i);
    summary_string->push_back(lms_names[i]);
  }

  int summary_alphabet_size = current_name + 1;
  return summary_alphabet_size;
}

/* Authors: Leon, Luka
 * Recursively calculates the SA and LCP array by solving the reduced problem on
 * a previously generated summary. Recursion stops when the summary contains
 * only unique characters in which case calculations are done naively. SA is
 * calculated with a simple bucket sort (which is guaranteed to be correct) and
 * the LCP array is calculated from the SA using Kasai's algorithm.
 */
void MakeSummarySuffixArray(const vector<int> &summary_string,
                            int summary_alphabet_size,
                            vector<int> *suffix_array,
                            vector<LCP_ARRAY_TYPE> *lcp_array) {
  int summary_len = summary_string.size();
  if (summary_alphabet_size == summary_len + 1) {
    (*suffix_array)[0] = summary_len;

    for (int x = 0; x < summary_len; x++) {
      int y = summary_string[x];
      (*suffix_array)[y] = x;
    }

    if (lcp_array != NULL) {
      SuffixArrayToLCP(summary_string, *suffix_array, lcp_array);
    }
  } else {
    BuildSuffixArray(summary_string, summary_alphabet_size, suffix_array,
                     lcp_array);
  }
}

/* Author: Luka
 * This functions is used to scale up the summary's LCP values to the LCP values
 * of the respective LMS-suffixes.
 */
template <typename T>
void ComputeLCPOfLMS(const vector<T> &text,
                     const vector<int> &summary_suffix_offsets,
                     const vector<int> &summary_suffix_array,
                     vector<LCP_ARRAY_TYPE> *summary_lcp_array) {
  int n = summary_suffix_offsets.size();
  if (n == 0) return;
  int sum = 0;

  // calculate the lengths of LMS-substrings in O(n) and store them in a
  // cumulative array from which lengths of intervals can be calculated in O(1)
  vector<int> cumulative_lms_lengths(n, -1);
  cumulative_lms_lengths[0] = 0;
  for (int i = 1; i < n; i++) {
    sum += summary_suffix_offsets[i] - summary_suffix_offsets[i - 1];
    cumulative_lms_lengths[i] = sum;
  }

  for (int k = 2, n = (*summary_lcp_array).size(); k < n; k++) {
    int j = summary_suffix_array[k];
    // length of same LMS-substrings scaled up
    int common_lsm_sum = cumulative_lms_lengths[j + (*summary_lcp_array)[k]] -
                         cumulative_lms_lengths[j];

    int first_pos_in_text =
        summary_suffix_offsets[summary_suffix_array[k - 1]] + common_lsm_sum;
    int second_pos_in_text = summary_suffix_offsets[j] + common_lsm_sum;

    // LMS-suffixes may have common characters beyond what can be induced from
    // their summary LCP values and this is calculated naively which is enough
    int num_same_chars_after_lsm =
        CountSameChars(text, first_pos_in_text, second_pos_in_text);

    (*summary_lcp_array)[k] = common_lsm_sum + num_same_chars_after_lsm;
  }
  (*summary_lcp_array)[0] = 0;
  (*summary_lcp_array)[1] = 0;
}

/* Authors: Leon, Luka
 * This is similar to the guessing sort, but this time we know exactly how to
 * sort the LMS-suffixes (induced from the summary).
 */
template <typename T>
void AccurateLMSSort(const vector<T> &text,
                     const vector<int> &summary_suffix_array,
                     const vector<int> &summary_suffix_offsets,
                     const vector<LCP_ARRAY_TYPE> &lms_lcp_values,
                     vector<int> *bucket_tails, vector<int> *suffix_array,
                     vector<LCP_ARRAY_TYPE> *lcp_array,
                     vector<int> *first_lms) {
  for (int i = summary_suffix_array.size() - 1; i > 0; i--) {
    int string_index = summary_suffix_offsets[summary_suffix_array[i]];

    T c = text[string_index];
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
}

/**
 * Authors: Leon, Luka
 * Calculates the suffix array of the input text using induced sorting in O(n).
 * Simultaneously calculates the LCP (longest common prefix) array of the
 * suffix array which only adds O(n) time complexity. Input text doesn't have
 * to contain the special sentinel character ('$') as its presence is implied.
 */
template <typename T>
void TBuildSuffixArray(const vector<T> &text, int alphabet_size,
                       vector<int> *suffix_array,
                       vector<LCP_ARRAY_TYPE> *lcp_array) {
  vector<char> typemap = BuildTypeMap(text);

  vector<int> bucket_sizes = FindBucketSizes(text, alphabet_size);

  int m = bucket_sizes.size();

  vector<int> bucket_heads(m);
  FindBucketHeads(bucket_sizes, &bucket_heads);
  vector<int> bucket_tails(m);
  FindBucketTails(bucket_sizes, &bucket_tails);
  vector<int> ls_seam = FindSeam(text, typemap, bucket_sizes);

  GuessLMSSort(text, typemap, &bucket_tails, suffix_array);

  InduceSortL(text, typemap, ls_seam, &bucket_heads, suffix_array);

  FindBucketTails(bucket_sizes, &bucket_tails);

  InduceSortS(text, typemap, ls_seam, &bucket_tails, suffix_array);

  vector<int> summary_string;
  vector<int> summary_suffix_offsets;
  int summary_alphabet_size = SummarizeSuffixArray(
      text, *suffix_array, typemap, &summary_string, &summary_suffix_offsets);

  vector<int> summary_suffix_array(summary_string.size() + 1, -1);
  vector<LCP_ARRAY_TYPE> summary_lcp_array(summary_string.size() + 1, -1);
  if (lcp_array != NULL) {
    MakeSummarySuffixArray(summary_string, summary_alphabet_size,
                           &summary_suffix_array, &summary_lcp_array);
  } else {
    MakeSummarySuffixArray(summary_string, summary_alphabet_size,
                           &summary_suffix_array, NULL);
  }

  if (lcp_array != NULL) {
    ComputeLCPOfLMS(text, summary_suffix_offsets, summary_suffix_array,
                    &summary_lcp_array);
    fill((*lcp_array).begin(), (*lcp_array).end(), -1);
  }

  fill((*suffix_array).begin(), (*suffix_array).end(), -1);
  FindBucketHeads(bucket_sizes, &bucket_heads);
  FindBucketTails(bucket_sizes, &bucket_tails);

  vector<int> first_lms(bucket_sizes.size(), -1);
  AccurateLMSSort(text, summary_suffix_array, summary_suffix_offsets,
                  summary_lcp_array, &bucket_tails, suffix_array, lcp_array,
                  &first_lms);

  InduceSortL(text, typemap, ls_seam, &bucket_heads, suffix_array, lcp_array,
              &first_lms);

  FindBucketTails(bucket_sizes, &bucket_tails);

  InduceSortS(text, typemap, ls_seam, &bucket_tails, suffix_array, lcp_array);
}

/* Author: Juraj
 * Top level call should be capable of accepting either char or int.
 */
void BuildSuffixArray(const std::vector<char> &text, int alphabet_size,
                      std::vector<int> *suffix_array,
                      std::vector<LCP_ARRAY_TYPE> *lcp_array) {
  TBuildSuffixArray(text, alphabet_size, suffix_array, lcp_array);
}

/* Author: Juraj
 * Top level call should be capable of accepting either char or int.
 */
void BuildSuffixArray(const std::vector<int> &text, int alphabet_size,
                      std::vector<int> *suffix_array,
                      std::vector<LCP_ARRAY_TYPE> *lcp_array) {
  TBuildSuffixArray(text, alphabet_size, suffix_array, lcp_array);
}
