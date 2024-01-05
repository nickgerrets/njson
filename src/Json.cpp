#include "njson/njson.h"

#include <iomanip>

namespace njson {

std::string Json::indentation_string = "\t";

/* -------------------------------------------------------------------------- */
/*                             Value Constructors                             */
/* -------------------------------------------------------------------------- */

Json::Value::Value() : as_int(0) {}
Json::Value::Value(Json::array&& _array) : as_array(std::move(_array)) {}
Json::Value::Value(Json::object&& _object) : as_object(std::move(_object)) {}
Json::Value::Value(Json::string const& _str) : as_string(_str) {}
Json::Value::Value(Json::number_float _float) : as_float(_float) {}
Json::Value::Value(Json::number_int _int) : as_int(_int) {}
Json::Value::Value(bool _bool) : as_bool(_bool) {}

Json::Value::~Value() {}

/* -------------------------------------------------------------------------- */
/*                              Json Constructors                             */
/* -------------------------------------------------------------------------- */

Json::Json(array&& array)
: type(Type::ARRAY), value(std::move(array)) {};

Json::Json(object&& object)
: type(Type::OBJECT), value(std::move(object)) {};

Json::Json() : type(Type::NULL_T) {}
Json::Json(null_t _null __attribute__((unused))) : type(Type::NULL_T) {}
Json::Json(string const& str) : type(Type::STRING), value(str) {}
Json::Json(const char* str) : type(Type::STRING), value(string {str}) {}
// Json::Json(number_float f) : type(Type::NUMBER_FLOAT) , value(f) {}
// Json::Json(number_int i) : type(Type::NUMBER_INT) , value(i) {}
Json::Json(bool b) : type(Type::BOOL) , value(b) {}

//	Destructor
Json::~Json() {
	//	Based on the type, destructor will act differently
	switch (this->type) {
		case Type::ARRAY: value.as_array.~array(); break ;
		case Type::OBJECT: value.as_object.~object(); break ;
		case Type::STRING: value.as_string.~string(); break ;
		default: break ;
	}
}

/* -------------------------------------------------------------------------- */
/*                                 check_type                                 */
/* -------------------------------------------------------------------------- */

template<> void Json::check_type<Json::array>(void) const {
	if (type != Type::ARRAY) {
		build_except(Type::ARRAY);
	}
}

template<> void Json::check_type<Json::object>(void) const {
	if (type != Type::OBJECT) {
		build_except(Type::OBJECT);
	}
}

template<> void Json::check_type<Json::string>(void) const {
	if (type != Type::STRING) {
		build_except(Type::STRING);
	}
}

template<> void Json::check_type<Json::number_float>(void) const {
	if (type != Type::NUMBER_FLOAT) {
		build_except(Type::NUMBER_FLOAT);
	}
}

template<> void Json::check_type<Json::number_int>(void) const {
	if (type != Type::NUMBER_INT)	 {
		build_except(Type::NUMBER_INT);
	}
}

template<> void Json::check_type<bool>(void) const {
	if (type != Type::BOOL)	 {
		build_except(Type::BOOL);
	}
}


/* -------------------------------------------------------------------------- */
/*                                     get                                    */
/* -------------------------------------------------------------------------- */

template<> Json::array& Json::get<Json::array>(void) {
	check_type<array>();
	return value.as_array;
}

template<> Json::object& Json::get<Json::object>(void) {
	check_type<object>();
	return value.as_object;
}

template<> Json::string& Json::get<Json::string>(void) {
	check_type<std::string>();
	return value.as_string;
}

template<> Json::number_float& Json::get<Json::number_float>(void) {
	check_type<number_float>();
	return value.as_float;
}

template<> Json::number_int& Json::get<Json::number_int>(void) {
	check_type<number_int>();
	return value.as_int;
}

template<> bool& Json::get<bool>(void) {
	check_type<bool>();
	return value.as_bool;
}

// Const

template<> Json::array const& Json::get<Json::array>(void) const {
	check_type<array>();
	return value.as_array;
}

template<> Json::object const& Json::get<Json::object>(void) const {
	check_type<object>();
	return value.as_object;
}

template<> Json::string const& Json::get<Json::string>(void) const {
	check_type<string>();
	return value.as_string;
}

template<> Json::number_float const& Json::get<Json::number_float>(void) const {
	check_type<number_float>();
	return value.as_float;
}

template<> Json::number_int const& Json::get<Json::number_int>(void) const {
	check_type<number_int>();
	return value.as_int;
}

template<> bool const& Json::get<bool>(void) const {
	check_type<bool>();
	return value.as_bool;
}

/* -------------------------------------------------------------------------- */
/*                                     is                                     */
/* -------------------------------------------------------------------------- */

template<> bool Json::is<Json::null_t>(void) const {
	return (type == Type::NULL_T);
}

template<> bool Json::is<Json::array>(void) const {
	return (type == Type::ARRAY);
}

template<> bool Json::is<Json::object>(void) const {
	return (type == Type::OBJECT);
}

template<> bool Json::is<Json::string>(void) const {
	return (type == Type::STRING);
}

template<> bool Json::is<Json::number_float>(void) const {
	return (type == Type::NUMBER_FLOAT);
}

template<> bool Json::is<Json::number_int>(void) const {
	return (type == Type::NUMBER_INT);
}

template<> bool Json::is<bool>(void) const {
	return (type == Type::BOOL);
}


/* -------------------------------------------------------------------------- */
/*                                Type Strings                                */
/* -------------------------------------------------------------------------- */

std::string Json::get_type_string(Type type) {
	switch (type) {
		case NULL_T:		return "null";
		case ARRAY:			return "array";
		case OBJECT:		return "object";
		case STRING:		return "string";
		case NUMBER_FLOAT:	return "number (floating)";
		case NUMBER_INT:	return "number (integer)";
		case BOOL:			return "boolean";
		default:			return "UNKNOWN";
	}
}

/* -------------------------------------------------------------------------- */
/*                              Add to map/vector                             */
/* -------------------------------------------------------------------------- */

void Json::add_to_object(const key_t& key, Json* json) {
	if (get_type() != Type::OBJECT)
		return ;
	get<object>().emplace(key, pointer_t {json});
}

void Json::add_to_object(const key_t& key, Json::pointer_t json) {
	if (get_type() != Type::OBJECT) return ;
	get<object>().emplace(key, std::move(json));
}

void Json::add_to_array(Json* json) {
	if (get_type() != Type::ARRAY) return ;
	get<array>().emplace_back(pointer_t {json});
}

void Json::add_to_array(Json::pointer_t json) {
	if (get_type() != Type::ARRAY) return ;
	get<array>().emplace_back(std::move(json));
}

/* -------------------------------------------------------------------------- */
/*                                    Find                                    */
/* -------------------------------------------------------------------------- */

//	Not sure about this, but it is easy to use.
//	returns an null type Json reference if key can't be found.
Json::pointer_t& Json::find(const key_t& key) {
	if (get_type() != Type::OBJECT)
		return (null_ref());
	auto& obj = get<object>();
	auto it = obj.find(key);
	if (it == obj.end())
		return (null_ref());
	return (it->second);
}

/* -------------------------------------------------------------------------- */
/*                                  Printing                                  */
/* -------------------------------------------------------------------------- */

void Json::print(std::ostream& out, bool pretty) const {
	print_impl(0, out, pretty);
	out << std::endl;
}

void Json::print_depth(size_t depth, std::ostream& out) const {
	while (depth-- > 0) {
		out << indentation_string;
	}
}

void Json::print_impl(size_t depth, std::ostream& out, bool pretty) const {
	switch (get_type()) {
		case Type::NUMBER_FLOAT :
			out << std::setprecision(15) << get<number_float>();
			break;
		case Type::NUMBER_INT :
			out << get<number_int>();
			break;
		case Type::BOOL :
			if (get<bool>())
				out << "true";
			else
				out << "false";
			break;
		case Type::STRING :
			out << '"' << get<string>() << '"';
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

void	Json::print_object(size_t depth, std::ostream& out, bool pretty) const {
	out << '{';
	if (value.as_object.size() == 0)
	{
		out << '}';
		return ;
	}
	if (pretty) out << '\n';
	for (auto it = value.as_object.begin(); it != value.as_object.end();)
	{
		auto const& pair = *it;
		if (pretty)
			print_depth(depth + 1, out);
		out << '"' << pair.first << "\": ";
		pair.second->print_impl(depth + 1, out, pretty);
		++it;
		if (it != value.as_object.end())
			out << ',';
		if (pretty) out << '\n'; else out << ' ';
	}
	if (pretty) print_depth(depth, out);
	out << '}';
}

void	Json::print_array(size_t depth, std::ostream& out, bool pretty) const
{
	out << "[";
	for (size_t i = 0; i < value.as_array.size(); ++i)
	{
		// if (pretty && value.array[i]->get_type() == Type::OBJECT)
		// 	print_depth(depth, out, pretty);
		if (pretty)
		{
			out << '\n';
			print_depth(depth + 1, out);
		}
		value.as_array[i]->print_impl(depth + 1, out, pretty);
		if (i < value.as_array.size() - 1)
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
