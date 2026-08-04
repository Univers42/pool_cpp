#pragma once

#include <iostream>
class AMateria {
	protected:
		std::string _type;

	public:
		AMateria(): _type("generic type"){}
		AMateria(const std::string& type): _type(type){}
		AMateria(const AMateria& other): _type(other.type){}
		AMateria& operator=(const AMateria& other){
			(void)other;
			return *this;
		}
		virtual ~AMateria(){}
		std::string const& getType() const {return (_type);}
		virtual AMateria* clone() const = 0;
		// need to be better defined.. 
		virtual void use(ICharacter& target){
			std::cout << "* uses generic materia on" << target.Name() << " *" << std::endl;
		}
}