#include "Json.h"

#include <fstream>
#include <iostream>

using namespace njson;

int	main(int argc, char **argv)
{
	using namespace njson;

	if (argc < 2)
	{
		std::cout << "usage: " << argv[0] << " <in_file>" << std::endl;
		return (EXIT_FAILURE);
	}

	Json::pointer_t json = parse(argv[1]);
	if (json)
		json->print(std::cout);

	return (0);
}
