#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

void merge(int *arr, int *buffer, int start, int mid, int end)
{
    int i = start, j = mid + 1, k = 0;
    while (i <= mid && j <= end)
    {
        buffer[k++] = (arr[i] <= arr[j]) ? arr[i++] : arr[j++];
    }
    while (i <= mid)
        buffer[k++] = arr[i++];
    while (j <= end)
        buffer[k++] = arr[j++];
    for (i = 0; i < k; i++)
        arr[start + i] = buffer[i];
}

void mergesort(int *arr, int *buffer, int start, int end)
{
    if (start < end)
    {
        int mid = start + (end - start) / 2;
        mergesort(arr, buffer, start, mid);
        mergesort(arr, buffer, mid + 1, end);
        merge(arr, buffer, start, mid, end);
    }
}

void parallel_mergesort(int *local_data, int local_size, int rank, int size, MPI_Comm comm)
{
    int *buffer = (int *)malloc(local_size * sizeof(int));
    mergesort(local_data, buffer, 0, local_size - 1);
    free(buffer);

    int step = 1;
    while (step < size)
    {
        if (rank % (2 * step) == 0)
        {
            if (rank + step < size)
            {
                int partner_size;
                MPI_Status status;
                MPI_Recv(&partner_size, 1, MPI_INT, rank + step, 0, comm, &status);

                int *partner_data = (int *)malloc(partner_size * sizeof(int));
                MPI_Recv(partner_data, partner_size, MPI_INT, rank + step, 0, comm, &status);

                int *merged_data = (int *)malloc((local_size + partner_size) * sizeof(int));
                int i = 0, j = 0, k = 0;
                while (i < local_size && j < partner_size)
                {
                    merged_data[k++] = (local_data[i] <= partner_data[j]) ? local_data[i++] : partner_data[j++];
                }
                while (i < local_size)
                    merged_data[k++] = local_data[i++];
                while (j < partner_size)
                    merged_data[k++] = partner_data[j++];

                free(local_data);
                free(partner_data);
                local_data = merged_data;
                local_size += partner_size;
            }
        }
        else
        {
            int partner = rank - step;
            MPI_Send(&local_size, 1, MPI_INT, partner, 0, comm);
            MPI_Send(local_data, local_size, MPI_INT, partner, 0, comm);
            break;
        }
        step *= 2;
    }

    if (rank == 0)
    {
        printf("Sorted Array: ");
        for (int i = 0; i < local_size; i++)
        {
            printf("%d ", local_data[i]);
        }
        printf("\n");
    }

    free(local_data);
}

int main(int argc, char **argv)
{
    int rank, size;
    int *array = NULL;
    int array_length = 0;
    int *local_data;
    int local_size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0)
    {
        FILE *file = fopen("random_numbers.bin", "rb");
        if (!file)
        {
            printf("Error opening file\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        fseek(file, 0, SEEK_END);
        array_length = ftell(file) / sizeof(int);
        rewind(file);

        array = (int *)malloc(array_length * sizeof(int));
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

    int chunk_size = (array_length + size - 1) / size;
    local_data = (int *)malloc(chunk_size * sizeof(int));
    MPI_Scatter(array, chunk_size, MPI_INT, local_data, chunk_size, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0)
        free(array);

    parallel_mergesort(local_data, chunk_size, rank, size, MPI_COMM_WORLD);

    MPI_Finalize();
    return 0;
}
