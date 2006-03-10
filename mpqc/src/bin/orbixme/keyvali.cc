
#include <strstream.h>
#include <iostream>
#include "keyvali.h"

#include <util/class/class.h>
#include <util/keyval/keyval.h>

C_KeyValCreatableImpl::C_KeyValCreatableImpl()
{
  dc_ = 0;
}

C_KeyValCreatableImpl::C_KeyValCreatableImpl(DescribedClass *dc)
{
  dc_ = 0;
  set_dc(dc);
}

void
C_KeyValCreatableImpl::keyval_create(const char *s, CORBA_Environment &IT_env)
{
  ExEnv::outn() << "create: " << s << endl;

  istrstream in(s);

  Ref<KeyVal> keyval = new ParsedKeyVal(in);

  Ref<DescribedClass> dc = keyval->describedclassvalue("object");
  if (dc.null()) {
      ExEnv::outn() << "create failed" << endl;
      return;
    }
  else {
      ExEnv::outn() << "created a \"" << dc->class_name() << "\"" << endl;
    }

  set_dc(dc.pointer());
}

C_KeyValCreatableImpl::~C_KeyValCreatableImpl()
{
  clear_dc();
}

void
C_KeyValCreatableImpl::set_dc(DescribedClass *dc)
{
  clear_dc();
  if (dc) {
      dc_ = dc;
      dc_->reference();
    }
}

void
C_KeyValCreatableImpl::clear_dc()
{
  if (dc_) {
      dc_->dereference();
      if (dc_->nreference() == 0) delete dc_;
      dc_ = 0;
    }
}

unsigned char
C_KeyValCreatableImpl::has_object(CORBA_Environment &)
{
  unsigned char result = (dc_ != 0);
  ExEnv::outn() << "has_object will return " << result << endl;
  return result;
}
