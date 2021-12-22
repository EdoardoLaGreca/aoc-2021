/*
--- Part Two ---

Next, you should verify the life support rating, which can be determined by multiplying the oxygen generator rating by the CO2 scrubber rating.

Both the oxygen generator rating and the CO2 scrubber rating are values that can be found in your diagnostic report - finding them is the tricky part. Both values are located using a similar process that involves filtering out values until only one remains. Before searching for either rating value, start with the full list of binary numbers from your diagnostic report and consider just the first bit of those numbers. Then:

    Keep only numbers selected by the bit criteria for the type of rating value for which you are searching. Discard numbers which do not match the bit criteria.
    If you only have one number left, stop; this is the rating value for which you are searching.
    Otherwise, repeat the process, considering the next bit to the right.

The bit criteria depends on which type of rating value you want to find:

    To find oxygen generator rating, determine the most common value (0 or 1) in the current bit position, and keep only numbers with that bit in that position. If 0 and 1 are equally common, keep values with a 1 in the position being considered.
    To find CO2 scrubber rating, determine the least common value (0 or 1) in the current bit position, and keep only numbers with that bit in that position. If 0 and 1 are equally common, keep values with a 0 in the position being considered.

For example, to determine the oxygen generator rating value using the same example diagnostic report from above:

    Start with all 12 numbers and consider only the first bit of each number. There are more 1 bits (7) than 0 bits (5), so keep only the 7 numbers with a 1 in the first position: 11110, 10110, 10111, 10101, 11100, 10000, and 11001.
    Then, consider the second bit of the 7 remaining numbers: there are more 0 bits (4) than 1 bits (3), so keep only the 4 numbers with a 0 in the second position: 10110, 10111, 10101, and 10000.
    In the third position, three of the four numbers have a 1, so keep those three: 10110, 10111, and 10101.
    In the fourth position, two of the three numbers have a 1, so keep those two: 10110 and 10111.
    In the fifth position, there are an equal number of 0 bits and 1 bits (one each). So, to find the oxygen generator rating, keep the number with a 1 in that position: 10111.
    As there is only one number left, stop; the oxygen generator rating is 10111, or 23 in decimal.

Then, to determine the CO2 scrubber rating value from the same example above:

    Start again with all 12 numbers and consider only the first bit of each number. There are fewer 0 bits (5) than 1 bits (7), so keep only the 5 numbers with a 0 in the first position: 00100, 01111, 00111, 00010, and 01010.
    Then, consider the second bit of the 5 remaining numbers: there are fewer 1 bits (2) than 0 bits (3), so keep only the 2 numbers with a 1 in the second position: 01111 and 01010.
    In the third position, there are an equal number of 0 bits and 1 bits (one each). So, to find the CO2 scrubber rating, keep the number with a 0 in that position: 01010.
    As there is only one number left, stop; the CO2 scrubber rating is 01010, or 10 in decimal.

Finally, to find the life support rating, multiply the oxygen generator rating (23) by the CO2 scrubber rating (10) to get 230.

Use the binary numbers in your diagnostic report to calculate the oxygen generator rating and CO2 scrubber rating, then multiply them together. What is the life support rating of the submarine?
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* SEQUENCE_LENGTH must be less than 32 */
#define SEQUENCE_LENGTH 12

enum rating_type {
	OXYGEN_GENERATOR,
	CO2_SCRUBBER
};

/* convert a binary representation to an integer */
int binary_to_num(char *binary) {
	int binary_len, i, result = 0;

	binary_len = strlen(binary);

	for (i = 0; i < binary_len; i++) {
		if (binary[i] == '1') {
			result |= 0x1 << (SEQUENCE_LENGTH - i - 1);
		}
	}
	
	return result;
}

int get_rating(FILE *f, enum rating_type rating) {
	char rating_str[SEQUENCE_LENGTH + 1];
	int result, digit_idx, orig_pos;

	memset(rating_str, 0, SEQUENCE_LENGTH + 1);
	orig_pos = ftell(f);
	
	for (digit_idx = 0; digit_idx < SEQUENCE_LENGTH; digit_idx++) {
		int zeros = 0, ones = 0, matches = 0;
		char buffer[SEQUENCE_LENGTH + 2];
		char last_match[SEQUENCE_LENGTH + 1]; /* last matched sequence */

		memset(buffer, 0, SEQUENCE_LENGTH + 2);
		memset(last_match, 0, SEQUENCE_LENGTH + 1);
		
		while (fgets(buffer, SEQUENCE_LENGTH + 2, f) != NULL) {
			char digit = 0;

			/* check if the string is not among the kept numbers */
			if (strncmp(buffer, rating_str, digit_idx) != 0) {
				/* skip the current string */
				continue;
			}

			strncpy(last_match, buffer, SEQUENCE_LENGTH);

			matches++;
			digit = buffer[digit_idx];

			switch (digit) {
			case '0':
				zeros++;
				break;
			case '1':
				ones++;
				break;
			}
		}

		if (rating == OXYGEN_GENERATOR) {
			if (zeros > ones) {
				rating_str[digit_idx] = '0';
			} else {
				rating_str[digit_idx] = '1';
			}
		} else if (rating == CO2_SCRUBBER) {
			if (zeros > ones) {
				rating_str[digit_idx] = '1';
			} else {
				rating_str[digit_idx] = '0';
			}
		}

		if (matches == 1 || digit_idx == SEQUENCE_LENGTH - 1) {
			if (matches == 1) {
				strncpy(rating_str, last_match, SEQUENCE_LENGTH);
			}

			result = binary_to_num(rating_str);
			break;
		}

		rewind(f);
	}

	fseek(f, orig_pos, SEEK_SET);

	return result;
}

int main() {
	char *filename = "input3.txt";
	FILE *fin;
	int generator_rating = 0, co2scr_rating = 0;

	fin = fopen(filename, "r");

	if (fin == NULL) {
		printf("File not found.\n");
		exit(EXIT_FAILURE);
	}

	/* check oxygen generator */
	generator_rating = get_rating(fin, OXYGEN_GENERATOR);

	rewind(fin);

	/* check co2 scrubber */
	co2scr_rating = get_rating(fin, CO2_SCRUBBER);

	printf("%d\n", generator_rating * co2scr_rating);
}
