#include "sa_to_lcp.h"

#include <vector>

#include "new/sais_new.h"

using namespace std;

/* Author: Luka
 * Calculates the LCP array from suffix array in O(n) using Kasai's algorithm.
 */
void SuffixArrayToLCP(const vector<int> &text, const vector<int> &SA,
                      vector<LCP_ARRAY_TYPE> *lcp_array) {
  int n = SA.size();
  vector<int> rank(n);

  for (int i = 0; i < n; i++) rank[SA[i]] = i;

  int lcp = 0;

  for (int i = 0; i < n; i++) {
    if (rank[i] > 0) {
      int j = SA[rank[i] - 1];
      while (true) {
        if (i + lcp == n - 1 || j + lcp == n - 1 ||
            text[i + lcp] != text[j + lcp]) {
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

/* Author: Juraj, Luka
 * Adapter function which interfaces between our vectors and the original
 * implementation's array pointers. Vector's data() function returns a pointer
 * to the array which is stored internally in the vector.
 */
void SAIS_SA_LCP(const vector<char> &text, vector<int> *suffix_array,
                 vector<int> *lcp_array) {
  int n = text.size();
  sais_new((unsigned char *)text.data(), (*suffix_array).data(),
           (*lcp_array).data(), n);
}
