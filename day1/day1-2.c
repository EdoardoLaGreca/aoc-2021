/*
--- Part Two ---

Considering every single measurement isn't as useful as you expected: there's just too much noise in the data.
Instead, consider sums of a three-measurement sliding window. Again considering the above example:

199  A      
200  A B    
208  A B C  
210    B C D
200  E   C D
207  E F   D
240  E F G  
269    F G H
260      G H
263        H

Start by comparing the first and second three-measurement windows. The measurements in the first window are marked A (199, 200, 208); their sum is 199 + 200 + 208 = 607. The second window is marked B (200, 208, 210); its sum is 618. The sum of measurements in the second window is larger than the sum of the first, so this first comparison increased.
Your goal now is to count the number of times the sum of measurements in this sliding window increases from the previous sum. So, compare A with B, then compare B with C, then C with D, and so on. Stop when there aren't enough measurements left to create a new three-measurement sum.
In the above example, the sum of each three-measurement window is as follows:

A: 607 (N/A - no previous sum)
B: 618 (increased)
C: 618 (no change)
D: 617 (decreased)
E: 647 (increased)
F: 716 (increased)
G: 769 (increased)
H: 792 (increased)

In this example, there are 5 sums that are larger than the previous sum.

Consider sums of a three-measurement sliding window. How many sums are larger than the previous sum?
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

int read_next_num(FILE *f) {
	char buffer[6];
	int num = -1;
	
	if (fgets(buffer, 6, f) == NULL)
		return num;

	sscanf(buffer, "%d", &num);

	return num;
}

int main() {
	char *filename = "input1.txt";
	FILE *fin; /* Input file stream */
	int curr_nums[3] = { 0, 0, 0 };
	int i, curr_num, curr_sum = 0, prev_sum, increase = 0;

	fin = fopen(filename, "r");
	
	if (fin == NULL) {
		printf("No file found.\n");
		exit(EXIT_FAILURE);
	}

	/* fill the 3 values */
	for (i = 0; i < 3; i++) {
		curr_nums[i] = read_next_num(fin);
	}

	curr_sum = curr_nums[0] + curr_nums[1] + curr_nums[2];

	while ((curr_num = read_next_num(fin)) != -1) {

		/* update values */
		prev_sum = curr_sum;
		curr_nums[0] = curr_nums[1];
		curr_nums[1] = curr_nums[2];
		curr_nums[2] = curr_num;
		curr_sum = curr_nums[0] + curr_nums[1] + curr_nums[2];
		
		if (curr_sum > prev_sum) {
			increase++;
		}
	}

	printf("%d\n", increase);

	return EXIT_SUCCESS;
}
