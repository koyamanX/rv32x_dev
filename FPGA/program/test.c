#define SIZE 32

void main(void)
{
    int a[SIZE][SIZE];
    int i, j;
    while (1)
    { // avoid caching
        for (i = 0; i < SIZE; i++)
        {
            for (j = 0; j < SIZE; j++)
            {
                a[j][i] = 0;
            }
        }
    }
}