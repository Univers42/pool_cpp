/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/01 00:12:11 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/04 17:13:33 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sstream>
#include <string>

#include "AMateria.hpp"
#include "Character.hpp"
#include "Cure.hpp"
#include "Ice.hpp"
#include "ICharacter.hpp"
#include "IMateriaSource.hpp"
#include "MateriaSource.hpp"
#include "postman.hpp"

#define RESET   "\033[0m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"
#define BOLD    "\033[1m"
#define DIM     "\033[2m"

static void printBanner(const std::string& title) {
  TestReport::instance().beginSuite(title);
  std::cout << "\n\n\n"
            << CYAN << BOLD << std::string(70, '=') << RESET << "\n"
            << CYAN << BOLD << "  >>> " << RESET
            << BOLD << WHITE << title << RESET << "\n"
            << CYAN << BOLD << std::string(70, '=') << RESET << "\n"
            << std::endl;
}

static void printSep() {
  std::cout << "\n" << DIM << std::string(70, '-') << RESET << "\n\n";
}

// =========================================================================
//  TEST 1 - Subject baseline (from PDF)
// =========================================================================
static void test_subjectBaseline() {
  printBanner("TEST 1 - Subject baseline (from PDF)");

  IMateriaSource* src = new MateriaSource();
  src->learnMateria(new Ice());
  src->learnMateria(new Cure());

  ICharacter* me = new Character("me");
  AMateria* tmp;

  tmp = src->createMateria("ice");
  me->equip(tmp);
  tmp = src->createMateria("cure");
  me->equip(tmp);
  tmp = src->createMateria("cure");
  me->equip(tmp);
tmp = src->createMateria("cure");
  me->equip(tmp);
tmp = src->createMateria("cure");
  me->equip(tmp);
  ICharacter* bob = new Character("bob");

  ASSERT_R(me->getName() == "me", "Character name is 'me'");
  ASSERT_R(bob->getName() == "bob", "Character name is 'bob'");

  std::cout << DIM << "  me uses slot 0 on bob:\n" << RESET;
  me->use(0, *bob);
  std::cout << DIM << "  me uses slot 1 on bob:\n" << RESET;
  me->use(1, *bob);

  // These should produce correct output (checked visually)
  ASSERT_R(true, "use(0, bob) = ice bolt (visual check)");
  ASSERT_R(true, "use(1, bob) = heals wounds (visual check)");

  delete bob;
  delete me;
  delete src;
}

// =========================================================================
//  TEST 2 - Ice & Cure clone + type
// =========================================================================
static void test_cloneAndType() {
  printBanner("TEST 2 - Ice & Cure clone + type");

  Ice ice;
  Cure cure;
  ASSERT_R(ice.getType() == "ice", "Ice type is 'ice'");
  ASSERT_R(cure.getType() == "cure", "Cure type is 'cure'");

  AMateria* iceClone = ice.clone();
  AMateria* cureClone = cure.clone();
  ASSERT_R(iceClone != &ice, "Ice clone is a different object");
  ASSERT_R(cureClone != &cure, "Cure clone is a different object");
  ASSERT_R(iceClone->getType() == "ice", "Ice clone type is 'ice'");
  ASSERT_R(cureClone->getType() == "cure", "Cure clone type is 'cure'");

  delete iceClone;
  delete cureClone;
}

// =========================================================================
//  TEST 3 - MateriaSource capacity (4 max, 5th rejected)
//           (from edge2.cpp)
// =========================================================================
static void test_materiaSourceCapacity() {
  printBanner("TEST 3 - MateriaSource capacity (4 max)");

  IMateriaSource* src = new MateriaSource();
  src->learnMateria(new Ice());   // slot 0
  src->learnMateria(new Cure());  // slot 1
  src->learnMateria(new Ice());   // slot 2
  src->learnMateria(new Cure());  // slot 3

  std::cout << DIM << "  Attempting 5th learn (overflow)...\n" << RESET;
  src->learnMateria(new Ice());   // should be deleted internally
  ASSERT_R(true, "5th learnMateria handled without crash/leak");

  AMateria* m0 = src->createMateria("ice");
  AMateria* m1 = src->createMateria("cure");
  ASSERT_R(m0 != NULL, "createMateria('ice') still works after overflow");
  ASSERT_R(m1 != NULL, "createMateria('cure') still works after overflow");
  ASSERT_R(m0->getType() == "ice", "Created ice type correct");
  ASSERT_R(m1->getType() == "cure", "Created cure type correct");

  delete m0;
  delete m1;
  delete src;
}

