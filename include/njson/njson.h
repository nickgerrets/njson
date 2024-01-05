#ifndef NJSON_H
# define NJSON_H

# include <cstddef>
# include <fstream>
# include <iostream>
# include <memory>
# include <map>
# include <vector>

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
		// pointer type, in this unique pointers are used. This is the return type of many functions
		using pointer_t = std::unique_ptr<Json>;

		using key_t = std::string;

		// JSON types that can for example be used as template arguments in the public functions
		using array = std::vector<pointer_t>;
		using object = std::map<key_t, pointer_t>;
		using string = std::string;
		using number_float = double;
		using number_int = int64_t;
		using number = number_float;
		using null_t = std::nullptr_t;

// =========================== ENUM  =========================== //
	public:
		// JSON types as enum
		enum Type {			// examples:
			NULL_T,			// "key": null
			ARRAY,			// "key": [1, 2, 3]
			OBJECT,			// "key": { "a": 1, "b": 2, "c": 3}
			STRING,			// "key": "hello there"
			NUMBER_FLOAT,	// "key": 123.456
			NUMBER_INT,		// "key": 123
			BOOL			// "key": false
		};

// =========================== UNION =========================== //
	private:
		// The value union is how the Json class handles it's internal data
		union Value {
			Value();
			Value(array&& _array);
			Value(object&& _object);
			Value(string const& _str);
			Value(number_float n);
			Value(number_int n);
			Value(bool b);

			~Value();

			array as_array;
			object as_object;
			string as_string;
			number_float as_float;
			number_int as_int;
			bool as_bool;
		};

// ======================== CONSTRUCTOR ======================== //
	public:
		// Constructors
		Json(); // basically a null-type json object
		Json(null_t _null);
		explicit Json(array&& array);
		explicit Json(object&& object);
		Json(string const& str);
		Json(const char* str);

		template <
			typename INTEGRAL,
			typename std::enable_if<std::is_integral<INTEGRAL>::value>::type* = nullptr>
		Json(INTEGRAL i) : type(Type::NUMBER_INT) , value(number_int(i)) {}

		template <
			typename FLOATING,
			typename std::enable_if<std::is_floating_point<FLOATING>::value>::type* = nullptr>
		Json(FLOATING f) : type(Type::NUMBER_FLOAT) , value(number_float(f)) {}

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
		template<typename T> T& get(void) {
			throw(json_exception("unsupported type"));
		}

		// CONST variant of get<T>() method
		template<typename T> T const& get(void) const {
			throw(json_exception("unsupported type"));
		}

		// returns true if the template argument type matches the type of the json node
		template<typename T> bool is(void) const {
			return false;
		}

		// returns the enum-type of this Json node
		Type get_type() const { return type; }

		static std::string get_type_string(Type type);

		std::string get_type_string(void) const { return get_type_string(this->get_type()); }

		// these functions can be used to add key-value pairs to a Json object
		void add_to_object(const key_t& key, Json* json);
		void add_to_object(const key_t& key, pointer_t json);

		// these functions can be used to add values to a Json array
		void add_to_array(Json* json);
		void add_to_array(pointer_t json);

		// Object insert
		template <typename T>
		void insert(key_t const& key, T _value) {
			if (get_type() != Type::OBJECT) {
				throw json_exception("Can't insert key-value pair, not an object!");
			}
			this->value.as_object.insert({key, pointer_t(new Json(_value))});
		}

		// Array insert
		template <typename T>
		void insert(T _value) {
			if (get_type() != Type::ARRAY) {
				throw json_exception("Can't insert value, not an array!");
			}
			this->value.as_array.push_back(pointer_t(new Json(_value)));
		}

// ============================ FIND =========================== //
	public:
		// find() method to get the value out of an object based on provided key
		// if the key isn't found, it returns a null-type json node
		pointer_t& find(key_t const& key);

		//	Find method for chain-finding in nested objects
		template<typename... Args>
		pointer_t& find(key_t const& first, Args... keys) {
			return (find(first)->find(keys...));
		}

		//	print the entire json-tree from this node (pretty = true also puts indentation and newlines)
		void print(std::ostream& out = std::cout, bool pretty = true) const;

		//	Allocation of a null-node pointer
		static pointer_t null_ptr(void) { return pointer_t {new Json()}; };

		//	Reference to a static null-node pointer
		static pointer_t& null_ref(void) { static pointer_t null_p = null_ptr(); return null_p; };

// =================== OPERATOR OVERLOADS ====================== //
	public:
		// returns true if the type isn't a null-type (so if there's data to get or not)
		operator bool() const { return type != Type::NULL_T; }

		// just calls print on the stream (only difference is that print() actually flushes)
		friend std::ostream& operator<<(std::ostream& stream, Json const& rhs) {
			rhs.print_impl(0, stream, true);
			return (stream);
		}

		friend std::ostream& operator<<(std::ostream& stream, pointer_t const& rhs) {
			rhs->print_impl(0, stream, true);
			return (stream);
		}

// ===================== PRIVATE HELPERS ======================= //
	private:
		// Check type THROWs when the type of the node doesn't match the template arg type
		template<typename T> void check_type(void) const {}

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
		class json_exception : public std::runtime_error {
			public:
				json_exception(std::string const& str) : runtime_error(str) {}
		};
	
	private:
		void build_except(Type given) const {
			throw(json_exception("incorrect type. Expected: "
				+ get_type_string(type)
				+ ", given: "
				+ get_type_string(given)));
		}

// ======================== MEMBER VARS ======================== //
	private:
		Type	type;
		Value	value;
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
	
	// ========================== METHODS ========================== //
	public:
		void open(std::string const& path);
		void close(void);

		std::istream& get_stream(void) { return stream; }

		bool has_error() const { return error; }
		std::string const& get_error_msg(void) const { return errmsg; }

		Json::pointer_t parse(void);

	// ========================== MEMBERS ========================== //
	private:
		std::ifstream holder;
		std::istream& stream;
		bool error;
		std::string errmsg;
	
	// ========================== PARSING ========================== //
	private:
		enum State {
			NONE,
			OBJECT,
			ARRAY,
			STRING,
			NUMBER,
			WORD
		};

		Json::pointer_t set_error(std::string const& str);
		State get_state_from_c(char c);
		Json::pointer_t run_state(State state);
		Json::pointer_t state_object(void);
		Json::pointer_t state_array(void);
		Json::pointer_t state_string(void);
		Json::pointer_t state_number(void);
		Json::pointer_t state_word(void);
};

} // namespace njson

#endif // NJSON_H
