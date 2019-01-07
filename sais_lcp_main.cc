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
    encoded[i] = text[i] - min_char;
  }

  return encoded;
}

vector<int> EncodeByLexicographicalOrder(const string& text, int alphabet_size=256) {
  int n = text.length();
  vector<int> encoded(n);

  vector<int> present_chars(alphabet_size, 0);

  // keep track of characters present in input text
  for (int i = 0; i < n; i++) {
    present_chars[text[i]] = 1;
  }

  int current_int_name = 0;
  for (int i = 0, n = present_chars.size(); i < n; i++) {
    // give names only to characters which are used in input text
    if (present_chars[i] == 1) {
      present_chars[i] = current_int_name;
      current_int_name++;
    }
  }

  // characters are encoded with a number which represents their lexicographical
  // order
  for (int i = 0; i < n; i++) {
    encoded[i] = present_chars[text[i]];
  }

  return encoded;
}

int main(int argc, char* argv[]) {
  int alphabet_size;
  // Check the number of parameters
  if (argc < 2) {
    // Tell the user how to run the program
    cerr << "Usage: " << argv[0] << " DATA_PATH ALPHABET_SIZE" << endl
         << "\tDATA_PATH - path to a .txt file containing a single line of text"
         << endl
         << "\tALPHABET_SIZE - (optional) size of the alphabet";
    /* "Usage messages" are a conventional way of telling the user
     * how to run a program if they enter the command incorrectly.
     */
    return 1;
  } else if (argc < 3) {
    alphabet_size = 256;
  } else {
    alphabet_size = stoi(argv[2]);
  }

  string filename = argv[1];
  string text = LoadFromFile(filename);
  int n = text.size() + 1;

  vector<int> encoded = EncodeAlphabetically(text);

  int cell_size = NumDigits(text.size()) + 1;
  
  // quick & dirty
  bool enable_printing = true;

  if (enable_printing) {
    vector<int> indices(n);
    vector<char> text_chars(n);
    for (int i = 0; i < n; i++) {
      indices[i] = i;
      text_chars[i] = text[i];
    }
    PrintVector(FindBucketSizes(encoded, alphabet_size), "Bucket sizes: ", cell_size);
    PrintVector(indices, "Index: ", cell_size);
    PrintVector(text_chars, "Text: ", cell_size);
    PrintVector(encoded, "Vector encoding: ", cell_size);
  } else {
    allow_printing = false;
    cout.setstate(ios_base::failbit);
  }

  vector<int> suffix_array(n, -1);
  vector<int> lcp_array(n, -1);
  BuildSuffixArray(encoded, alphabet_size, 0, &suffix_array, &lcp_array);

  if (enable_printing) {
    allow_printing = true;
    cout.clear();
  }

  PrintVector(suffix_array, "Suffix array:    ", cell_size);
  PrintVector(lcp_array, "LCP array:       ", cell_size);
  vector<int> correct_lcp(n);
  SuffixArrayToLCP(encoded, suffix_array, &correct_lcp);
  PrintVector(correct_lcp, "LCP should be: ", cell_size);
  
  cout.clear();
  if (lcp_array == correct_lcp) {
    cout << "LCP is OK" << endl;
  } else {
    cout << "LCP is WRONG" << endl;
  }
}