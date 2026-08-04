#pragma once
#include "AMateria.hpp"
#include "Character.hpp"

class Ice : public AMateria{
	public:
		Ice():AMateria("Ice"){}
		Ice(const Ice& other) : AMateria(other){}
		/**
			subject told us that it doesn't make sense to copy the _type so nothing should happen 
		*/
		Ice& operator=(const Ice& other){
			(void)other;
			return (*this);
		}
		virtual ~Ice(){}
		virtual AMateria* clone() const{ return new Ice(*this); }
		virtual void use(ICharacter& target){
			std::cout << "* Shoots an ice bolt at " << target.getName() << " *" << std::endl;
		}
		
};