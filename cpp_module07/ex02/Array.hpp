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

  // Deep-copies n elements into a fresh buffer. Frees the buffer and
  // rethrows if an element copy throws, so callers never leak it. Copying
  // BEFORE touching *this is what gives copy ctor and operator= the strong
  // exception guarantee (a throwing T can otherwise leak the new buffer in
  // the ctor, or leave _array dangling in operator=).
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
  Array() : _array(NULL), _size(0) {}

  explicit Array(unsigned int n) : _array(new T[n]()), _size(n) {}

  Array(const Array& src)
      : _array(cloneBuffer(src._array, src._size)), _size(src._size) {}

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
