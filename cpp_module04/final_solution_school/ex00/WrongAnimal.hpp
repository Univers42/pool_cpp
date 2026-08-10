#ifndef CPP_MODULE04_EX00_WRONGANIMAL_HPP_
#define CPP_MODULE04_EX00_WRONGANIMAL_HPP_

#include <string>

// Deliberately flawed base: makeSound() is NOT virtual, so a call through a
// WrongAnimal* is resolved at compile time and always runs this version.
class WrongAnimal {
 protected:
  std::string type;

 public:
  WrongAnimal();
  WrongAnimal(const WrongAnimal& src);
  WrongAnimal& operator=(const WrongAnimal& rhs);
  virtual ~WrongAnimal();

  std::string getType() const;
  void makeSound() const;
};

#endif  // CPP_MODULE04_EX00_WRONGANIMAL_HPP_
