#include <sstream>
#include <algorithm>

#include "ph.hpp"

string DBFILE="phbase";

string prompt_key()
{
  string s;
  while(s.empty())
  {
    cerr << "please type your access passkey: ";
    cin>>s;
  }
  return s;
}

bool load_db(memomap& memos, undecrypted& raw, string& passkey)
{
  ifstream is(DBFILE.data(), ios_base::binary);
  if(is.fail())
  {
    cout << "can't open database" << endl;
    return true;
  }
  while(!is.eof())
  {
    Memo pm;
    if(!pm.read(is, passkey))
    {
      break;
    }
    if(pm)
    {
      for(set<string>::const_iterator ssi = pm.keyes().begin();
          ssi != pm.keyes().end(); ssi++)
      {
        memos.insert(pair<string,string>(*ssi, pm.val()));
      }
    }
    else
    {
      raw.push_back(pm);
    }
  }
  return true;
}

vector<string> options;

void parse_options(int argc, char** argv)
{
  for(int i = 0; i < argc; ++i)
  {
    options.push_back(string(argv[i]));
  }
}

size_t after = 0;

int op_ind(string op)
{
  if(after >= options.size())
  {
    return -1;
  }
  vector<string>::iterator it = find(options.begin() + after + 1,options.end(),op);
  if(it == options.end())
  {
    return -1;
  }
  return it - options.begin();
}

bool get_opt(size_t ind, string& opt)
{
  if(ind >= options.size())
  {
    return false;
  }
  opt = options[ind];
  return true;
}

void use_max(size_t& dst, const size_t& src)
{
  if(src > dst)
  {
    dst = src;
  }
}

void print_matched(const memomap& memos, const string& key = "", int symbols = -1)
{
  pair<memomap::const_iterator, memomap::const_iterator> matches;
  if(!key.empty())
    matches = memos.equal_range(key);
  for(memomap::const_iterator it = matches.first; it != matches.second; ++it)
  {
    cout << "'" << (*it).first << "' => [" << smooth((*it).second, symbols) << "]" << endl;
  }
}

#define ADD "-add"
#define PASS "-pass"
#define KEY "-key"

#define GET "-get"
#define SYMBOLS "-sym"

#define LIST "-list"

#define RM "-rm"

int read_symbols(int& ind, bool mand = false)
{
  ind = op_ind(SYMBOLS);
  int symbols = -1;
  if(ind > 0)
  {
    string sym_op;
    if(get_opt(ind + 1,sym_op))
    {
      istringstream stm(sym_op);
      stm >> symbols;
    }
  }
  if(mand && symbols < 0)
  {
    cout << "failed to read symbols count" << endl;
  }
  return symbols;
}

bool g_modified = false;  // did database modified

