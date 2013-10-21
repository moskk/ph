#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <string>

#define OBF (unsigned int)(3533234449)

class bytevector : public std::vector<char>
{
public:
  bytevector();
  bytevector(unsigned int i);
  bytevector(std::basic_string<char>::iterator b, std::basic_string<char>::iterator e);
  bytevector(std::basic_string<char>::const_iterator b, std::basic_string<char>::const_iterator e);
  void append(const unsigned int& i);
  void append(const std::vector<char>& v);
};

char mk_rand_ch();

std::string mk_salt();

unsigned int mk_hash(const std::string& s);

std::string prompt(const std::string& msg = "");

bool get_yn();

#endif // UTILS_H
