#include "sa_to_lcp.h"

#include <vector>

#include "sais_lcp.h"

using namespace std;

// To construct and return LCP using Kasai's algorithm in O(n)
void KasaiSAToLCP(const vector<int> &text, const vector<int> &suffix_array,
                  vector<int> *lcp_array) {
  int n = suffix_array.size();

  // An auxiliary array to store inverse of suffix array
  // elements. For example if suffix_array[0] is 5, the
  // inverse_suffix_array[5] would store 0.  This is used to get next
  // suffix string from suffix array.
  vector<int> inverse_suffix_array(n, 0);

  for (int i = 0; i < n; i++) {
    inverse_suffix_array[suffix_array[i]] = i;
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
    int j = suffix_array[inverse_suffix_array[i] + 1];

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
  (*lcp_array)[1] = 0;
}

void RankSAToLCP(const vector<int> &text, const vector<int> &suffix_array,
                 vector<int> *lcp_array) {
  int n = text.size(), k = 0;
  vector<int> text2(text);
  text2.resize(text2.size() + 1);
  n = text2.size();
  for (int i = 0; i < n; i++) {
    text2[i]++;
  }
  text2[n - 1] = 0;
  vector<int> rank(n + 1, 0);

  for (int i = 0; i < n; i++) rank[suffix_array[i]] = i;

  for (int i = 0; i < n; i++, k ? k-- : 0) {
    if (rank[i] == n - 1) {
      k = 0;
      continue;
    }
    int j = suffix_array[rank[i] + 1];
    while (i + k < n && j + k < n && text2[i + k] == text2[j + k]) k++;
    (*lcp_array)[rank[i] + 1] = k;
  }

  (*lcp_array)[0] = 0;
}

void ComputeLCPArray(const vector<int> &text, const vector<int> &SA,
                     vector<int> *lcp_array) {
  int n = SA.size();
  vector<int> rank(n);

  for (int i = 0; i < n; i++) rank[SA[i]] = i;

  int lcp = 0;

  for (int i = 0; i < n; i++) {
    if (rank[i] > 0) {
      int j = SA[rank[i] - 1];
      while (true) {
        if (i + lcp == n - 1 || j + lcp == n - 1 || text[i + lcp] != text[j + lcp]) {
          break;
        }
        lcp++;
      }
      (*lcp_array)[rank[i]] = lcp;
      if (lcp > 0) lcp--;
    }
  }

  (*lcp_array)[0] = 0;
}

void SuffixArrayToLCP(const vector<int> &text, const vector<int> &suffix_array,
                      vector<int> *lcp_array) {
  // RankSAToLCP(text, suffix_array, lcp_array);
  // KasaiSAToLCP(text, suffix_array, lcp_array);
  ComputeLCPArray(text, suffix_array, lcp_array);
}