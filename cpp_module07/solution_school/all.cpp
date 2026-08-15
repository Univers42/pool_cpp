// Module 07 redone from scratch in one file, without looking at ../ex00-ex02.
// The FIX: notes are bugs this file really had before it would compile.
//
//   c++ -std=c++98 -Wall -Wextra -Werror all.cpp -o all && ./all

#include <cstddef>
#include <exception>
#include <iostream>
#include <string>

// Wrapping these in a class is the other way to solve what ../ex00 solves
// with a leading `::`: the call has to name one function and only one. Written
// bare, swap(str1, str2) compiles and quietly runs std::swap, because <string>
// carries a swap made for basic_string and that one is more specialised than
// mine. Tools::swap can't be taken over like that.
//
// Same shape as ScalarConverter in module 06: all static, constructors
// private.
class Tools {
 public:
  template <typename T>
  static void swap(T& a, T& b) {
    // FIX: was `T tmp; tmp = a;`, which forces T to be default-constructible
    // for nothing and costs a build plus an assign instead of one copy.
    T tmp = a;
    a = b;
    b = tmp;
  }

  // FIX: both took `T&`, so const objects and literals were refused and
  // Tools::min(1, 2) would not build. const T& also stops the comparison
  // copying anything, which matters as soon as T is a std::string.
  // On a tie the ternary already gives back b, the second argument, so there
  // is no equality case to write.
  template <typename T>
  static const T& min(const T& a, const T& b) {
    return (a < b ? a : b);
  }

  template <typename T>
  static const T& max(const T& a, const T& b) {
    return (a > b ? a : b);
  }

  // FIX: the body used array[i] while the parameter was named arr, and it was
  // T& instead of T*. With T* the element type keeps its const, so a const
  // array gives T = const int and a callback taking int& will not bind.
  template <typename T, typename F>
  static void iter(T* array, const size_t length, F f) {
    for (size_t i = 0; i < length; ++i) f(array[i]);
  }

 private:
  // Declared, never defined, so Tools t; fails at link time.
  Tools();
  Tools(const Tools& other);
  Tools& operator=(const Tools& other);
  ~Tools();
};

// FIX: this class was one line long and did not compile - the constructor
// initialised a member called A_n that did not exist, and left _array dangling.
// Owning raw memory means all three of destructor, copy constructor and copy
// assignment, or two Arrays end up sharing one buffer and delete[] runs twice.
template <typename T>
class Array {
 public:
  class OutOfBoundsException : public std::exception {
   public:
    virtual const char* what() const throw() {
      return "Error: Array index is out of bounds!";
    }
  };

  Array() : _array(NULL), _n(0) {}

  // explicit, so a bare 5 never silently becomes a 5-element array.
  // The () on new T[n]() value-initialises: without it the ints hold garbage.
  explicit Array(unsigned int n) : _array(new T[n]()), _n(n) {}

  Array(const Array& src) : _array(clone(src._array, src._n)), _n(src._n) {}

  Array& operator=(const Array& rhs) {
    if (this != &rhs) {
      // Build the new buffer BEFORE freeing the old one: if T's copy throws,
      // *this is still untouched. Freeing first would leave _array dangling.
      T* fresh = clone(rhs._array, rhs._n);
      delete[] _array;
      _array = fresh;
      _n = rhs._n;
    }
    return *this;
  }

  ~Array() { delete[] _array; }

  // Two overloads: which one runs depends on the constness of the Array, not
  // of the index. The index is unsigned, so a[-2] wraps to a huge value and
  // the same `index >= _n` check catches it - no separate negative case.
  T& operator[](unsigned int index) {
    if (index >= _n) throw OutOfBoundsException();
    return _array[index];
  }

  const T& operator[](unsigned int index) const {
    if (index >= _n) throw OutOfBoundsException();
    return _array[index];
  }

  unsigned int size() const { return _n; }

 private:
  T* _array;
  unsigned int _n;

  static T* clone(const T* src, unsigned int n) {
    if (n == 0) return NULL;
    T* fresh = new T[n]();
    try {
      for (unsigned int i = 0; i < n; ++i) fresh[i] = src[i];
    } catch (...) {
      delete[] fresh;  // never leak a buffer nobody owns yet
      throw;
    }
    return fresh;
  }
};

static void printInt(const int& n) { std::cout << n << " "; }
static void increment(int& n) { n++; }

// A function TEMPLATE, which the subject requires iter to accept once it is
// instantiated - print<std::string> below decays to an ordinary function
// pointer. The template name on its own would not: there is nothing to call.
template <typename T>
static void print(const T& elem) {
  std::cout << elem << " ";
}

int main(void) {
  int a = 2;
  int b = 3;

  Tools::swap(a, b);
  std::cout << "a = " << a << ", b = " << b << std::endl;
  // FIX: this line said "max" while calling min.
  std::cout << "min( a, b ) = " << Tools::min(a, b) << std::endl;
  std::cout << "max( a, b ) = " << Tools::max(a, b) << std::endl;

  std::string c = "chaine1";
  std::string d = "chaine2";
  Tools::swap(c, d);
  std::cout << "c = " << c << ", d = " << d << std::endl;
  std::cout << "min( c, d ) = " << Tools::min(c, d) << std::endl;
  std::cout << "max( c, d ) = " << Tools::max(c, d) << std::endl;

  // The tie rule is invisible to a value check - both answers are 5 - so the
  // only way to see it is to compare addresses.
  int x = 5, y = 5;
  std::cout << "on a tie min returns the second argument: "
            << (&Tools::min(x, y) == &y ? "yes" : "no") << std::endl;

  int nums[] = {1, 2, 3, 4, 5};
  const size_t len = sizeof(nums) / sizeof(nums[0]);

  std::cout << "before: ";
  Tools::iter(nums, len, printInt);
  Tools::iter(nums, len, increment);
  std::cout << "\nafter:  ";
  Tools::iter(nums, len, printInt);
  std::cout << std::endl;

  // A const array deduces T = const std::string. Handing this one `increment`
  // would not compile, which is the whole lesson.
  const std::string words[] = {"Hello", "42", "C++98"};
  Tools::iter(words, 3, print<std::string>);
  std::cout << std::endl;

  Array<int> numbers(5);
  std::cout << "new T[n]() zeroes: " << numbers[0] << ", size " << numbers.size()
            << std::endl;
  for (unsigned int i = 0; i < numbers.size(); ++i)
    numbers[i] = static_cast<int>(i) * 10;

  Array<int> copy(numbers);
  copy[0] = -1;
  std::cout << "deep copy: original[0] = " << numbers[0] << ", copy[0] = "
            << copy[0] << std::endl;

  try {
    numbers[-2] = 0;
  } catch (const std::exception& e) {
    std::cout << "numbers[-2]: " << e.what() << std::endl;
  }
  try {
    numbers[5] = 0;
  } catch (const std::exception& e) {
    std::cout << "numbers[5]:  " << e.what() << std::endl;
  }

  return 0;
}
