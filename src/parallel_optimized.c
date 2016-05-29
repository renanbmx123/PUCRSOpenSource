#include "parallel_optimized.h"

int
main(int argc, char *argv[])
{
	int array[ARRAY_SIZE];
	int i;
	for (i = 0; i < ARRAY_SIZE /3; i++) {
//		printf("primeiro for - array[i] = %d\n", i);
		array[i] = i;
	}

	for (i = ARRAY_SIZE /3; i < ARRAY_SIZE * 2 / 3; i++) {
//		printf("segundo for - array[i] = %d\n", i - ARRAY_SIZE / 3);
		array[i] = i - ARRAY_SIZE / 3;
	}

	for (i = ARRAY_SIZE * 2 / 3; i < ARRAY_SIZE; i++) {
//		printf("terceiro for - array[i] = %d\n", i - ARRAY_SIZE * 2 / 3);
		array[i] = i - ARRAY_SIZE * 2 / 3;
	}


	printf("Root process unordered:\n");
	for (i = 0; i < ARRAY_SIZE; i++)
		printf("[%03d] ", array[i]);
	printf("\n");

	int *array2 = interleaving3(array, ARRAY_SIZE);

	printf("Root process ordered:\n");
	for (i = 0; i < ARRAY_SIZE; i++)
		printf("[%03d] ", array2[i]);
	printf("\n");


//	int my_rank;
//	int proc_n;
//	MPI_Status status;
//	int i;
//
//	MPI_Init(&argc, &argv);
//	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
//	MPI_Comm_size(MPI_COMM_WORLD, &proc_n);
//
//	if (my_rank == 0)
//	{
//		int array[ARRAY_SIZE];
//		for (i = 0; i < ARRAY_SIZE; i++)
//			array[i] = ARRAY_SIZE - i;
//
//#ifdef DEBUG
//		printf("Root process unordered:\n");
//		for (i = 0; i < ARRAY_SIZE; i++)
//			printf("[%03d] ", array[i]);
//		printf("\n");
//#endif
//
//		int *result = order(my_rank, array, ARRAY_SIZE, status);
//
//#ifdef DEBUG
//		printf("Root process ordered:\n");
//		for (i = 0; i < ARRAY_SIZE; i++)
//			printf("[%03d] ", result[i]);
//		printf("\n");
//#endif
//
//	}
//	else
//	{
//		int size;
//
//		MPI_Recv(&size, 1,
//				 MPI_INT, parent(my_rank), SIZE_TAG,
//				 MPI_COMM_WORLD, &status);
//
//		int *array = calloc(size, sizeof(int));
//
//		MPI_Recv(array, size,
//				 MPI_INT, parent(my_rank), SEND_DOWN_TAG,
//				 MPI_COMM_WORLD, &status);
//
//		if(is_leaf(size, ARRAY_SIZE, proc_n, my_rank))
//		{
//			bubble_sort(size, array);
//
//			MPI_Send(array, size,
//					 MPI_INT, parent(my_rank), SEND_UP_TAG,
//					 MPI_COMM_WORLD);
//		}
//		else
//		{
//			array = order(my_rank, array, size, status);
//			MPI_Send(array, size,
//					 MPI_INT, parent(my_rank), SEND_UP_TAG,
//					 MPI_COMM_WORLD);
//		}
//	}
//
//	MPI_Finalize();
	return 0;
}

int*
order(int my_rank, int array[], int size, MPI_Status status)
{
	int half_size = size / 2;
	// Send size
	MPI_Send(&half_size, 1,
			 MPI_INT, left_child(my_rank), SIZE_TAG,
			 MPI_COMM_WORLD);

	MPI_Send(&half_size, 1,
			 MPI_INT, right_child(my_rank), SIZE_TAG,
			 MPI_COMM_WORLD);

	// Send array
	MPI_Send(array, half_size,
			 MPI_INT, left_child(my_rank), SEND_DOWN_TAG,
			 MPI_COMM_WORLD);

	MPI_Send(array + half_size, half_size,
			 MPI_INT, right_child(my_rank), SEND_DOWN_TAG,
			 MPI_COMM_WORLD);

	// Receive response
	MPI_Recv(array, half_size,
			 MPI_INT, left_child(my_rank), SEND_UP_TAG,
			 MPI_COMM_WORLD, &status);

	MPI_Recv(array + half_size, half_size,
			 MPI_INT, right_child(my_rank), SEND_UP_TAG,
			 MPI_COMM_WORLD, &status);

	return interleaving(array, size);
}