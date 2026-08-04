#include <iostream>

class ICharacter {
	public:
		virtual ~ICharacter();
		virtual std::string const& getName() const = 0;
		virtual void equip(AMateria* m) = 0;
		virtual void unequip(int idx) = 0;
		virtual void use(int idx, ICharacter& target) = 0;
};

class  Character: public ICharacter {
	private:
		const std::string	_name;
		AMateria*			_inventory[4];
	
	public:
		Character() : _name("Default") {
			for (int i = 0; i < 4; ++i)
				_inventory[i] = NULL;
		}
		Character(const std::string& name): _name(name){
			for (int i = 0; i < 4; ++i)
				_inventory[i] = NULL;
		}
		Character(const Character& other) : {
			int i = 0;
			try {
				for (int i = 0; i < 4; i++)
					_inventory[i]  = _inventory[i] ? _inventory[i]->clone() : NULL;
			} catch(...){
				while (--i > 0)
					delete _inventory[i];
				throw;
			}
			
		}
		Character& operator=(const Character& other) {
			if (this != &other)
			{
				_name = other._name;
				AMateria* fresh[4];
				int i = 0;
				try {
					for (; i < 4; ++i)
						fresh[i] = _inventory[i] ? _inventory[i]->clone() : NULL;
				} catch (...) {
					while (--i > 0) delete fresh[i];
					throw;
				}
				for (int j = 0; j < 4; j++) {
					delete _inventory[j];
					_inventory[j] = fresh[j];
				}
			}
			return (*this);
		}
		~Character(){
			for (int i = 0; i < 4; i++){
				if (_inventory[i]) delete _inventory[i];
			}
		}
		virtual std::string const& getName() const {return _name;}
		virtual void equip(AMateria* m) {
			if (!m)
				return ;
			for (int i = 0; i < 4; ++i)
				if (_inventory[i] == m) return ;
			for (int i = 0; i < 4; ++i) {
				if (!_inventory[i]) {
					_inventory[i] = m;
					return;
				}
			}
		}
		// the unequip must not delete the materia
		virtual void unequip(int idx) {
			if (idx >= 0 && idx < 4)
				_inventory[idx] = NULL;
		}
		void Character::use(int idx, ICharacter& target) {
			if (idx >= 0 && idx < 4 && _inventory[idx])
				_inventory[idx]->use(target);
		}

};