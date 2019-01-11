#ifndef _SAIS_LCP_H
#define _SAIS_LCP_H 1

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

	/* find the suffix array SA of T[0..n-1]
	use a working space (excluding T and SA) of at most 2n+O(lg n) */
	int sais_lcp(const unsigned char *T, int *SA, int *LCP, int n);
	
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* _SAIS_LCP_H */
