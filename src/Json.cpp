#include "njson/njson.h"

#include <iomanip>

namespace njson {

std::string Json::indentation_string = "\t";

//	Value union constructors (w/ move semantics)
Json::Value::Value() : i(0) {}
Json::Value::Value(Json::array&& array) : array(std::move(array)) {}
Json::Value::Value(Json::object&& object) : object(std::move(object)) {}
Json::Value::Value(Json::string const& str) : str(str) {}
Json::Value::Value(double d) : d(d) {}
Json::Value::Value(int i) : i(i) {}
Json::Value::Value(bool b) : boolean(b) {}

Json::Value::~Value() {}

//	Constructors using move semantics
Json::Json(array&& array)
: m_type(Type::ARRAY), m_value(std::move(array)) {};
Json::Json(object&& object)
: m_type(Type::OBJECT), m_value(std::move(object)) {};

//	Json constructors
Json::Json() : m_type(Type::NULL_T) {}
Json::Json(string const& str) : m_type(Type::STRING), m_value(str) {}
Json::Json(const char* str) : m_type(Type::STRING), m_value(string{str}) {}
Json::Json(double d) : m_type(Type::DOUBLE) , m_value(d) {}
Json::Json(int i) : m_type(Type::INT) , m_value(i) {}
Json::Json(bool b) : m_type(Type::BOOL) , m_value(b) {}

//	Destructor
Json::~Json()
{
	//	Based on the type, destructor will act differently
	switch (get_type())
	{
		case Type::ARRAY:
			destroy_array();
			break ;
		case Type::OBJECT:
			destroy_object();
			break ;
		case Type::STRING:
			//	Call destructor of string
			get<std::string>().~basic_string();
			break ;
		default:
			break ;
	}
}

void	Json::destroy_array()
{
	//	Call destructor of vector
	get<array>().~vector();
}

void	Json::destroy_object()
{
	//	Call destructor of map
	get<object>().~unordered_map();
}

//	to simplify adding to the map/vector
void	Json::add_to_object(const key& key, Json* json)
{
	if (get_type() != Type::OBJECT)
		return ;
	get<object>().emplace(key, pointer {json});
}

void	Json::add_to_object(const key& key, Json::pointer json)
{
	if (get_type() != Type::OBJECT)
		return ;
	get<object>().emplace(key, std::move(json));
}

void	Json::add_to_array(Json* json)
{
	if (get_type() != Type::ARRAY)
		return ;
	get<array>().emplace_back(pointer {json});
}

void	Json::add_to_array(Json::pointer json)
{
	if (get_type() != Type::ARRAY)
		return ;
	get<array>().emplace_back(std::move(json));
}

//	Not sure about this, but it is easy to use.
//	returns an null type Json reference if key can't be found.
Json::pointer& Json::find(const key& key)
{
	if (get_type() != Type::OBJECT)
		return (null_ref());
	auto it = get<object>().find(key);
	if (it == get<object>().end())
		return (null_ref());
	return (it->second);
}

//	Printing
void	Json::print(std::ostream& out, bool pretty) const
{
	print_impl(0, out, pretty);
	out << std::endl;
}

void	Json::print_depth(size_t depth, std::ostream& out) const
{
	while (depth-- > 0)
		out << indentation_string;
}

void	Json::print_impl(size_t depth, std::ostream& out, bool pretty) const
{
	switch (get_type())
	{
		case Type::DOUBLE :
			out << std::setprecision(15) << get<double>();
			break;
		case Type::INT :
			out << get<int>();
			break;
		case Type::BOOL :
			if (get<bool>())
				out << "true";
			else
				out << "false";
			break;
		case Type::STRING :
			out << '"' << get<std::string>() << '"';
			break;
		case Type::OBJECT :
			print_object(depth, out, pretty);
			break;
		case Type::ARRAY :
			print_array(depth, out, pretty);
			break;
		case Type::NULL_T :
			out << "null";
			break;
		default:
			out << "UNKNOWN_TYPE";
	}
}

void	Json::print_object(size_t depth, std::ostream& out, bool pretty) const
{
	out << '{';
	if (m_value.object.size() == 0)
	{
		out << '}';
		return ;
	}
	if (pretty) out << '\n';
	for (auto it = m_value.object.begin(); it != m_value.object.end();)
	{
		auto const& pair = *it;
		if (pretty)
			print_depth(depth + 1, out);
		out << '"' << pair.first << "\": ";
		pair.second->print_impl(depth + 1, out, pretty);
		++it;
		if (it != m_value.object.end())
			out << ',';
		if (pretty) out << '\n'; else out << ' ';
	}
	if (pretty) print_depth(depth, out);
	out << '}';
}

void	Json::print_array(size_t depth, std::ostream& out, bool pretty) const
{
	out << "[";
	for (size_t i = 0; i < m_value.array.size(); ++i)
	{
		// if (pretty && m_value.array[i]->get_type() == Type::OBJECT)
		// 	print_depth(depth, out, pretty);
		if (pretty)
		{
			out << '\n';
			print_depth(depth + 1, out);
		}
		m_value.array[i]->print_impl(depth + 1, out, pretty);
		if (i < m_value.array.size() - 1)
		{
			out << ", ";
		}
	}
	if (pretty)
	{
		out << '\n';
		print_depth(depth, out);
	}
	out << "]";
}

}	//	namespace njson
