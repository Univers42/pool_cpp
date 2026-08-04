#pragma once

#include <iostream>
#include <string>

class ICharacter;

class AMateria
{
	protected:
		std::string _type;

	public:
		AMateria() : _type("generic") {}
		AMateria(const std::string& type) : _type(type) {}

		AMateria(const AMateria& other)
			: _type(other._type)
		{}

		AMateria& operator=(const AMateria& other)
		{
			(void)other;
			return *this;
		}

		virtual ~AMateria() {}

		std::string const& getType() const
		{
			return _type;
		}

		virtual AMateria* clone() const = 0;

		virtual void use(ICharacter& target)
		{
			std::cout << "* uses generic materia *"
					  << std::endl;
			(void)target;
		}
};