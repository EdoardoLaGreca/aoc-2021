/*
--- Day 4: Giant Squid ---

You're already almost 1.5km (almost a mile) below the surface of the ocean, already so deep that you can't see any sunlight. What you can see, however, is a giant squid that has attached itself to the outside of your submarine.

Maybe it wants to play bingo?

Bingo is played on a set of boards each consisting of a 5x5 grid of numbers. Numbers are chosen at random, and the chosen number is marked on all boards on which it appears. (Numbers may not appear on all boards.) If all numbers in any row or any column of a board are marked, that board wins. (Diagonals don't count.)

The submarine has a bingo subsystem to help passengers (currently, you and the giant squid) pass the time. It automatically generates a random order in which to draw numbers and a random set of boards (your puzzle input). For example:

7,4,9,5,11,17,23,2,0,14,21,24,10,16,13,6,15,25,12,22,18,20,8,19,3,26,1

22 13 17 11  0
 8  2 23  4 24
21  9 14 16  7
 6 10  3 18  5
 1 12 20 15 19

 3 15  0  2 22
 9 18 13 17  5
19  8  7 25 23
20 11 10 24  4
14 21 16 12  6

14 21 17 24  4
10 16 15  9 19
18  8 23 26 20
22 11 13  6  5
 2  0 12  3  7

After the first five numbers are drawn (7, 4, 9, 5, and 11), there are no winners, but the boards are marked as follows (shown here adjacent to each other to save space):

22 13 17 11  0         3 15  0  2 22        14 21 17 24  4
 8  2 23  4 24         9 18 13 17  5        10 16 15  9 19
21  9 14 16  7        19  8  7 25 23        18  8 23 26 20
 6 10  3 18  5        20 11 10 24  4        22 11 13  6  5
 1 12 20 15 19        14 21 16 12  6         2  0 12  3  7

After the next six numbers are drawn (17, 23, 2, 0, 14, and 21), there are still no winners:

22 13 17 11  0         3 15  0  2 22        14 21 17 24  4
 8  2 23  4 24         9 18 13 17  5        10 16 15  9 19
21  9 14 16  7        19  8  7 25 23        18  8 23 26 20
 6 10  3 18  5        20 11 10 24  4        22 11 13  6  5
 1 12 20 15 19        14 21 16 12  6         2  0 12  3  7

Finally, 24 is drawn:

22 13 17 11  0         3 15  0  2 22        14 21 17 24  4
 8  2 23  4 24         9 18 13 17  5        10 16 15  9 19
21  9 14 16  7        19  8  7 25 23        18  8 23 26 20
 6 10  3 18  5        20 11 10 24  4        22 11 13  6  5
 1 12 20 15 19        14 21 16 12  6         2  0 12  3  7

At this point, the third board wins because it has at least one complete row or column of marked numbers (in this case, the entire top row is marked: 14 21 17 24 4).

The score of the winning board can now be calculated. Start by finding the sum of all unmarked numbers on that board; in this case, the sum is 188. Then, multiply that sum by the number that was just called when the board won, 24, to get the final score, 188 * 24 = 4512.

To guarantee victory against the giant squid, figure out which board will win first. What will your final score be if you choose that board?
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define NUMBERS_MAX_LENGTH 2

struct cell {
	int number;
	int is_drawn;
	int drawn_idx; /* this is the index of number in drawn list */
};

struct drawn_num {
	int number;
	struct drawn_num *next;
};

/* append a drawn number to the list and return a pointer to it*/
struct drawn_num *add_drawn_number(struct drawn_num *last_item, int new_num) {
	struct drawn_num *new_drawn = malloc(sizeof(struct drawn_num));

	if (new_drawn == NULL)
		return NULL;

	new_drawn->number = new_num;
	new_drawn->next = NULL;

	if (last_item != NULL)
		last_item->next = new_drawn;

	return new_drawn;
}

/* get position of a number in the list of drawn numbers */
int get_position(struct drawn_num *list, int number) {
	int counter = 0;

	while (list != NULL) {
		if (list->number == number) {
			return counter;
		}
		
		counter++;

		list = list->next;
	}

	return -1;
}

/* de-allocate all drawn numbers */
void delete_drawn_numbers(struct drawn_num *l) {
	while (l != NULL) {
		struct drawn_num *next = l->next;
		free(l);
		l = next;
	}
}

/* read drawn numbers from file and append them to the list */
void read_drawn_numbers(FILE *f, struct drawn_num **list) {
	struct drawn_num *last_item;

	char buffer[NUMBERS_MAX_LENGTH + 2];
	int buff_idx = 0;

	/* read drawn numbers */
	while ((buffer[buff_idx] = fgetc(f))) {
		int new_num;

		if (buffer[buff_idx] != ',' && buffer[buff_idx] != '\n') {
			buff_idx++;
			continue;
		}	

		/* character is either a comma ',' or a newline '\n' */

		sscanf(buffer, "%d", &new_num);

		/* add item */
		if (*list == NULL) {
			/* first item of the list */
			*list = add_drawn_number(*list, new_num);
			last_item = *list;
		} else {
			last_item = add_drawn_number(last_item, new_num);
		}

		if (buffer[buff_idx] == '\n') break;

		memset(buffer, 0, sizeof(buffer));
		buff_idx = 0;
	}
}

