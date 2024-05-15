#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

#define PASSWD_FILE "/etc/passwd"
#define GROUP_FILE "/etc/group"
#define UID_COLUMN 		2
#define GECOS_COLUMN 	7
// This guest group should be a newly created group that no other user uses.
#define GUEST_GROUP 		50
#define GUEST_PWD_HASH "|MD5|rounds=1|salt=''|084e0343a0486ff05530df6c705c8bb4|>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
#define GUEST_SHELL "/bin/42sh"
#define GUEST_GECOS "Guest account|>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"


int fatal(char* message);
int findDelimiter(FILE *passwdFile, char delimiter);
int8_t findHighestuid(FILE *passwd);
int findUidColumn(FILE *passwd);
int findEOL(FILE *passwdFile);
int main(int argc, char *argv[]);
unsigned int readUInt(FILE *passwd);
void writeEntry(FILE *passwd, FILE *group, int uid);


int main(int argc, char *argv[]){
	FILE *passwd;
	FILE *group;

	passwd = fopen(PASSWD_FILE, "r+");
	if(passwd == NULL){ fatal("Unable to open " PASSWD_FILE "\n"); }

	group = fopen(GROUP_FILE, "a");
	if(group == NULL){ fatal("Unable to open " GROUP_FILE "\n"); }

	int8_t newuid = findHighestuid(passwd) + 1;

	//Make sure we can never accidentally create a root account...
	if(newuid < 1){
		fatal("No free UID for new user.");
	}

	if(fseek(passwd, 0, SEEK_END)){
		fatal("Could not seek");
	}

	writeEntry(passwd, group, newuid);
	printf("New guest user created\nUsername: guest_%d\nPassword: guest\n\n", newuid);

	fclose(passwd);
	fclose(group);

	return 0;
}

void writeEntry(FILE *passwd, FILE *group, int uid){
	int res;

	res = fprintf(passwd, "guest_%d:%s:%d:%d::/:%s:%s\n",
			uid, GUEST_PWD_HASH, uid, uid,	GUEST_SHELL, GUEST_GECOS);
	if(res < 0){
		fatal("Unable to write entry to " PASSWD_FILE "\n");
	}

	res = fprintf(group, "guest_%d:x:%d:", uid, GUEST_GROUP);
	if(res < 0){
		fatal("Unable to write entry to " GROUP_FILE "\n");
	}
}

int8_t findHighestuid(FILE *passwd){
	int8_t highest = 0;
	while(!feof(passwd)){
		if(findUidColumn(passwd)){
			if(feof(passwd)){break;}
			fatal("Invalid passwd record\n");
		}
		int8_t tmp = (int8_t)readUInt(passwd);
		if(tmp > highest){ highest = tmp; }
		findEOL(passwd);
	}

	return highest;
}

int findUidColumn(FILE *passwd){
	for(int i=0; i<UID_COLUMN; i++){
		int res = findDelimiter(passwd, ':');
		if(res){ return res; }
	}
	return 0;
}

unsigned int readUInt(FILE *passwd){
	char buffer[10] = {0};

	int c;
	for(int i=0; i<10; i++){
		c = fgetc(passwd);
		if(c < '0' || c > '9'){ break; }

		buffer[i] = c;
 	}

	if(ferror(passwd)){ fatal("Error while reading " PASSWD_FILE "\n"); }
	if(feof(passwd)){ return EOF; }
	if(c != ':'){ fatal("Invalid passwd entry\n"); }

	return (unsigned int)strtoul(buffer, NULL, 10);
}


int findEOL(FILE *passwdFile){
	return findDelimiter(passwdFile, '\n');
}

int findDelimiter(FILE *passwdFile, char delimiter){
	int c = 0;
	while((c = fgetc(passwdFile)) != EOF){
		if((unsigned char)c == delimiter){ break; }
		if((unsigned char)c == '\n'){ fatal("Invalid passwd entry\n"); }
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