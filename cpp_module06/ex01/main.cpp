/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/25 21:56:00 by marvin            #+#    #+#             */
/*   Updated: 2025/12/25 21:56:00 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include <iostream>
#include "Serializer.hpp"
#include "Data.hpp"

int main() {
  // Create a Data object with some data
  Data originalData = {42, 3.14159, "Test Data"};
  Data* originalPtr = &originalData;

  std::cout << "=== Serializer Test ===" << std::endl;
  std::cout << "\nOriginal Data:" << std::endl;
  std::cout << "  Address: " << originalPtr << std::endl;
  std::cout << "  ID: " << originalData.id << std::endl;
  std::cout << "  Value: " << originalData.value << std::endl;
  std::cout << "  Name: " << originalData.name << std::endl;

  // Serialize the pointer
  uintptr_t serialized = Serializer::serialize(originalPtr);
  std::cout << "\nSerialized value: 0x" << std::hex << serialized << std::dec
            << std::endl;

  // Deserialize back to pointer
  Data* deserializedPtr = Serializer::deserialize(serialized);

  std::cout << "\nDeserialized Data:" << std::endl;
  std::cout << "  Address: " << deserializedPtr << std::endl;
  std::cout << "  ID: " << deserializedPtr->id << std::endl;
  std::cout << "  Value: " << deserializedPtr->value << std::endl;
  std::cout << "  Name: " << deserializedPtr->name << std::endl;

  // Verify that the pointers are equal
  std::cout << "\n=== Verification ===" << std::endl;
  if (originalPtr == deserializedPtr) {
    std::cout << "✓ SUCCESS: Pointers are equal!" << std::endl;
    std::cout << "  Original pointer:     " << originalPtr << std::endl;
    std::cout << "  Deserialized pointer: " << deserializedPtr << std::endl;
  } else {
    std::cout << "✗ FAILURE: Pointers are NOT equal!" << std::endl;
  }

  // Additional test: Verify data integrity
  if (deserializedPtr->id == originalData.id &&
      deserializedPtr->value == originalData.value &&
      deserializedPtr->name == originalData.name) {
    std::cout << "✓ SUCCESS: Data integrity verified!" << std::endl;
  } else {
    std::cout << "✗ FAILURE: Data corruption detected!" << std::endl;
  }

  // The following would not compile (Serializer is non-instantiable):
  // Serializer s;                      // Error: constructor is private
  // Serializer s2 = Serializer();      // Error: constructor is private
  // Serializer* sPtr = new Serializer; // Error: constructor is private

  return 0;
}
