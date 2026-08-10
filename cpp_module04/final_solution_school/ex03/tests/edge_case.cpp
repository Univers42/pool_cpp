// Subject-provided main, verbatim. Extra tests live in tests/test.cpp.

//#include "Character.hpp"
//#include "Cure.hpp"
//#include "Ice.hpp"
//#include "MateriaSource.hpp"

/**
An interface is not "nothing". It's a promise taht any object implementing it has certain 
functions.
*/
// int main() {
//   IMateriaSource* src = new MateriaSource();
//   src->learnMateria(new Ice());
//   src->learnMateria(new Cure());

//   ICharacter* me = new Character("me");

//   AMateria* tmp;
//   tmp = src->createMateria("ice");
//   me->equip(tmp);
//   tmp = src->createMateria("cure");
//   me->equip(tmp);

//   ICharacter* bob = new Character("bob");

//   me->use(0, *bob);
//   me->use(1, *bob);

//   delete bob;
//   delete me;
//   delete src;

//   return 0;
// }


#include <iostream>

#include "../Character.hpp"
#include "../Ice.hpp"
#include "../Cure.hpp"
#include "../IMateriaSource.hpp"
#include "../MateriaSource.hpp"

/**
canonical form
polymorphism
clone()
deep copy
ownership transfer
deleting through abse pointers
inventory limits
learning limits
null pointers
unknown materia
doublde delete prevention
memory leaks
use-after-free prevention
self assignment
*/
int main(void) {
    {
        IMateriaSource* src = new MateriaSource();
        src->learnMateria(new Ice());
        src->learnMateria(new Cure());
        ICharacter* me = new Character("me");
        AMateria* tmp;
        tmp = src->createMateria("ice");
        me->equip(tmp);
        tmp = src->createMateria("cure");
        me->equip(tmp);
        Character bob("bob");
        me->use(0, bob);
        me->use(1, bob);
        delete me;
        delete src;
    }
    // inventory full
    {
        Character hero("hero");
        hero.equip(new Ice());
        hero.equip(new Ice());
        hero.equip(new Cure());
        hero.equip(new Cure());
        // should fail
		AMateria* extra = new Ice();
        hero.equip(extra);
		delete extra;
        // if equip refuses ownership
        // we must delete it afterwards
    }
    // unequip test
    {
    Character hero("hero");

    AMateria* ice = new Ice();

    std::cout << "\n========== BEFORE EQUIP ==========\n";
    hero.printInventory();

    hero.equip(ice);

    std::cout << "\n========== AFTER EQUIP ==========\n";
    hero.printInventory();

    hero.unequip(0);

    std::cout << "\n========== AFTER UNEQUIP ==========\n";
    hero.printInventory();

    std::cout << "\nExternal pointer still owns: "
              << ice << " (" << ice->getType() << ")\n";

    delete ice;
}
    //double equip test
    {
        Character hero("hero");
        AMateria* ice = new Ice();
        hero.equip(ice);
        hero.printInventory();
        hero.unequip(0);
        hero.printInventory();
        hero.unequip(0);
        hero.printInventory();
        delete ice;
        // should do nothing
    }

    // Use empty slot
    {
        std::cout << "\n USE of emmpty slot, nothing should happen" << std::endl;
        Character hero("hero");
        Character enemy("enemy");
        hero.printInventory();
        hero.use(0, enemy);
        hero.printInventory();
    }
    // Invalid Index
    {
        Character hero("hero");
        Character enemy("enemy");
        hero.use(-1, enemy);
        hero.use(4, enemy);
        hero.use(42, enemy);
        hero.unequip(-2);
        hero.unequip(5);
    }
    // Equip null pointer
    {
        Character hero("hero");
        hero.equip(NULL);
        // should do nothing;
    }
    // unknown materia
    {
        MateriaSource src;
        src.learnMateria(new Ice());
        AMateria* test = src.createMateria("fire");
        if (!test)
            std::cout << "Unknown materia corectly returned NULL" << std::endl;
        test = src.createMateria("ice");
        if (test){
            std::cout << "known materia, correctly returned \n";
            delete test;
        }
    }
    // MATERIA SOURCE FULL
    {
        MateriaSource src;
        src.learnMateria(new Ice());
        src.learnMateria(new Ice());
        src.learnMateria(new Cure());
        src.learnMateria(new Ice());
        // 5th should be rejected;
        // delete if ownership isn't taken
    }
    // copy character
    {
        Character original("original");
        original.equip(new Ice());
        original.equip(new Cure());
        Character copy(original);
        Character enemy("enemy");
        original.use(0, enemy);
        copy.use(0, enemy);
        // should be different poiiinters internally
    }
    // assin a character
    {
        Character a("A");
        a.equip(new Ice());
        a.equip(new Ice());
        Character b("B");
        b = a;
        Character enemy("enemy");
        a.use(0, enemy);
        b.use(0, enemy);
    }
    // self assignment
    {
        std::cout << "\n self assignment test\n";
        Character hero("hero");
        Character* p = &hero;
        *p = hero;

        /**
        or

        Character hero("hero");
        Character& ref = hero;
        hero = ref;
        */
    }
    // copy materiasource
    {
        MateriaSource src;
        src.learnMateria(new Ice());
        src.learnMateria(new Cure());
        MateriaSource copy(src);
        AMateria* m = copy.createMateria("ice");
        delete m;
    }
    // assign materiasource
    {
        MateriaSource a;
        MateriaSource b;
        a.learnMateria(new Ice());
        a.learnMateria(new Cure());
        b = a;
        AMateria* m = b.createMateria("cure");
        delete m;
    }
    {
        AMateria* m = new Ice();
        delete m;
    }
    // many clones
    {
        MateriaSource src;
        src.learnMateria(new Ice());
        for (int i = 0; i< 100; i++){
            AMateria* m = src.createMateria("ice");
            delete m;
        }
    }
    
    //// leak test
    //{
    //    Character hero("hero");
    //    hero.equip(new Ice());
    //    //hero destructor shoulld delete it.
    //    //if not:
    //    //valgrind report definitely lsot.
    //}

    //// double delete test
    //{
    //    Character hero("hero");
    //    AMateria* ice = new Ice();
    //    hero.equip(ice);
    //    delete ice;
    //    // hero destructor deletes again
    //    // -> double free detected.
    //}

    //// use after free
    //{
    //    AMateria* m = new Ice();
    //    delete m;
    //    Character enemy("enemy");
    //    m->use(enemy);
    //    // ASan;
    //    // heap-use-after-free
    //}

    // dangling pointer
    {
        Character hero("hero");
        AMateria* ice = new Ice();
        hero.equip(ice);
        hero.unequip(0);
        delete ice;
        hero.use(0, hero);
        //if unequip forgot to clear slot
        // use() dereferences freed memory
    }
}

/**
Question:
is there any countdown of use ? or we just learn te spell and we can use indefinitely?

*/