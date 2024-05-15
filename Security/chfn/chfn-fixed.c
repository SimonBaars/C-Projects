#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>

#define PASSWD_FILE "/etc/passwd"
#define UID_COLUMN 		2
#define MAX_UID_DIGITS 	10
#define GECOS_COLUMN 	7


int fatal(char* message);
int findDelimiter(FILE *passwdFile, char delimiter, FILE *output);
int findEOL(FILE *passwdFile);
int main(int argc, char *argv[]);
int testUid(FILE *passwd);
int findGecosEnd(FILE *passwdFile, FILE *output);
void setFinger(FILE *passwd);
void printFinger(FILE *passwd);

int main(int argc, char *argv[]){
	FILE *passwd;

	passwd = fopen(PASSWD_FILE, "r+");
	(passwd == NULL) && fatal("Unable to open " PASSWD_FILE "\n");

	while(!feof(passwd)){
		findDelimiter(passwd, ':', NULL);
		findDelimiter(passwd, ':', NULL);
		if(testUid(passwd) == 1){ break; }
		findEOL(passwd);
	}
	feof(passwd) && fatal("User record not found\n");

	for(int i=UID_COLUMN + 1; i<GECOS_COLUMN; i++){
		findDelimiter(passwd, ':', NULL) && fatal("User record not found\n");
	}

	if(isatty(fileno(stdin))){
		printFinger(passwd);
	}else{
		setFinger(passwd);
	}

	return 0; //unreachable
}

void printFinger(FILE *passwd){
	fputs("Current finger information:\n", stdout);
	findGecosEnd(passwd, stdout);

	fputs("\n\nUse pipe to update this information:\n"
			"     echo -ne \"<new information>\" | chfn\n\n", stdout);

	fclose(passwd);

	exit(2);
}

void setFinger(FILE *passwd){
	int i = 0; char c = 0;
	while((c = fgetc(stdin)) != EOF){
		// Modification by Chin Jitae: Made chfn compatible with newer 128 byte
		// GECOS fields
		i > 128 && fatal("Input too long\n");
		if(isatty(fileno(stdin)) && c == '\n'){ break; }
		if(!isalnum(c))
			continue; // We don't accept non alphanumeric characters!
		i++, fputc(c, passwd) != EOF || fatal("Unable to write\n");
	}

	ferror(stdin) && fatal("Unable to read new information\n");
	printf("%d written\n", i);

	if(i < 64){
		fputc('|', passwd) == EOF && fatal("Unable to write\n");
		i++;
	}

	while(i < 64){i++, fputc('>', passwd) == EOF && fatal("Unable to write\n");}

	fclose(passwd);
	exit(0);
}

int findGecosEnd(FILE *passwdFile, FILE *output){
	int c = 0;
	while((c = fgetc(passwdFile)) != EOF){
		if((unsigned char)c == '\n' || (unsigned char)c == '|'){ break; }
		output == NULL || fputc(c, output) != EOF || fatal("Unable to write\n");
	}

	if(ferror(passwdFile)){	fatal("Error while reading " PASSWD_FILE "\n");	}
	if(feof(passwdFile)){ return EOF; }
	return 0;
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

	ferror(passwd) && fatal("Error while reading " PASSWD_FILE "\n");
	if(feof(passwd)){ return EOF; }

	c == ':' || fatal("Cannot read passwd entry\n");

	return (strtoul(buffer, NULL, 10) == uid);
}


int findEOL(FILE *passwdFile){
	return findDelimiter(passwdFile, '\n', NULL);
}

int findDelimiter(FILE *passwdFile, char delimiter, FILE *output){
	int c = 0;
	while((c = fgetc(passwdFile)) != EOF){
		if((unsigned char)c == delimiter){ break; }
		output == NULL || fputc(c, output) != EOF || fatal("Unable to write\n");
	}

	if(ferror(passwdFile)){	fatal("Error while reading " PASSWD_FILE "\n");	}
	if(feof(passwdFile)){ return EOF; }
	return 0;
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