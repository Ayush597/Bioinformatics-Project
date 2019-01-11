#ifndef SAIS_LCP_H_
#define SAIS_LCP_H_

#include <iostream>
#include <string>
#include <type_traits>
#include <vector>

#define LCP_ARRAY_TYPE short int

void BuildSuffixArray(const std::vector<int> &text, int alphabet_size,
                      std::vector<int> *suffix_array,
                      std::vector<LCP_ARRAY_TYPE> *lcp_array);

#endif
