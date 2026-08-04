class IMateriaSource {
	public:
		virtual ~IMateriaSource() {}
		virtual void learnMateria(AMateria* m) = 0;
		virtual AMateria* createMateria(std::string const& type) = 0;
};

class MateriaSource : public IMateriaSource {
	private:
		AMateria* _templates[4];

	public:
		MateriaSource() {
			for (int i = 0; i < 4; i++)
				_templates[i] = NULL;
		}

		MateriaSource(const MateriaSource& other) {
			for (int i = 0; i < 4; i++)
				_templates[i] = NULL;

			int i = 0;
			try {
				for (; i < 4; i++)
					_templates[i] = other._templates[i]
						? other._templates[i]->clone()
						: NULL;
			} catch (...) {
				while (i--)
					delete _templates[i];
				throw;
			}
		}

		MateriaSource& operator=(const MateriaSource& other) {
			if (this != &other) {
				AMateria* fresh[4];
				for (int i = 0; i < 4; i++)
					fresh[i] = NULL;

				int i = 0;
				try {
					for (; i < 4; i++)
						fresh[i] = other._templates[i]
							? other._templates[i]->clone()
							: NULL;
				} catch (...) {
					while (i--)
						delete fresh[i];
					throw;
				}

				for (int i = 0; i < 4; i++) {
					delete _templates[i];
					_templates[i] = fresh[i];
				}
			}
			return *this;
		}

		~MateriaSource() {
			for (int i = 0; i < 4; i++)
				delete _templates[i];
		}

		virtual void learnMateria(AMateria* m) {
			if (!m)
				return;

			for (int i = 0; i < 4; i++) {
				if (_templates[i] == NULL) {
					_templates[i] = m;
					return;
				}
			}

			delete m;
		}

		virtual AMateria* createMateria(std::string const& type) {
			for (int i = 0; i < 4; i++) {
				if (_templates[i] && _templates[i]->getType() == type)
					return _templates[i]->clone();
			}
			return NULL;
		}
};