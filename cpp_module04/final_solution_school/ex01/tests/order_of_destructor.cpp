#include <iostream>

#include "../Cat.hpp"
#include "../Dog.hpp"
#include "../utils.hpp"

void foo()
{
    std::cout << "---- Enter foo() ----\n";

    Dog dog;

    std::cout << "Dog is alive inside foo()\n";

    std::cout << "---- Leave foo() ----\n";
}

int main()
{
    std::cout << "Program starts\n\n";

    foo();

    std::cout << "\nBack in main()\n";

	{
		std::cout << "Beginning" << std::endl;
		Dog dog1;
		{
			Dog Dog2;
			std::cout << "Inside block" << std::endl;
		}
		std::cout << "Back outside block" << std::endl;
	}
}