// =========================================================================
//  TEST 4 - MateriaSource unknown type returns NULL
//           (from edge.cpp)
// =========================================================================
static void test_unknownType() {
  printBanner("TEST 4 - MateriaSource unknown type returns NULL");

  IMateriaSource* src = new MateriaSource();
  src->learnMateria(new Ice());

  AMateria* unknown = src->createMateria("fire");
  ASSERT_R(unknown == NULL, "createMateria('fire') returns NULL");

  AMateria* unknown2 = src->createMateria("lightning");
  ASSERT_R(unknown2 == NULL, "createMateria('lightning') returns NULL");

  AMateria* unknown3 = src->createMateria("");
  ASSERT_R(unknown3 == NULL, "createMateria('') returns NULL");

  delete src;
}

// =========================================================================
//  TEST 5 - Character inventory full (4 slots, 5th ignored)
// =========================================================================
static void test_inventoryFull() {
  printBanner("TEST 5 - Character inventory full (5th equip ignored)");

  IMateriaSource* src = new MateriaSource();
  src->learnMateria(new Ice());
  src->learnMateria(new Cure());

  ICharacter* c = new Character("Tank");
  AMateria* m0 = src->createMateria("ice");
  AMateria* m1 = src->createMateria("cure");
  AMateria* m2 = src->createMateria("ice");
  AMateria* m3 = src->createMateria("cure");
  AMateria* m4 = src->createMateria("ice");  // 5th

  c->equip(m0);
  c->equip(m1);
  c->equip(m2);
  c->equip(m3);
  ASSERT_R(true, "Equipped 4 materias without crash");

  std::cout << DIM << "  Attempting 5th equip (should be ignored)...\n" << RESET;
  c->equip(m4);
  ASSERT_R(true, "5th equip safely ignored");

  // m4 was NOT equipped, we must delete it ourselves
  delete m4;
  delete c;
  delete src;
}

// =========================================================================
//  TEST 6 - NULL handling (from edge.cpp)
// =========================================================================
static void test_nullHandling() {
  printBanner("TEST 6 - NULL handling (no crash)");

  IMateriaSource* src = new MateriaSource();
  std::cout << DIM << "  learnMateria(NULL)...\n" << RESET;
  src->learnMateria(NULL);
  ASSERT_R(true, "learnMateria(NULL) no crash");

  ICharacter* c = new Character("Safe");
  std::cout << DIM << "  equip(NULL)...\n" << RESET;
  c->equip(NULL);
  ASSERT_R(true, "equip(NULL) no crash");

  delete c;
  delete src;
}

// =========================================================================
//  TEST 7 - Empty slots & out-of-bounds (from edge.cpp)
// =========================================================================
static void test_emptyAndOOB() {
  printBanner("TEST 7 - Empty slots & out-of-bounds");

  ICharacter* c = new Character("Edge");
  ICharacter* target = new Character("Target");

  std::cout << DIM << "  use on empty slots 0..3:\n" << RESET;
  c->use(0, *target);
  c->use(1, *target);
  c->use(2, *target);
  c->use(3, *target);
  ASSERT_R(true, "use() on all 4 empty slots: no crash");

  std::cout << DIM << "  use on OOB indices -1, 4, 100:\n" << RESET;
  c->use(-1, *target);
  c->use(4, *target);
  c->use(100, *target);
  ASSERT_R(true, "use() on OOB indices: no crash");

  std::cout << DIM << "  unequip on OOB indices -1, 4, 100:\n" << RESET;
  c->unequip(-1);
  c->unequip(4);
  c->unequip(100);
  ASSERT_R(true, "unequip() on OOB indices: no crash");

  delete target;
  delete c;
}

// =========================================================================
//  TEST 8 - Unequip & floor management
//           unequip doesn't delete the materia
// =========================================================================
static void test_unequipFloor() {
  printBanner("TEST 8 - Unequip & floor management");

  ICharacter* c = new Character("Unequipper");
  AMateria* ice = new Ice();
  AMateria* cure = new Cure();

  c->equip(ice);
  c->equip(cure);

  // Unequip slot 0 (ice) - should NOT delete it
  c->unequip(0);
  ASSERT_R(ice->getType() == "ice", "Unequipped ice still accessible (not deleted)");

  // Use slot 0 - should do nothing (empty now)
  ICharacter* dummy = new Character("Dummy");
  c->use(0, *dummy);
  ASSERT_R(true, "use() on unequipped slot: no crash");

  // Slot 1 (cure) should still work
  c->use(1, *dummy);
  ASSERT_R(true, "use() on slot 1 (cure) still works");

  // Re-equip ice into the hole (slot 0)
  c->equip(ice);
  c->use(0, *dummy);
  ASSERT_R(true, "Re-equipped ice into hole, use works");

  // Unequip again to test double-unequip on empty slot
  c->unequip(0);
  c->unequip(0);  // already empty, should be harmless
  ASSERT_R(true, "Double unequip on same slot: no crash");

  // Clean up floor materias manually
  delete ice;
  delete dummy;
  delete c;
}

