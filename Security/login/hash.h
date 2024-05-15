#ifndef HASH_H_
#define HASH_H_

#include <stddef.h>

#define PASSWD_STRING_LENGTH 106

#define MD5_HASH_LENGTH 16
#define MD5_STRING_CHARS 55
#define MD5_STRING_PADDING_CHARS PASSWD_STRING_LENGTH - MD5_STRING_CHARS

int createMD5HashString(char hashString[PASSWD_STRING_LENGTH], char *password);


#endif /* HASH_H_ */