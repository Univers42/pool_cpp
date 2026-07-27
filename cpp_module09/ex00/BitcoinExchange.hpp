#ifndef CPP_MODULE09_EX00_BITCOINEXCHANGE_HPP_
#define CPP_MODULE09_EX00_BITCOINEXCHANGE_HPP_

#include <map>
#include <string>

class BitcoinExchange {
 public:
  BitcoinExchange();
  BitcoinExchange(const BitcoinExchange &other);
  BitcoinExchange &operator=(const BitcoinExchange &other);
  ~BitcoinExchange();

  // Loads the "date,exchange_rate" CSV; false if the file cannot be opened.
  bool loadDatabase(const std::string &path);
  // Reads "date | value" lines, prints results / subject error messages.
  // Returns false (after printing the subject's open error) if the file
  // cannot be opened, true otherwise.
  bool processInputFile(const std::string &path) const;

 private:
  std::map<std::string, double> _rates;

  bool getRate(const std::string &date, double &rate) const;
  static bool isValidDate(const std::string &date);
  static bool isValidValue(const std::string &value, double &out);
  static std::string trim(const std::string &s);
};

#endif  // CPP_MODULE09_EX00_BITCOINEXCHANGE_HPP_
