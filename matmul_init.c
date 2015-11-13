/*
 *  matmul_init.c
 *  
 *
 *  Created by Amy Takayesu on 11/1/15.
 *  Copyright 2015 __MyCompanyName__. All rights reserved.
 *
 */

#include "matmul_init.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#define N 1600

int main(int argc, char **argv) {
	int i,j, my_rank, num_procs, p, row, col;
	double *A;
	double *B;
	double *C;
	
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
	
	p = sqrt(num_procs);
	
	//double A[N/p][N/p];
	//double B[N/p][N/p];
	//double C[N/p][N/p];
	A = (double *)malloc((N/p)*(N/p)*sizeof(double));
	B = (double *)malloc((N/p)*(N/p)*sizeof(double));
	C = (double *)malloc((N/p)*(N/p)*sizeof(double));
	
	row = my_rank / p;
	col = my_rank % (int)p;
	
	// Fill in the arrays A & B
	for (i=0; i < N/p; i++) {
		for (j=0; j < N/p; j++) {
			A[i * N/p + j] = (row * N/p) + i;
			B[i * N/p + j] = ((row * N/p) + i) + ((col * N/p) + j);
		}
	}
	
	printf("Block of A on rank %d at coordinates (%d,%d)\n", my_rank, row, col);
	for (i=0; i < N/p; i++) {
		for (j=0; j < N/p; j++) {
			printf("%f   ", A[i * N/p + j]);
		}
		printf("\n");
	}
	
	printf("Block of B on rank %d at coordinates (%d,%d)\n", my_rank, row, col);
	for (i=0; i < N/p; i++) {
		for (j=0; j < N/p; j++) {
			printf("%f   ", B[i * N/p + j]);
		}
		printf("\n");
	}
	
	MPI_Finalize();
	return(0);
}

