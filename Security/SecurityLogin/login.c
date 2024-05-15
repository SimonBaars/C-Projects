#include "login.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <crypt.h>

#include "defines.h"
#include "hash.h"
#include "passwdEntry.h"
#include "passwdReader.h"

int checkPasswd(passwdEntry_t *entry, char* passwd);
ssize_t askPass(char *passwd, FILE *stream);
void printAsHex(char *c, size_t n);

int main() {
	char username[MAX_USERNAME_LENGTH];
	passwdEntry_t entry;

	int success = 0;
	for(int tries = LOGIN_TRIES; tries > 0; tries--){
		printf("login: ");
		scanf("%"STR(MAX_USERNAME_LENGTH)"s", username);

		if(username[0] == 0){ continue; }
		if(findEntry(username, &entry) == 0){
			success = 1;
		}

		char passwd[MAX_PASSWORD_LENGTH + 1];
		int passLen;

		if((passLen = askPass(passwd, stdin)) <= 0){
			warning("Failed to get password\n");
			success = 0;
		}

		int res;
		if(success && (res = checkPasswd(&entry, passwd)) != 1){
			success = 0;
		}

		if(success){ break; }

		warning("\nlogin incorrect\n");
	}

	if(!success){
		return fatal("Login failed\n");
	}

	if(entry.gid == 0 || entry.uid == 0){
		return fatal("Root login not allowed\n");
	}

//	printf("SUCCESS\n");
//	printf(" UNAME: %s\n",  entry.username);
//	printf("   UID: %d\n", entry.uid);
//	printf("   GID: %d\n", entry.gid);
//	printf(" GECOS: %s\n",  entry.gecos);
//	printf("  HOME: %s\n",  entry.home);
//	printf(" SHELL: %s\n",  entry.shell);
//	printf("\n\n");
//	fflush(stdout);
	chdir(entry.home);
	execl("/bin/su", "su", "--shell", entry.shell, entry.username, (char *)NULL); //Never returns on success
	perror("Error executing su");
	return 1;
}

int checkPasswd(passwdEntry_t *entry, char* passwd){
	if(entry->passwordHash.type == MD5_HASH){
		md5Hash_t testHash;
		memset(&testHash, 0, sizeof(md5Hash_t));
		testHash.rounds = 1;

		calculate_md5(passwd, strlen(passwd), &testHash);

//		printf(" HTYPE: %d\n", entry->passwordHash.type);
//		printf("ROUNDS: %d\n", entry->passwordHash.md5.rounds);
//		printf("  MD5S: ["); printAsHex(entry->passwordHash.md5.salt, 8); printf("]\n");
//		printf("  MD5H: ["); printAsHex(entry->passwordHash.md5.hash, 16);	printf("]\n");
//		printf("WILL BE COMPARED TO:\n");
//		printf("ROUNDS: %d\n", testHash.rounds);
//		printf("  MD5S: ["); printAsHex(testHash.salt, 8); printf("]\n");
//		printf("  MD5H: ["); printAsHex(testHash.hash, 16);	printf("]\n");

		if(memcmp(&entry->passwordHash.md5, &testHash, sizeof(md5Hash_t)) == 0){
			return 1;
		}

	}else if(entry->passwordHash.type == SHA512_HASH){
		char *testHash;
		testHash = crypt(passwd, entry->passwordHash.sha512.cryptString);
		if(strcmp(testHash, entry->passwordHash.sha512.cryptString) == 0){
			return 1;
		}
	}else{
		return -100;
	}

	return 0;
}

ssize_t askPass(char *passwd, FILE *stream)
{
  struct termios old, new;

  /* Turn echoing off and fail if we can’t. */
  if (tcgetattr (fileno (stream), &old) != 0)
    return -1;
  new = old;
  new.c_lflag &= ~ECHO;
  if (tcsetattr (fileno (stream), TCSAFLUSH, &new) != 0)
    return -1;

  /* Read the password. */
  printf("Password: ");
  scanf("%"STR(MAX_PASSWORD_LENGTH)"s", passwd);
  printf("\n");

  /* Restore terminal. */
  (void) tcsetattr (fileno (stream), TCSAFLUSH, &old);

  return strlen(passwd);
}

void printAsHex(char *c, size_t n){
	for(size_t i=0; i<n; i++){
		printf("%x", (unsigned char)c[i]);
	}
}

int fatal(char* error){
	fputs(error, stderr);
	fflush(stderr);
	sleep(1);
	exit(1);
	return -1; //unreachable
}

void warning(char* warning){
	fputs(warning, stderr);
	fflush(stderr);
}
