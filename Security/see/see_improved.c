/* See LICENSE file for copyright and license details. */

#include <limits.h>
#include <linux/limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void concatenate(FILE *fp1, FILE *fp2);
char* getFullPath(char* input);
void illegalPath(char* path);
void testPermission(char *path);

int main(int argc, char *argv[])
{
	FILE *fp;

	if (argc == 1) {
		printf("Usage: %s <FILE> [FILE]...\n", argv[0]);
	} else {
		for (argv++; *argv; argv++) {

			char* path = getFullPath(*argv);
			testPermission(path);

			if (!(fp = fopen(*argv, "r"))) {
				perror("Unable to open file");
				exit(1);
			}

			concatenate(fp, stdout);

			if (fclose(fp)) {
				perror("Error while closing file pointer");
				exit(1);
			}
		}
	}

	return 0;
}

char* getFullPath(char *input){
	static char path[PATH_MAX];

	size_t inputLen = strlen(input);

	// Avoid buffer overflow
	if(inputLen >= PATH_MAX){
		fprintf(stderr, "Input too long: %s", input);
		exit(1);
	}

	// Test if input is an absolute path
	if(*input == '/'){
		strcpy(path, input);
		return path;
	}

	// If not, prefix it with the current path
	if(getcwd(path, PATH_MAX - 1) == NULL){
		perror("Could not get working path");
		exit(1);
	}

	if(path[strlen(path)-1] != '/'){
		path[strlen(path)] = '/';
		path[strlen(path)+1] = 0;
	}

	// Avoid buffer overflow
	if(strlen(path) + inputLen >= PATH_MAX){
		fprintf(stderr, "Input too long: %s", input);
		exit(1);
	}

	strncat(path, input, PATH_MAX -1);

	return path;
}

/*
 * 	TODO
 * 		Should only allow to see in own home directory.
 * 		But how do we detect the current users login name?
 *
 * 		For now, the most important account must be protected:
 * 			/home/sarm_mina
 */
void testPermission(char *path){
	if(strcmp(getlogin(), "sarm_mina") == 0){
		illegalPath(path);
	}
}

void illegalPath(char* path){
	fputs("\033[0;5;41m     Illegal access attempt     \033[0m\n", stderr);
	fprintf(stderr,
			"\033[0;31mAttempt to access file %s has been recorded\033[0m\n",
			path);
	exit(1);
}

void concatenate(FILE *fp1, FILE *fp2)
{
	char buffer[BUFSIZ];
	size_t count;

	while ((count = fread(buffer, 1, BUFSIZ, fp1)) > 0) {
		if(fwrite(buffer, 1, count, fp2) != count){
			perror("Could not write data");
			exit(1);
		}
	}
}