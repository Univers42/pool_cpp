#pragma once
#include "AMateria.hpp"

class Cure : public AMateria{
	public:
		Cure():AMateria("Cure"){}
		Cure(const Cure& other) : AMateria(other){}
		/**
			subject told us that it doesn't make sense to copy the _type so nothing should happen 
		*/
		Cure& operator=(const Cure& other){
			(void)other;
			return (*this);
		}
		virtual ~Cure(){}
		virtual AMateria* clone() const{ return new Cure(*this); }
		virtual void use(Icharacter& target){
			std::cout << "* heals " << target.getName() << "'s wounds *" << std::endl;
		}
		
};