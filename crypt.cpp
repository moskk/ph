
#include "crypt.h"

using namespace std;

void crypt(unsigned int& i)
{
  i ^= OBF;
}

void crypt(unsigned int& i, const string& passkey)
{
  i ^= mk_hash(passkey);
}

void crypt(bytevector& v, const string& val)
{
  const size_t lv = v.size();
  const size_t lk = val.length();
  for(size_t i = 0; i < lv; ++i)
  {
    v[i]^=val[i%lk];
  }
}
