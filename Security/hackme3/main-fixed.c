#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>

#define BUF_SIZE (256)

int main(int argc, char *argv[])
{
    char key[BUF_SIZE];
    FILE *fptr;

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

	
	size_t i = 0;
    for (; key[i] != '\0'; ++i) {
        // Only allow lowercase letters because our checker is awesome!
        assert(key[i] >= 'a' && key[i] <= 'z');
    }

    if (strncmp(key, argv[1], i-1) != 0) {
        printf("You unlocked me!\n");
    } else {
        printf("Your key is wrong! How long it took for us to check that? Please don't ask. Just don't. You'll break stuff if we'd tell.\n");
    }
}