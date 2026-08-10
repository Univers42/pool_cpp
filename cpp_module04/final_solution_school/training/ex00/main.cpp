#include <iostream>
#include <string>

#ifndef DEBUG
# define DEBUG false
#endif
/**
LESSON: 1
- virtual dispatch (*Animal -> Dog)
- Non virtual dispatch( Wronganimal* -> WrongCat)
- Copy constructor and assignment operator
- virtual destructor
LESSON:2 composition (has ?) vs inheritance (is ?)
- the rule of three
- deep copy vs shallow copy
LESSON:3
instead of saying 
Animal a;
we ask ourself if it already exist ?
not really so we only ever use 
- Dog
- Cat

so animal becomes just a  concept..
if a class has at least one pure virtual function (= 0), the entire 
class becomes abstract
*/

namespace debug {
    const bool enabled = DEBUG;
    template<typename... Args>
    void log(Args&&... args){
        if (!enabled)
            return ;
        (std::cout << ... << args) << std::endl;
    }
}

class Brain{
    private:
        std::string ideas[100];
    public:
        void setIdea(int index, const std::string& idea){ ideas[index] = idea; }
        std::string getIdea(int index) const { return ideas[index]; }
};


namespace good{
    // from the ex02 we need to create it as abstract class using = 0 per default to tell that is not instantiable...
    class Animal {
        protected:
            std::string type;

        public: 
            Animal(): type("Animal") {debug::log("Animal Constructor");}
            Animal(const std::string& t): type(t) {debug::log("Animal constructor");}
            Animal(const Animal& other): type(other.type) {debug::log("copy constructor Animal");}
            Animal& operator=(const Animal& other) {
                if (this != &other)
                    this->type = other.type;
                return (*this);
            }

            virtual ~Animal(){debug::log("Animal destructor");}
            virtual void makeNoise(void) const = 0;
            std::string getType(void) const {return (this->type);}
    };
    
    class Dog: public Animal {
        private:
            // composition
            Brain* b;

        public:
            Dog(void): Animal("Dog"){b = new Brain(), debug::log("dog constructor");}
            Dog(const Dog& other): Animal(other) {this->b = new Brain(*other.b);debug::log("copy constructor");}
            Dog& operator=(const Dog& other){
                if (this != &other)
                    Animal::operator=(other);
                return (*this);
            }
            ~Dog(){delete b;debug::log("Dog Destructor");}
            void makeNoise(void) const{
                std::cout<<"wouf!" << std::endl;
            }
            void setIdea(int i, const std::string& s) { b->setIdea(i, s);}
            std::string getIdea(int i) const { return (b->getIdea(i)); }
    };
    
    class Cat: public Animal {
        private:
        // composition
            Brain* b;

        public:
            Cat(void): Animal("Cat"){debug::log("cat::constructor"); b = new Brain();}
            Cat(const Cat& other): Animal(other){debug::log("cat copy constructor");}
            Cat& operator=(const Cat& other) {
                if (this != &other)
                    Animal::operator=(other);
                return (*this);
            }
            ~Cat(){debug::log("cat destructor"); delete b;}
            void makeNoise(void) const{
                std::cout<<"meow!" << std::endl;
            }
    };
}

namespace wrong {
    class Animal {
        protected:
            std::string type;

        public:
            Animal(): type(""){/*ctor*/}
            Animal(const std::string& t): type(t){/**to call directly the animal from inherited classes */}
            Animal(const Animal& other): type(other.type){/*copy ctor*/}
            Animal& operator=(const Animal& other){
                if (this != &other)
                    this->type = other.type;
                return (*this);
            }
            ~Animal(void){debug::log("Animal destructor");}
            // here voluntarely not put to virtual to print the noise by default and not he good noise
            void makeSound(void) const {
                std::cout << "Animal produce a noise" << std::endl;
            }
            std::string getType(void) const {return (this->type);}
    };

    class Cat: public Animal {
        public:
            Cat(void): Animal("Cat"){}
            Cat(const Cat& other): Animal(other){}
            Cat& operator=(const Cat& other){
                if (this != &other)
                    Animal::operator=(other);
                return (*this);
            }
            ~Cat(){}
            void makeSound(void) const {
                std::cout << "Wrong meow!" << std::endl;
            }
    };
}

namespace term
{
    constexpr const char* reset  = "\033[0m";
    constexpr const char* bold   = "\033[1m";

    constexpr const char* red    = "\033[31m";
    constexpr const char* green  = "\033[32m";
    constexpr const char* yellow = "\033[33m";
    constexpr const char* blue   = "\033[34m";
    constexpr const char* purple = "\033[35m";
    constexpr const char* cyan   = "\033[36m";
}

