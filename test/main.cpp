#include "njson/njson.h"

#include <fstream>
#include <iostream>

#define P(x) std::cout << x << std::endl;
#define NL std::cout << '\n';

using namespace njson;

int on_no_arg(void) {

	P("-- parsing empty json:");
	JsonParser parser_empty("json/empty.json");
	Json::pointer_t json_empty = parser_empty.parse();
	json_empty->print();
	NL;

	P("== parsing valid json ==");
	JsonParser parser("json/test.json");
	if (parser.has_error()) {
		std::cout << parser.get_error_msg() << std::endl;
		return 1;
	}
	auto json = parser.parse();
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
	std::cout << json->find("string")->get<Json::string>() << std::endl;
	NL;

	P("-- find() 'string' node and get() int (should throw):");
	try
	{
		std::cout << json->find("string")->get<Json::number_int>() << std::endl;
	}
	catch(const Json::json_exception& e) { std::cerr << "json_exception: " << e.what() << std::endl; }
	NL;

	P("-- check if root node is() an object (it is):");
	std::cout << std::boolalpha << json->is<Json::object>() << std::endl;
	NL;

	P("-- check if root node is() an array (it's not):");
	std::cout << std::boolalpha << json->is<Json::array>() << std::endl;
	NL;

	P("-- insert key 'int' with value of 5, then print:");
	int x = 5;
	json->insert("int", x);
	json->find("int")->print();

	P("-- insert boolean true into array with key 'arr', then print array:");
	json->find("arr")->insert(true);
	json->find("arr")->print();

	return (EXIT_SUCCESS);
}

int	main(int argc, char **argv) {
	Json::set_indentation_string("  ");

	if (argc < 2)
		return on_no_arg();

	JsonParser json_file(argv[1]);
	auto json = json_file.parse();
	if (json_file.has_error()) {
		std::cerr << json_file.get_error_msg() << std::endl;
	}

	if (!json) {
		std::cerr << "null-node" << std::endl;
		return (EXIT_FAILURE);
	}

	std::cout << json << std::endl;
	
	return (EXIT_SUCCESS);
}
