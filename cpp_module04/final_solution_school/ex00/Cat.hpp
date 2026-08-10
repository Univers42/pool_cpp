#ifndef CPP_MODULE04_EX00_CAT_HPP_
#define CPP_MODULE04_EX00_CAT_HPP_

#include "Animal.hpp"

class Cat : public Animal {
 public:
  Cat();
  Cat(const Cat& src);
  Cat& operator=(const Cat& rhs);
  virtual ~Cat();

  virtual void makeSound() const;
};

#endif  // CPP_MODULE04_EX00_CAT_HPP_
