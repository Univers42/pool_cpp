/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MateriaSource.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/01 00:17:18 by dlesieur          #+#    #+#             */
/*   Updated: 2026/08/04 17:13:09 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// MateriaSource.hpp
#ifndef CPP_MODULE04_EX03_MATERIASOURCE_HPP_
#define CPP_MODULE04_EX03_MATERIASOURCE_HPP_

#include <string>
#include "AMateria.hpp"
#include "IMateriaSource.hpp"

/**
FACTORY: prototype factory
It's not a character and it's not a spell. 

Let's think about harry potter:
we have:
spells->`ice`, `cure`
wizard->`character`
spell library / spell book : `materia source`

The wizard doesn't know how to create spells from scratcch
Instead he goes to the library and says:
- "give me an Ice spell"
The library already knows what an ice spell looks like.

In our project:
We have 
`ice
cure
`

both inherit form 
`AMateria`

Materia source stores on prototype of each spell

_templates[0] --> Ice prototype
_templates[1] --> cure prototype
_templates[2] --> NULL
_templates[3] --> NULL

There are no speels our characters uses
They're just examples kept in memory

learnMateria()

```cpp
Materia src;
src.learnMateria(new Ice());
src.learnMateria(new Cure());
```

The source has now learned two materia typese

MateriaSource searches:

AMateria* spell = src.createMateria("ice");
Materiasource searches:

```cpp
_templates[0]
is this ice?
```
yes then it does

`return templates[0]->clone();`

it creates a copy

Let's imagine it returned the prototype directly:

```cpp
AMateria* a = src.createMateria("ice");
AMateria* b = src.createMateria("ice");
```
now both pointers point to  the same Ice object

then 
delete a;
would destroyy the object;

nw b woudl point ot freed memoryy;
crash
*/
class MateriaSource : public IMateriaSource {
 private:
  AMateria* _templates[4];

 public:
  MateriaSource();
  MateriaSource(const MateriaSource& src);
  MateriaSource& operator=(const MateriaSource& rhs);
  virtual ~MateriaSource();

  virtual void learnMateria(AMateria*);
  virtual AMateria* createMateria(std::string const& type);
};
#endif  // CPP_MODULE04_EX03_MATERIASOURCE_HPP_
