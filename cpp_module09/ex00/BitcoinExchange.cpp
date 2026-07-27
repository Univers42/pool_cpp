#include "BitcoinExchange.hpp"

#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iostream>

BitcoinExchange::BitcoinExchange() {}

BitcoinExchange::BitcoinExchange(const BitcoinExchange &other)
    : _rates(other._rates) {}

BitcoinExchange &BitcoinExchange::operator=(const BitcoinExchange &other) {
  if (this != &other) _rates = other._rates;
  return *this;
}

BitcoinExchange::~BitcoinExchange() {}

bool BitcoinExchange::loadDatabase(const std::string &path) {
  std::ifstream file(path.c_str());
  if (!file.is_open()) return false;

  // ponytail: trusts the subject-provided CSV; unparsable lines are skipped
  std::string line;
  while (std::getline(file, line)) {
    size_t comma = line.find(',');
    if (comma == std::string::npos) continue;
    std::string date = trim(line.substr(0, comma));
    std::string price = trim(line.substr(comma + 1));
    char *end = NULL;
    double rate = std::strtod(price.c_str(), &end);
    if (end != price.c_str() && *end == '\0') _rates[date] = rate;
  }
  return true;
}

bool BitcoinExchange::processInputFile(const std::string &path) const {
  std::ifstream file(path.c_str());
  if (!file.is_open()) {
    std::cerr << "Error: could not open file." << std::endl;
    return false;
  }

  std::string line;
  bool first = true;
  while (std::getline(file, line)) {
    if (first) {
      first = false;
      if (trim(line) == "date | value") continue;  // skip header
    }

    size_t pipe = line.find('|');
    if (pipe == std::string::npos) {
      std::cerr << "Error: bad input => " << trim(line) << std::endl;
      continue;
    }
    std::string date = trim(line.substr(0, pipe));
    std::string valueStr = trim(line.substr(pipe + 1));

    if (!isValidDate(date)) {
      std::cerr << "Error: bad input => " << date << std::endl;
      continue;
    }
    double value;
    if (!isValidValue(valueStr, value)) continue;

    double rate;
    if (!getRate(date, rate)) {
      std::cerr << "Error: no rate for date => " << date << std::endl;
      continue;
    }
    std::cout << date << " => " << valueStr << " = " << value * rate
              << std::endl;
  }
  return true;
}

// Closest lower-or-equal date; false if date precedes the whole DB.
bool BitcoinExchange::getRate(const std::string &date, double &rate) const {
  std::map<std::string, double>::const_iterator it = _rates.upper_bound(date);
  if (it == _rates.begin()) return false;
  --it;
  rate = it->second;
  return true;
}

bool BitcoinExchange::isValidDate(const std::string &date) {
  if (date.length() != 10 || date[4] != '-' || date[7] != '-') return false;
  for (size_t i = 0; i < date.length(); ++i) {
    if (i == 4 || i == 7) continue;
    if (!std::isdigit(static_cast<unsigned char>(date[i]))) return false;
  }
  int year = std::atoi(date.substr(0, 4).c_str());
  int month = std::atoi(date.substr(5, 2).c_str());
  int day = std::atoi(date.substr(8, 2).c_str());
  if (month < 1 || month > 12 || day < 1) return false;
  static const int kDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  int max = kDays[month - 1];
  bool leap = (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
  if (month == 2 && leap) max = 29;
  return day <= max;
}

bool BitcoinExchange::isValidValue(const std::string &value, double &out) {
  // Lexical check first: the subject wants a plain decimal float or int.
  // strtod alone also eats hex ("0x5"), scientific ("1e2") and "infinity",
  // and a bare "-0" would sneak past the < 0 range check below.
  std::string::size_type i = 0;
  const bool negative = !value.empty() && value[i] == '-';
  if (negative || (!value.empty() && value[i] == '+')) ++i;
  bool digits = false;
  bool dot = false;
  bool clean = i < value.length();
  for (; clean && i < value.length(); ++i) {
    if (value[i] >= '0' && value[i] <= '9')
      digits = true;
    else if (value[i] == '.' && !dot)
      dot = true;
    else
      clean = false;
  }
  if (!clean || !digits) {
    std::cerr << "Error: bad input => " << value << std::endl;
    return false;
  }
  if (negative) {
    std::cerr << "Error: not a positive number." << std::endl;
    return false;
  }
  out = std::strtod(value.c_str(), NULL);
  if (out > 1000) {
    std::cerr << "Error: too large a number." << std::endl;
    return false;
  }
  return true;
}

std::string BitcoinExchange::trim(const std::string &s) {
  size_t start = s.find_first_not_of(" \t\r\n");
  if (start == std::string::npos) return "";
  size_t end = s.find_last_not_of(" \t\r\n");
  return s.substr(start, end - start + 1);
}
