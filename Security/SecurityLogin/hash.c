#include "hash.h"

#include <openssl/evp.h>
#include <openssl/ossl_typ.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

void padPasswdString(char hashString[PASSWD_STRING_LENGTH + 1]);
int calculate_md5(char *message, size_t length, unsigned char hash[MD5_HASH_LENGTH]);

int calculate_md5(char *message, size_t length, unsigned char hash[MD5_HASH_LENGTH]){
	EVP_MD_CTX mdctx = EVP_MD_CTX_new();
	const EVP_MD *md;
	unsigned char md_value[EVP_MAX_MD_SIZE];
	int md_len;

	OpenSSL_add_all_digests();

	md = EVP_get_digestbyname("md5");
	if(!md){
		fprintf(stderr, "System does not support md5 digest!\n");
		return -15;
	}

	EVP_MD_CTX_init(&mdctx);
	EVP_DigestInit_ex(&mdctx, md, NULL);
	EVP_DigestUpdate(&mdctx, message, length);
	EVP_DigestFinal_ex(&mdctx, md_value, (unsigned*)&md_len);
	EVP_MD_CTX_cleanup(&mdctx);

	memcpy(hash, md_value, 16);

	return 0;
}

// |MD5|rounds=1|salt=''|5f4dcc3b5aa765d61d8327deb882cf99|>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// |MD5|rounds=1|salt=''|319f4d26e3c536b5dd871bb2c52e3178|PADDING

//TODO Implement salt
//TODO Implement multiple rounds
int createMD5HashString(char hashString[PASSWD_STRING_LENGTH + 1], char *password){
	unsigned char hash[MD5_HASH_LENGTH];
	char *ptr = hashString;

	if(calculate_md5(password, strlen(password), hash) != 0){
		return -1;
	}

	memcpy(ptr, "|MD5|rounds=1|salt=''|", 22);
	ptr += 22;

	for(int i=0; i<MD5_HASH_LENGTH; i++){
		sprintf(ptr, "%02x", hash[i]);
		ptr += 2;
	}

	*ptr = 0;

	padPasswdString(hashString);
	return 0;
}

void padPasswdString(char hashString[PASSWD_STRING_LENGTH + 1]){
	size_t initLen = strlen(hashString);

	if(initLen < PASSWD_STRING_LENGTH){
		hashString[initLen] = '|';
	}

	for(size_t i = initLen + 1; i<PASSWD_STRING_LENGTH; i++){
		hashString[i] = '>';
	}

	hashString[PASSWD_STRING_LENGTH] = 0;
}