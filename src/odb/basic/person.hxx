#ifndef PERSON_HXX
#define PERSON_HXX

#include <string>
#include <odb/core.hxx>

class person
{
public:
  person(const char *name, const char *email, unsigned short age) : name_(name), email_(email), age_(age) {}

  const std::string& name() const { return name_; }
  void name(const std::string& name) { name_ = name; }

  const std::string& email() const { return email_; }
  void email(const std::string& email) { email_ = email; }

  unsigned short age() const { return age_; }
  void age(unsigned short age) { age_ = age; }

private:
  friend class odb::access;

  person() {}
  std::string name_;
  std::string email_;
  unsigned short age_;
};

#pragma db object(person)
#pragma db member(person::name_) id

#endif // PERSON_HXX