/**
Design pattern
*/
namespace game{
    class ICharacter {
        public:
            virtual ~ICharacter() {}
            virtual std::string const& getName() const = 0;
            virtual void equip(AMateria* m) = 0;
            virtual void unequip(int idx) = 0;
            virtual void use(int idx, ICharacter& target) = 0;
    };

    class AMateria;
    class IMateriaSource {
      public:
        virtual -IMateriaSource();
        virtual void learnMateria(AMateria*) = 0;
        virtual AMateria* createMateria(std::string, const& type) = 0;  
    };

    class MateriaSource: public IMateriaSource {
        private:
            AMateria* _templates[4];

        public:
            MateriaSource(){
                for (int i = 0; i < 4; i++)
                    this->_templates[i] = NULL;
            }

            MateriaSource(const MateriaSource& other){
                int i = 0;
                
                try {
                    for (; i < 4; i++){
                        this->_templates[i] = other._templates[i] ? other._templates[i]->clone() : NULL;
                    } catch(...) {
                        while (i-- > 0)
                            delete this->templates[i];
                        throw;
                    }
                }
            }
            MateriaSource& operator=(const MateriaSource& rhs){
                if (this != &rhs) {
                    AMateria* fresh[4];
                    int i = 0;
                    try {
                        for (; i < 4; i++)
                            fresh[i] = rhs._templates[i] ? rhs._templates[i]->clone() : NULL;
                    } catch(...) {
                        while (i-- > 0) delete fresh[i];
                        throw;
                    }
                }
                return (*this);
            }
            virtual ~MateriaSource(){
                for (int i = 0; i < 4; i++)
                    if (this->_templates[i])
                        delete (this->_templates[i]);
            }
            virtual learnMateria(AMateria* m){
                if (!m) return;
                for (int i = 0; i < 4; i++){
                    if (this->_templates[i] == NULL) {
                        this->_templates[i] = m->clone();
                        delete m;
                        return ;
                    }
                }
                delete m;
            }
            virtual createMateria(std::string const& type){
                for (int i = 0; i < 4; i++) {
                    if (this->_templates[i] && this->_templates[i]->getType() == type) {
                        return (this->_templates[i]->clone());
                    }
                }
                return (NULL);
            }
    };

    // this class is pure class abstract meaning it's not instantiable..
    class AMateria {
        protected:
            std::string type;

        public:
            AMateria(): type("Generic") {/*primary constructor*/}
            // this function will be used furthermore to set character with materia.. directly.. because otherwise we should use a getter..
            explicit AMateria(const std::string& type): type(type){/*secondayr construcotr*/}
            AMateria(const AMateria& other): type(other.type){}
            AMateria& operator=(const AMateria& other){
                if (this != &other)
                    this->type = other.type;
                return (*this);
            }
            ~AMateria(){}
            const std::string& getType() const { return (this->type); }; // getters return the materia type..
            virtual AMateria& clone() const = 0;
            virtual void use(ICharacter& target) {
                std::cout << "* uses generic materia on " << target.getName() << " *" << std::endl;
            }
    };


    class Character: public ICharacter {
        private:
            std::string _name;
            AMateria* _inventory[4]; // inventory of 4 slots cap max;
        
        public:
            Character(): _name("default") {
                for (int i = 0; i < 4; i++)
                    _inventory[i] = NULL;
            }

            explicit Character(const std::string& name): _name(name){
                for (int i = 0; i < 4; i++)
                    _inventory[i] = NULL;
            }

            Character(const Character& other){
                int i = 0;
                try {
                    for (; i < 4; i++)
                        this->_inventory[i] = src.inventory[i] ? src.inventory[i]->clone() : NULL;
                } catch (...){
                    while (i-- > 0)
                        delete this->_inventory[i];
                    throw;
                }
            }

            Character& operator=(const Character& other){
                if (this != &rhs)
                    this->_name = other.name;
                AMateria* fresh[4];
                int i = 0;
                try{
                    for (; i < 4; i++)
                        fresh[i] = other._inventory[i] ? other._inventory[i]->clone(): NULL;
                } catch (...){
                    while (i-- > 0) delete fresh[i];
                    throw;
                }
                for (int j = 0; j < 4; j++) {
                    delete this->_inventory[j];
                    this->inventory[j] = fresh[j];c
                }
                return (*this);
            }

            ~Character(){
                for (int i = 0; i < 4; i++)
                    if (this->_inventory[i])
                    delete this->_inventory[i];
            }

