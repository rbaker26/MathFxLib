#include <iostream>
#include <iomanip>
#include "mathFx.h"

int main()
{
	using namespace mathFx;
	degree deg(45);
	std::cout << std::setprecision(20) << std::fixed;
	std::cout << sin<degree>(deg).get_value() << std::endl;
	std::cout << cos<degree>(deg).get_value() << std::endl;


	
	std::cin.get();

	return 0;
}
