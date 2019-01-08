#include <algorithm>
#include <chrono>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <memory>
#include <random>
#include <sstream>

#include "sa_to_lcp.h"
#include "sais_lcp.h"
#include "sais_util.h"

using namespace std;

bool allow_printing = true;

int main(int argc, char* argv[]) {
  if (argc < 4) {
    cerr << "Usage: " << argv[0] << " SIZE_OF_RANDOM_VECTORS ALPHABET_SIZE ENABLE_PRINTING"
         << endl;
    return 1;
  }

  int n = stoi(argv[1]);
  int alphabet_size = stoi(argv[2]);
  allow_printing = stoi(argv[3]);

  int cell_size = NumDigits(n) + 1;

  auto seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::default_random_engine dre(seed);
  std::uniform_int_distribution<int> di(0, alphabet_size - 1);

  vector<int> input(n);
  vector<int> suffix_array(n + 1);
  vector<int> lcp_array(n + 1);
  vector<int> correct_lcp(n + 1);

  vector<int> indices(n);
  if (allow_printing) {
    for (int i = 0; i < n; i++) {
      indices[i] = i;
    }
  } else {
    cout.setstate(ios_base::failbit);
  }

  while (true) {
    generate(input.begin(), input.end(), [&]{ return di(dre);});

    if (allow_printing) {
      PrintVector(input, "gen input: ", cell_size);
      cin.get();
    }

    fill(suffix_array.begin(), suffix_array.end(), -1);
    fill(lcp_array.begin(), lcp_array.end(), -1);
    fill(correct_lcp.begin(), correct_lcp.end(), -1);

    PrintVector(FindBucketSizes(input, alphabet_size),
                "Bucket sizes: ", cell_size);
    PrintVector(indices, "Index: ", cell_size);

    BuildSuffixArray(input, alphabet_size, 0, &suffix_array, &lcp_array);

    SuffixArrayToLCP(input, suffix_array, &correct_lcp);

    if (lcp_array != correct_lcp) {
      allow_printing = true;
      cout.clear();
    }

    PrintVector(suffix_array, "Suffix array:    ", cell_size);
    PrintVector(lcp_array, "LCP array:       ", cell_size);
    PrintVector(correct_lcp, "LCP should be: ", cell_size);
      
    if (lcp_array != correct_lcp) {
      PrintVector(input, "Found wrong LCP for input: ", cell_size);
      return 1;
    }
  }
}