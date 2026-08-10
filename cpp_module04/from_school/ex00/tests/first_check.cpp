#include "Animal.hpp"
#include "Logger.hpp"
#include "Srgb.hpp"

int main() {
    // Create animals
    Animal* dog = new Animal();
    Animal* cat = new Animal();
    Animal* animals[3];

    // Display with decorators
    PRINT_LOG(SuccessLog("First animal created: " + dog->getType()));
    std::cout << "First animal: " << dog->getType() << "\n";
    
    PRINT_LOG(SuccessLog("Second animal created: " + cat->getType()));
    std::cout << "Second animal: " << cat->getType() << "\n";
    
    // Initialize array of animals
    PRINT_LOG(WarnLog("Initializing array of 3 animals"));
    for (int i = 0; i < 3; ++i)
        animals[i] = new Animal();

    // Clean up
    PRINT_LOG(WarnLog("Cleaning up memory"));
    {
        delete dog;
        delete cat;
        for (int i = 0; i < 3; ++i)
            delete animals[i];
    }

    return 0;
}