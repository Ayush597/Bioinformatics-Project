#ifndef SA_TO_LCP_H_
#define SA_TO_LCP_H_

#include <vector>

void SuffixArrayToLCP(const std::vector<int> &text,
                      const std::vector<int> &suffixArr,
                      std::vector<int> *lcp_array);

#endif