#include "njson/njson.h"

#include <fstream>
#include <iostream>

#define P(x) std::cout << x << std::endl;
#define NL std::cout << '\n';

using namespace njson;

int on_no_arg(void)
{
	P("-- parsing...");
	Json::pointer json = parse("json/test.json");
	NL;
	// json->print(std::cout);

	P("-- find() '_' node and print:");
	json->find("_")->print();
	NL;

	P("-- find() 'abc' node (non-existant) and print:");
	json->find("abc")->print();
	NL;

	P("-- find() nested 'object'/'object+'/'string' node and print:");
	json->find("object", "object+", "string")->print();
	NL;

	P("-- find() nested 'object'/'object+'/'abc' node (non-existant) and print:");
	json->find("object", "object+", "abc")->print();
	NL;

	P("-- find() 'string' node and get() string:");
	std::cout << json->find("string")->get<std::string>() << std::endl;
	NL;

	P("-- find() 'string' node and get() int (should throw):");
	try
	{
		std::cout << json->find("string")->get<int>() << std::endl;
	}
	catch(const Json::json_exception& e) { std::cerr << "json_exception: " << e.what() << std::endl; }
	NL;

	P("-- check if root node is() an object (it is):");
	std::cout << std::boolalpha << json->is<Json::object>() << std::endl;
	NL;

	P("-- check if root node is() an array (it's not):");
	std::cout << std::boolalpha << json->is<Json::array>() << std::endl;
	NL;

	return (EXIT_SUCCESS);
}

int	main(int argc, char **argv)
{
	if (argc < 2)
		return on_no_arg();

	Json::pointer json = parse(argv[1]);
	if (!(*json))
		return (EXIT_FAILURE);

	std::cout << json << std::endl;
	
	return (EXIT_SUCCESS);
}
