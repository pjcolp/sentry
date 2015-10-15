#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define ARR_SIZE 64*1024*1024
int main(int argc, char **argv)
{
    // int *arr = (int *)malloc (ARR_SIZE);
    int *arr;
    int steps = ARR_SIZE; // Total numbers of accesses to perform
    int i, j, idx, fd, length;
    struct timeval t1, t2;

    if (argc < 2)
    {
        printf("cachetime <SIZE>\n");
        exit(-1);
    }

    fd = open ("/dev/aesonsoc-mem", O_RDWR);
    if (fd < 0)
    {
        printf("Memory device doesn't exist\n");
        exit(-1);
    }

    length = atoi(argv[1]);

    arr = mmap(NULL, length, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0); 

    memset(arr, 0, length);

    j = 256;    // size of array to traverse, in integers
    while (j <= length/4)
    {
        gettimeofday(&t1, NULL);
        for (i = 0; i < steps; i++)
        {
            // Accessing after 32 bytes (8 ints) should cause a miss every time
            // if array is larger than the available cache
            idx = (i * 8) % j;
            // printf ("%d\t", idx);
            arr[idx]++; 
        }
        // printf ("\n");
        gettimeofday(&t2, NULL);

        t2.tv_usec -= t1.tv_usec;
        t2.tv_sec -= t1.tv_sec;

        if (t2.tv_usec < 0)
        {
            t2.tv_usec += 1000000;
            t2.tv_sec -= 1;
        }

        printf ("Length = %d, time = %ld.%ld\n", j*4, t2.tv_sec, t2.tv_usec);

        if (j < 128*1024/4 || j >= 2*1024*1024/4)
        {
            j *= 2;
        }
        else
        {
            j += 128*1024/4;
        }
    }

}
