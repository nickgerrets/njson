#include "Json.h"

int	main(void)
{
	using namespace njson;

	Json*	json = parse("akfijajfaiw.json");
	if (json)
		json->print();
	delete json;

	return (0);
}

/* OLD TESTS
//	Lets create and add stuff to Array json node
	//	NEED TO ALLOCATE because the parent node will delete it's children
	Json*	arrayNode = new Json(Json::Array{});
	arrayNode->getArray().emplace_back(new Json());	//	Null
	arrayNode->getArray().emplace_back(new Json("Also a string"));
	arrayNode->getArray().emplace_back(new Json(Json::Object{}));	//	Empty object

	//	Lets create and add stuff to Object Json node
	Json*	objectNode = new Json(Json::Object{});
	objectNode->getObject()["array"] = arrayNode;	//	Add the array from before to the object
	objectNode->getObject()["string"] = new Json("This is a string");
	objectNode->getObject()["number"] = new Json(69.69);
	objectNode->getObject()["nullt"] = new Json();
	objectNode->addToObject("object",
		new Json(Json::Object{
			{"string", new Json("HI!")},
			{"number", new Json(10.0)}
		}));

	//	Simpler way to add to object
	Json	rootNode(Json::Object{});
	rootNode.addToObject("key", objectNode);
	rootNode.addToObject("string", new Json("hello there!"));

	rootNode.print();
	std::cout << std::endl;

	//	Find within rootnode
	rootNode.find("key", "object", "string").print();
	std::cout << std::endl;
*/
