#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define CHUNK_SIZE 10000000

int main()
{
    FILE *fp = fopen("random_numbers.bin", "wb");
    if (!fp) {
        printf("Error opening random_numbers.bin file\n");
        return 1;
    }

    srand(time(NULL));

    for (int chunk = 0; chunk < ARRAY_VALUE / CHUNK_SIZE; chunk++) {
        int *data = (int *)malloc(CHUNK_SIZE * sizeof(int));
        if (!data) {
            printf("Error allocating memory\n");
            fclose(fp);
            return 1;
        }

        for (int i = 0; i < CHUNK_SIZE; i++) {
            data[i] = rand();
        }

        fwrite(data, sizeof(int), CHUNK_SIZE, fp);
        free(data);
    }

    int remainder = ARRAY_VALUE % CHUNK_SIZE;
    if (remainder > 0) {
        int *data = (int *)malloc(remainder * sizeof(int));
        if (!data) {
            printf("Error allocating memory for remainder\n");
            fclose(fp);
            return 1;
        }

        for (int i = 0; i < remainder; i++) {
            data[i] = rand();
        }

        fwrite(data, sizeof(int), remainder, fp);
        free(data);
    }

    fclose(fp);
    return 0;
}
