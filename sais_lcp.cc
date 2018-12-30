#include "sais_lcp.h"

#include <algorithm>

#include "sa_to_lcp.h"
#include "sais_util.h"

using namespace std;

void GuessLMSSort(const vector<int> &text, vector<int> &bucket_sizes,
                  const vector<char> &type_map, vector<int> *suffix_array,
                  vector<int> *lcp_array) {
  vector<int> bucket_tails = FindBucketTails(bucket_sizes);

  int n = text.size();
  for (int i = 0; i < n; i++) {
    // not the start of an LMS suffix
    if (!(IsLMSChar(i, type_map))) continue;

    int bucket_index = text[i];
    (*suffix_array)[bucket_tails[bucket_index]] = i;
    bucket_tails[bucket_index] -= 1;
  }

  (*suffix_array)[0] = n;
}

void InduceSortL(const vector<int> &text, const vector<int> &bucket_sizes,
                 const vector<char> &typemap, vector<int> *suffix_array,
                 vector<int> *lcp_array) {
  vector<int> bucket_heads = FindBucketHeads(bucket_sizes);

  int n = suffix_array->size();
  for (int i = 0; i < n; i++) {
    if ((*suffix_array)[i] == -1) continue;

    int j = (*suffix_array)[i] - 1;
    if (j < 0) continue;
    if (typemap[j] != kLType) continue;

    int bucket_index = text[j];
    (*suffix_array)[bucket_heads[bucket_index] + 1] = j;
    bucket_heads[bucket_index] += 1;
  }
}

void InduceSortS(const vector<int> &text, const vector<int> &bucket_sizes,
                 const vector<char> &typemap, vector<int> *suffix_array,
                 vector<int> *guessed_lcp_array) {
  vector<int> bucket_tails = FindBucketTails(bucket_sizes);

  int n = suffix_array->size();
  for (int i = n - 1; i >= 0; i--) {
    int j = (*suffix_array)[i] - 1;

    if (j < 0) continue;
    if (typemap[j] != kSType && typemap[j] != kSStarType) continue;

    int bucket_index = text[j];
    (*suffix_array)[bucket_tails[bucket_index]] = j;
    bucket_tails[bucket_index] -= 1;
  }
}

