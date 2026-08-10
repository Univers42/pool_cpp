#ifndef CPP_MODULE04_EX00_WRONGCAT_HPP_
#define CPP_MODULE04_EX00_WRONGCAT_HPP_

#include "WrongAnimal.hpp"

// makeSound() here only HIDES WrongAnimal::makeSound() (no virtual in the
// base), so it is never reached through a WrongAnimal* — that is the lesson.
class WrongCat : public WrongAnimal {
 public:
  WrongCat();
  WrongCat(const WrongCat& src);
  WrongCat& operator=(const WrongCat& rhs);
  virtual ~WrongCat();

  void makeSound() const;
};

#endif  // CPP_MODULE04_EX00_WRONGCAT_HPP_
