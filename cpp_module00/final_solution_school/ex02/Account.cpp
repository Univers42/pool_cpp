/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Account.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 01:20:32 by marvin            #+#    #+#             */
/*   Updated: 2026/03/02 15:11:27 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Account.hpp"

#include <ctime>
#include <iomanip>
#include <iostream>

// diff <(sed 's/^\[[^]]*\] //' out.log) <(sed 's/^\[[^]]*\] //' 19920104_091532.log)

// Static member initialization
int Account::_nbAccounts = 0;
int Account::_totalAmount = 0;
int Account::_totalNbDeposits = 0;
int Account::_totalNbWithdrawals = 0;

// Private default constructor
Account::Account(void) {}

Account::Account(int initial_deposit)
    : _accountIndex(_nbAccounts),
      _amount(initial_deposit),
      _nbDeposits(0),
      _nbWithdrawals(0) {
  _nbAccounts++;
  _totalAmount += initial_deposit;
  _displayTimestamp();
  std::cout << " index:" << _accountIndex << ";amount:" << _amount << ";created"
            << std::endl;
}

Account::~Account(void) {
  _displayTimestamp();
  std::cout << " index:" << _accountIndex << ";amount:" << _amount << ";closed"
            << std::endl;
}

int Account::getNbAccounts(void) { return _nbAccounts; }

int Account::getTotalAmount(void) { return _totalAmount; }

int Account::getNbDeposits(void) { return _totalNbDeposits; }

int Account::getNbWithdrawals(void) { return _totalNbWithdrawals; }

void Account::displayAccountsInfos(void) {
  _displayTimestamp();
  std::cout << " accounts:" << getNbAccounts() << ";total:" << getTotalAmount()
            << ";deposits:" << getNbDeposits()
            << ";withdrawals:" << getNbWithdrawals() << std::endl;
}

void Account::makeDeposit(int deposit) {
  _displayTimestamp();
  std::cout << " index:" << _accountIndex << ";p_amount:" << _amount
            << ";deposit:" << deposit;
  _amount += deposit;
  _totalAmount += deposit;
  _nbDeposits++;
  _totalNbDeposits++;
  std::cout << ";amount:" << _amount << ";nb_deposits:" << _nbDeposits
            << std::endl;
}

bool Account::makeWithdrawal(int withdrawal) {
  _displayTimestamp();
  std::cout << " index:" << _accountIndex << ";p_amount:" << _amount
            << ";withdrawal:";
  if (withdrawal > _amount) {
    std::cout << "refused" << std::endl;
    return (false);
  }
  _amount -= withdrawal;
  _totalAmount -= withdrawal;
  _nbWithdrawals++;
  _totalNbWithdrawals++;
  std::cout << withdrawal << ";amount:" << _amount
            << ";nb_withdrawals:" << _nbWithdrawals << std::endl;
  return (true);
}

int Account::checkAmount(void) const { return (_amount); }

void Account::displayStatus(void) const {
  _displayTimestamp();
  std::cout << " index:" << _accountIndex << ";amount:" << _amount
            << ";deposits:" << _nbDeposits << ";withdrawals:" << _nbWithdrawals
            << std::endl;
}

void Account::_displayTimestamp(void) {
  std::time_t t = std::time(NULL);
  std::tm* tm = std::localtime(&t);

  std::cout << "[" << std::setfill('0') << std::setw(4) << (tm->tm_year + 1900)
            << std::setw(2) << (tm->tm_mon + 1) << std::setw(2) << tm->tm_mday
            << "_" << std::setw(2) << tm->tm_hour << std::setw(2) << tm->tm_min
            << std::setw(2) << tm->tm_sec << "]";
}