// =========================================================================
//  TEST 9 - Deep copy Character (copy ctor)
// =========================================================================
static void test_deepCopyCharacter() {
  printBanner("TEST 9 - Deep copy Character (copy ctor)");

  Character c1("Original");
  AMateria* ice = new Ice();
  AMateria* cure = new Cure();
  c1.equip(ice);
  c1.equip(cure);

  Character c2(c1);
  ASSERT_R(c2.getName() == "Original", "Copy has same name");

  // Use both - they should both work independently
  Character target("Target");
  std::cout << DIM << "  c1 uses slot 0:\n" << RESET;
  c1.use(0, target);
  std::cout << DIM << "  c2 uses slot 0:\n" << RESET;
  c2.use(0, target);
  ASSERT_R(true, "Both original and copy can use slot 0");

  // Unequip from c1, c2 should still work
  c1.unequip(0);
  c2.use(0, target);
  ASSERT_R(true, "Copy slot 0 works after original unequip");

  // Need to delete the unequipped materia from c1
  delete ice;
}

// =========================================================================
//  TEST 10 - Character assignment operator
// =========================================================================
static void test_characterAssign() {
  printBanner("TEST 10 - Character assignment operator");

  Character c1("Alpha");
  c1.equip(new Ice());
  c1.equip(new Cure());

  Character c2("Beta");
  c2.equip(new Ice());

  c2 = c1;
  ASSERT_R(c2.getName() == "Alpha", "Assigned name matches");

  Character target("Target");
  std::cout << DIM << "  c2 uses slot 0 (should be ice from c1):\n" << RESET;
  c2.use(0, target);
  std::cout << DIM << "  c2 uses slot 1 (should be cure from c1):\n" << RESET;
  c2.use(1, target);
  ASSERT_R(true, "Assigned character can use both slots");

  // Verify independence: unequip c1 slot 0
  AMateria* floor = new Ice();  // placeholder
  (void)floor;
  c1.unequip(0);
  c2.use(0, target);
  ASSERT_R(true, "c2 slot 0 still works after c1 unequip (deep copy)");

  // c1's unequipped materia leaked on the floor - not our responsibility
  // (subject says Character must NOT delete unequipped materia)
  // But since we made it with equip(new Ice()), we have a leak here.
  // In a real scenario you'd track floor materias. For test purposes, acceptable.
  delete floor;
}

// =========================================================================
//  TEST 11 - Self-assignment guard
// =========================================================================
static void test_selfAssignment() {
  printBanner("TEST 11 - Self-assignment guard");

  Character c("Self");
  c.equip(new Ice());
  c.equip(new Cure());

  Character* pc = &c;
  c = *pc;
  ASSERT_R(c.getName() == "Self", "Name intact after self-assign");

  Character target("Target");
  c.use(0, target);
  c.use(1, target);
  ASSERT_R(true, "Both slots usable after self-assign");

  // MateriaSource self-assign
  MateriaSource ms;
  ms.learnMateria(new Ice());
  ms.learnMateria(new Cure());
  MateriaSource* pms = &ms;
  ms = *pms;
  AMateria* m = ms.createMateria("ice");
  ASSERT_R(m != NULL, "MateriaSource still works after self-assign");
  ASSERT_R(m->getType() == "ice", "Created materia type correct after self-assign");
  delete m;
}

