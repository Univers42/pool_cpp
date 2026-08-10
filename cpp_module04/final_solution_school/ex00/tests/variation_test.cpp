#include <iostream>

#include "../Animal.hpp"
#include "../Cat.hpp"
#include "../Dog.hpp"
#include "../WrongAnimal.hpp"
#include "../WrongCat.hpp"
#include <iostream>
#include "../utils.hpp"
#include <typeinfo>

/**
LESSON:
Virtual workss whenever we're accessing the object
throgh a base class pointer or base-class reference.

- constructor execute fromm the base class to the derived class. 
- Destructors execute in the reverse order: derived class, the base class.
*/
int main() {
const Animal* a = new Dog();
const Animal* b = new Cat();
const WrongAnimal* c = new WrongCat();

const Dog d1;
const Cat c1;

Dog d2 = d1;
Cat c2 = c1;

Dog d3(d1);
Cat c3(c1);

Dog dogs[3];
Cat cats[3];

const Animal* mix_animals[4];

mix_animals[0] = &d1;
mix_animals[1] = &c1;
mix_animals[2] = new Dog();
mix_animals[3] = new Cat();


banner("POINTERS HEAP");
std::cout << "a               : " << a->getType() << " [expected: Dog]" << std::endl;
std::cout << "b               : " << b->getType() << " [expected: Cat]" << std::endl;
std::cout << "c               : " << c->getType() << " [expected: WrongCat]" << std::endl;

banner("STACK OBJECTS");
std::cout << "d1              : " << d1.getType() << " [expected: Dog]" << std::endl;
std::cout << "c1              : " << c1.getType() << " [expected: Cat]" << std::endl;

banner("COPY INITIALIZATION");
std::cout << "d2 = d1         : " << d2.getType() << " [expected: Dog]" << std::endl;
std::cout << "c2 = c1         : " << c2.getType() << " [expected: Cat]" << std::endl;

banner("DIRECT INITIALIZATION");
std::cout << "d3(d1)          : " << d3.getType() << " [expected: Dog]" << std::endl;
std::cout << "c3(c1)          : " << c3.getType() << " [expected: Cat]" << std::endl;

banner("ARRAY OF DOGS");
for (int i = 0; i < 3; ++i)
{
    std::cout << "dogs[" << i << "]        : "
              << dogs[i].getType()
              << " [expected: Dog]" << std::endl;
}

banner("ARRAY OF CATS");
for (int i = 0; i < 3; ++i)
{
    std::cout << "cats[" << i << "]        : "
              << cats[i].getType()
              << " [expected: Cat]" << std::endl;
}

banner("ARRAY OF POINTERS");
std::cout << "mix_animals[0]  : "
          << mix_animals[0]->getType()
          << " [expected: Dog]" << std::endl;

std::cout << "mix_animals[1]  : "
          << mix_animals[1]->getType()
          << " [expected: Cat]" << std::endl;

std::cout << "mix_animals[2]  : "
          << mix_animals[2]->getType()
          << " [expected: Dog]" << std::endl;

std::cout << "mix_animals[3]  : "
          << mix_animals[3]->getType()
          << " [expected: Cat]" << std::endl;


banner("VIRTUAL POLYMORPHISM");
a->makeSound();
b->makeSound();
banner("WITHOUT VIRTUAL");
c->makeSound();


    {
        banner("OBJECT SLICING");
        /**
        This looks like we're storing Doog inside Animal, but that's not
        what happen. 
        we instead building a temporary Dog
        Then we copy into animal, so we don't store the whole dog.
        it coopies on ly the animal subobject
        The dog part is litterally slicded off.
        This is why it's called `object slicing`
        Then the temporary dog is destroy. all that left is animal

        i thought first that the makeSound alone would allow virtual dispatch.
        however, virtual dispatch only chooses between overrides that belong to the object's actual dynamic
        type.

        The actual object is now:
        - Animal
        not Dog
        so the call becomes
        Animal::makeSound()
        The key idea is not heap vs stack. The key idea is pointer/reference vs object by value.

        */
        // object by value
        Animal a = Dog();
        a.makeSound();

        //pointer
        Animal *b = new Dog();
        b->makeSound();
        delete b;

        // by reference, a reference is just another name for the ssame object, nothing is copied. so the object is still a dog
        Dog dog;
        Animal& c = dog;
        c.makeSound();

        Dog d(dog);
        d.makeSound();

        Cat cat;
        cat.makeSound();
    }
    {
        banner("VIRTUAL DESTRUCTOR");
        Animal* animal = new Dog();
        delete animal;
        //expected:
        // Dog destructor
        // Animal destructor
        // [NOTE]: without a vitual destructor, we would only get `Animal` destructor (undefined behavior)
    }
    {
        banner("REFERENCE POLYMORPHISM");
        const Animal& dogRef = d1;
        const Animal& catRef = c1; 
        dogRef.makeSound();
        catRef.makeSound();
    }
    {
        banner("Dynamic dispatch inside loops");
        for (int i = 0; i < 4; i++){
            std::cout << mix_animals[i]->getType() << " -> ";
            mix_animals[i]->makeSound();
        }
    }
    {
        banner("CONSTRUCTION ORDER");
        Dog d;
        // we'll see something like
        // Animal constructor
        // Dog constructor
        // Dog destructor
        // Animal Destructor
    }
    {
        banner("COPY ASSIGNMENT");
        Dog d4;
        d4 = d1;
        d4.makeSound();
    }
    {
        banner("const correctness");
        const Animal* animal = new Dog();
        animal->makeSound();
        std::cout << animal->getType() << std::endl;
        delete animal;
    }
    {
        banner("BASE POINTER TO DERIVED OBJECT");
        Animal* animal = new Dog();
        std::cout << animal->getType() << std::endl;
        animal->makeSound();
        delete animal;
    }
    {
        banner("RTTI");
        Animal* animal = new Dog();
        Dog* dog = dynamic_cast<Dog*>(animal);
        if (dog)
        {
            std::cout << "This is a Dog!" << std::endl;
            dog->makeSound();
        }
        delete animal;
    }
    {
        Animal* animal = new Cat();
        Dog* dog = dynamic_cast<Dog*>(animal);
        if (!dog)
            std::cout << "Not a Dog." <<std::endl;
        delete animal;
        // output: not a dog;
    }
    {
        Animal* animal = new Dog();
        try {
            Dog& dog = dynamic_cast<Dog&>(*animal);
            dog.makeSound();
        } catch (std::bad_cast&){
            std::cout << "Bad Cast!" << std::endl;
        }
        delete animal;
    }
    {
        Animal* animals[4];
        animals[0] =  new Dog();
        animals[1] = new Cat();
        animals[2] = new Dog();
        animals[3] = new Cat();
        for (int i = 0; i < 4; ++i) {
            if (Dog* dog = dynamic_cast<Dog*>(animals[i]))
                std::cout << "Found a dog!" <<std::endl;
            if (Cat* cat = dynamic_cast<Cat*>(animals[i]))
                std::cout << "Found a Cat!" << std::endl;
        }
        for (int i = 0; i < 4; ++i) delete animals[i];
    }
    delete a;
    delete b;
    delete c;
    delete mix_animals[2];
    delete mix_animals[3];
    
}
