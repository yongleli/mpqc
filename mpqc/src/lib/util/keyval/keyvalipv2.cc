
#include <iostream.h>
#include <strstream.h>
#include <fstream.h>
#include <stdlib.h>
#include <string.h>

#include <util/misc/formio.h>
#include <util/keyval/ipv2.h>
#include <util/keyval/keyval.h>

ParsedKeyVal::ParsedKeyVal(IPV2*i):
nfile(0),
file(0),
nfp(0)
{
  ipv2 = i;
}

ParsedKeyVal::ParsedKeyVal():
nfile(0),
file(0),
nfp(0)
{
  ipv2 = new IPV2;
}

ParsedKeyVal::ParsedKeyVal(const char* name):
nfile(0),
file(0),
nfp(0)
{
  ipv2 = new IPV2;
  read(name);
}

ParsedKeyVal::ParsedKeyVal(istream& fp):
nfile(0),
file(0),
nfp(0)
{
  ipv2 = new IPV2;
  read(fp);
}

ParsedKeyVal::ParsedKeyVal(const char* keyprefix, const RefKeyVal& keyval):
nfile(0),
file(0),
nfp(0)
{
  ipv2 = new IPV2;

  char* filespec = new char[strlen(keyprefix)+6];
  strcpy(filespec,keyprefix);
  strcat(filespec,"files");

  char* dirspec = new char[strlen(keyprefix)+6];
  strcpy(dirspec,keyprefix);
  strcat(dirspec,"dir");

  char* directory = keyval->pcharvalue(dirspec);
  if (!directory) {
      directory = getenv("SCLIBDIR");
      if (directory) {
          char *tmp = strchr(directory,'=');
          if (!tmp) tmp = directory;
          else tmp = &tmp[1];

          directory = strcpy(new char[strlen(tmp)+1], tmp);
        }
      else {
          directory = strcpy(new char[strlen(SRCLIBDIR)+1], SRCLIBDIR);
        }
    }

  int nfiles = keyval->count(filespec);
  for (int i=0; i<nfiles; i++) {
      char* filename = keyval->pcharvalue(filespec,i);
      char* fullname;
      if (directory) {
          fullname = new char[strlen(directory)+strlen(filename)+1];
          strcpy(fullname,directory);
          strcat(fullname,filename);
        }
      else {
          fullname = filename;
        }
      read(fullname);
      if (directory) {
          delete[] filename;
        }
      delete[] fullname;
    }

  if (directory) delete[] directory;

  delete[] dirspec;
  delete[] filespec;

}

void
ParsedKeyVal::read(const char* name)
{
  ifstream infp(name,ios::in);
  if (infp.bad()) {
    cerr << scprintf("ParsedKeyVal couldn't open %s\n",name);
    exit(1);
    }

  int i;
  char**newfile = new char*[nfile+1];
  for (i=0; i<nfile; i++) newfile[i] = file[i];
  if (file) delete[] file;
  file = newfile;
  newfile[nfile] = strdup(name);
  nfile++;

  read(infp);
  nfp--; // read(infp) will incr nfp, but this isn't desired so undo
}

void ParsedKeyVal::read(istream&infp)
{
  nfp++;
  ipv2->read(infp,cerr,"<stream>");
}

void
ParsedKeyVal::parse_string(const char *str)
{
  istrstream in(str);
  ipv2->read(in,cerr,"<string>");
}

ParsedKeyVal::~ParsedKeyVal()
{
  delete ipv2;
  for (int i=0; i<nfile; i++) free(file[i]);
  delete[] file;
}

static KeyVal::KeyValError maperr(IPV2::Status err)
  {
  if (err == IPV2::OK            ) return KeyVal::OK;
  if (err == IPV2::KeyNotFound ) return KeyVal::UnknownKeyword;
  if (err == IPV2::OutOfBounds ) return KeyVal::UnknownKeyword;
  if (err == IPV2::Malloc        ) return KeyVal::OperationFailed;
  if (err == IPV2::NotAnArray  ) return KeyVal::UnknownKeyword;
  if (err == IPV2::NotAScalar  ) return KeyVal::HasNoValue;
  if (err == IPV2::Type          ) return KeyVal::WrongType;
  if (err == IPV2::HasNoValue  ) return KeyVal::HasNoValue;
  if (err == IPV2::ValNotExpd  ) return KeyVal::OperationFailed;
  return KeyVal::OperationFailed;
  }

const char* ParsedKeyVal::stringvalue(const char* key)
{
  const char* result;
  seterror(maperr(ipv2->value_v((char *)key,&result,0,0)));
  if (error() != OK) {
      result = 0;
    }
  return result;
}

const char*
ParsedKeyVal::classname(const char* key)
{
  const char* result;
  seterror(maperr(ipv2->classname_v((char *)key,&result,0,0)));
  return result;
}

const char*
ParsedKeyVal::truekeyword(const char*key)
{
  const char* result;
  seterror(maperr(ipv2->truekeyword_v((char *)key,&result,0,0)));
  if (!result && error() == OK) return key;
  else return result;
}

void ParsedKeyVal::errortrace(ostream&fp,int n)
{
  offset(fp,n); fp << "ParsedKeyVal: error: \"" << errormsg() << "\"" << endl;
  if (nfp) {
      offset(fp,n);
      fp << "    reading from " << nfp << " files with unknown names" << endl;
    }
  for (int i=0; i<nfile; i++) {
      offset(fp,n); fp << "    reading from \"" << file[i] << "\"" << endl;
    }
}

void ParsedKeyVal::dump(ostream&fp,int n)
{
  offset(fp,n); fp << "ParsedKeyVal: error: \"" << errormsg() << "\"" << endl;
  if (nfp) {
      offset(fp,n);
      fp << "    reading from " << nfp << " files with unknown names" << endl;
    }
  for (int i=0; i<nfile; i++) {
      offset(fp,n); fp << "    reading from \"" << file[i] << "\"" << endl;
    }
  offset(fp,n); fp << "The IPV2 tree:" << endl;
  ipv2->print_tree(fp);

}