/* mark drawn numbers */
void mark_drawn(struct cell board[5][5], struct drawn_num *list) {
	int index = 0;
	
	while (list != NULL) {
		int row, column;
		
		for (row = 0; row < 5; row++) {
			for (column = 0; column < 5; column++) {
				struct cell *curr_cell = &board[row][column];
				
				if (curr_cell->number == list->number) {
					curr_cell->is_drawn = 1;
					curr_cell->drawn_idx = index;
				}
			}
		}

		index++;
		list = list->next;
	}
}

/* check if board is a winning board and return (as a parameter) the last number
   called when the board won
*/
int check_board(struct cell board[5][5], int *last_num) {
	int is_rows, is_winning = 0, last_number = 0, last_number_idx = INT_MAX;

	for (is_rows = 1; is_rows >= 0; is_rows--) {
		int i;

		/* check rows or columns (see is_rows) */
		for (i = 0; i < 5; i++) {
			int j, curr_is_winning = 1, curr_last_number = 0,
			curr_last_number_idx = 0;
			
			/* check single items */
			for (j = 0; j < 5; j++) {
				struct cell current_cell;

				if (is_rows) {
					current_cell = board[i][j];
				} else {
					current_cell = board[j][i];
				}
				
				is_winning = is_winning && current_cell.is_drawn;
				
				if (current_cell.drawn_idx > curr_last_number_idx) {
					curr_last_number = current_cell.number;
					curr_last_number_idx = current_cell.drawn_idx;
				}
			}

			if (curr_last_number_idx < last_number_idx) {
				last_number = curr_last_number;
				last_number_idx = curr_last_number_idx;
			}
			
			is_winning = is_winning || curr_is_winning;
		}
	}

	if (is_winning) {
		*last_num = last_number;
		return 1;
	}

	return 0;
}

/* remove is_drawn mark in a board from a certain index (excluded) to the end of
   the drawn numbers list
*/ 
void remove_mark(struct cell board[5][5], int index, struct drawn_num *list) {
	int current_index = 0;

	while (list != NULL) {
		if (current_index > index) {
			int row;
			for (row = 0; row < 5; row++) {

				int column;
				for (column = 0; column < 5; column++) {
					struct cell *curr_cell = &board[row][column];

					if (curr_cell->number == list->number) {
						curr_cell->is_drawn = 0;
					}
				}
			}
		}

		current_index++;
		list = list->next;
	}
}

int calc_score(struct cell board[5][5], int last_num) {
	int row, column, unmarked_sum = 0;

	for (row = 0; row < 5; row++) {
		for (column = 0; column < 5; column++) {
			struct cell current_cell = board[row][column];
			
			if (!current_cell.is_drawn) {
				unmarked_sum += current_cell.number;
			}
		}
	}

	return unmarked_sum * last_num;
}

int main() {
	char *filename = "input4.txt";
	FILE *fin;

	/* list of drawn numbers */
	struct drawn_num *list = NULL;

	fin = fopen(filename, "r");

	if (fin == NULL) {
		printf("File not found.\n");
		exit(EXIT_FAILURE);
	}

	read_drawn_numbers(fin, &list);

	{
		/* this buffer is long enough for each row of the boards, it has enough
		   space for 5 numbers (each of length NUMBERS_MAX_LENGTH) and 4 spaces
		   between each number (each of the same length as the numbers);
		*/
		char buffer[(NUMBERS_MAX_LENGTH * 5) + (NUMBERS_MAX_LENGTH * 4)];

		struct cell board[5][5];
		struct cell winning_board[5][5];
		int row = 0, winning_last_num = INT_MAX, winning_last_num_pos = INT_MAX;

		memset(board, 0, sizeof(board));
		memset(winning_board, 0, sizeof(winning_board));
		
		/* read boards */
		while (fgets(buffer, sizeof(buffer), fin) != NULL) {

			if (buffer[0] == '\n') {
				if (row != 0) {
					int is_winning, last_num, last_num_pos;

					mark_drawn(board, list);
					
					is_winning = check_board(board, &last_num);
					last_num_pos = get_position(list, last_num);

					if (is_winning && last_num_pos < winning_last_num_pos) {
						winning_last_num = last_num;
						winning_last_num_pos = last_num_pos;

						memcpy(winning_board, board, sizeof(winning_board));
					}
				}
	
				/* reset; prepare to read a new board */
				row = 0;
				memset(board, 0, sizeof(board));
				continue;
			}

			/* scan current row */
			sscanf(buffer, "%d %d %d %d %d", &board[row][0].number,
			&board[row][1].number, &board[row][2].number,
			&board[row][3].number, &board[row][4].number);
			
			memset(buffer, 0, sizeof(buffer));
			row++;
		}

		remove_mark(winning_board, winning_last_num_pos, list);
		printf("%d\n", calc_score(winning_board, winning_last_num));
		delete_drawn_numbers(list);
	}
	
	return 0;
}
