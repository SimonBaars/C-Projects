#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int allowed_to_see_secret = 0;

void not_the_secret() {
    printf("Too bad...");
}

void secret() { system("cat ~/flag.txt"); }

void print_me(char *string)
{
    // 512 characters should be enough, right?
    char buffer[512];
    strncpy(buffer, string, 512); // Yes, no buffer overflow possible!!

    // Those printf arguments are too hard!
    printf(buffer);

    if (allowed_to_see_secret) {
        secret();
    } else {
        not_the_secret();
    }
}

int main(int argc, char **argv)
{
    print_me(argv[1]);

    return 0;
}
