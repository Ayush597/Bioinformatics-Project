#include <algorithm>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <time.h>

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

  if (!enable_printing) {
    allow_printing = false;
    cout.setstate(ios_base::failbit);
  }

  vector<int> suffix_array(n, -1);
  vector<LCP_ARRAY_TYPE> lcp_array(n, -1);

  clock_t start, finish;
  double measuredTime;

  start = clock();
  BuildSuffixArray(encoded, alphabet_size, &suffix_array, &lcp_array);
  finish = clock();
  measuredTime = (double)(finish - start) / (double)CLOCKS_PER_SEC;
  cerr << "Our time: " << measuredTime << '\n';

  if (enable_printing) {
    allow_printing = true;
    cout.clear();
  }

  cout.clear();
}
