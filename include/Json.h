#include <vector>
#include <unordered_map>
#include <memory>
#include <ostream>

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
	public:

// =========================== TYPES =========================== //
		using key_t = std::string;
		using pointer_t = std::unique_ptr<Json>;
		using array_t = std::vector<pointer_t>;
		using object_t = std::unordered_map<key_t, pointer_t>;

// =========================== ENUM  =========================== //
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
			Value(array_t&& array);
			Value(object_t&& object);
			Value(const std::string& str);
			Value(double d);
			Value(int i);
			Value(bool b);

			//	Destructor
			~Value();

			//	Values
			array_t		array;
			object_t	object;
			std::string	str;
			double		d;
			int			i;
			bool		boolean;
		};

// ======================== CONSTRUCTOR ======================== //
	public:
		//	Constructors
		Json();
		Json(array_t&& array);
		Json(object_t&& object);
		Json(const std::string& str);
		Json(const char* str);
		Json(int i);
		Json(double d);
		Json(bool b);

		//	Destructor
		~Json();

// ========================== GETTERS ========================== //
	public:
		template<typename T> T& get_value(void) 				{ throw_except("unsupported type"); }
		template<> array_t& get_value<array_t>(void)			{ check_type<array_t>(); return m_value.array; }
		template<> object_t& get_value<object_t>(void)			{ check_type<object_t>(); return m_value.object; }
		template<> std::string& get_value<std::string>(void)	{ check_type<std::string>(); return m_value.str; }
		template<> double& get_value<double>(void)				{ check_type<double>(); return m_value.d; }
		template<> int& get_value<int>(void)					{ check_type<int>(); return m_value.i; }
		template<> bool& get_value<bool>(void)					{ check_type<bool>(); return m_value.boolean; }

		//	CONST
		template<typename T> T const& get_value(void) const 				{ throw_except("unsupported type"); }
		template<> array_t const& get_value<array_t>(void) const			{ check_type<array_t>(); return m_value.array; }
		template<> object_t const& get_value<object_t>(void) const			{ check_type<object_t>(); return m_value.object; }
		template<> std::string const& get_value<std::string>(void) const	{ check_type<std::string>(); return m_value.str; }
		template<> double const& get_value<double>(void) const				{ check_type<double>(); return m_value.d; }
		template<> int const& get_value<int>(void) const					{ check_type<int>(); return m_value.i; }
		template<> bool const& get_value<bool>(void) const					{ check_type<bool>(); return m_value.boolean; }

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

		void	addToObject(const key_t& key, Json::pointer_t value);
		void	addToArray(Json::pointer_t value);

// ============================ FIND =========================== //

		//	Find method to get the value out of the object map
		pointer_t&	find(const key_t& key);

		//	Find method for chain-finding in object maps
		// template<typename... Args>
		// Json&	find(const key_t& first, Args... keys)
		// {
		// 	return (find(first).find(keys...));
		// }

		void	print(std::ostream& out) const;

// ========================== HELPERS ========================== //
	private:
		template<typename T> void check_type(void) const	{ }
		template<> void check_type<array_t>(void) const		{ if (m_type != Type::ARRAY)	throw_except("incorrect type"); }
		template<> void check_type<object_t>(void) const	{ if (m_type != Type::OBJECT)	throw_except("incorrect type"); }
		template<> void check_type<std::string>(void) const	{ if (m_type != Type::STRING)	throw_except("incorrect type"); }
		template<> void check_type<double>(void) const		{ if (m_type != Type::DOUBLE)	throw_except("incorrect type"); }
		template<> void check_type<int>(void) const			{ if (m_type != Type::INT)		throw_except("incorrect type"); }
		template<> void check_type<bool>(void) const		{ if (m_type != Type::BOOL)		throw_except("incorrect type"); }
		// template<> void check_type<std::nullptr_t>(void) const { return (m_type == Type::NULL_T); }

		//	Destruction of array and object types
		void	destroyArray();
		void	destroyObject();

		//	Printing
		void	printImpl(size_t depth, std::ostream& out) const;
		void	printDepth(size_t depth, std::ostream& out) const;
		void	printObject(size_t depth, std::ostream& out) const;
		void	printArray(size_t depth, std::ostream& out) const;

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

Json::pointer_t	parse(const char* fname);
Json::pointer_t	parse(const std::string& fname);

}	//	namespace njson
