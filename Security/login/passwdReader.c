#include "passwdReader.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "defines.h"
#include "hash.h"
#include "login.h"

static const char stopSet[] = "\n:";

int parseUsername(char *username, char **input);
int parseString(char *str, size_t maxLength, char **input);
int parsePassword(hash_t *hash, char **input);
int parseUnsigned(int *u, char **input);
int parseEmtpy(char **input);
int parseHome(char *home, char **input);
int parseShell(char *shell, char **input);
int parseGecos(char *gecos, char **input);
int parseEntry(passwdEntry_t *entry, char* str);
int isEntry(char* username, char* entryString);
int findEntry(char* username, passwdEntry_t *entry);

int findEntry(char* username, passwdEntry_t *entry){
	char lineBuffer[MAX_PASSWD_LINE_LENGTH];
	FILE *passwdFile;

	passwdFile = fopen(PASSWD_FILE, "r");
	if(passwdFile == NULL)
	{
		fatal("Unable to open passwd file\n");
	}

	while(fgets(lineBuffer, MAX_PASSWD_LINE_LENGTH, passwdFile) != NULL)
	{
		int ret;
		if((ret = parseEntry(entry, lineBuffer)) < 0){
			fatal("Invalid passwd format\n");
		}
		if(strcmp(username, entry->username) == 0){	break; }
	}

	if(feof(passwdFile)){
		fclose(passwdFile);
		return -1;
	}

	fclose(passwdFile);
	return 0;
}

int isEntry(char* username, char* entryString){
	for(int i=0; i<MAX_USERNAME_LENGTH; i++){
		if(entryString[i] == ':'){
			return (username[i] == 0);
		}

		if(username[i] != entryString[i]){
			return 0;
		}
	}
	return 0;
}

int parseEntry(passwdEntry_t *entry, char* str){
	char *ptr = str;

	memset(entry, 0, sizeof(passwdEntry_t));

	if(parseUsername(entry->username, &ptr)){ return -101; }
	if(parsePassword(&entry->passwordHash, &ptr)){ return -102; }
	if(parseUnsigned(&entry->uid, &ptr)){ return -103; }
	if(parseUnsigned(&entry->gid, &ptr)){ return -104; }
	if(parseEmtpy(&ptr)){ return -105; }
	if(parseHome(entry->home, &ptr)){ return -106; }
	if(parseShell(entry->shell, &ptr)){ return -107; }
	if(parseGecos(entry->gecos, &ptr)){ return -108; }

	return 0;
}

int parseUsername(char *username, char **input){
	return parseString(username, MAX_USERNAME_LENGTH, input);
}

int parseString(char *str, size_t maxLength, char **input){
	for(size_t i=0; i<maxLength; i++, (*input)++){
		if(**input == ':' || **input == 0 || **input == '\n'){ break; }
		str[i] = **input;
	}
	if(**input != ':'){ return -1; }

	(*input)++;
	return 0;
}

int parsePassword(hash_t *hash, char **input){
	size_t length;
	char copy[PASSWD_STRING_LENGTH+1] = {0};

	length = strcspn (*input, stopSet);
	if(length != PASSWD_STRING_LENGTH || (*input)[length] != ':'){
		return -1;
	}

	strncpy(copy, *input, PASSWD_STRING_LENGTH);
	*input += PASSWD_STRING_LENGTH + 1;
	return parsePasswordHashString(copy, hash);
}

int parseUnsigned(int *u, char **input){
	char *tailPtr;
	*u = strtoul(*input, &tailPtr, 10);
	if(*tailPtr != ':'){ return -1; };

	*input = tailPtr + 1;
	return 0;
}

int parseEmtpy(char **input){
	if(**input != ':'){ return -1; };

	(*input)++;
	return 0;
}

int parseHome(char *home, char **input){
	return parseString(home, MAX_HOME_PATH_LENGTH, input);
}

int parseShell(char *shell, char **input){
	return parseString(shell, MAX_SHELL_PATH_LENGTH, input);
}

int parseGecos(char *gecos, char **input){
	for(size_t i=0; i<GECOS_LENGTH; i++, (*input)++){
		if(**input == ':' || **input == 0 || **input == '\n'){ return -1; }
		gecos[i] = **input;
	}
	if(**input != '\n' && **input != 0){ return -1; }

	return 0;
}