#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define BUF_SIZE (256)

int string_equal(char *a, char *b)
{
    size_t i;
    for (i = 0; a[i] != '\0' && b[i] != '\0'; ++i) {
        if (a[i] != b[i]) return 0;
    }
    return a[i] == b[i];
}

int string_equal_loop(char *a, char *b, size_t iterations)
{
    volatile int res;
    for (volatile size_t i = 0; i < iterations; ++i) {
        res = string_equal(a, b);
    }
    res = string_equal(a, b);
    return res;
}

int main(int argc, char *argv[])
{
    char key[BUF_SIZE];
    FILE *fptr;
    long double time=0;
    struct timespec ts, end;

    if (argc < 2) {
        printf("Usage: '%s $key'\n", argv[0]);
        return 1;
    }

    if (!(fptr = fopen("key", "r"))) {
        fprintf(stderr, "Error! opening file");
        return 1;
    }

    fscanf(fptr, "%[^\n]", key);

    fclose(fptr);

    for (size_t i = 0; key[i] != '\0'; ++i) {
        // Only allow lowercase letters because our checker is awesome!
        assert(key[i] >= 'a' && key[i] <= 'z');
    }

    if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts)) {
        return 1;
    }
    time = (ts.tv_sec + ((long double)ts.tv_nsec / 100000000L));

    if (string_equal_loop(key, argv[1], 10000000L)) {
        printf("You unlocked me!\n");
    } else {
        if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end)) {
            return 1;
        }
        time = (end.tv_sec + (((long double)end.tv_nsec) / 100000000L)) - time;
        printf("After %Lf seconds we found your key to be wrong!\n", time);
    }
}