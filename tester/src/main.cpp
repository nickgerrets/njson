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
	// json->print(std::cout);

	try
	{
		std::cout << json->find("string")->get_value<std::string>() << std::endl;

		//	trying to get as int (throws exception)
		std::cout << json->find("string")->get_value<int>() << std::endl;
	}
	catch(const Json::json_exception& e)
	{
		std::cerr << e.what() << '\n';

		//	examples:
		std::cerr << "json incorrectly formatted!" << std::endl;
	}

	return (0);
}
