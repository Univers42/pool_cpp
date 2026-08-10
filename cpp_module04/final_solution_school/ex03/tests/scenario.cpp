#include "../AMateria.hpp"
#include "../Character.hpp"
#include "../Cure.hpp"
#include "../Ice.hpp"
#include "../ICharacter.hpp"
#include "../IMateriaSource.hpp"
#include "../MateriaSource.hpp"
#include <iostream>


int main()
{
    std::cout << "=========================================\n";
    std::cout << "       POKEMON BATTLE SIMULATOR\n";
    std::cout << "=========================================\n\n";

    //-------------------------------------------------------
    // Professor Oak prepares all available moves
    //-------------------------------------------------------

    MateriaSource professor;

    professor.learnMateria(new Ice());
    professor.learnMateria(new Cure());

	std::cout << "Trying to create ice... \n";
	AMateria* test = professor.createMateria("ice");
	std::cout << "Returned: " << test << '\n';
	delete test;
    //-------------------------------------------------------
    // Trainers enter the arena
    //-------------------------------------------------------

    Character pikachu("Pikachu");
    Character charizard("Charizard");

    //-------------------------------------------------------
    // Professor teaches moves
    //-------------------------------------------------------

    AMateria* pikaIce  = professor.createMateria("ice");
    AMateria* pikaHeal = professor.createMateria("cure");

    AMateria* charIce  = professor.createMateria("ice");
    AMateria* charHeal = professor.createMateria("cure");

    pikachu.equip(pikaIce);
    pikachu.equip(pikaHeal);

    charizard.equip(charIce);
    charizard.equip(charHeal);

#ifdef DEBUG
    std::cout << "\n========== INVENTORIES ==========\n";
    pikachu.printInventory();
    charizard.printInventory();
#endif

    //-------------------------------------------------------
    // Round 1
    //-------------------------------------------------------

    std::cout << "\n========== ROUND 1 ==========\n";
    pikachu.use(0, charizard);

    //-------------------------------------------------------
    // Round 2
    //-------------------------------------------------------

    std::cout << "\n========== ROUND 2 ==========\n";
    charizard.use(0, pikachu);

    //-------------------------------------------------------
    // Round 3
    //-------------------------------------------------------

    std::cout << "\n========== ROUND 3 ==========\n";
    pikachu.use(1, pikachu);

    //-------------------------------------------------------
    // Round 4
    //-------------------------------------------------------

    std::cout << "\n========== ROUND 4 ==========\n";
    charizard.use(1, charizard);

    //-------------------------------------------------------
    // Pikachu drops Ice
    //-------------------------------------------------------

    std::cout << "\n========== DROP MOVE ==========\n";

    pikachu.unequip(0);

#ifdef DEBUG
    pikachu.printInventory();
#endif

    //-------------------------------------------------------
    // Trying to use removed move
    //-------------------------------------------------------

    std::cout << "\n========== FAILED ATTACK ==========\n";

    pikachu.use(0, charizard);

    //-------------------------------------------------------
    // Charizard attacks again
    //-------------------------------------------------------

    std::cout << "\n========== FINAL ROUND ==========\n";

    charizard.use(0, pikachu);

    //-------------------------------------------------------
    // Clean up dropped Materia
    //-------------------------------------------------------

    std::cout << "\n========== CLEANING FLOOR ==========\n";

    delete pikaIce;     // unequipped -> Character won't delete it

    std::cout << "\nBattle finished!\n";

    return 0;
}