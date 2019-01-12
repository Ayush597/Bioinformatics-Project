#include "sais_util.h"

#include <algorithm>
#include <climits>
#include <vector>

using namespace std;

/* Authors: Leon, Luka
 * Finds the bucket heads. A c-bucket's head points to the start of the c-bucket
 * where the first L suffix (which starts with the c character) should be
 * placed.
 */
void FindBucketHeads(const vector<int> &bucket_sizes,
                     vector<int> *bucket_heads) {
  int n = bucket_sizes.size();

  int offset = 1;
  for (int i = 0; i < n; i++) {
    (*bucket_heads)[i] = offset;
    offset += bucket_sizes[i];
  }
}

/* Authors: Leon, Luka
 * Finds the bucket tails. A c-bucket's tails points to the end of the c-bucket
 * where the first S suffix (which starts with the c character) should be
 * placed.
 */
void FindBucketTails(const vector<int> &bucket_sizes,
                     vector<int> *bucket_tails) {
  int n = bucket_sizes.size();

  int offset = 1;
  for (int i = 0; i < n; i++) {
    offset += bucket_sizes[i];
    (*bucket_tails)[i] = offset - 1;
  }
}

/* Author: Luka
 * Finds the minimum value in the array between the give indices. Both indices
 * are inclusive and negative values are ignored (a negative value represents a
 * location where the LCP value hasn't been calculated).
 */
int FindMinInRange(const vector<LCP_ARRAY_TYPE> &array, int start_index,
                   int end_index) {
  int min = INT_MAX;
  for (int i = start_index; i <= end_index; i++) {
    if (array[i] < 0 && array[i] < min) {
      min = array[i];
    }
  }
  return min;
}
