#include <iostream>

#include "../Cat.hpp"
#include "../Dog.hpp"
#include "../utils.hpp"


int	main(void)
{
	std::cout << "Creating basic...\n";
	Dog basic;
	basic.setIdea(0, "I love bones");
	std::cout << "\nEntering scope...\n";
	{
		Dog tmp = basic;	// copy constructr
		std::cout << "basic:	" << basic.getIdea(0) << std::endl;
		std::cout << "tmp: 	" << tmp.getIdea(0) << std::endl;
		tmp.setIdea(0, "I love cats");
		std::cout << "\nAfter modifying tmp:" << std::endl;
		std::cout << "basic: " << basic.getIdea(0) << std::endl;
		std::cout << "tmp: " << tmp.getIdea(0) << std::endl;
	}
	std::cout << "\nExited scope." << std::endl;
	std::cout << "basic still exists." << std::endl;
	std::cout << "basic: " << basic.getIdea(0) << std::endl;
	std::cout << "\nEnd of main." << std::endl;
}

//output
// I want a bone
// I want a cat
