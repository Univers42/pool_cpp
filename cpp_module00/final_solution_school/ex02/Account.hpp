/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Account.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 01:19:21 by marvin            #+#    #+#             */
/*   Updated: 2026/03/02 15:11:02 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef __ACCOUNT_H__
#define __ACCOUNT_H__

// diff <(sed 's/^\[[^]]*\] //' out.log)
// diff <(sed 's/^\[[^]]*\] //' out.log) <(sed 's/^\[[^]]*\] //' 19920104_091532.log)

class Account {
 public:
  typedef Account t;

  static int getNbAccounts(void);
  static int getTotalAmount(void);
  static int getNbDeposits(void);
  static int getNbWithdrawals(void);
  static void displayAccountsInfos(void);

  /**Marking single-parameter constructors explicit
   * prevents unintended implicit conversions.
   */
  explicit Account(int initial_deposit);
  ~Account(void);
  void makeDeposit(int deposit);
  bool makeWithdrawal(int withdrawal);
  int checkAmount(void) const;
  void displayStatus(void) const;

 private:
  static int _nbAccounts;
  static int _totalAmount;
  static int _totalNbDeposits;
  static int _totalNbWithdrawals;

  static void _displayTimestamp(void);

  int _accountIndex;
  int _amount;
  int _nbDeposits;
  int _nbWithdrawals;
  Account(void);
};

#endif /* __ACCOUNT_H__ */
