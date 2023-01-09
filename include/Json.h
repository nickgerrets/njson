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
		using key_t = std::string;
		using pointer_t = std::unique_ptr<Json>;
		using array_t = std::vector<pointer_t>;
		using object_t = std::unordered_map<key_t, pointer_t>;

		enum class Type
		{
			NULL_T,
			ARRAY,
			OBJECT,
			STRING,
			DOUBLE,
			INT,
			BOOL
		};

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
			object_t		object;
			std::string	str;
			double		d;
			int			i;
			bool		boolean;
		};

	public:
		//	Constructors
		Json();
		Json(array_t&& array);
		Json(object_t&& object);
		Json(const std::string& str);
		Json(const char* str);	//	This one is in there so you can use a string-literal
		Json(int i);
		Json(double d);
		Json(bool b);

		//	Destructor
		~Json();

		//	Getters
		array_t&			getArray() { return m_value.array; }
		object_t&			getObject() { return m_value.object; }
		const std::string&	getString() const { return m_value.str; }
		double				getDouble() const { return m_value.d; }
		int					getInt() const { return m_value.i; }
		bool				getBool() const { return m_value.boolean; }
		Type				getType() const { return m_type; }

		void	addToObject(const key_t& key, Json::pointer_t value);
		void	addToArray(Json::pointer_t value);

		//	Find method to get the value out of the object map
		Json&	find(const key_t& key);

		//	Find method for chain-finding in object maps
		template<typename... Args>
		Json&	find(const key_t& first, Args... keys)
		{
			return (find(first).find(keys...));
		}

		void	print(std::ostream& out) const;

	private:
		//	Destruction of array and object types
		void	destroyArray();
		void	destroyObject();

		//	Printing
		void	printImpl(size_t depth, std::ostream& out) const;
		void	printDepth(size_t depth, std::ostream& out) const;
		void	printObject(size_t depth, std::ostream& out) const;
		void	printArray(size_t depth, std::ostream& out) const;

	private:
		Type	m_type;
		Value	m_value;

};

Json::pointer_t	parse(const char* fname);
Json::pointer_t	parse(const std::string& fname);

}	//	namespace njson
