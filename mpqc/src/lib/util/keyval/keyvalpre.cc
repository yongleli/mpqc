
extern "C" {
#include <ctype.h>
#include <stdlib.h>
}
#include <util/keyval/keyval.h>

//////////////////////////////////////////////////////////////////////
// utility functions

static void getnewkey(char*newkey,const char*key,int n1)
  {
  sprintf(newkey,"%s:%d",key,n1);
  }

static void getnewkey(char*newkey,const char*key,int n1,int n2)
  {
  sprintf(newkey,"%s:%d:%d",key,n1,n2);
  }

static void getnewkey(char*newkey,const char*key,int n1,int n2,int n3)
  {
  sprintf(newkey,"%s:%d:%d:%d",key,n1,n2,n3);
  }

static void getnewkey(char*newkey,const char*key,int n1,int n2,int n3,int n4)
  {
  sprintf(newkey,"%s:%d:%d:%d:%d",key,n1,n2,n3,n4);
  }

///////////////////////////////////////////////////////////////////////
// PrefixKeyVal

PrefixKeyVal::PrefixKeyVal(const RefKeyVal&kv,const char *prefix,
                           int n1,int n2,int n3,int n4):
keyval(kv)
{
  setup(prefix,4,n1,n2,n3,n4);
}

PrefixKeyVal::PrefixKeyVal(const RefKeyVal&kv,const char *prefix,
                           int n1,int n2,int n3):
keyval(kv)
{
  setup(prefix,3,n1,n2,n3,0);
}

PrefixKeyVal::PrefixKeyVal(const RefKeyVal&kv,const char *prefix,
                           int n1,int n2):
keyval(kv)
{
  setup(prefix,2,n1,n2,0,0);
}

PrefixKeyVal::PrefixKeyVal(const RefKeyVal&kv,const char *prefix,int n1):
keyval(kv)
{
  setup(prefix,1,n1,0,0,0);
}

PrefixKeyVal::PrefixKeyVal(const RefKeyVal&kv,const char *prefix):
keyval(kv)
{
  setup(prefix,0,0,0,0,0);
}


PrefixKeyVal::PrefixKeyVal(const char *prefix,const RefKeyVal&kv,
                           int n1,int n2,int n3,int n4):
keyval(kv)
{
  setup(prefix,4,n1,n2,n3,n4);
}

PrefixKeyVal::PrefixKeyVal(const char *prefix,const RefKeyVal&kv,
                           int n1,int n2,int n3):
keyval(kv)
{
  setup(prefix,3,n1,n2,n3,0);
}

PrefixKeyVal::PrefixKeyVal(const char *prefix,const RefKeyVal&kv,
                           int n1,int n2):
keyval(kv)
{
  setup(prefix,2,n1,n2,0,0);
}

PrefixKeyVal::PrefixKeyVal(const char *prefix,const RefKeyVal&kv,int n1):
keyval(kv)
{
  setup(prefix,1,n1,0,0,0);
}

PrefixKeyVal::PrefixKeyVal(const char *prefix,const RefKeyVal&kv):
keyval(kv)
{
  setup(prefix,0,0,0,0,0);
}

PrefixKeyVal::PrefixKeyVal(const RefKeyVal&kv,int n1):
keyval(kv)
{
  setup(0,1,n1,0,0,0);
}

PrefixKeyVal::PrefixKeyVal(const RefKeyVal&kv,int n1,int n2):
keyval(kv)
{
  setup(0,2,n1,n2,0,0);
}

PrefixKeyVal::PrefixKeyVal(const RefKeyVal&kv,int n1,int n2,int n3):
keyval(kv)
{
  setup(0,3,n1,n2,n3,0);
}

PrefixKeyVal::PrefixKeyVal(const RefKeyVal&kv,int n1,int n2,int n3,int n4):
keyval(kv)
{
  setup(0,4,n1,n2,n3,n4);
}

void PrefixKeyVal::setup(const char*pref,int n_dim,int n1,int n2,int n3,int n4)
{
  if (!pref) {
      prefix = 0;
    }
  else {
      char newtoken[MaxKeywordLength];
      if (n_dim == 0) strcpy(newtoken,pref);
      else if (n_dim == 1) getnewkey(newtoken,pref,n1);
      else if (n_dim == 2) getnewkey(newtoken,pref,n1,n2);
      else if (n_dim == 3) getnewkey(newtoken,pref,n1,n2,n3);
      else if (n_dim == 4) getnewkey(newtoken,pref,n1,n2,n3,n4);
      prefix = new char[strlen(newtoken)+1];
      strcpy(prefix,newtoken);
    }
  return;
}

PrefixKeyVal::~PrefixKeyVal()
{
  if (prefix) {
      delete[] prefix;
      prefix=0;
    }
}

void PrefixKeyVal::errortrace(ostream&fp,int n)
{
  offset(fp,n); fp << "PrefixKeyVal: error: \"" << errormsg() << "\"" << endl;
  offset(fp,n); fp << "  prefix:" << endl;
  offset(fp,n); fp << "    \"" << prefix << "\"" << endl;
  offset(fp,n); fp << "  keyval:" << endl;
  keyval->errortrace(fp,n + OffsetDelta);
}

void PrefixKeyVal::dump(ostream&fp,int n)
{
  offset(fp,n); fp << "PrefixKeyVal: error: \"" << errormsg() << "\"" << endl;
  offset(fp,n); fp << "  prefixes:" << endl;
  offset(fp,n); fp << "    \"" << prefix << "\"" << endl;
  offset(fp,n); fp << "  keyval:" << endl;
  keyval->dump(fp,n + OffsetDelta);
}

int PrefixKeyVal::getnewprefixkey(const char*key,char*newkey)
{
  int result=0;

  if (key[0] == ':') {
      strcpy(newkey,key);
      result = keyval->exists(key);
      seterror(keyval->error());
    }
  else {
      sprintf(newkey,"%s:%s",prefix,key);
      result = keyval->exists(newkey);
      seterror(keyval->error());
    }
  return result;
}

RefKeyValValue
PrefixKeyVal::key_value(const char * arg)
{
  char newkey[MaxKeywordLength];
  getnewprefixkey(arg,newkey);
  RefKeyValValue result(keyval->value(newkey));
  seterror(keyval->error());
  return result;
}

int PrefixKeyVal::key_exists(const char* key)
{
  char newkey[MaxKeywordLength];
  return getnewprefixkey(key,newkey);
}
