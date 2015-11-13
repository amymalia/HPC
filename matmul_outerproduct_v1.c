/*
 *  matmul_outerproduct_v1.c
 *  
 *
 *  Created by Amy Takayesu on 11/9/15.
 *  Copyright 2015 __MyCompanyName__. All rights reserved.
 *
 */

#include "matmul_outerproduct_v1.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#define N 1600

int main(int argc, char **argv) {
	int i,j,k, my_rank, num_procs, p, row, col, currRank;
	double partialSum, finalSum;
	double *A;
	double *B;
	double *C;
	double *bufferA;
	double *bufferB;
	
	MPI_Group orig_group, row_group, col_group;
	MPI_Comm row_comm, col_comm;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
	
	p = sqrt(num_procs);
	
	A = (double *)malloc((N/p)*(N/p)*sizeof(double));
	B = (double *)malloc((N/p)*(N/p)*sizeof(double));
	C = (double *)malloc((N/p)*(N/p)*sizeof(double));
	bufferA = (double *)malloc((N/p)*(N/p)*sizeof(double));
	bufferB = (double *)malloc((N/p)*(N/p)*sizeof(double));
	
	row = my_rank / p;
	col = my_rank % (int)p;
	
	// Fill in the arrays A & B
	for (i=0; i < N/p; i++) {
		for (j=0; j < N/p; j++) {
			A[i * N/p + j] = (row * N/p) + i;
			B[i * N/p + j] = ((row * N/p) + i) + ((col * N/p) + j);
		}
	}
		
	// Start the timer
	double start_time;
	MPI_Barrier(MPI_COMM_WORLD);
	if (my_rank == 0) {  
		start_time = MPI_Wtime();
	}
	
	MPI_Status status;
	for (currRank = 0; currRank < p; currRank++) {
		//Broadcast to A to row and B to column
		if (currRank == row){
			for (i=col; i <= col + (p*(p-1)); i++) { 
				if ((i != my_rank) && ((i % p) == col)) {
					MPI_Send(B, (N/p)*(N/p), MPI_DOUBLE, i, p, MPI_COMM_WORLD); 
				}
			}
		}
		if (currRank == col) {
			for (i=p * row; i < p*(row+1); i++) {
				if (i != my_rank) {
					MPI_Send(A, (N/p)*(N/p), MPI_DOUBLE, i, p, MPI_COMM_WORLD);
				}
			}
		}
		
		//Matrix multiplication
		if ((currRank == row) && (currRank == col)) {
			for (k=0; k < N/p; k++) {
				for (i=0; i < N/p; i++) {
					for (j=0; j < N/p; j++) {
						C[i * N/p + j] += A[i * N/p + k] * B[k * N/p + j];
					}
				}
			}
		}
		else if (currRank == row) {
			MPI_Recv(bufferA, (N/p)*(N/p), MPI_DOUBLE, currRank + p*row, p, MPI_COMM_WORLD, &status);
			for (k=0; k < N/p; k++) {
				for (i=0; i < N/p; i++) {
					for (j=0; j < N/p; j++) {
						C[i * N/p + j] += bufferA[i * N/p + k] * B[k * N/p + j];
					}
				}
			}
		}
		else if (currRank == col) {
			MPI_Recv(bufferB, (N/p)*(N/p), MPI_DOUBLE, (p * currRank) + col, p, MPI_COMM_WORLD, &status);
			for (k=0; k < N/p; k++) {
				for (i=0; i < N/p; i++) {
					for (j=0; j < N/p; j++) {
						C[i * N/p + j] += A[i * N/p + k] * bufferB[k * N/p + j];
					}
				}
			}
		}
		else {
			MPI_Recv(bufferA, (N/p)*(N/p), MPI_DOUBLE, currRank + p*row, p, MPI_COMM_WORLD, &status);
			MPI_Recv(bufferB, (N/p)*(N/p), MPI_DOUBLE, (p * currRank) + col, p, MPI_COMM_WORLD, &status);
			for (k=0; k < N/p; k++) {
				for (i=0; i < N/p; i++) {
					for (j=0; j < N/p; j++) {
						C[i * N/p + j] += bufferA[i * N/p + k] * bufferB[k * N/p + j];
					}
				}
			}
		}
	}
	
	//Summation
	for (i=0; i < N/p; i++) {
		for (j=0; j < N/p; j++) {
			partialSum += C[i * N/p + j];
		}
	}
	
	MPI_Reduce(&partialSum, &finalSum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

	//Verification
	if (my_rank == 0) {
		printf("The final sum is: %lf\n", finalSum);
		printf("The formula calculated sum is: %lf\n", (((double)N*N*N)*(N-1)*(N-1))/2);
		fprintf(stdout,"TOTAL RUNTIME: %f\n", MPI_Wtime() - start_time);
	}
	
	MPI_Finalize();
	return(0);
}
