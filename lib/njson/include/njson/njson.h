#include <vector>
#include <unordered_map>
#include <memory>
#include <iostream>

namespace njson {

/*
	In essence the Json class just holds it's type and value,
	with value being an union. The complexity comes from the
	array and object not being trivial data types, requiring
	you to construct and instance of the Json class with the
	type predetermined.
*/

class Json
{
// =========================== TYPES =========================== //
	public:
		using key =			std::string;
		using pointer =		std::unique_ptr<Json>;

		// json types
		using array =		std::vector<pointer>;
		using object =		std::unordered_map<key, pointer>;
		using string =		std::string;
		using null =		std::nullptr_t;
		// no reason to define double, int, bool

// =========================== ENUM  =========================== //
	public:
		enum Type
		{
			NULL_T,
			ARRAY,
			OBJECT,
			STRING,
			DOUBLE,
			INT,
			BOOL
		};

// =========================== UNION =========================== //
	private:
		union Value
		{
			//	Constructors
			Value();
			Value(array&& array);
			Value(object&& object);
			Value(const std::string& str);
			Value(double d);
			Value(int i);
			Value(bool b);

			//	Destructor
			~Value();

			//	Values
			array		array;
			object	object;
			std::string	str;
			double		d;
			int			i;
			bool		boolean;
		};

// ======================== CONSTRUCTOR ======================== //
	public:
		//	Constructors
		Json();
		Json(array&& array);
		Json(object&& object);
		Json(const std::string& str);
		Json(const char* str);
		Json(int i);
		Json(double d);
		Json(bool b);

		//	Destructor
		~Json();

// ========================== GETTERS ========================== //
	public:
		// returns the value of this json node as the type of the template argument
		// if type is incorrect or unsupported, the functions THROWs
		template<typename T> T& get(void) 				{ throw_except("unsupported type"); }
		template<> array& get<array>(void)				{ check_type<array>(); return m_value.array; }
		template<> object& get<object>(void)			{ check_type<object>(); return m_value.object; }
		template<> std::string& get<std::string>(void)	{ check_type<std::string>(); return m_value.str; }
		template<> double& get<double>(void)			{ check_type<double>(); return m_value.d; }
		template<> int& get<int>(void)					{ check_type<int>(); return m_value.i; }
		template<> bool& get<bool>(void)				{ check_type<bool>(); return m_value.boolean; }

		//	CONST
		template<typename T> T const& get(void) const 				{ throw_except("unsupported type"); }
		template<> array const& get<array>(void) const				{ check_type<array>(); return m_value.array; }
		template<> object const& get<object>(void) const			{ check_type<object>(); return m_value.object; }
		template<> std::string const& get<std::string>(void) const	{ check_type<std::string>(); return m_value.str; }
		template<> double const& get<double>(void) const			{ check_type<double>(); return m_value.d; }
		template<> int const& get<int>(void) const					{ check_type<int>(); return m_value.i; }
		template<> bool const& get<bool>(void) const				{ check_type<bool>(); return m_value.boolean; }

		// returns true if the template argument type matches the type of the json node
		template<typename T> bool is(void) const	{ return false; }
		template<> bool is<array>(void) const		{ return (m_type == Type::ARRAY); }
		template<> bool is<object>(void) const		{ return (m_type == Type::OBJECT); }
		template<> bool is<std::string>(void) const	{ return (m_type == Type::STRING); }
		template<> bool is<double>(void) const		{ return (m_type == Type::DOUBLE); }
		template<> bool is<int>(void) const			{ return (m_type == Type::INT); }
		template<> bool is<bool>(void) const		{ return (m_type == Type::BOOL); }

		Type get_type() const { return m_type; }

		static std::string const get_type_string(Type type)
		{
			switch (type)
			{
				case NULL_T:	return "null";
				case ARRAY:		return "array";
				case OBJECT:	return "object";
				case STRING:	return "string";
				case DOUBLE:	return "double";
				case INT:		return "integer";
				case BOOL:		return "boolean";
				default:		return "UNKNOWN";
			}
		}

		void add_to_object(const key& key, Json* json);
		void add_to_object(const key& key, Json::pointer json);
		void add_to_array(Json* json);
		void add_to_array(Json::pointer json);

// ============================ FIND =========================== //
	public:
		//	Find method to get the value out of an object
		pointer& find(key const& key);

		//	Find method for chain-finding in nester objects
		template<typename... Args>
		pointer& find(key const& first, Args... keys)
		{
			return (find(first)->find(keys...));
		}

		//	print the entire json-tree from this node
		void print(std::ostream& out = std::cout) const;

// =================== OPERATOR OVERLOADS ====================== //
	public:
		operator bool() const { return m_type != Type::NULL_T; }

// ===================== PRIVATE HELPERS ======================= //
	private:
		// Check type THROWs when the type of the node doesn't match the template arg type
		template<typename T> void check_type(void) const	{ }
		template<> void check_type<array>(void) const		{ if (m_type != Type::ARRAY)	throw_except("incorrect type"); }
		template<> void check_type<object>(void) const		{ if (m_type != Type::OBJECT)	throw_except("incorrect type"); }
		template<> void check_type<std::string>(void) const	{ if (m_type != Type::STRING)	throw_except("incorrect type"); }
		template<> void check_type<double>(void) const		{ if (m_type != Type::DOUBLE)	throw_except("incorrect type"); }
		template<> void check_type<int>(void) const			{ if (m_type != Type::INT)		throw_except("incorrect type"); }
		template<> void check_type<bool>(void) const		{ if (m_type != Type::BOOL)		throw_except("incorrect type"); }
		// template<> void check_type<std::nullptr_t>(void) const { return (m_type == Type::NULL_T); }

		//	Destruction of array and object types
		void	destroy_array();
		void	destroy_object();

		//	Printing
		void	print_impl(size_t depth, std::ostream& out) const;
		void	print_depth(size_t depth, std::ostream& out) const;
		void	print_object(size_t depth, std::ostream& out) const;
		void	print_array(size_t depth, std::ostream& out) const;

// ======================== EXCEPTIONS ========================= //
	public:
		class json_exception : public std::runtime_error
		{
			public:
				json_exception(std::string const& str) : runtime_error(str) {}
		};
	
	private:
		void throw_except(std::string const& str) const { throw(json_exception(str + ": " + get_type_string(m_type))); }

// ======================== MEMBER VARS ======================== //
	private:
		Type	m_type;
		Value	m_value;

};

Json::pointer parse(const char* fname);
Json::pointer parse(const std::string& fname);

}	//	namespace njson
