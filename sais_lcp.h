#ifndef SAIS_LCP_H_
#define SAIS_LCP_H_

#include <iostream>
#include <string>
#include <type_traits>
#include <vector>

void BuildSuffixArray(const std::vector<int> &text, int alphabet_size,
                      std::vector<int> *suffix_array,
                      std::vector<int> *lcp_array);

#endif