int SummarizeSuffixArray(const std::vector<int> &text,
                         const std::vector<int> &suffix_array,
                         const std::vector<char> &typemap,
                         std::vector<int> *summary_string,
                         std::vector<int> *summary_suffix_offsets) {
  // this array will store the names of LMS substrings in the positions
  // that they appear in the original string
  vector<int> lms_names(text.size() + 1, -1);

  int current_name = 0;
  int last_lms_suffix_offset = suffix_array[0];

  // the first LMS substring is always the empty string
  lms_names[suffix_array[0]] = current_name;

  for (int i = 1, n = suffix_array.size(); i < n; i++) {
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

  for (int i = 0, n = lms_names.size(); i < n; i++) {
    if (lms_names[i] == -1) continue;

    summary_suffix_offsets->push_back(i);
    summary_string->push_back(lms_names[i]);
  }

  int summary_alphabet_size = current_name + 1;
  return summary_alphabet_size;
}

void MakeSummarySuffixArray(const vector<int> &summary_string,
                            int summary_alphabet_size, int debug_depth,
                            vector<int> *suffix_array, vector<int> *lcp_array) {
  int summary_len = summary_string.size();
  if (summary_alphabet_size == summary_len) {
    (*suffix_array)[0] = summary_len;

    for (int x = 0, n = summary_len; x < n; x++) {
      int y = summary_string[x];
      (*suffix_array)[y + 1] = x;
    }

    SuffixArrayToLCP(summary_string, *suffix_array, lcp_array);
  } else {
    BuildSuffixArray(summary_string, summary_alphabet_size, debug_depth,
                     suffix_array, lcp_array);
  }
}

vector<int> ComputeLCPOfLMS(const vector<int> &text,
                            const vector<int> &summary_suffix_offsets,
                            const vector<int> &summary_suffix_array,
                            const vector<int> &summary_lcp_array) {
  vector<int> lms_lcp_values(summary_lcp_array.size(), 0);
  int sum = 0;
  vector<int> cumulative_lms_lengths(summary_suffix_offsets.size() - 1, -1);
  for (int i = 0, n = summary_suffix_offsets.size(); i < n - 1; i++) {
    sum += summary_suffix_offsets[i + 1] - summary_suffix_offsets[i];
    cumulative_lms_lengths[i] = sum;
  }

  for (int k = 2, n = summary_lcp_array.size(); k < n; k++) {
    int j = summary_suffix_array[k];
    int common_lsm_sum = cumulative_lms_lengths[j + summary_lcp_array[k]] -
                         cumulative_lms_lengths[j];

    int first_pos_in_text =
        summary_suffix_offsets[summary_suffix_array[k - 1]] + common_lsm_sum;
    int second_pos_in_text = summary_suffix_offsets[j] + common_lsm_sum;

    int num_same_chars_after_lsm = 0;
    while (true) {
      if (text[first_pos_in_text] == text[second_pos_in_text]) {
        num_same_chars_after_lsm++;
      } else {
        break;
      }
      first_pos_in_text++;
      second_pos_in_text++;
    }

    lms_lcp_values[k] = common_lsm_sum + num_same_chars_after_lsm;
  }

  return lms_lcp_values;
}

void AccurateLMSSort(const vector<int> &text, const vector<int> &bucket_sizes,
                     const vector<char> &typemap,
                     const vector<int> &summary_suffix_array,
                     const vector<int> &summary_lcp_array,
                     const vector<int> &summary_suffix_offsets,
                     const vector<int> &lms_lcp_values,
                     vector<int> *suffix_array, vector<int> *lcp_array) {

  vector<int> bucket_tails = FindBucketTails(bucket_sizes);
  for (int i = summary_suffix_array.size() - 1; i > 1; i--) {
    int string_index = summary_suffix_offsets[summary_suffix_array[i]];

    int bucket_index = text[string_index];
    (*suffix_array)[bucket_tails[bucket_index]] = string_index;
    (*lcp_array)[bucket_tails[bucket_index]] = lms_lcp_values[i];
    bucket_tails[bucket_index] -= 1;
  }

  (*suffix_array)[0] = text.size();
}

void BuildSuffixArray(const vector<int> &text, int alphabet_size,
                      int debug_depth, vector<int> *suffix_array,
                      vector<int> *lcp_array) {
  int cell_size = NumDigits(text.size()) + 1;
  if (allow_printing) {
    cout << endl;
  }
  PrintVector(text, "Input: ", cell_size, debug_depth);

  vector<char> typemap = BuildTypeMap(text);
  PrintVector(typemap, "Suffix types: ", cell_size, debug_depth);

  // vector<vector<int>> sorted_suffixes;
  // for (int i = 0, n = typemap.size(); i < n; i++) {
  //   if (typemap[i] == kSStarType) {
  //     vector<int> sub_vector(text.begin() + i, text.end());
  //     sorted_suffixes.push_back(sub_vector);
  //   }
  // }
  // sort(sorted_suffixes.begin(), sorted_suffixes.end());
  // for (int i = 0, n = sorted_suffixes.size(); i < n; i++) {
  //   PrintVector(sorted_suffixes[i], "", cell_size, debug_depth);
  // }

  vector<int> bucket_sizes = FindBucketSizes(text, alphabet_size);

  GuessLMSSort(text, bucket_sizes, typemap, suffix_array, lcp_array);
  PrintVector(*suffix_array, "Guessed SA: ", cell_size, debug_depth);

  InduceSortL(text, bucket_sizes, typemap, suffix_array, lcp_array);
  PrintVector(*suffix_array, "After L induced: ", cell_size, debug_depth);

  InduceSortS(text, bucket_sizes, typemap, suffix_array, lcp_array);
  PrintVector(*suffix_array, "After S induced: ", cell_size, debug_depth);

  vector<int> summary_string;
  vector<int> summary_suffix_offsets;
  int summary_alphabet_size = SummarizeSuffixArray(
      text, *suffix_array, typemap, &summary_string, &summary_suffix_offsets);
  PrintVector(summary_string, "Summary string: ", cell_size, debug_depth);
  PrintVector(summary_suffix_offsets, "Suffix offsets: ", cell_size,
              debug_depth);

  vector<int> summary_suffix_array(summary_string.size() + 1, -1);
  vector<int> summary_lcp_array(summary_string.size() + 1, -1);
  MakeSummarySuffixArray(summary_string, summary_alphabet_size, debug_depth + 1,
                         &summary_suffix_array, &summary_lcp_array);
  PrintVector(summary_suffix_array, "Summary SA: ", cell_size, debug_depth);
  PrintVector(summary_lcp_array, "Summary LCP: ", cell_size, debug_depth);

  vector<int> lms_lcp_values = ComputeLCPOfLMS(
      text, summary_suffix_offsets, summary_suffix_array, summary_lcp_array);
  PrintVector(lms_lcp_values, "LMS LCP: ", cell_size, debug_depth);

  fill((*suffix_array).begin(), (*suffix_array).end(), -1);

  AccurateLMSSort(text, bucket_sizes, typemap, summary_suffix_array,
                  summary_lcp_array, summary_suffix_offsets, lms_lcp_values,
                  suffix_array, lcp_array);
  PrintVector(*suffix_array, "Accurate SA: ", cell_size, debug_depth);
  PrintVector(*lcp_array, "Accurate LCP: ", cell_size, debug_depth);

  InduceSortL(text, bucket_sizes, typemap, suffix_array, lcp_array);
  PrintVector(*suffix_array, "Acc after L: ", cell_size, debug_depth);

  InduceSortS(text, bucket_sizes, typemap, suffix_array, lcp_array);
  PrintVector(*suffix_array, "Acc after S: ", cell_size, debug_depth);

  SuffixArrayToLCP(text, *suffix_array, lcp_array);
  PrintVector(*lcp_array, "LCP scaled: ", cell_size, debug_depth);

  if (allow_printing) {
    cout << endl;
  }
}