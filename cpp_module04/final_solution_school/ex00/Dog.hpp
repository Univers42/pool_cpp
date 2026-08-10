#ifndef CPP_MODULE04_EX00_DOG_HPP_
#define CPP_MODULE04_EX00_DOG_HPP_

#include "Animal.hpp"

class Dog : public Animal {
 public:
  Dog();
  Dog(const Dog& src);
  Dog& operator=(const Dog& rhs);
  virtual ~Dog();

  virtual void makeSound() const;
};

#endif  // CPP_MODULE04_EX00_DOG_HPP_
