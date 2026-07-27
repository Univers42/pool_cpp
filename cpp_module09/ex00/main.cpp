#include <iostream>

#include "BitcoinExchange.hpp"

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Error: could not open file." << std::endl;
    return 1;
  }
  BitcoinExchange btc;
  if (!btc.loadDatabase("data.csv")) {
    std::cerr << "Error: could not open database file." << std::endl;
    return 1;
  }
  return btc.processInputFile(argv[1]) ? 0 : 1;
}
