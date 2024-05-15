#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>

#include "hash.h"

//Stringify macro's
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#define PASSWD_FILE "/etc/passwd"
#define UID_COLUMN 		2
#define MAX_UID_DIGITS 	10
#define GECOS_COLUMN 	7
#define MAX_PASSWORD_LENGTH 16

ssize_t askPass(char *passwd, char *prompt, FILE *stream);
int testDelimiter(FILE *passwdFile, char delimiter);
int findDelimiter(FILE *passwdFile, char delimiter);
int findEOL(FILE *passwdFile);
int testUid(FILE *passwd);
int fatal(char* message);

int main(){
	FILE *passwd;
	long hashPos;
	char newPassword[MAX_PASSWORD_LENGTH + 1];
	char buffer[MAX_PASSWORD_LENGTH + 1];


	askPass(buffer, "New password: ", stdin);
	askPass(newPassword, "Repeat: ", stdin);

	if(strcmp(buffer, newPassword) != 0){
		fatal("Passwords do not match\n");
	}

	passwd = fopen(PASSWD_FILE, "r+");
	if(passwd == NULL){fatal("Unable to open " PASSWD_FILE "\n"); };

	while(!feof(passwd)){
		findDelimiter(passwd, ':');
		if((hashPos = ftell(passwd)) < 0){ fatal("Unable to get file pos\n"); }
		findDelimiter(passwd, ':');
		long curPos;
		if((curPos = ftell(passwd)) < 0){ fatal("Unable to get file pos\n"); }
		if(curPos - hashPos != PASSWD_STRING_LENGTH + 1){
			if(feof(passwd)){ break; }
			fatal("Invalid passwd format\n");
		}
		if(testUid(passwd) == 1){ break; }
		findEOL(passwd);
	}

	if(feof(passwd)){
		fatal("User record not found\n");
	}

	char hashString[PASSWD_STRING_LENGTH + 1];
	if(createMD5HashString(hashString, newPassword) != 0){
		fatal("Unable to create hash\n");
	}

	if(fseek(passwd, hashPos, SEEK_SET) != 0){ fatal("Unable to set file pos\n"); }
	if(fputs(hashString, passwd) < 0){ fatal("Unable to write to file\n"); }

	fclose(passwd);

	puts("Password updated");

	return 0;
}

ssize_t askPass(char *passwd, char *prompt, FILE *stream)
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
  fputs(prompt, stdout);
  scanf("%"STR(MAX_PASSWORD_LENGTH)"s", passwd);
  fputs("\n", stdout);

  /* Restore terminal. */
  (void) tcsetattr (fileno (stream), TCSAFLUSH, &old);

  return strlen(passwd);
}

int testUid(FILE *passwd){
	char buffer[MAX_UID_DIGITS] = {0};
	uid_t uid = getuid();

	int c;
	for(int i=0; i<MAX_UID_DIGITS; i++){
		c = fgetc(passwd);
		if(c < '0' || c > '9'){ break; }

		buffer[i] = c;
 	}

	if(ferror(passwd)) {fatal("Error while reading " PASSWD_FILE "\n"); }
	if(feof(passwd)){ return EOF; }

	if(c != ':'){ fatal("Cannot read passwd entry\n"); }

	return (strtoul(buffer, NULL, 10) == uid);
}

int findEOL(FILE *passwdFile){
	return findDelimiter(passwdFile, '\n');
}
int findDelimiter(FILE *passwdFile, char delimiter){
	int res;
	while((res = testDelimiter(passwdFile, delimiter)) > 0){}
	return res;
}

int testDelimiter(FILE *passwdFile, char delimiter){
	int c = fgetc(passwdFile);
	if(c == EOF){
		if(ferror(passwdFile)){	fatal("Error while reading "PASSWD_FILE"\n"); }
		if(feof(passwdFile)){ return EOF; }
	}

	return !(c == delimiter);
}

int fatal(char* message){
	if(errno){
		perror(message);
	}else{
		fputs(message, stderr);
	}
	exit(1);

	//UNREACHABLE
	return 0;
}