#include "sa_to_lcp.h"

#include <vector>

using namespace std;

// To construct and return LCP using Kasai's algorithm in O(n)
void SuffixArrayToLCP(const vector<int> &text, const vector<int> &suffixArr,
                      vector<int> *lcp_array) {
  int n = suffixArr.size();

  // An auxiliary array to store inverse of suffix array
  // elements. For example if suffixArr[0] is 5, the
  // inverse_suffix_array[5] would store 0.  This is used to get next
  // suffix string from suffix array.
  vector<int> inverse_suffix_array(n, 0);

  for (int i = 0; i < n; i++) {
    inverse_suffix_array[suffixArr[i]] = i;
  }

  // Initialize length of previous LCP
  int k = 0;

  // Process all suffixes one by one starting from
  // first suffix in txt[]
  for (int i = 0; i < n; i++) {
    // If the current suffix is at n-1, then we don’t
    // have next substring to consider. So lcp is not
    // defined for this substring, we put zero.
    if (inverse_suffix_array[i] == n - 1) {
      k = 0;
      continue;
    }

    // j contains index of the next substring to
    // be considered  to compare with the present
    // substring, i.e., next string in suffix array
    int j = suffixArr[inverse_suffix_array[i] + 1];

    // Directly start matching from k'th index as
    // at-least k-1 characters will match
    while (i + k < n && j + k < n && text[i + k] == text[j + k]) {
      k++;
    }

    // lcp for the present suffix.
    (*lcp_array)[inverse_suffix_array[i] + 1] = k;

    // Deleting the starting character from the string.
    if (k > 0) {
      k--;
    }
  }

  // first element in LCP is undefined because it has no predecessor
  (*lcp_array)[0] = 0;
}