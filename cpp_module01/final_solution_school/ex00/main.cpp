/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 16:00:20 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/02 21:06:13 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "Zombie.hpp"

#ifdef TESTS
/**
 * @brief Extended test suite — compile with -DTESTS to enable.
 */
int main(void) {
  std::cout << C_BOLD << C_MAGENTA
            << "\n=== 1. TESTING STACK ALLOCATION (randomChump) ===" << C_RESET
            << std::endl;
  std::cout << C_YELLOW << "-> Calling randomChump(\"StackRunner\")" << C_RESET
            << std::endl;
  randomChump("StackRunner");
  std::cout << C_DIM
            << "-> Notice how StackRunner was automatically destroyed right "
               "after announcing itself!"
            << C_RESET << std::endl;

  std::cout << C_BOLD << C_MAGENTA
            << "\n=== 2. TESTING HEAP ALLOCATION (newZombie) ===" << C_RESET
            << std::endl;
  std::cout << C_YELLOW << "-> Calling newZombie(\"HeapWalker\")" << C_RESET
            << std::endl;
  Zombie* heapWalker = newZombie("HeapWalker");

  std::cout << C_YELLOW
            << "-> Making HeapWalker announce itself from main scope:"
            << C_RESET << std::endl;
  heapWalker->announce();

  std::cout << C_YELLOW << "-> Deleting HeapWalker manually:" << C_RESET
            << std::endl;
  delete heapWalker;
  std::cout << C_DIM << "-> HeapWalker is now completely destroyed." << C_RESET
            << std::endl;

  std::cout << C_BOLD << C_MAGENTA
            << "\n=== 3. TESTING EDGE CASES ===" << C_RESET << std::endl;

  std::cout << C_YELLOW << "\n[Edge Case A: Empty Name]" << C_RESET
            << std::endl;
  randomChump("");

  std::cout << C_YELLOW << "\n[Edge Case B: Special Characters]" << C_RESET
            << std::endl;
  Zombie* special = newZombie("Z0mb!e_#42");
  special->announce();
  delete special;

  std::cout << C_YELLOW << "\n[Edge Case C: Very Long Name]" << C_RESET
            << std::endl;
  randomChump("HubertBlaineWolfeschlegelsteinhausenbergerdorffSr");

  std::cout << C_BOLD << C_MAGENTA << "\n=== END OF TESTS ===" << C_RESET
            << "\n"
            << std::endl;

  return 0;
}

#else
/**
 * @brief Default main — contrasts stack lifetime vs heap lifetime.
 *
 * @details
 * ### Stack allocation (randomChump)
 * The zombie is born and dies *inside* randomChump's scope.
 * The destructor fires the moment the function returns, before the next
 * line of main() even executes.  The caller has zero control over when
 * it is destroyed.
 *
 * ### Heap allocation (newZombie)
 * The zombie is born inside newZombie but the destructor is NOT called when
 * that function returns — ownership is transferred to the caller via the
 * returned pointer.  The zombie stays alive across function boundaries until
 * the caller explicitly calls delete.  Forgetting to do so is a memory leak.
 *
 * Expected output (destructor messages show the exact moment of death):
 * @code
 *   [born]  StackZombie
 *   [brain] StackZombie: BraiiiiiiinnnzzzZ...
 *   [dead]  StackZombie            ← dies here, inside randomChump
 *   --- back in main, StackZombie is already gone ---
 *   [born]  HeapZombie
 *   --- HeapZombie is still alive across the function boundary ---
 *   [brain] HeapZombie: BraiiiiiiinnnzzzZ...
 *   [dead]  HeapZombie             ← dies only when we say so
 * @endcode
 */
int main(void) {
  // ── Stack zombie ──────────────────────────────────────────────────────────
  // Born inside randomChump, announces once, then destroyed when the function
  // returns — before the next line of main() executes.
  // The caller has zero control over when it is destroyed.
  std::cout << "--- STACK: randomChump(\"StackZombie\") ---" << std::endl;
  randomChump("StackZombie");
  std::cout << "--- back in main: StackZombie is already destroyed ---\n"
            << std::endl;

  // ── Heap zombie ───────────────────────────────────────────────────────────
  // Born inside newZombie, but ownership is transferred to us via the pointer.
  // It survives any number of scopes and function calls until WE call delete.
  std::cout << "--- HEAP: newZombie(\"HeapZombie\") ---" << std::endl;
  Zombie* heap = newZombie("HeapZombie");
  std::cout << "--- back in main: HeapZombie is still alive ---\n" << std::endl;

  // We can use it as many times as we like across multiple scopes.
  std::cout << "--- use #1 (from main scope) ---" << std::endl;
  heap->announce();

  {
    // New inner scope — a stack zombie born here dies when this block ends.
    // The heap zombie is completely unaffected.
    std::cout << "\n--- entering inner scope ---" << std::endl;
    std::cout << "--- use #2 (from inner scope) ---" << std::endl;
    heap->announce();
    std::cout << "--- leaving inner scope (HeapZombie survives) ---"
              << std::endl;
  }

  std::cout << "\n--- use #3 (back in main, after inner scope closed) ---"
            << std::endl;
  heap->announce();

  // Only now do we decide the zombie's lifetime is over.
  std::cout << "\n--- calling delete: HeapZombie dies now ---" << std::endl;
  delete heap;
  std::cout << "--- HeapZombie is gone ---" << std::endl;

  return 0;
}
#endif