            std::string csdonst& getName() const { return (this->_name)};
            void equip(AMateria* m) {
                if (!m)
                    return ;
                // refuse double ownership; equipping the same pointer twice would make
                // the destructor delete it twice
                for (int i = 0; i < 4; i++)
                    if (this->_inventory[i] == m) return ;
                for (int i = 0; i < 4; i++)
                    if (this->inventory[i] == NULL){
                        this->inventory[i] = m;
                        return ;
                    }
            }

            void unequip(int index) {
                if (index >= 0 && index < 4){
                    this->_inventory[index] = NULL;
                }
            }

            /**
                here it happens something interesting as inventory slots
                are seeded with the type AMateria we use its use function memebr that 
                points to an interface character... so first why ? if it's used to attakc
                how it knows which to targets
            */
            void use(int idx, ICharacter& target) {
                if (idx >= 0 && idx < 4 && this->_inventory[idx])
                    this->_inventory[idx]->use(target);
            }

    };

    class Ice: public AMateria {
        public:
            Ice():AMateria("ice") {}
            Ice(const Ice& other): AMateria(other){}
            Ice& operator=(const Ice& other){(void)rhs; return (*this);}
            virtual ~Ice();
            virtual AMateria() const{return new Ice(*this);}
            virtual void use(ICharacter& target){
                std::cout << "* shoots an ice bolt at " << target.getName() << " *" << std::endl;
            }
    };

    class Cure: publice AMateria {
        public:
            Cure(): AMateria("cure") {}
            Cure(cons Cure& other): AMateria(other){}
            Cure& operator=(const Cure& other){(void)other; return *this;}
            ~Cure(){}
            virtual AMateria* clone() const {return new Cure(*this);}
            virtual void use(ICharacter& target){
                std::cout << "* heals" << target.getName() << "'s wounds *" << std::endl;
            }
    };
    
}

void banner(const std::string& title,
            const char* color = term::cyan)
{
    const std::size_t padding = 4;
    const std::size_t width = title.size() + padding * 2;

    std::cout << '\n';

    std::cout << color << term::bold;
    std::cout << "╔";
    for (std::size_t i = 0; i < width; i++)
        std::cout << "═";
    std::cout << "╗\n";

    std::cout << "║";
    for (std::size_t i = 0; i < padding; i++)
        std::cout << ' ';

    std::cout << title;

    for (std::size_t i = 0; i < padding; i++)
        std::cout << ' ';
    std::cout << "║\n";

    std::cout << "╚";
    for (std::size_t i = 0; i < width; i++)
        std::cout << "═";
    std::cout << "╝";

    std::cout << term::reset << "\n\n";
}


