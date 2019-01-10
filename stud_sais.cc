#include "stud_sais.h"

#include <iostream>
#include <string>
#include <vector>

unsigned char mask[] = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };
#define tget(i) ( (t[(i)/8]&mask[(i)%8]) ? 1 : 0 )
#define tset(i, b) t[(i)/8]=(b) ? (mask[(i)%8]|t[(i)/8]) : ((~mask[(i)%8])&t[(i)/8])
#define chr(i) (cs==sizeof(int)?((int*)s)[i]:((unsigned char *)s)[i])
#define isLMS(i) (i>0 && tget(i) && !tget(i-1))

template <typename T>
int NumDigits(T number) {
  if (std::is_same<T, char>::value) {
    return 1;
  }
  int digits = 0;
  if (number <= 0) digits = 1;
  while (number) {
    number /= 10;
    digits++;
  }
  return digits;
}

bool allow_printing = true;

template <typename T>
void PrintVector(const std::vector<T> &v, const std::string &name = "",
                 int cell_size = 0, int depth = 0) {
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
    std::cout << *i << "  ";
  }
  std::cout << std::endl;
}

template <typename T>
void PrintValues(T *arr, int size) {
	std::vector<T> temp(arr, arr + size);
	PrintVector(temp, "", 4);
}

static void getCounts(const void *s, int *counts, int n, int K, int cs) {
	int i;
	for (i = 0; i <= K; ++i) { counts[i] = 0; }
	for (i = 0; i < n; ++i) { ++counts[chr(i)]; }
}

static void getBuckets(const int *counts, int *bkt, int K, bool end) {
	int i, sum = 0;
	if (end) { for (i = 0; i <= K; ++i) { sum += counts[i]; bkt[i] = sum; } }
	else { for (i = 0; i <= K; ++i) { sum += counts[i]; bkt[i] = sum - counts[i]; } }
}

void induceSA(const unsigned char *t, int *SA, const void *s, const int *counts, int *bkt, int n, int K, int cs) {
	int i, j;

	// Induce L-type
	getBuckets(counts, bkt, K, false); // find starts of buckets
	for (i = 0; i < n; i++) {
		j = SA[i] - 1;
		if (j >= 0 && !tget(j)) SA[bkt[chr(j)]++] = j;
	}

	PrintValues(SA, n);

	// Induce S-type
	getBuckets(counts, bkt, K, true); // find ends of buckets
	for (i = n - 1; i >= 0; i--) {
		j = SA[i] - 1;
		if (j >= 0 && tget(j)) SA[--bkt[chr(j)]] = j;
	}

	PrintValues(SA, n);
}

int findMinima(int *LCP, int first, int last)
{
	int i = 0, min;

	while ((min = LCP[first + i]) < 0 && first + i < last) i++;
	for (i = first + i; i <= last; i++)
		if (LCP[i] < min && 0 <= LCP[i])
			min = LCP[i];

	return min;
}

void induceSaLcp(const unsigned char *t, int *SA, int *LCP, const void *s, const int *counts, int *bkt, const int *bs, const int *be, int n, int K, int cs) {
	int i, j, k, m, *st, *bseam, previous, current, p;

	getBuckets(counts, bkt, K, false);
	st = (int*)malloc(sizeof(int)*(K + 1));
	memset(st, -1, (K + 1)*sizeof(int));

	bseam = (int*)malloc(sizeof(int)*(K + 1));
	for (i = 0; i < K; i++) bseam[i] = bs[i] - 1;
	for (i = 0; i < n; i++) if (!tget(i)) bseam[chr(i)]++;

	// Induce L-types
	for (i = 0; i < n; i++) {
		j = SA[i] - 1;

		if (j >= 0 && !tget(j))
		{
			if (st[chr(j)] == -1) // It's the first element of bucket 
			{
				LCP[bkt[chr(j)]] = 0;
			}
			else
			{
				m = SA[bkt[chr(j)] - 1] + 1;

				if (chr(m) != chr(j + 1))	// Only first elements match
				{
					LCP[bkt[chr(j)]] = 1;
				}
				else
				{
					previous = st[chr(j)];
					LCP[bkt[chr(j)]] = findMinima(LCP, previous + 1, i) + 1;
				}
			}

			st[chr(j)] = i;
			SA[bkt[chr(j)]++] = j;

			// L-S seam
			if (bkt[chr(j)] - 1 == bseam[chr(j)])
			{
				previous = SA[bkt[chr(j)] - 1];
				current = -1;
				for (k = bkt[chr(j)]; k < be[chr(j)]; k++)
				{
					if (LCP[k] == -2)
					{
						current = k;
						break;
					}
				}
				if (current == -1) continue;
				p = 0;
				while (chr(previous + p) == chr(SA[current] + p) && SA[current] + p < n && previous + p < n) p++;
				LCP[k] = p;
			}
		}
	}

	free(bseam);

	// Inducing S-types
	getBuckets(counts, bkt, K, true);
	memset(st, -1, (K + 1)*sizeof(int));

	for (i = n - 1; i >= 0; i--) {
		j = SA[i] - 1;

		if (j >= 0 && tget(j))
		{
			SA[--bkt[chr(j)]] = j;
			if (st[chr(j)] != -1)
			{
				m = SA[bkt[chr(j)] + 1] + 1;
				if (chr(m) != chr(j + 1))	// only first characters match 
				{
					LCP[bkt[chr(j)] + 1] = 1;
				}
				else
				{
					previous = st[chr(j)];
					LCP[bkt[chr(j)] + 1] = findMinima(LCP, i + 1, previous) + 1;
				}
			}

			// L-S seam
			if (SA[bkt[chr(j)] - 1] >= 0 && !tget(SA[bkt[chr(j)] - 1]) && bkt[chr(j)] > bs[chr(j)])
			{
				previous = SA[bkt[chr(j)] - 1];
				current = SA[bkt[chr(j)]];
				p = 0;
				while (chr(previous + p) == chr(current + p)) p++;
				LCP[bkt[chr(j)]] = p;
			}
			else if (bkt[chr(j)] == bs[chr(j)]) // When S-type is at the start of the bucket
			{
				LCP[bkt[chr(j)]] = 0;
			}

			st[chr(j)] = i;
		}
	}

	free(st);
}

