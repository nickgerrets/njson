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
	public:
		using Key = std::string;
		using Array = std::vector<Json*>;
		using Object = std::unordered_map<Key, Json*>;

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
			Value(Array&& array);
			Value(Object&& object);
			Value(const std::string& str);
			Value(double d);
			Value(int i);
			Value(bool b);

			//	Destructor
			~Value();

			//	Values
			Array		array;
			Object		object;
			std::string	str;
			double		d;
			int			i;
			bool		boolean;
		};

	public:
		//	Constructors
		Json();
		Json(Array&& array);
		Json(Object&& object);
		Json(const std::string& str);
		Json(const char* str);	//	This one is in there so you can use a string-literal
		Json(int i);
		Json(double d);
		Json(bool b);

		//	Destructor
		~Json();

		//	Getters
		Array&				getArray() { return m_value.array; }
		Object&				getObject() { return m_value.object; }
		const std::string&	getString() const { return m_value.str; }
		double				getDouble() const { return m_value.d; }
		int					getInt() const { return m_value.i; }
		bool				getBool() const { return m_value.boolean; }
		Type				getType() const { return m_type; }

		void	addToObject(const Key& key, Json* value);
		void	addToArray(Json* value);

		//	Find method to get the value out of the Object map
		Json&	find(const Key& key);

		//	Find method for chain-finding in Object maps
		template<typename... Args>
		Json&	find(const Key& first, Args... keys)
		{
			return (find(first).find(keys...));
		}

		void	print(size_t depth = 0) const;

	private:
		//	Destruction of array and object types
		void	destroyArray();
		void	destroyObject();

		//	Printing
		void	printDepth(size_t depth) const;
		void	printObject(size_t depth) const;
		void	printArray(size_t depth) const;


	private:
		Type	m_type;
		Value	m_value;

};

Json*	parse(const char* fname);
Json*	parse(const std::string& fname);

}	//	namespace njson
