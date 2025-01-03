#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

void merge(int *arr, int *left, int left_count, int *right, int right_count, int rank)
{
    double merge_start_time = MPI_Wtime();
    int i = 0, j = 0, k = 0;
    while (i < left_count && j < right_count)
    {
        if (left[i] < right[j])
            arr[k++] = left[i++];
        else
            arr[k++] = right[j++];
    }
    while (i < left_count)
        arr[k++] = left[i++];
    while (j < right_count)
        arr[k++] = right[j++];
    double merge_end_time = MPI_Wtime();
    if (rank == 0)
    {
        printf("\nLOG: Merge time: %f seconds\n", merge_end_time - merge_start_time);
    }
}

void mergesort(int *arr, int n, int rank)
{
    if (n < 2)
        return;

    double mergesort_start_time = MPI_Wtime();

    int mid = n / 2;
    int *left = (int *)malloc(mid * sizeof(int));
    int *right = (int *)malloc((n - mid) * sizeof(int));

    for (int i = 0; i < mid; i++)
        left[i] = arr[i];
    for (int i = mid; i < n; i++)
        right[i - mid] = arr[i];

    mergesort(left, mid, rank);
    mergesort(right, n - mid, rank);
    merge(arr, left, mid, right, n - mid, rank);

    free(left);
    free(right);
    double mergesort_end_time = MPI_Wtime();
    if (rank == 0)
    {
        printf("\nLOG: Mergesort time: %f seconds\n", mergesort_end_time - mergesort_start_time);
    }
}

int main(int argc, char **argv)
{
    int *array = NULL;
    int array_length;
    int *local_array;
    int local_size;
    int *counts;
    int *displs;
    double start_time, end_time;

    MPI_Init(&argc, &argv);

    int number_of_process, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &number_of_process);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0)
    {
        FILE *file = fopen("random_numbers.bin", "rb");
        if (!file)
        {
            printf("Error opening random_numbers.bin file\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        fseek(file, 0, SEEK_END);
        long file_size = ftell(file);
        array_length = file_size / sizeof(int);
        fseek(file, 0, SEEK_SET);

        array = (int *)malloc(file_size);
        fread(array, sizeof(int), array_length, file);
        fclose(file);

        printf("Original Array: ");
        for (int i = 0; i < array_length; i++)
        {
            printf("%d ", array[i]);
        }
        printf("\n\n");
        printf("EOF");
        printf("\n\n");
    }

    MPI_Bcast(&array_length, 1, MPI_INT, 0, MPI_COMM_WORLD);

    counts = (int *)malloc(number_of_process * sizeof(int));
    displs = (int *)malloc(number_of_process * sizeof(int));

    int base_size = array_length / number_of_process;
    int extra = array_length % number_of_process;

    for (int i = 0; i < number_of_process; i++)
    {
        counts[i] = base_size + (i < extra ? 1 : 0);
        displs[i] = i > 0 ? displs[i - 1] + counts[i - 1] : 0;
    }

    local_size = counts[rank];
    local_array = (int *)malloc(local_size * sizeof(int));

    MPI_Scatterv(array, counts, displs, MPI_INT, local_array, local_size, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0)
        free(array);

    start_time = MPI_Wtime();

    mergesort(local_array, local_size, rank);

    int merge_steps = number_of_process;
    int *recv_array = NULL;
    int recv_size;

    for (int step = 1; step < number_of_process; step *= 2)
    {
        if (rank % (2 * step) == 0)
        {
            if (rank + step < number_of_process)
            {
                MPI_Recv(&recv_size, 1, MPI_INT, rank + step, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                recv_array = (int *)malloc(recv_size * sizeof(int));
                MPI_Recv(recv_array, recv_size, MPI_INT, rank + step, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                int *merged_array = (int *)malloc((local_size + recv_size) * sizeof(int));
                merge(merged_array, local_array, local_size, recv_array, recv_size, rank);
                free(local_array);
                free(recv_array);
                local_array = merged_array;
                local_size += recv_size;
            }
        }
        else
        {
            int dest = rank - step;
            MPI_Send(&local_size, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
            MPI_Send(local_array, local_size, MPI_INT, dest, 0, MPI_COMM_WORLD);
            free(local_array);
            break;
        }
    }

    end_time = MPI_Wtime();

    if (rank == 0)
    {
        printf("\nOrdened Array:\n");
        for (int i = 0; i < array_length; i++)
        {
            printf("%d ", local_array[i]);
        }
        printf("\n");
        printf("\nLOG:Total time: %f seconds\n", end_time - start_time);
        free(local_array);
    }

    free(counts);
    free(displs);

    MPI_Finalize();
    return 0;
}