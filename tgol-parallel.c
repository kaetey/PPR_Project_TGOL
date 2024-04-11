#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

// change row and column value to set the canvas size
#define ROW 300
#define COL 300

// generate matrix canvas with random values (live and dead cells)
void generate_initial_field(int current_generation[][COL]) {
	#pragma omp parallel for collapse(2)
	/*#pragma omp master
	printf("Threads: %d\n", omp_get_num_threads());*/
	for (int i = 0; i < ROW; i++) {
		for (int j = 0; j < COL; j++) {
			current_generation[i][j] = rand() % 2;
		}
	}
}

// creates row boundary
void print_row_line() {
	printf("\n");
	for (int i = 0; i < COL; i++) { printf(" ---"); }
	printf("\n");
}

// print matrix
void print_field(int matrix[][COL]) {
	print_row_line();
	for (int i = 0; i < ROW; i++) {
		printf(":");
		for (int j = 0; j < COL; j++) {
			printf(" %d :", matrix[i][j]);
		}
		print_row_line();
	}
}

// returns the count of alive neighbours
int count_live_neighbour_cell(int current_generation[][COL], int r, int c) {
	int count = 0;

	#pragma omp parallel for reduction(+:count) collapse(2)
	/*#pragma omp master
	printf("Threads: %d\n", omp_get_num_threads());*/
	for (int i = r - 1; i <= r + 1; i++) {
		for (int j = c - 1; j <= c + 1; j++) {
			if ((i == r && j == c) || (i < 0 || j < 0) || (i >= ROW || j >= COL)) {
				continue;
			}
			if (current_generation[i][j] == 1) {
				count++;
			}
		}
	}
	return count;
}

// next canvas values based on live neighbour count
void generate_next_generation(int current_generation[][COL], int next_generation[][COL]) {
	int neighbour_live_cell;

	#pragma omp parallel for collapse(2) private(neighbour_live_cell)
	/*#pragma omp master
	printf("Threads: %d\n", omp_get_num_threads());*/
	for (int i = 0; i < ROW; i++) {
		for (int j = 0; j < COL; j++) {
			neighbour_live_cell = count_live_neighbour_cell(current_generation, i, j);
			if (current_generation[i][j] == 1 && (neighbour_live_cell == 2 || neighbour_live_cell == 3)) {
				next_generation[i][j] = 1;
			}

			else if (current_generation[i][j] == 0 && neighbour_live_cell == 3) {
				next_generation[i][j] = 1;
			}

			else {
				next_generation[i][j] = 0;
			}
		}
	}
}

void generate_edge_cases(int current_generation[][COL]) {
	//Reset the canvas
	memset(current_generation, 0, sizeof(int) * COL * COL);

	// Empty field
	// (No action needed after memset)

	// Full field
	for (int i = 0; i < COL; i++) {
		for (int j = 0; j < COL; j++) {
			current_generation[i][j] = 1;
		}
	}

	// Single live cell
	current_generation[COL / 2][COL / 2] = 1;

	// Small clusters
	current_generation[1][1] = 1;
	current_generation[1][2] = 1;
	current_generation[2][1] = 1;
	current_generation[2][2] = 1;

	// One cell thick borders
	for (int i = 0; i < COL; i++) {
		current_generation[0][i] = 1;
		current_generation[COL - 1][i] = 1;
		current_generation[i][0] = 1;
		current_generation[i][COL - 1] = 1;
	}

	// Oscillators at edges
	current_generation[1][0] = 1;
	current_generation[1][1] = 1;
	current_generation[1][2] = 1; // Blinker
}

//generates different boundary conditions for test case 6
void generate_boundary_conditions(int current_generation[][COL]) {
	//Reset the canvas
	memset(current_generation, 0, sizeof(int) * COL * COL);

	//Place live cells at each corner
	/*current_generation[0][0] = 1;
	current_generation[0][COL - 1] = 1;
	current_generation[COL - 1][0] = 1;
	current_generation[COL - 1][COL - 1] = 1;*/

	//Define a row of live cells along the top edge
	/*for (int i = 0; i < COL; i++) {
		current_generation[0][i] = 1;
	}*/

	//Define a glider pattern near the bottom-right corner to move across the boundary
	int offset = COL - 4; // Ensure there's enough room for the glider to be placed
	current_generation[offset][offset + 1] = 1;
	current_generation[offset + 1][offset + 2] = 1;
	current_generation[offset + 2][offset] = 1;
	current_generation[offset + 2][offset + 1] = 1;
	current_generation[offset + 2][offset + 2] = 1;
}

int main() {
	int current_generation[ROW][COL], next_generation[ROW][COL];

	/*//---------------------------------------------------------------------
	//Test case 1: Speedup
	int test_case[5][5] = {
		{0, 0, 1, 0, 0},
		{1, 0, 1, 0, 0},
		{0, 1, 1, 0, 0},
		{0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0}
	};
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) {
			current_generation[i][j] = test_case[i][j];
		}
	}
	//Measures time taken by parallel execution
	double start_time = omp_get_wtime();
	generate_next_generation(current_generation, next_generation);
	double parallel_time = omp_get_wtime() - start_time;

	//setting threads to 1 for serial comparison
	omp_set_num_threads(1);
	start_time = omp_get_wtime();
	generate_next_generation(current_generation, next_generation);
	double serial_time = omp_get_wtime() - start_time;

	// Calculate speedup
	double speedup = serial_time / parallel_time;

	// Print the results
	printf("Parallel Time: %f\n", parallel_time);
	printf("Serial Time: %f\n", serial_time);
	printf("Speedup: %f\n", speedup);
	//-----------------------------------------------------------------------*/
	

	//Set number of threads
	//int num_threads = omp_get_max_threads();
	/*int num_threads = 4;
	omp_get_max_threads(num_threads);
	printf("Number of Threads: %d\n", num_threads);*/

	//generate initial matrix
	generate_initial_field(current_generation);

	// print initial array matrix
	printf("Initial Stage:");
	print_field(current_generation);

	//Set up boundary condition for test case 6
	/*generate_boundary_conditions(current_generation);
	print_field(current_generation);*/

	//Set up edge cases for test case 3
	/*generate_edge_cases(current_generation);
	print_field(current_generation);*/

	//execution time start
	double start_time = omp_get_wtime();

	//generate next generation
	generate_next_generation(current_generation, next_generation);
	
	//execution time start
	double end_time = omp_get_wtime();

	//print execution time
	printf("Size: %dx%d, Execution Time: %f seconds\n", ROW, COL, end_time - start_time);

	// print next generation array matrix
	/*printf("\nNext Generation:");
	print_field(next_generation);*/
	
	return 0;
}