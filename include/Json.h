#include <vector>
#include <unordered_map>
#include <memory>
#include <iostream>

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

	private:
		union Value
		{
			//	Constructors
			Value();
			Value(Array&& array);
			Value(Object&& object);
			Value(const std::string& str);
			Value(double d);
			Value(bool b);

			//	Destructor
			~Value();

			//	Values
			Array		array;
			Object		object;
			std::string	str;
			double		number;
			bool		boolean;
		};

	public:
		enum class Type
		{
			NULL_T,
			ARRAY,
			OBJECT,
			STRING,
			NUMBER,
			BOOL
		};

	public:
		//	Constructors
		Json();
		Json(Array&& array);
		Json(Object&& object);
		Json(const std::string& str);
		Json(const char* str);	//	This one is in there so you can use a string-literal
		Json(double d);
		Json(bool b);

		//	Destructor
		~Json();

		//	Getters
		Array&				getArray() { return m_value.array; }
		Object&				getObject() { return m_value.object; }
		const std::string&	getString() const { return m_value.str; }
		double				getNumber() const { return m_value.number; }
		bool				getBool() const { return m_value.boolean; }
		Type				getType() const { return m_type; }

		void	addToObject(const Key& key, Json* value);
		void	addToArray(Json* value);

		//	Not sure if I should leave this in
		Json&	find(const Key& key);

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