#ifndef CRYPT_H
#define CRYPT_H

#include "utils.h"

void crypt(unsigned int& i);

void crypt(unsigned int& i, const std::string& passkey);

void crypt(bytevector& v, const std::string& val);

#endif // CRYPT_H
