#include "Json.h"

#include <fstream>
#include <iostream>

using namespace njson;

int	main(void)
{
	using namespace njson;

	std::ofstream	file;
	file.open("out.txt");

	Json*	json = parse("tester/webserv.json");
	if (json)
		json->print(std::cout);

	delete json;

	return (0);
}
