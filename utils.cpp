#include <iostream>

#include "utils.h"

//class bytevector : public vector<char>
//{
//public:
//  bytevector():vector<char>(){}
//  bytevector(unsigned int i):vector<char>(i){}
//  bytevector(basic_string<char>::iterator b, basic_string<char>::iterator e):vector<char>(b,e){}
//  bytevector(basic_string<char>::const_iterator b, basic_string<char>::const_iterator e):vector<char>(b,e){}
//  void append(const unsigned int& i)
//  {
//    insert(end(), (char*)&i, ((char*)&i) + sizeof(i));
//  }
//  void append(const vector<char>& v)
//  {
//    insert(end(), v.begin(), v.end());
//  }
//};

using namespace std;

bytevector::bytevector():vector<char>(){}

bytevector::bytevector(unsigned int i):vector<char>(i){}

bytevector::bytevector(basic_string<char>::iterator b, basic_string<char>::iterator e):vector<char>(b,e){}

bytevector::bytevector(basic_string<char>::const_iterator b, basic_string<char>::const_iterator e):vector<char>(b,e){}

void bytevector::append(const unsigned int& i)
{
  insert(end(), (char*)&i, ((char*)&i) + sizeof(i));
}

void bytevector::append(const vector<char>& v)
{
  insert(end(), v.begin(), v.end());
}

static const char alphanum[] =
    "0123456789"
    "!@#$%^&*_"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz";
char mk_rand_ch()
{
  return alphanum[rand() % sizeof(alphanum)];
}

#define MAX_SALT_LEN 0xff
#define MIN_SALT_LEN 0x8f

string mk_salt()
{
  long l = MIN_SALT_LEN + (rand() % (MAX_SALT_LEN - MIN_SALT_LEN));
  string res;
  res.reserve(MAX_SALT_LEN);
  while(l--)
  {
    res.push_back(mk_rand_ch());
  }
  return res;
}

unsigned int mk_hash(const string& s)
{
  unsigned int res = 0;
  const int l = s.length();
  for(int i = 0; i < l; ++i)
  {
    res *= OBF;
    res += (unsigned int)(s[i]);
  }
  return res;
}

string prompt(const string& msg)
{
  string s;
  while(s.empty())
  {
    std::cerr << msg;
    std::cin >> s;
  }
  return s;
}
















