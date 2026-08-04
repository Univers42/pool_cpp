#pragma once

#include <string>
#include "ICharacter.hpp"
#include "AMateria.hpp"

class Character : public ICharacter
{
	private:
		const std::string _name;
		AMateria* _inventory[4];

	public:

		Character() : _name("Default")
		{
			for (int i = 0; i < 4; i++)
				_inventory[i] = NULL;
		}

		Character(const std::string& name)
			: _name(name)
		{
			for (int i = 0; i < 4; i++)
				_inventory[i] = NULL;
		}


		Character(const Character& other)
			: _name(other._name)
		{
			for (int i = 0; i < 4; i++)
				_inventory[i] = other._inventory[i]
					? other._inventory[i]->clone()
					: NULL;
		}


		Character& operator=(const Character& other)
		{
			if (this != &other)
			{
				// _name cannot be assigned because it is const

				for (int i = 0; i < 4; i++)
				{
					delete _inventory[i];

					_inventory[i] =
						other._inventory[i]
						? other._inventory[i]->clone()
						: NULL;
				}
			}
			return *this;
		}


		~Character()
		{
			for (int i = 0; i < 4; i++)
				delete _inventory[i];
		}


		std::string const& getName() const
		{
			return _name;
		}


		void equip(AMateria* m)
		{
			if (!m)
				return;

			for (int i = 0; i < 4; i++)
			{
				if (!_inventory[i])
				{
					_inventory[i] = m;
					return;
				}
			}
		}


		void unequip(int idx)
		{
			if (idx >= 0 && idx < 4)
				_inventory[idx] = NULL;
		}


		void use(int idx, ICharacter& target)
		{
			if (idx >= 0 && idx < 4 && _inventory[idx])
				_inventory[idx]->use(target);
		}
};