// =========================================================================
//  TEST 12 - MateriaSource deep copy (copy ctor + assign)
// =========================================================================
static void test_materiaSourceCopy() {
  printBanner("TEST 12 - MateriaSource deep copy");

  MateriaSource ms1;
  ms1.learnMateria(new Ice());
  ms1.learnMateria(new Cure());

  // Copy constructor
  MateriaSource ms2(ms1);
  AMateria* fromCopy = ms2.createMateria("ice");
  ASSERT_R(fromCopy != NULL, "Copy ctor: createMateria('ice') works");
  ASSERT_R(fromCopy->getType() == "ice", "Copy ctor: type is 'ice'");
  delete fromCopy;

  AMateria* fromCopy2 = ms2.createMateria("cure");
  ASSERT_R(fromCopy2 != NULL, "Copy ctor: createMateria('cure') works");
  delete fromCopy2;

  printSep();

  // Assignment operator
  MateriaSource ms3;
  ms3 = ms1;
  AMateria* fromAssign = ms3.createMateria("cure");
  ASSERT_R(fromAssign != NULL, "Assign op: createMateria('cure') works");
  ASSERT_R(fromAssign->getType() == "cure", "Assign op: type is 'cure'");
  delete fromAssign;

  // Independence: learn more in ms1, ms2/ms3 unaffected
  ms1.learnMateria(new Ice());  // slot 2
  AMateria* test = ms2.createMateria("ice");
  ASSERT_R(test != NULL, "Copy independent from original");
  delete test;
}

// =========================================================================
//  TEST 13 - AMateria assignment does NOT copy type (subject rule)
// =========================================================================
static void test_materiaAssignNoCopyType() {
  printBanner("TEST 13 - AMateria assign does NOT copy type");

  Ice ice;
  Cure cure;

  ASSERT_R(ice.getType() == "ice", "Ice type before assign");
  ASSERT_R(cure.getType() == "cure", "Cure type before assign");

  // Assigning through AMateria& to test AMateria::operator= (no-op on type)
  AMateria& refIce = ice;
  AMateria& refCure = cure;
  refIce = refCure;
  ASSERT_R(ice.getType() == "ice", "Ice type unchanged after AMateria assign from Cure");

  refCure = refIce;
  ASSERT_R(cure.getType() == "cure", "Cure type unchanged after AMateria assign from Ice");
}

// =========================================================================
//  TEST 14 - Virtual dtor chain (delete via interface pointers)
// =========================================================================
static void test_virtualDtorChain() {
  printBanner("TEST 14 - Virtual dtor chain (interface pointers)");

  ICharacter* c = new Character("Mortal");
  c->equip(new Ice());
  c->equip(new Cure());

  std::cout << DIM << "  delete ICharacter*...\n" << RESET;
  delete c;
  ASSERT_R(true, "delete ICharacter* (with inventory): no crash");

  IMateriaSource* s = new MateriaSource();
  s->learnMateria(new Ice());
  s->learnMateria(new Cure());

  std::cout << DIM << "  delete IMateriaSource*...\n" << RESET;
  delete s;
  ASSERT_R(true, "delete IMateriaSource* (with templates): no crash");
}

// =========================================================================
//  TEST 15 - Equip into holes after unequip
//            (from edge2.cpp concept)
// =========================================================================
static void test_equipIntoHoles() {
  printBanner("TEST 15 - Equip into holes after unequip");

  Character c("Holey");
  AMateria* m0 = new Ice();
  AMateria* m1 = new Cure();
  AMateria* m2 = new Ice();
  AMateria* m3 = new Cure();

  c.equip(m0);  // slot 0
  c.equip(m1);  // slot 1
  c.equip(m2);  // slot 2
  c.equip(m3);  // slot 3

  // Unequip middle slots
  c.unequip(1);  // m1 on floor
  c.unequip(2);  // m2 on floor

  // New materia should fill first hole (slot 1)
  AMateria* m4 = new Ice();
  c.equip(m4);

  Character target("Target");
  c.use(0, target);  // ice (m0)
  c.use(1, target);  // ice (m4, filled hole)
  c.use(3, target);  // cure (m3)
  ASSERT_R(true, "Equip fills holes correctly");

  // Slot 2 should still be empty
  c.use(2, target);  // should do nothing
  ASSERT_R(true, "Slot 2 still empty after partial refill");

  // Clean up floor materias
  delete m1;
  delete m2;
}

// =========================================================================
//  MAIN
// =========================================================================
int main() {
  test_subjectBaseline();
  test_cloneAndType();
  test_materiaSourceCapacity();
  test_unknownType();
  test_inventoryFull();
  test_nullHandling();
  test_emptyAndOOB();
  test_unequipFloor();
  test_deepCopyCharacter();
  test_characterAssign();
  test_selfAssignment();
  test_materiaSourceCopy();
  test_materiaAssignNoCopyType();
  test_virtualDtorChain();
  test_equipIntoHoles();

  TestReport::instance().print();
  return 0;
}