bool exec_commands(memomap&memos)
{
  int index = -1;
  while(true)
  {
    cout << endl << endl;
    // add key-value pair
    index = op_ind(ADD);
    if(index > 0)
    {
      cout << "you requested addition to base" << endl;
      int pass_ind = op_ind(PASS);
      string new_pass;
      if(pass_ind < 0)
      {
        cout << "no value specified" << endl;
        new_pass = prompt("input new value: ");
        if(new_pass.empty())
        {
          cout << "bad value" << endl;
          return false;
        }
      }
      else if(!get_opt(pass_ind + 1, new_pass))
      {
        cout << "can't read value" << endl;
        return false;
      }
      int key_ind = op_ind(KEY);
      string new_key;
      if(key_ind < 0)
      {
        cout << "no key specified" << endl;
        new_key = prompt("input new key: ");
        if(new_key.empty())
        {
          cout << "bad key" << endl;
          return false;
        }
      }
      else if(!get_opt(key_ind + 1, new_key))
      {
        cout << "can't read key" << endl;
        return false;
      }
      cout << "value: [" << new_pass << "]" << endl
           << "key: [" << new_key << "]" << endl
           << "is it all correct? ";
      if(!get_yn())
      {
        cout << "addition canceled" << endl;
      }
      else
      {
        memos.insert(pair<string,string>(new_key,new_pass));
        g_modified = true;
      }
      use_max(after, index);
      use_max(after, key_ind + 1);
      use_max(after, pass_ind + 1);
      continue;
    }

    // watch value by key
    index = op_ind(GET);
    if(index > 0)
    {
      cout << "you requested getting value by key" << endl;
      int key_ind = op_ind(KEY);
      string rq_key;
      if(key_ind < 0)
      {
        cout << "no key specified" << endl;
        rq_key = prompt("input new key: ");
        if(rq_key.empty())
        {
          cout << "bad key" << endl;
          return false;
        }
      }
      else if(!get_opt(key_ind + 1, rq_key))
      {
        cout << "can't read key" << endl;
        return false;
      }
      int sym_ind = 0;
      int symbols = read_symbols(sym_ind);

      cout << "key [" << rq_key << "]" << endl;
      if(symbols > 0)
      {
        cout << "show first " << symbols << " symbols" << endl;
      }
      /*      cout << "is it all correct? ";
      if(!get_yn())
      {
        cout << "operation canceled" << endl;
      }
      else*/
      {
        print_matched(memos, rq_key, symbols);
      }
      use_max(after, key_ind);
      use_max(after, sym_ind);
      continue;
    }

    // remove pair by key
    index = op_ind(RM);
    if(index > 0)
    {
      cout << "you requested removing value by key";
      int key_ind = op_ind(KEY);
      string rm_key;
      if(key_ind < 0)
      {
        cout << endl << "no key specified" << endl;
        rm_key = prompt("input key to remove: ");
        if(rm_key.empty())
        {
          cout << "bad key" << endl;
          return false;
        }
      }
      else if(!get_opt(key_ind + 1, rm_key))
      {
        cout << "can't read key" << endl;
        return false;
      }
      int sym_ind = 0;
      int symbols = read_symbols(sym_ind);
      cout << "matched pairs:" << endl;
      print_matched(memos, rm_key, symbols);
      pair<memomap::iterator,memomap::iterator> matches = memos.equal_range(rm_key);
      for(memomap::iterator it = matches.first; it != matches.second;)
      {
        cout << "confirm removing of '" << (*it).first << "' => [" << smooth((*it).second, symbols) << "] ";
        if(get_yn())
        {
          memomap::iterator tit = it;
          ++it;
          memos.erase(tit);
          g_modified = true;
        }
        else
        {
          ++it;
        }
      }
      cout << "no [more] such keys found" << endl;
      use_max(after, index);
      use_max(after, key_ind);
      continue;
    }

    // print all pairs for current passkey
    index = op_ind(LIST);
    if(index > 0)
    {
      cout << "you requested to list all values" << endl;
      int sym_ind = 0;
      int symbols = read_symbols(sym_ind);
      /*      cout << "you requested to list all values" << endl
           << "right? ";
      if(!get_yn())
      {
        cout << "operation canceled" << endl;
      }
      else*/
      for(memomap::const_iterator mmssi = memos.begin(); mmssi != memos.end();++mmssi)
      {
        cout << "'" << (*mmssi).first << "' => [" << smooth((*mmssi).second, symbols) << "]" << endl;
      }
      cout << "total " << memos.size() << " item(s)" << endl;
      use_max(after, index);
      use_max(after, sym_ind);
      continue;
    }

    if(index < 0)
    {
      cout << "no [more] commands found" << endl;
      break;
    }
  } // while(true)
  return true;
}

bool save_db(memomap& memos, undecrypted& raw, const string& key)
{
  if(g_modified)
  {
    bytevector bv;

    for(memomap::const_iterator mmssi = memos.begin(); mmssi != memos.end();++mmssi)
    {
      Memo pm((*mmssi).second, (*mmssi).first);
      if(!pm.write(bv, key))
      {
        return false;
      }
    }
    for(undecrypted::iterator uit = raw.begin(); uit != raw.end(); ++uit)
    {
      (*uit).write(bv, key);
    }
    ofstream os(DBFILE.data(), ios_base::binary);
    if(!os.is_open())
    {
      return false;
    }
    if(bv.size())
    {
      os.write(&bv.front(),bv.size());
    }
  }
  return true;
}

bool got_commands()
{
  typedef vector<string> vs;
  vs cmds;
  cmds.push_back(ADD);
  cmds.push_back(GET);
  cmds.push_back(LIST);
  cmds.push_back(RM);
  for(vs::iterator i = cmds.begin(); i != cmds.end(); ++i)
  {
    if(op_ind(*i) >= 1)
    {
      return true;
    }
  }
  return false;
}

string usage()
{
  string usge;
  usge.append("add key-value pair in database: \n");
  usge.append("\tph -add \n");
  usge.append("get key-value pair by key: \n");
  usge.append("\tph -get \n");
  usge.append("list all key-value key crypted by current passkey: \n");
  usge.append("\tph -list \n");
  usge.append("remove key-value pair by key: \n");
  usge.append("\tph -rm \n");
  return usge;
}

int main(int argc, char *argv[])
{
  cout << "\nsee https://github.com/moskk/ph for more information and newest versions\n\n";
  parse_options(argc, argv);
  if(!got_commands())
  {
    cout << usage();
    cout << "no command line options given, exiting" << endl;
    return 1;
  }
  memomap memos;
  undecrypted raw;
  string key = prompt_key();
  if(!load_db(memos, raw, key))
  {
    cout << "failed to load values database" << endl;
    return 1;
  }
  if(!exec_commands(memos))
  {
    cout << "failed to execute commands" << endl;
    return 1;
  }
  if(!save_db(memos, raw, key))
  {
    cout << "failed to save values database" << endl;
    return 1;
  }
  cout << "finished" << endl;
  return 0;
}