void SA_LCP_IS(const void *s, int *SA, int *LCP, int n, int K, int cs, bool level0) {
	int i, j;
	unsigned char *t = (unsigned char *)malloc(n / 8 + 1);

	tset(n - 2, 0); tset(n - 1, 1);
	for (i = n - 3; i >= 0; i--)
		tset(i, (chr(i) < chr(i + 1) || (chr(i) == chr(i + 1) && tget(i + 1) == 1)) ? 1 : 0);

	// sort all the S-substrings
	int *counts = (int *)malloc(sizeof(int)*(K + 1));
	int *bkt = (int *)malloc(sizeof(int)*(K + 1));

	getCounts(s, counts, n, K, cs);
	getBuckets(counts, bkt, K, true);
	for (i = 0; i < n; i++) SA[i] = -1;
	for (i = 1; i < n; i++)
		if (isLMS(i)) SA[--bkt[chr(i)]] = i;

	PrintValues(SA, n);

	induceSA(t, SA, s, counts, bkt, n, K, cs);

	free(bkt);
	free(counts);

	// put all the sorted substrings into the first n1 items of SA
	int n1 = 0;
	for (i = 0; i < n; i++)
		if (isLMS(SA[i])) SA[n1++] = SA[i];

	// find the lexicographic names of all substrings
	for (i = n1; i < n; i++) SA[i] = -1;
	int name = 0, prev = -1;
	for (i = 0; i < n1; i++) {
		int pos = SA[i]; bool diff = false;
		for (int d = 0; d < n; d++) {
			if (prev == -1 || chr(pos + d) != chr(prev + d) || tget(pos + d) != tget(prev + d))
			{
				diff = true; break;
			}
			else if (d>0 && (isLMS(pos + d) || isLMS(prev + d))) break;
		}
		if (diff) { name++; prev = pos; }
		pos = (pos % 2 == 0) ? pos / 2 : (pos - 1) / 2;
		SA[n1 + pos] = name - 1;
	}
	for (i = n - 1, j = n - 1; i >= n1; i--)
		if (SA[i] >= 0) SA[j--] = SA[i];

	// Solve the reduced problem	
	int *SA1 = SA, *s1 = SA + n - n1;
	if (name < n1)
		SA_LCP_IS((unsigned char*)s1, SA1, NULL, n1, name - 1, sizeof(int), false);
	else
		for (i = 0; i < n1; i++) SA1[s1[i]] = i;

	// Induce the result for the original problem
	counts = (int *)malloc(sizeof(int)*(K + 1));
	bkt = (int *)malloc(sizeof(int)*(K + 1));
	getCounts(s, counts, n, K, cs);
	getBuckets(counts, bkt, K, true);
	for (i = 1, j = 0; i < n; i++)
		if (isLMS(i)) s1[j++] = i;
	for (i = 0; i < n1; i++) SA1[i] = s1[SA1[i]];
	for (i = n1; i < n; i++) SA[i] = -1;

	// INDUCING THE LCP ARRAY: Put the sorted S*-suffixes into their corresponding S-buckets, without changing their order.
	if (level0)
	{
		int j, p, *bs, *be;

		memset(LCP, -1, sizeof(int)*n);
		bs = (int*)malloc(sizeof(int)*(K + 1));
		be = (int*)malloc(sizeof(int)*(K + 1));
		getBuckets(counts, bs, K, false);
		getBuckets(counts, be, K, true);

		LCP[0] = 0;
		j = SA[0];
		// Calculate LCP values for S*-suffixes
		for (i = 1; i < n1; ++i) {
			p = 0;
			while (chr(SA[i] + p) == chr(j + p)) p++;
			LCP[i] = p;
			j = SA[i];
		}

		for (i = n1 - 1; i >= 0; i--) {
			j = SA[i]; SA[i] = -1;
			SA[--bkt[chr(j)]] = j;

			p = LCP[i]; LCP[i] = -1;
			LCP[bkt[chr(j)]] = p;
		}

		// mark L-S seam in LCP array
		for (i = 0; i < K + 1; i++)
			if (bkt[i] != bs[i] && bkt[i] != be[i])
				LCP[bkt[i]] = -2;

		induceSaLcp(t, SA, LCP, s, counts, bkt, bs, be, n, K, cs);

		free(bs); free(be);
	}
	else
	{
		for (i = n1 - 1; i >= 0; i--) {
			j = SA[i]; SA[i] = -1;
			SA[--bkt[chr(j)]] = j;
		}
		induceSA(t, SA, s, counts, bkt, n, K, cs);
	}

	free(counts); free(bkt); free(t);
}