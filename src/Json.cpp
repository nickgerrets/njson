#include "Json.h"

#include <iomanip>

namespace njson {

//	Value union constructors (w/ move semantics)
Json::Value::Value() : i(0) {}
Json::Value::Value(Array&& array) : array(std::move(array)) {}
Json::Value::Value(Object&& object) : object(std::move(object)) {}
Json::Value::Value(const std::string& str) : str(str) {}
Json::Value::Value(double d) : d(d) {}
Json::Value::Value(int i) : i(i) {}
Json::Value::Value(bool b) : boolean(b) {}

Json::Value::~Value() {}

//	Constructors using move semantics
Json::Json(Array&& array)
: m_type(Type::ARRAY), m_value(std::move(array)) {};
Json::Json(Object&& object)
: m_type(Type::OBJECT), m_value(std::move(object)) {};

//	Json constructors
Json::Json() : m_type(Type::NULL_T) {}
Json::Json(const std::string& str) : m_type(Type::STRING), m_value(str) {}
Json::Json(const char* str) : m_type(Type::STRING), m_value(std::string{str}) {}
Json::Json(double d) : m_type(Type::DOUBLE) , m_value(d) {}
Json::Json(int i) : m_type(Type::INT) , m_value(i) {}
Json::Json(bool b) : m_type(Type::BOOL) , m_value(b) {}

//	Destructor
Json::~Json()
{
	//	Based on the type, destructor will act differently
	switch (getType())
	{
		case Type::ARRAY:
			destroyArray();
			break ;
		case Type::OBJECT:
			destroyObject();
			break ;
		case Type::STRING:
			//	Call destructor of string
			getString().~basic_string();
			break ;
		default:
			break ;
	}
}

void	Json::destroyArray()
{
	//	delete individual elements
	for (auto* json : getArray())
		delete json;
	//	Call destructor of vector
	getArray().~vector();
}

void	Json::destroyObject()
{
	//	delete individual elements
	for (auto& pair : getObject())
		delete pair.second;
	//	Call destructor of map
	getObject().~unordered_map();
}

//	to simplify adding to the map/vector
void	Json::addToObject(const Key& key, Json* value)
{
	if (getType() != Type::OBJECT)
		return ;
	getObject().emplace(key, value);
}

void	Json::addToArray(Json* value)
{
	if (getType() != Type::ARRAY)
		return ;
	getArray().emplace_back(value);
}

//	Not sure about this, but it is easy to use.
//	returns an null type Json reference if key can't be found.
Json&	Json::find(const Key& key)
{
	static Json empty{};

	if (getType() != Type::OBJECT)
		return (empty);
	auto it = getObject().find(key);
	if (it == getObject().end())
		return (empty);
	return (*it->second);
}

//	Printing
void	Json::print(std::ostream& out) const
{
	printImpl(0, out);
}

void	Json::printDepth(size_t depth, std::ostream& out) const
{
	while (depth-- > 0)
		out << "  ";
}

void	Json::printImpl(size_t depth, std::ostream& out) const
{
	switch (getType())
	{
		case Type::DOUBLE :
			out << std::setprecision(15) << getDouble();
			break;
		case Type::INT :
			out << getInt();
			break;
		case Type::BOOL :
			if (getBool())
				out << "true";
			else
				out << "false";
			break;
		case Type::STRING :
			out << '"' << getString() << '"';
			break;
		case Type::OBJECT :
			printObject(depth, out);
			break;
		case Type::ARRAY :
			printArray(depth, out);
			break;
		case Type::NULL_T :
			out << "null";
			break;
		default:
			out << "UNKNOWN_TYPE";
	}
}

void	Json::printObject(size_t depth, std::ostream& out) const
{
	out << "\n";
	printDepth(depth, out);
	out << '{';
	if (m_value.object.size() == 0)
	{
		out << '}' << std::endl;
		return ;
	}
	out << std::endl;
	for (const auto& pair : m_value.object)
	{
		printDepth(depth + 1, out);
		out << '"' << pair.first << "\": ";
		pair.second->printImpl(depth + 1, out);
		out << std::endl;
	}
	printDepth(depth, out);
	out << '}' << std::endl;
}

void	Json::printArray(size_t depth, std::ostream& out) const
{
	out << "[ ";
	for (size_t i = 0; i < m_value.array.size(); i++)
	{
		m_value.array[i]->printImpl(depth + 1, out);
		if (i < m_value.array.size() - 1)
		{
			if (m_value.array[i]->getType() == Type::OBJECT)
				printDepth(depth, out);
			out << ", ";
		}
	}
	if (m_value.array[m_value.array.size() - 1]->getType() == Type::OBJECT)
		printDepth(depth, out);
	out << " ]";
}

}	//	namespace njson
