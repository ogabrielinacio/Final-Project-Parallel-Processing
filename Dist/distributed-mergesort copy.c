#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

void merge(int* arr, int start, int mid, int end) {
    int n1 = mid - start + 1;
    int n2 = end - mid;
    int* left = (int*)malloc(n1 * sizeof(int));
    int* right = (int*)malloc(n2 * sizeof(int));

    for (int i = 0; i < n1; i++) left[i] = arr[start + i];
    for (int i = 0; i < n2; i++) right[i] = arr[mid + 1 + i];

    int i = 0, j = 0, k = start;
    while (i < n1 && j < n2) {
        if (left[i] <= right[j])
            arr[k++] = left[i++];
        else
            arr[k++] = right[j++];
    }

    while (i < n1) arr[k++] = left[i++];
    while (j < n2) arr[k++] = right[j++];

    free(left);
    free(right);
}

int* merge_local(int* arr1, int n1, int* arr2, int n2) {
    int* result = (int*)malloc((n1 + n2) * sizeof(int));
    int i = 0, j = 0, k;
    for (k = 0; k < n1 + n2; k++) {
        if (i >= n1) {
            result[k] = arr2[j++];
        } else if (j >= n2) {
            result[k] = arr1[i++];
        } else if (arr1[i] < arr2[j]) {
            result[k] = arr1[i++];
        } else {
            result[k] = arr2[j++];
        }
    }
    return result;
}

void mergesort(int *arr, int start, int end)
{
    if (start < end)
    {
        int mid = start + (end - start) / 2;
        mergesort(arr, start, mid);          
        mergesort(arr, mid + 1, end);        
        merge(arr, start, mid, end);  
    }
}

int main(int argc, char **argv)
{
    int number_of_elements;
    int* data = NULL;
    int chunk_size, own_chunk_size;
    int* chunk;
    double time_taken;
    MPI_Status status;

    int number_of_process, rank_of_process;
    int rc = MPI_Init(&argc, &argv);

    if (rc != MPI_SUCCESS) {
        MPI_Abort(MPI_COMM_WORLD, rc);
    }

    MPI_Comm_size(MPI_COMM_WORLD, &number_of_process);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank_of_process);


    if (rank_of_process == 0)
    {
        FILE *file = fopen("random_numbers.bin", "rb");
        if (!file)
        {
            printf("Error opening random_numbers.bin file\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        fseek(file, 0, SEEK_END);
        long file_size = ftell(file);
        number_of_elements = file_size / sizeof(int);
        fseek(file, 0, SEEK_SET);

        data = (int *)malloc(file_size);
        fread(data, sizeof(int), number_of_elements, file);
        fclose(file);

        printf("Original Array: ");
        for (int i = 0; i < number_of_elements; i++)
        {
            printf("%d ", data[i]);
        }
        printf("\n\n");
        printf("EOF");
        printf("\n\n");
    }

    MPI_Barrier(MPI_COMM_WORLD);
    time_taken = -MPI_Wtime();

    MPI_Bcast(&number_of_elements, 1, MPI_INT, 0, MPI_COMM_WORLD);

    chunk_size = (number_of_elements + number_of_process - 1) / number_of_process;

    chunk = (int *)malloc(chunk_size * sizeof(int));

    MPI_Scatter(data, chunk_size, MPI_INT, chunk, chunk_size, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank_of_process == 0) {
        free(data);
    }

    own_chunk_size = (number_of_elements >= chunk_size * (rank_of_process + 1)) ? chunk_size : (number_of_elements - chunk_size * rank_of_process);

    mergesort(chunk, 0, own_chunk_size);

    for (int step = 1; step < number_of_process; step *= 2)
    {
        if (rank_of_process % (2 * step) != 0)
        {
            MPI_Send(chunk, own_chunk_size, MPI_INT, rank_of_process - step, 0, MPI_COMM_WORLD);
            break;
        }
        if (rank_of_process + step < number_of_process)
        {
           int received_chunk_size = (number_of_elements >= chunk_size * (rank_of_process + 2 * step)) ? (chunk_size * step) : (number_of_elements - chunk_size * (rank_of_process + step));
            int* chunk_received = (int*)malloc(received_chunk_size * sizeof(int));
            MPI_Recv(chunk_received, received_chunk_size, MPI_INT, rank_of_process + step, 0, MPI_COMM_WORLD, &status);

            data = merge_local(chunk, own_chunk_size, chunk_received, received_chunk_size);

            free(chunk);
            free(chunk_received);
            chunk = data;
            own_chunk_size += received_chunk_size;
        }
    }

    time_taken += MPI_Wtime();

    if (rank_of_process == 0) {
        printf("\nOrdened Array: ");
        for (int i = 0; i < number_of_elements; i++) {
            printf("%d ", chunk[i]);
        }
        printf("\n");
        printf("\n");
        printf("\nMergeSort %d ints on %d procs: %f secs\n", number_of_elements, number_of_process, time_taken);

        free(chunk);
    }

    MPI_Finalize();
    return 0;
}