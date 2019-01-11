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

bool test_generation = false;
bool allow_printing;

void EnablePrinting(bool is_enabled) {
  if (is_enabled) {
    allow_printing = true;
    std::cout.clear();
  } else {
    allow_printing = false;
    std::cout.setstate(std::ios_base::failbit);
  }
}

int main(int argc, char* argv[]) {
  if (argc < 4) {
    cerr << "Usage: " << argv[0] << " SIZE_OF_RANDOM_VECTORS ALPHABET_SIZE ENABLE_PRINTING"
         << endl;
    return 1;
  }

  int n = stoi(argv[1]);
  int alphabet_size = stoi(argv[2]);
  bool enable_printing = stoi(argv[3]);

  int cell_size = NumDigits(n) + 1;

  auto seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::default_random_engine dre(seed);
  std::uniform_int_distribution<int> di(0, alphabet_size - 1);

  vector<int> input(n);
  vector<char> text_chars(n + 1);
  vector<int> suffix_array(n + 1);
  vector<int> correct_sa(n + 1);
  vector<int> lcp_array(n + 1);
  vector<int> correct_lcp(n + 1);

  EnablePrinting(enable_printing);

  vector<int> indices(n);
  if (enable_printing) {
    for (int i = 0; i < n; i++) {
      indices.at(i) = i;
    }
  }

  while (true) {
    generate(input.begin(), input.end(), [&]{ return di(dre);});
    for (int i = 0; i < n; i++) {
      text_chars.at(i) = (char )(input.at(i) + 'a');
    }
    text_chars.at(n) = 0;
    PrintVector(text_chars, "Chars: ", cell_size);

    if (test_generation) {
      EnablePrinting(true);
    }

    if (allow_printing) {
      PrintVector(input, "gen input: ", cell_size);
      // cin.get();
    }

    if (test_generation && !enable_printing) {
      EnablePrinting(false);
    }

    fill(suffix_array.begin(), suffix_array.end(), -1);
    fill(correct_sa.begin(), correct_sa.end(), 0);
    fill(lcp_array.begin(), lcp_array.end(), -1);
    fill(correct_lcp.begin(), correct_lcp.end(), 0);

    PrintVector(FindBucketSizes(input, alphabet_size),
                "Bucket sizes: ", cell_size);
    PrintVector(indices, "Index: ", cell_size);

    BuildSuffixArray(input, alphabet_size, &suffix_array, &lcp_array);

    SAIS_SA_LCP(text_chars, &correct_sa, &correct_lcp);
    // SuffixArrayToLCP(input, suffix_array, &correct_lcp);

    if (lcp_array != correct_lcp || test_generation) {
      EnablePrinting(true);
    }

    PrintVector(suffix_array, "Suffix array:    ", cell_size);
    PrintVector(correct_sa, "SA should be: ", cell_size);
    PrintVector(lcp_array, "LCP array:       ", cell_size);
    PrintVector(correct_lcp, "LCP should be: ", cell_size);
    cout << endl;

    if (lcp_array != correct_lcp) {
      PrintVector(input, "Found wrong LCP for input: ", cell_size);
      return 1;
    }

    if (test_generation && !enable_printing) {
      EnablePrinting(false);
    }
  }
}