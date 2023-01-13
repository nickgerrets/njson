#include <vector>
#include <unordered_map>
#include <memory>
#include <iostream>
#include <fstream>

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
// ======================== INDENTATION ======================== //
	private:
		// the string used as a level of indentation (default: "\t")
		static std::string indentation_string;

	public:
		static void set_indentation_string(std::string const& str) { indentation_string = str; }
		static std::string const& get_indentation_string(void) { return indentation_string; }

// =========================== TYPES =========================== //
	public:
		// key type, it's always going to be a string
		using key =			std::string;

		// pointer type, in this unique pointers are used. This is the return type of many functions
		using pointer =		std::unique_ptr<Json>;

		// JSON types that can for example be used as template arguments in the public functions
		using array =		std::vector<pointer>;
		using object =		std::unordered_map<key, pointer>;
		using string =		std::string;
		using null =		std::nullptr_t;
		// no reason to define double, int, bool

// =========================== ENUM  =========================== //
	public:
		// JSON types as enum
		enum Type
		{				// examples:
			NULL_T,		// "key": null
			ARRAY,		// "key": [1, 2, 3]
			OBJECT,		// "key": { "a": 1, "b": 2, "c": 3}
			STRING,		// "key": "hello there"
			DOUBLE,		// "key": 123.456
			INT,		// "key": 123
			BOOL		// "key": false
		};

// =========================== UNION =========================== //
	private:
		// The value union is how the Json class handles it's internal data
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
		// Constructors
		Json(); // basically a null-type json object
		Json(array&& array);
		Json(object&& object);
		Json(const std::string& str);
		Json(const char* str);
		Json(int i);
		Json(double d);
		Json(bool b);

		Json(Json const& other) = delete; // delete copy constuctor
		Json& operator=(Json const& other) = delete; // delete copy-assignment

		// Destructor
		~Json();

// ========================== GETTERS ========================== //
	public:
		// returns a reference to the value of this json node as the type of the template argument
		// if type is incorrect or unsupported, the functions THROWs
		// if you don't want an exception, you can first check the type with: is<T>() or with get_type()
		template<typename T> T& get(void) 				{ throw_except("unsupported type"); }
		template<> array& get<array>(void)				{ check_type<array>(); return m_value.array; }
		template<> object& get<object>(void)			{ check_type<object>(); return m_value.object; }
		template<> std::string& get<std::string>(void)	{ check_type<std::string>(); return m_value.str; }
		template<> double& get<double>(void)			{ check_type<double>(); return m_value.d; }
		template<> int& get<int>(void)					{ check_type<int>(); return m_value.i; }
		template<> bool& get<bool>(void)				{ check_type<bool>(); return m_value.boolean; }

		// CONST variant of get<T>() method
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

		// returns the enum-type of this Json node
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

		std::string const get_type_string(void) { return get_type_string(this->get_type()); }

		// these functions can be used to add key-value pairs to a Json object
		void add_to_object(const key& key, Json* json);
		void add_to_object(const key& key, Json::pointer json);

		// these functions can be used to add values to a Json array
		void add_to_array(Json* json);
		void add_to_array(Json::pointer json);

// ============================ FIND =========================== //
	public:
		// find() method to get the value out of an object based on provided key
		// if the key isn't found, it returns a null-type json node
		pointer& find(key const& key);

		//	Find method for chain-finding in nested objects
		template<typename... Args>
		pointer& find(key const& first, Args... keys)
		{
			return (find(first)->find(keys...));
		}

		//	print the entire json-tree from this node (pretty = true also puts indentation and newlines)
		void print(std::ostream& out = std::cout, bool pretty = true) const;

// =================== OPERATOR OVERLOADS ====================== //
	public:
		// returns true if the type isn't a null-type (so if there's data to get or not)
		operator bool() const { return m_type != Type::NULL_T; }

		// just calls print on the stream (only difference is that print() actually flushes)
		friend std::ostream& operator<<(std::ostream& stream, Json const& rhs)
		{
			rhs.print_impl(0, stream, true);
			return (stream);
		}

		friend std::ostream& operator<<(std::ostream& stream, Json::pointer const& rhs)
		{
			rhs->print_impl(0, stream, true);
			return (stream);
		}

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
		void	print_impl(size_t depth, std::ostream& out, bool pretty) const;
		void	print_object(size_t depth, std::ostream& out, bool pretty) const;
		void	print_array(size_t depth, std::ostream& out, bool pretty) const;
		void	print_depth(size_t depth, std::ostream& out) const;

// ======================== EXCEPTIONS ========================= //
	public:
		// this exception can get thrown by improper getting of json values (types don't match).
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

/*



*/
class JsonParser
{
	// ======================== CONSTRUCTOR ======================== //
	public:
		JsonParser();
		JsonParser(std::string const& path);
		JsonParser(char const* path);
		JsonParser(std::istream& stream);

		JsonParser(JsonParser const& other) = delete;
		JsonParser& operator=(JsonParser const& other) = delete;

		~JsonParser() {}

		void open(std::string const& path);
		void close(void);

		std::istream& get_stream(void) { return stream; }

		bool has_error() const { return error; }
		std::string const& get_error_msg(void) const { return errmsg; }

		Json::pointer parse(void);

	private:
		std::ifstream holder;
		std::istream& stream;
		bool error;
		std::string errmsg;
};

}	//	namespace njson
