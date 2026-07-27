// Standalone C++98 test for ex03: Materia deep copy, floor behavior,
// equip/unequip/use. Exits non-zero on first failure.

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

#include "Character.hpp"
#include "Cure.hpp"
#include "Ice.hpp"
#include "MateriaSource.hpp"

#define CHECK(cond)                                                        \
  do {                                                                     \
    if (!(cond)) {                                                         \
      std::cerr << "FAIL " << __FILE__ << ":" << __LINE__ << ": " << #cond \
                << std::endl;                                              \
      std::exit(1);                                                        \
    }                                                                      \
  } while (0)

// Capture what `use` prints so output can be asserted, not eyeballed.
static std::string captureUse(ICharacter& c, int idx, ICharacter& target) {
  std::ostringstream buf;
  std::streambuf* old = std::cout.rdbuf(buf.rdbuf());
  c.use(idx, target);
  std::cout.rdbuf(old);
  return buf.str();
}

int main() {
  // --- types, clone, verbatim output -------------------------------------
  Ice ice;
  Cure cure;
  CHECK(ice.getType() == "ice");
  CHECK(cure.getType() == "cure");

  AMateria* iclone = ice.clone();
  CHECK(iclone != &ice);
  CHECK(iclone->getType() == "ice");
  delete iclone;

  // AMateria assignment must NOT copy the type.
  AMateria& ri = ice;
  ri = cure;
  CHECK(ice.getType() == "ice");

  // --- MateriaSource ------------------------------------------------------
  MateriaSource src;
  src.learnMateria(new Ice());
  src.learnMateria(new Cure());
  src.learnMateria(new Ice());
  src.learnMateria(new Cure());
  src.learnMateria(new Ice());  // 5th: full, absorbed without leak (valgrind)
  CHECK(src.createMateria("fire") == NULL);
  AMateria* m = src.createMateria("cure");
  CHECK(m != NULL && m->getType() == "cure");
  delete m;

  // MateriaSource deep copy: copy keeps working after original dies.
  MateriaSource* orig = new MateriaSource();
  orig->learnMateria(new Ice());
  MateriaSource copySrc(*orig);
  delete orig;
  m = copySrc.createMateria("ice");
  CHECK(m != NULL && m->getType() == "ice");
  delete m;

  // --- Character equip/use/unequip ---------------------------------------
  Character hero("hero");
  Character bob("bob");
  hero.equip(new Ice());
  hero.equip(new Cure());
  CHECK(captureUse(hero, 0, bob) == "* shoots an ice bolt at bob *\n");
  CHECK(captureUse(hero, 1, bob) == "* heals bob's wounds *\n");
  CHECK(captureUse(hero, 2, bob) == "");    // empty slot: nothing
  CHECK(captureUse(hero, -1, bob) == "");   // OOB: nothing
  CHECK(captureUse(hero, 4, bob) == "");    // OOB: nothing
  hero.unequip(-1);                         // OOB: no-op
  hero.unequip(4);                          // OOB: no-op

  // 5th equip into a full inventory is ignored; caller keeps ownership.
  hero.equip(new Ice());
  hero.equip(new Cure());
  AMateria* fifth = new Ice();
  hero.equip(fifth);
  delete fifth;  // was not equipped, still ours
  CHECK(captureUse(hero, 3, bob) == "* heals bob's wounds *\n");

  // Equipping the SAME pointer twice must not create double ownership
  // (the destructor would delete it twice). Second equip is refused, so
  // the next materia lands in slot 1.
  Character dup("dup");
  AMateria* once = new Ice();
  dup.equip(once);
  dup.equip(once);
  dup.equip(new Cure());
  CHECK(captureUse(dup, 1, bob) == "* heals bob's wounds *\n");

  // Floor behavior: unequip does NOT delete, materia stays usable.
  AMateria* floor = new Cure();
  Character solo("solo");
  solo.equip(floor);
  solo.unequip(0);
  CHECK(floor->getType() == "cure");        // still alive
  CHECK(captureUse(solo, 0, bob) == "");    // slot now empty
  solo.equip(floor);                        // re-equip into the hole
  CHECK(captureUse(solo, 0, bob) == "* heals bob's wounds *\n");
  // solo owns it again; its destructor deletes it.

  // --- Character deep copy ------------------------------------------------
  Character* a = new Character("a");
  a->equip(new Ice());
  a->equip(new Cure());
  Character b(*a);
  Character c("c");
  c.equip(new Cure());  // will be deleted by assignment (valgrind checks)
  c = *a;
  delete a;  // copies must be independent of the dead original
  CHECK(b.getName() == "a");
  CHECK(c.getName() == "a");
  CHECK(captureUse(b, 0, bob) == "* shoots an ice bolt at bob *\n");
  CHECK(captureUse(c, 1, bob) == "* heals bob's wounds *\n");

  // Self-assignment must not free the inventory.
  Character& cref = c;
  c = cref;
  CHECK(captureUse(c, 0, bob) == "* shoots an ice bolt at bob *\n");

  std::cout << "OK" << std::endl;
  return 0;
}
