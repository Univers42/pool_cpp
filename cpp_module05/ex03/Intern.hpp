#ifndef CPP_MODULE05_EX03_INTERN_HPP_
#define CPP_MODULE05_EX03_INTERN_HPP_

#include <string>

class AForm;

// No name, no grade, no unique characteristics. Makes forms.
class Intern {
 public:
  Intern();
  Intern(const Intern& other);
  Intern& operator=(const Intern& other);
  ~Intern();

  // Returns a new form matching formName (caller deletes), or NULL with an
  // error message if the name is unknown.
  AForm* makeForm(const std::string& formName,
                  const std::string& target) const;
};

#endif  // CPP_MODULE05_EX03_INTERN_HPP_