int main(void){
    //const good::Animal* meta = new good::Animal();
    const good::Animal* j = new good::Dog();
    const good::Animal* i = new good::Cat();
    const wrong::Animal* x = new wrong::Cat();

    std::cout << j->getType() << " " << std::endl;
    std::cout << i->getType() << " " << std::endl;
    std::cout << x->getType() << " wrong " << std::endl;
    x->makeSound();
    i->makeNoise();
    j->makeNoise();
    //meta->makeNoise();
    //runtime polymorphism
    //{
    //    banner("Polymorphism runtime test");
    //    good::Animal* animals[] = {
    //        new good::Animal(),
    //        new good::Dog(),
    //        new good::Cat()
    //    }; 
    //    for (int i = 0; i < 3; i++)
    //    {
    //        std::cout << animals[i]->getType() << ": ";
    //        animals[i]->makeNoise();
    //    }
    //    for (int i = 0;i < 3; i++)
    //        delete animals[i];
    //}

    //wrongAnimal
    {
        banner("WrongAnimal");
        wrong::Animal* a = new wrong::Cat();
        a->makeSound();
        delete a;
    }

    // copy ctor
    {
        banner("copy ctor");
        good::Dog d1;
        good::Dog d2(d1);
        std::cout << d1.getType() << std::endl;
        std::cout << d2.getType() << std::endl;
        d2.makeNoise();
    }

    // assignment operator
    {
        banner("assignment operator");
        good::Cat c1;
        good::Cat c2;
        c2 = c1;
        std::cout << c2.getType() << '\n';
        c2.makeNoise();
    }

    //self assignment
    {
        banner("Self assignment");
        good::Dog d;
        d = d;
        d.makeNoise();
    }
    // reference polymorphism
    {
        banner("referece polymorphism");
        banner("destructor order");
        good::Dog d;
        good::Animal& ref = d;
        ref.makeNoise();
    }

    //destructor order
    //{
    //    
    //    good::Animal* a = new good::Dog();
    //    delete a;
    //}
    
    // type check
    //{
    //    banner("type check");
    //    good::Animal a;
    //    good::Dog d;
    //    good::Cat c;
    //    std::cout << a.getType() << std::endl;
    //    std::cout << d.getType() << std::endl;
    //    std::cout << c.getType() << std::endl;
    //}

    // const correctness
    {
        banner("const correctness");
        const good::Dog d;
        d.makeNoise();
        d.getType();
    }

    // construction / destruction order
    {
        banner("construction / destruction order");
        good::Dog d;
    }

    // stack vs heap
    {
        banner("stack vs heap");
        good::Dog* f = new good::Dog();
        delete f;
    }
    
    //delete meta;
    delete j;
    delete i;
    delete x;

    {
        /**
            What is the type of `a` is using hte wrong namespac ?
            Animal*
            the compiler says:
            `a` is an `Animal*`, so I'll call `Animal::makeSound()`
            It doesn't care that the object is actually a `Dog`
            This is called `static binding`
                            Compiler
                                |
                                v
            Animal*---------------------> Animal::makeSound()
            With Virtual now using the good namespace
            The compiler cannot decide which function to call
            instead it generates codes lie:
            ```
            ask the object what it really is
            ``` 
            At runtime:
            ```
            Animal
                |
                v
            object in memory
            ```
            The object knows its real type now.
            This is where the `vtable` kicks in.
            Think of every polymorphic object as secretly containing another pointer
            A dog object is actually something like 

            ```
                +----------------+
                | vptr           |-----+
                | type           |     |
                +----------------+     |
                                       |
                                       ▼
                                +------------------+
                                | Dog::makeSound() |
                                | Dog::~Dog()      |
                                +------------------+
            That hidden pointer is called the `vptr`
            it points tot the `virtual table
            Dog object
            The object itself tells the program
            "I'm actually a Dog"
            in runtime it goes like thsi
            Animal* a = new Dog;
            a->makeSound();

            +++
                look at object
                ↓
                follow vptr
                ↓
                loop up makeSound
                ↓
                call function

            +++

            Finally the virutal is useful because the pointer
            Animal*
            can point to
            Animal
            Dog
            Cat
            Horse
            Lion
            and we don¡'t have to write something like this
            if (dynamic_cast<Dog*>(a))
            ....
            else if (dynamic_cast<Cat*>(a))
            ...
            the object already knows what it is.
            Same thing for destructor
        */
        good::Cat d;
        good::Animal* a = &d;
        a->makeNoise(); // if good namespace meow, otherwise animal proff of `static binding`
    }
    // create and fill an array of Animal objects. Half of it will be dog objects
    {
        good::Animal* animals[100];
        for (int i = 0; i < 100; i++){
            if (i >= 50)
                animals[i] = new good::Dog();
            else
                animals[i] = new good::Cat();
        }
        for (int i = 0; i < 100; i++)
            delete animals[i];
    }
    {
        good::Animal* animals[100];

        for (int i = 0; i < 100; i++)
            animals[i] = (i < 50) ? static_cast<good::Animal*>(new good::Dog()) : static_cast<good::Animal*>(new good::Cat());
        for (int i = 0; i < 100; i++)
        {
            std::cout << animals[i]->getType() << std::endl;
            animals[i]->makeNoise();
        }
        for (int i = 0; i < 100 ; i++)
            delete animals[i];
    }

    // Deep Copy
    {
        banner("Deep Copy");
        good::Dog d1;
        d1.setIdea(0, "I love bones");
        good::Dog d2(d1);
        d2.setIdea(0, "I love pizza");
        std::cout << d1.getIdea(0) << std::endl;
        std::cout << d2.getIdea(0) << std::endl;
    }
    
    // deep assignment
    {
        banner("Deep assignment");
        good::Dog d1;
        good::Dog d2;

        d1.setIdea(0, "Bone");
        d2.setIdea(0, "Pizza");
        d2 = d1;
        d2.setIdea(0, "Carrot");
        std::cout << d1.getIdea(0) << std::endl;
        std::cout << d2.getIdea(0) << std::endl;
    }
    IMateriaSource* src = new MateriaSource();
    src->learnMateria(new Ice());
    src->learnMateria(new Cure());
    







    ICharacter* se = new Charater("me");
    AMateria* tmp;
    tmp = src->createMateria("ice");
    me->equip(tmp);
    tmp  = src->createMateria("cure");
    me->equip(tmp);
    ICharacter* bob = new Character("bob");
    me->use(0, *bob);
    me->use(1, *bob);
    delete bob;
    delete me;
    delete src;
    return (0)
}
