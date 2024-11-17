#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main()
{
    int *data = (int *)malloc(ARRAY_VALUE * sizeof(int));
    if (!data) {
        printf("Error allocating memory\n");
        return 1;
    }

    srand(time(NULL));

    for (int i = 0; i < ARRAY_VALUE; i++) {
        data[i] = rand();
    }

    FILE *fp = fopen("random_numbers.bin", "wb");
    if (!fp) {
        printf("Error opening random_numbers.bin file\n");
        free(data);
        return 1;
    }

    fwrite(data, sizeof(int), ARRAY_VALUE, fp);

    fclose(fp);
    free(data);

    return 0;
}
