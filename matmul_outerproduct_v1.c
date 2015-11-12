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
	int i,j,k, my_rank, num_procs, p, row, col, currRank, partialSum, finalSum;
	
	MPI_Group orig_group, row_group, col_group;
	MPI_Comm row_comm, col_comm;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
	
	p = sqrt(num_procs);
	
	double A[N/p][N/p];
	double B[N/p][N/p];
	double C[N/p][N/p];
	double bufferA[N/p][N/p];
	double bufferB[N/p][N/p];
	
	row = my_rank / p;
	col = my_rank % (int)p;
	
	// Fill in the arrays A & B
	for (i=0; i < N/p; i++) {
		for (j=0; j < N/p; j++) {
			A[i][j] = (row * N/p) + i;
			B[i][j] = ((row * N/p) + i) + ((col * N/p) + j);
		}
	}
	
	printf("Block of A on rank %d at coordinates (%d,%d)\n", my_rank, row, col);
	for (i=0; i < N/p; i++) {
		for (j=0; j < N/p; j++) {
			printf("%f   ", A[i][j]);
		}
		printf("\n");
	}
	
	printf("Block of B on rank %d at coordinates (%d,%d)\n", my_rank, row, col);
	for (i=0; i < N/p; i++) {
		for (j=0; j < N/p; j++) {
			printf("%f   ", B[i][j]);
		}
		printf("\n");
	}
	
	//Split necessary processes into new groups, for row and column
	//MPI_Comm_group(MPI_COMM_WORLD, &orig_group);
	
	//Broadcast to A to row and B to column
	MPI_Status status;
	for (currRank = 0; currRank < p; currRank++) {
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
		if ((currRank == row) && (currRank == col)) {
			for (k=0; k < N/p; k++) {
				for (i=0; i < N/p; i++) {
					for (j=0; j < N/p; j++) {
						C[i][j] += A[i][k] * B[k][j];
						printf("1 i:%d; j:%d; k:%d; currRank:%d; my_rank:%d; %f * %f\n", i,j,k, currRank, my_rank, A[i][k], B[k][j]);
					}
				}
			}
			//printf("[X][X] Process: %d; currRank: %d\n", my_rank, currRank);
		}
		else if (currRank == row) {
			MPI_Recv(bufferA, (N/p)*(N/p), MPI_DOUBLE, currRank + p*row, p, MPI_COMM_WORLD, &status);
			for (k=0; k < N/p; k++) {
				for (i=0; i < N/p; i++) {
					for (j=0; j < N/p; j++) {
						C[i][j] += bufferA[i][k] * B[k][j];
						printf("2 i:%d; j:%d; k:%d; currRank:%d; my_rank:%d; %f * %f\n", i,j,k, currRank, my_rank, bufferA[i][k], B[k][j]);
					}
				}
			}
			printf("Block of bufferA on rank %d at process %d\n", my_rank, currRank);
			for (i=0; i < N/p; i++) {
				for (j=0; j < N/p; j++) {
					printf("%f   ", bufferA[i][j]);
				}
				printf("\n");
			}
			//printf("BUFFERA Process: %d; currRank: %d\n", my_rank, currRank);
		}
		else if (currRank == col) {
			MPI_Recv(bufferB, (N/p)*(N/p), MPI_DOUBLE, (p * currRank) + col, p, MPI_COMM_WORLD, &status);
			for (k=0; k < N/p; k++) {
				for (i=0; i < N/p; i++) {
					for (j=0; j < N/p; j++) {
						C[i][j] += A[i][k] * bufferB[k][j];
						printf("3 i:%d; j:%d; k:%d; currRank:%d; my_rank:%d; %f * %f\n", i,j,k, currRank, my_rank, A[i][k], bufferB[k][j]);
					}
				}
			}
			//printf("BUFFERB Process: %d; currRank: %d\n", my_rank, currRank);
		}
		else {
			MPI_Recv(bufferA, (N/p)*(N/p), MPI_DOUBLE, currRank + p*row, p, MPI_COMM_WORLD, &status);
			MPI_Recv(bufferB, (N/p)*(N/p), MPI_DOUBLE, (p * currRank) + col, p, MPI_COMM_WORLD, &status);
			for (k=0; k < N/p; k++) {
				for (i=0; i < N/p; i++) {
					for (j=0; j < N/p; j++) {
						C[i][j] += bufferA[i][k] * bufferB[k][j];
						printf("4 i:%d; j:%d; k:%d; currRank:%d; my_rank:%d; %f * %f\n", i,j,k, currRank, my_rank, bufferA[i][k], B[k][j]);
					}
				}
			}
			//printf("BUFFERA, BUFFERB Process: %d; currRank: %d\n", my_rank, currRank);
		}
	}
	printf("Block of C on rank %d at process %d\n", my_rank, currRank);
	for (i=0; i < N/p; i++) {
		for (j=0; j < N/p; j++) {
			printf("%f   ", C[i][j]);
			partialSum += C[i][j];
		}
		printf("\n");
	}
	printf("partialSum = %d, rank is %d\n", partialSum, my_rank);
	MPI_Reduce(&partialSum, &finalSum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

	if (my_rank == 0) {
		printf("The final sum is: %d\n", finalSum);
		printf("The formula calculated sum is: %d\n", ((N*N*N)*(N-1)*(N-1))/2);
	}
	
	MPI_Finalize();
	return(0);
}
