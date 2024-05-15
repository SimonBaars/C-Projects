#include <stdio.h>
#include <crypt.h>
#include <stdlib.h>
#include <string.h>

void secretFunction()
{
    system("cat /home/supervisor/flag.txt");
}

int scanner(const char *data, char *buffer, size_t buflen)
{
    char format[32];
    if (buflen == 0)
        return 0;
    snprintf(format, sizeof(format), "%%%ds", (int)(buflen-1));
    return sscanf(data, format, buffer);
}

void echo()
{
    char buffer[50];
    char * password = "$6$$1Vo4v9OMNnUF7xl6dsb8BNbymstnMGqE89e1unEzLik1LKjErq4muBB9mddmM73uHe2ldOb2qfOQsEDoPR81e/";
    printf("Enter some text:\n");
    scanner("%s", buffer, 50);
    if(strcmp(password,crypt(buffer,"$6$")) == 0){
	    secretFunction();
	}
    printf("You entered: %s\n", buffer);
}

int main()
{
    echo();

    return 0;
}
