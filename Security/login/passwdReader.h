/*
 * passwdReader.h
 *
 *  Created on: Apr 24, 2016
 *      Author: bitblade
 */

#ifndef PASSWDREADER_H_
#define PASSWDREADER_H_

#define PASSWD_FILE "/etc/passwd"
#define MAX_PASSWD_LINE_LENGTH 1024

#include "passwdEntry.h"


int findEntry(char* username, passwdEntry_t *entry);

#endif /* PASSWDREADER_H_ */