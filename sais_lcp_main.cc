#include <algorithm>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>

#include "sa_to_lcp.h"
#include "sais_lcp.h"
#include "sais_util.h"

using namespace std;

bool allow_printing = true;

string LoadFromFile(const string& filename) {
  ifstream input_stream(filename);

  stringstream ss;
  ss << input_stream.rdbuf();

  return ss.str();
}

vector<int> EncodeAlphabetically(const string& text) {
  int n = text.length();
  vector<int> encoded(n);

  int min_char = *min_element(begin(text), end(text));

  for (int i = 0; i < n; i++) {
    encoded.at(i) = text.at(i) - min_char;
  }

  return encoded;
}

vector<int> EncodeByLexicographicalOrder(const string& text,
                                         int alphabet_size = 256) {
  int n = text.length();
  vector<int> encoded(n);

  vector<int> present_chars(alphabet_size, 0);

  // keep track of characters present in input text
  for (int i = 0; i < n; i++) {
    present_chars.at(text.at(i)) = 1;
  }

  int current_int_name = 0;
  for (int i = 0, n = present_chars.size(); i < n; i++) {
    // give names only to characters which are used in input text
    if (present_chars.at(i) == 1) {
      present_chars.at(i) = current_int_name;
      current_int_name++;
    }
  }

  // characters are encoded with a number which represents their lexicographical
  // order
  for (int i = 0; i < n; i++) {
    encoded.at(i) = present_chars.at(text.at(i));
  }

  return encoded;
}

int main(int argc, char* argv[]) {
  bool enable_printing = true;
  int alphabet_size = 256;
  // Check the number of parameters
  if (argc < 2) {
    // Tell the user how to run the program
    cerr << "Usage: " << argv[0] << " DATA_PATH ENABLE_PRINTING ALPHABET_SIZE" << endl
         << "\tDATA_PATH - path to a .txt file containing a single line of text"
         << endl << "\tENABLE_PRINTING - if steps should be printed"
         << "\tALPHABET_SIZE - (optional) size of the alphabet";
    /* "Usage messages" are a conventional way of telling the user
     * how to run a program if they enter the command incorrectly.
     */
    return 1;
  } else if (argc == 3) {
    if (stoi(argv[2]) == 0) {
      enable_printing = false;
    }
  } else if (argc == 4) {
    alphabet_size = stoi(argv[3]);
  }

  string filename = argv[1];
  string text = LoadFromFile(filename);
  int n = text.size() + 1;

  vector<int> encoded = EncodeAlphabetically(text);

  int cell_size = NumDigits(text.size()) + 1;

  // quick & dirty

  vector<int> indices(n);
  vector<char> text_chars(n);
  for (int i = 0; i < n; i++) {
    if (i < n - 1) {
      text_chars.at(i) = text.at(i);
    }
  }
  if (enable_printing) {
    for (int i = 0; i < n; i++) {
      indices.at(i) = i;
    }
    // PrintVector(FindBucketSizes(encoded, alphabet_size),
    //             "Bucket sizes: ", cell_size);
    // PrintVector(FindSeam(encoded, BuildTypeMap(encoded),
    //                      FindBucketSizes(encoded, alphabet_size)), "L/S seams: ");
    PrintVector(indices, "Index: ", cell_size);
    PrintVector(text_chars, "Text: ", cell_size);
    PrintVector(encoded, "Vector encoding: ", cell_size);
  } else {
    allow_printing = false;
    cout.setstate(ios_base::failbit);
  }

  vector<int> suffix_array(n, -1);
  vector<int> lcp_array(n, -1);
  
  bool is_shit_broke = false;
  try {
    // BuildSuffixArray(encoded, alphabet_size, 0, &suffix_array, &lcp_array);
    BuildSuffixArray(encoded, alphabet_size, 0, &suffix_array, NULL);
  } catch (...) {
    cerr << "shit is broke yo" << endl;
    is_shit_broke = true;
  }

  if (enable_printing) {
    allow_printing = true;
    cout.clear();
  }

  vector<int> correct_sa(n);
  vector<int> correct_lcp(n);
  SAIS_SA_LCP(text_chars, &correct_sa, &correct_lcp);
  vector<int> correct_rank_lcp(n);
  if (!is_shit_broke) {
    SuffixArrayToLCP(encoded, suffix_array, &correct_rank_lcp);
    PrintVector(suffix_array, "Suffix array: ", cell_size);
  }
  PrintVector(correct_sa, "SA should be: ", cell_size);
  if (!is_shit_broke) {
    PrintVector(lcp_array, "LCP array: ", cell_size);
    PrintVector(correct_rank_lcp, "LCP rank: ", cell_size);
  }
  PrintVector(correct_lcp, "LCP should be: ", cell_size);
  
  // vector<int> int_sa(n, 0);
  // vector<int> encoded_expanded(n);
  // for (int i = 0; i < n; i++) {
  //   if (i == n - 1) {
  //     encoded_expanded.at(i) = 0;
  //   } else {
  //     encoded_expanded.at(i) = encoded.at(i) + 1;
  //   }
  // }
  // SAIS_SA(256, encoded_expanded, &int_sa);
  // PrintVector(int_sa, "int SA: ", cell_size);

  cout.clear();

  if (correct_rank_lcp != correct_lcp) {
    cout << "Rank is wrong" << endl;
  }

  if (suffix_array != correct_sa) {
    cout << "Wrong SA!" << endl;
  }

  if (lcp_array == correct_lcp) {
    cout << "LCP is OK" << endl;
  } else {
    cout << "LCP is WRONG" << endl;
  }
}