#ifndef SAIS_LCP_H_
#define SAIS_LCP_H_

#include <iostream>
#include <string>
#include <vector>

void BuildSuffixArray(const std::vector<int> &text,
                      int alphabet_size,
                      int debug_depth,
                      std::vector<int> *suffix_array,
                      std::vector<int> *lcp_array);

template <class T>
int NumDigits(T number)
{
    int digits = 0;
    if (number <= 0) digits = 1;
    while (number) {
        number /= 10;
        digits++;
    }
    return digits;
}

extern bool allow_printing;

template<typename T>
void PrintVector(const std::vector<T> &v, const std::string& name="", int cell_size=0, int depth=0) {
    if (!allow_printing) {
        return;
    }
    for (int i = 0; i < depth; i++) {
        std::cout << "   ";
    }
    std::cout << name;
    int padding = 20 - name.length();
    for (int i = 0; i < padding; i++) {
        std::cout << ' ';
    }
    for (auto i = v.begin(); i != v.end(); ++i) {
        int number_padding = cell_size - NumDigits(*i);
        for (int i = 0; i < number_padding; i++) {
            std::cout << ' ';
        }
        std::cout << *i << ' ';
    }
    std::cout << std::endl;
}

#endif