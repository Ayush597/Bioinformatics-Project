#ifndef SA_TO_LCP_H_
#define SA_TO_LCP_H_

#include <vector>
#include "sais_lcp.h"

void SuffixArrayToLCP(const std::vector<int> &text,
                      const std::vector<int> &suffixArr,
                      std::vector<LCP_ARRAY_TYPE> *lcp_array);

void SAIS_SA_LCP(const std::vector<char> &text, std::vector<int> *suffix_array, std::vector<int> *lcp_array);

void SAIS_SA(int alphabet_size, const std::vector<int> &text, std::vector<int> *suffix_array);

#endif
