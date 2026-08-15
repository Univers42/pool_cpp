/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Array.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/01 12:04:33 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/01 12:08:38 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CPP_MODULE07_EX02_ARRAY_HPP_
#define CPP_MODULE07_EX02_ARRAY_HPP_

#include <cstddef>
#include <exception>

template <typename T>
class Array {
 private:
  T* _array;
  unsigned int _size;

  // Build the whole new buffer first, and only then let the caller touch
  // *this. Both bugs I had came from doing it the other way round: copying
  // straight into _array leaked the buffer when an element copy threw (the
  // object was still half-built, so no destructor ever ran for it), and
  // freeing _array before allocating left it dangling when new T[n]() threw.
  // The catch here is what stops the fresh buffer leaking on the way out.
  static T* cloneBuffer(const T* src, unsigned int n) {
    if (n == 0) return NULL;
    T* fresh = new T[n]();
    try {
      for (unsigned int i = 0; i < n; ++i) fresh[i] = src[i];
    } catch (...) {
      delete[] fresh;
      throw;
    }
    return fresh;
  }

 public:
  // _array stays NULL rather than allocating: the subject forbids preventive
  // allocation, and _size == 0 makes operator[] throw on every index anyway.
  Array() : _array(NULL), _size(0) {}

  // The ternary is not about safety: new T[0]() is legal and delete[] accepts
  // what it returns, and _size == 0 makes operator[] throw on every index, so
  // the pointer is never read either way. It buys two smaller things - one
  // fewer heap allocation for an empty array, and one representation for
  // size 0 instead of two, since cloneBuffer already answers NULL for n == 0.
  // It also leaves nothing to argue about against the subject's ban on
  // preventive allocation.
  explicit Array(unsigned int n)
      : _array(n > 0 ? new T[n]() : NULL), _size(n) {}

  Array(const Array& src)
      : _array(cloneBuffer(src._array, src._size)), _size(src._size) {}

  // The self-assignment guard is only an optimisation here: cloneBuffer runs
  // before the delete[], so a = a would read live memory and work anyway. It
  // just saves a pointless allocate-and-copy.
  Array& operator=(const Array& rhs) {
    if (this != &rhs) {
      T* fresh = cloneBuffer(rhs._array, rhs._size);
      delete[] _array;
      _array = fresh;
      _size = rhs._size;
    }
    return (*this);
  }

  ~Array() { delete[] _array; }

  T& operator[](unsigned int index) {
    if (index >= _size) throw OutOfBoundsException();
    return (_array[index]);
  }

  // Reading only: const T& is what stops a caller writing through it.
  const T& operator[](unsigned int index) const {
    if (index >= _size) throw OutOfBoundsException();
    return (_array[index]);
  }

  unsigned int size() const { return (_size); }

  class OutOfBoundsException : public std::exception {
   public:
    virtual const char* what() const throw() {
      return "Error: Array index is out of bounds!";
    }
  };
};

#endif  // CPP_MODULE07_EX02_ARRAY_HPP_
