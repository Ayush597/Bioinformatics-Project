#include <stdio.h>
#include <time.h>
#include "stud_sais_lcp_original.h"
#include "stud_sais.h"
#include <iostream>

bool compare_solutions(const int *arr1, const int *arr2, int n) {
	for (int i = 0; i < n; i++)
	if (arr1[i] != arr2[i]) {
		printf("\n different at: %d, %d != %d\n", i, arr1[i], arr2[i]);
		return false;
	}
	return true;
}

void Print(const int *arr, int len) {
	for (int i = 0; i < len; i++)
		printf("%d ", arr[i]);
	printf("\n");
}

int main(int argc, const char *argv[]) {
	FILE *fp;
	const char *fname;
	unsigned char *T;
	int *SA1, *SA2, *LCP1, *LCP2;
	long n;
	clock_t start, finish;

	fname = argv[1];
	// Open a file for reading.
	if ((fp = fopen(fname, "rb")) == NULL) {
		fprintf(stderr, "%s: Cannot open file `%s': ", argv[0], fname);
		perror(NULL);
		exit(EXIT_FAILURE);
	}

	// Get the file size.
	if (fseek(fp, 0, SEEK_END) == 0) {
		n = ftell(fp);
		rewind(fp);
		if (n < 0) {
			fprintf(stderr, "%s: Cannot ftell `%s': ", argv[0], fname);
			perror(NULL);
			exit(EXIT_FAILURE);
		}
	}
	else {
		fprintf(stderr, "%s: Cannot fseek `%s': ", argv[0], fname);
		perror(NULL);
		exit(EXIT_FAILURE);
	}

	// Allocate memory.
	T = (unsigned char *)malloc((size_t)(n+1) * sizeof(unsigned char));
	SA1 = (int *)malloc((size_t)(n+1) * sizeof(int));
	SA2 = (int *)malloc((size_t)(n + 1) * sizeof(int));
	if ((T == NULL) || (SA1 == NULL) || (SA2 == NULL)) {
		fprintf(stderr, "%s: Cannot allocate memory.\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	// Read n bytes of data.
	if (fread(T, sizeof(unsigned char), (size_t)n, fp) != (size_t)n) {
		fprintf(stderr, "%s: %s `%s': ",
			argv[0],
			(ferror(fp) || !feof(fp)) ? "Cannot read from" : "Unexpected EOF in",
			argv[1]);
		perror(NULL);
		exit(EXIT_FAILURE);
	}
	fclose(fp);

	T[n] = 0;


	LCP1 = (int*)malloc((size_t)(n + 1)*sizeof(int));
	LCP2 = (int*)malloc((size_t)(n + 1)*sizeof(int));

	printf("SA-IS LCP - start...\n");
	start = clock();
	SA_LCP_IS(T, SA1, LCP1, n + 1, 256, sizeof(unsigned char), true);
	finish = clock();
	double myTime = (double)(finish - start) / (double)CLOCKS_PER_SEC;
	printf("Time: %.4f sec\n", myTime);
	std::cerr << "Student time: " << myTime << "\n";

	printf("SA-IS LCP original - start...\n");
	start = clock();
	sais_lcp(T, SA2, LCP2, n+1);
	finish = clock();
	printf("Time: %.4f sec\n", (double)(finish - start) / (double)CLOCKS_PER_SEC);

	bool correct = compare_solutions(SA1, SA2, n+1);
	if (correct)
		printf("SA solutions are the same\n");
	else
		printf("SA solutions are not the same\n");

	correct = compare_solutions(LCP1, LCP2, n+1);
	if (correct)
		printf("LCP solutions are the same\n");
	else
		printf("LCP solutions are not the same\n");

	return 0;
}
