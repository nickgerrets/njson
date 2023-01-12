# njson
A simple representation of JSON and a JSON file parser in C++.

## Example Usage

Find the `string` key in the `json` node. `json` is a unique_ptr to an instance of the Json class with the object type (an unordered_map):

	```
	Json::pointer string_node = json->find("string");
	```

We can now validate the type of this node and get it's value:

	```
	if (string_node->is<std::string>())
	{
		std::string& str = string_node->get<std::string>();

		// if desired we can also alter the actual string
		str += "adding more words";
	}
	```

Without the `is<T>()` method we can still call `get<T>()`, but it will throw and exception if we specify the wrong type as the template argument. This exception can of course be catch-ed and handled appropriately by the user if desired.

We can also use `get<T>()` on objects and arrays to get a reference to the data structure:

	```
	// Json::array is a std::vector
	Json::array server_nodes = json->find("servers")->get<Json::array>();

	// And of course we can iterate and use range-based for-loops
	for (Json::pointer const& n : server_nodes)
		...
	```

Printing/outputing json is quite easy, any std::ostream will work:

	```
	// through stream operators:
	std::cout << json << std::end;

	// or with the print function:
	json->print();
	```

## Json Node

A json node hold a value and the type of it's value.
The types can be:
- **null**		- a `std::nullptr_t` type node that doesn't hold a value
- **object**	- a `std::unordered_map<std::string, std::unique_ptr<Json>>`
- **array**		- a `std::vector` of `std::unique_ptr<Json>`
- **string**	- a `std::string`
- **double**
- **int**
- **bool**
