#ifndef PH_HPP
#define PH_HPP

#include <set>
#include <fstream>
#include <iostream>
#include <map>

#include "crypt.h"

using namespace std;
typedef multimap<string,string> memomap;
class Memo;
typedef vector<Memo> undecrypted;

bool read(ifstream& file, string& s, const string& passkey)
{
  unsigned int len = 0;
  file.read((char*)&len,sizeof(len));
  if(file.fail())
    return false;
  crypt(len);
  crypt(len, passkey);
  bytevector v(len);
  file.read(&v[0], len);
  if(file.fail())
    return false;
  crypt(v, passkey);
  s = string(v.begin(), v.end());
  return true;
}

bool write(bytevector& buf, const string& s, const string& passkey)
{
  unsigned int len = s.length();
  bytevector v(s.begin(), s.end());
  crypt(v, passkey);
  crypt(len);
  crypt(len, passkey);
  buf.append(len);
  buf.append(v);
  return true;
}

bool read(ifstream& file, unsigned int& i, const string& passkey)
{
  file.read((char*)&i,sizeof(i));
  if(file.fail())
    return false;
  crypt(i);
  crypt(i, passkey);
  return true;
}

bool write(bytevector& buf, unsigned int i, const string& passkey)
{
  crypt(i);
  crypt(i, passkey);
  buf.append(i);
  return true;
}

string smooth(const string& s, int len)
{
  if(len < 0 || len >= int(s.length()))
    return s;
  string p(s);
  p.replace(p.begin()+len, p.end(), p.length()-len, '*');
  return p;
}

class Memo
{
  set<string> m_key;
  string m_val;
  bytevector m_raw;
public:
  Memo(){}
  Memo(const string& val, const string& key):m_val(val)
  {m_key.clear();m_key.insert(key);}
  const set<string>& keyes()
  {
    return m_key;
  }
  const string val(int len = -1)
  {
    return smooth(m_val, len);
  }
  void set_val(const string& val)
  {
    m_val = val;
  }
  void add_key(const string& key)
  {
    m_key.insert(key);
  }
  bool rm_key(const string& key)
  {
    return bool(m_key.erase(key));
  }
  bool read(ifstream& file, const string& passkey)
  {
    unsigned int raw_size(0),pass_hash_in_file(0);
    file.read((char*)&raw_size, sizeof(raw_size));
    if(file.fail())
      return false;
    file.read((char*)&pass_hash_in_file, sizeof(pass_hash_in_file));
    if(file.fail())
      return false;
    crypt(raw_size);
    unsigned int pass_hash_in_mem = mk_hash(passkey);
    crypt(pass_hash_in_mem);
    if(pass_hash_in_file == pass_hash_in_mem)
    {
      if(!::read(file, m_val, passkey))
        return false;
      unsigned int na = 0;
      if(!::read(file, na, passkey))
        return false;
      while(na--)
      {
        string val_key;
        if(!::read(file, val_key, passkey))
        {
          m_key.clear();
          return false;
        }
        m_key.insert(val_key);
      }
      return true;
    }
    else
    {
      m_raw.clear();
      m_raw.reserve(raw_size);
      m_raw.append(pass_hash_in_file);
      bytevector bv(raw_size - sizeof(pass_hash_in_file));
      file.read(&bv[0], raw_size - sizeof(pass_hash_in_file));
      m_raw.append(bv);
      return true;
    }
    return false;
  }
  bool write(bytevector& file, const string& passkey)
  {
    if(valid())
    {
      bytevector tmp;
      if(!::write(tmp, m_val, passkey))
        return false;
      int na = m_key.size();
      if(!::write(tmp, na, passkey))
        return false;
      for(set<string>::iterator ssi = m_key.begin(); ssi != m_key.end();++ssi)
      {
        if(!::write(tmp, *ssi, passkey))
          return false;
      }
      unsigned int key_hash = mk_hash(passkey);
      crypt(key_hash);
      unsigned int raw_size = tmp.size() + sizeof(key_hash);
      crypt(raw_size);
      file.append(raw_size);
      file.append(key_hash);
      file.append(tmp);
      return true;
    }
    else if(m_raw.size())
    {
      unsigned int raw_size(m_raw.size());
      crypt(raw_size);
      file.append(raw_size);
      file.append(m_raw);
      return true;
    }
    return false;
  }
  bool valid()
  {
    return !m_key.empty();
  }
  operator bool()
  {
    return valid();
  }
};


#endif // PH_HPP
