#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "mergesort.h"

static void split_merge_sort(char* arr, int st, int end, size_t width,
		compare_func_t compar, void* data, char* tmp)
{

	if (end - st <= 1)
		return; // 1 element left - sorted

	int mid = (end + st) / 2;
	// these calls end up switching tmp and arr over and over :/
	split_merge_sort(tmp, st, mid, width, compar, data, arr);
	split_merge_sort(tmp, mid, end, width, compar, data, arr);

	int idx_l = st, idx_r = mid;
	for (int i=st; i<end; i++) {
		if (idx_l >= mid && idx_r < end) { // end of left reached, but right exists
			memcpy(&arr[i*width], &tmp[idx_r*width], width);
			idx_r ++;
		}
		else if (idx_r >= end && idx_l < mid) { // end of right reached, but left exists
			memcpy(&arr[i*width], &tmp[idx_l*width], width);
			idx_l ++;
		}
		else { // both exists, so we compare
			int comp = compar(&tmp[idx_l*width], &tmp[idx_r*width], data);
			if (comp > 0) // left is greater, so move right elem first
				memcpy(&arr[i*width], &tmp[width*idx_r++], width);
			else // left is less than right or comparison is equal, move the left element so that order is maintained
				memcpy(&arr[i*width], &tmp[width*idx_l++], width);
		}
	}
}


int msort_r(void* arr, int len, size_t width, compare_func_t compar, void* data)
{
	char* tmp = calloc(len, width);
	memcpy(tmp, arr, len*width); // create a copy to use for merge sort
	int st = 0, end = len;
	split_merge_sort(arr, st, end, width, compar, data, tmp);
	free(tmp);
	return 0;
}
