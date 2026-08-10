#ifndef CPP_MODULE04_EX00_ANIMAL_HPP_
#define CPP_MODULE04_EX00_ANIMAL_HPP_

#include <string>

// Polymorphic base: virtual makeSound() gives dynamic dispatch, virtual
// destructor guarantees the derived destructor runs on delete via Animal*.
class Animal {
 protected:
  std::string type;

 public:
  Animal();
  // one attribute explicit block implicit conversion like Animal* an = 2;
  explicit Animal(const std::string&);
  Animal(const Animal& src);
  Animal& operator=(const Animal& rhs);
  virtual ~Animal();

  std::string getType() const;
  virtual void makeSound() const;
};

#endif  // CPP_MODULE04_EX00_ANIMAL_HPP_
