/*
 * passwd_entry.h
 *
 *  Created on: Mar 30, 2016
 *      Author: bitblade
 */

#ifndef PASSWDENTRY_H_
#define PASSWDENTRY_H_

#include "defines.h"
#include "hash.h"

typedef struct {
	char 	username[MAX_USERNAME_LENGTH+1];
	hash_t 	passwordHash;
	int	 	uid;
	int  	gid;
	char 	gecos[GECOS_LENGTH+1];
	char 	home[MAX_HOME_PATH_LENGTH+1];
	char 	shell[MAX_SHELL_PATH_LENGTH+1];

} passwdEntry_t;

#endif /* PASSWDENTRY_H_ */