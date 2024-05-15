/*
 * defines.h
 *
 *  Created on: Apr 22, 2016
 *      Author: bitblade
 */

#ifndef DEFINES_H_
#define DEFINES_H_

//Stringify macro's
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)


#define MAX_USERNAME_LENGTH 16
#define MAX_PASSWORD_LENGTH 16
#define MAX_ID_DIGITS 10
#define GECOS_LENGTH 64
#define MAX_SHELL_PATH_LENGTH 64
#define MAX_HOME_PATH_LENGTH 64

#define LOGIN_TRIES 3

#endif /* DEFINES_H_ */