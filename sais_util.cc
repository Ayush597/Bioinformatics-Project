#include "sais_util.h"

#include <algorithm>
#include <climits>
#include <vector>

using namespace std;

void FindBucketHeads(const vector<int> &bucket_sizes,
                     vector<int> *bucket_heads) {
  int n = bucket_sizes.size();

  int offset = 1;
  for (int i = 0; i < n; i++) {
    (*bucket_heads)[i] = offset;
    offset += bucket_sizes[i];
  }
}

void FindBucketTails(const vector<int> &bucket_sizes,
                     vector<int> *bucket_tails) {
  int n = bucket_sizes.size();

  int offset = 1;
  for (int i = 0; i < n; i++) {
    offset += bucket_sizes[i];
    (*bucket_tails)[i] = offset - 1;
  }
}

int FindMinInRange(const vector<LCP_ARRAY_TYPE> &array, int start_index, int end_index) {
  int min = INT_MAX;
  for (int i = start_index; i <= end_index; i++) {
    if (array[i] != -1 && array[i] < min) {
      min = array[i];
    }
  }
  return min;
}
