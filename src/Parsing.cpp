#include "Json.h"

#include <fstream>

namespace njson {

enum class State
{
	NONE,
	OBJECT,
	ARRAY,
	STRING,
	NUMBER,
	WORD
};

#define CHECK_COMMA true

static State	getStateFromChar(char c);
static Json*	runState(std::ifstream& file, State state);

static bool	isDelim(char c)
{
	return (c == ',' || c == ':' || c == '}' || c == '{' || c == '[' || c == ']');
}

static void	skipWS(std::ifstream& file)
{
	while (std::isspace(file.peek()))
		file.get();
}

static void	trim(std::string& str)
{

	size_t p1 = str.find_last_of('\"');
	size_t p2 = str.find_first_of('\"');

	if (p1 != std::string::npos)
		str.erase(p1, 1);
	if (p2 != std::string::npos)
		str.erase(p2, 1);
}

static std::string	getNextToken(std::ifstream& file)
{
	skipWS(file);
	
	std::string str = {(char)file.get()};

	if (isDelim(str[0]))
	{
#ifdef DEBUG
		std::cout << "TEST token = |" << str << "| delim" << std::endl;
#endif
		return (str);
	}

	if (str[0] == '"')
	{
		while (!file.eof())
		{
			char c = file.get();
			str += c;
			if (c == '\"')
			{
#ifdef DEBUG
				std::cout << "TEST token = |" << str << "| str" << std::endl;
#endif
				return (str);
			}
		}
	}

	while (!file.eof() && !isDelim(file.peek()) && !std::isspace(file.peek()))
		str += file.get();
#ifdef DEBUG
	std::cout << "TEST token = |" << str << "| other" << std::endl;
#endif

	return (str);
}

static Json*	stateObject(std::ifstream& file)
{
#ifdef DEBUG
	std::cerr << "TEST entered OBJECT" << std::endl;
#endif

	Json*	object = new Json(Json::Object{});

	std::string	token;
	while (!file.eof() && (token = getNextToken(file)) != "}")
	{
		std::string	key;

		if (object->getObject().size() > 0)
		{
			if (token[0] == ',')
			{
				skipWS(file);
				token = getNextToken(file);
			}
			else if (CHECK_COMMA)
			{
				std::cerr << "njson: [ERROR] bad object. (no ,)" << std::endl;
				delete object;
				return (NULL);
			}
		}
		if (isDelim(token[0]))
		{
			std::cerr << "njson: [ERROR] bad object." << std::endl;
			delete object;
			return (NULL);
		}
		key = token;
		if (key.back() == '\"' && key.front() != '\"')
		{
			std::cerr << "njson: [ERROR] key not encapsulated by '\"'." << std::endl;
			delete object;
			return (NULL);
		}
		trim(key);
		token = getNextToken(file);
		if (token[0] != ':')
		{
			std::cerr << "njson: [ERROR] bad object (no :)." << std::endl;
			delete object;
			return (NULL);
		}
		skipWS(file);
		object->getObject()[key] = runState(file, getStateFromChar(file.get()));
		if (!object->getObject()[key])
		{
			delete object;
			return (NULL);
		}
	}
	return (object);
}

static Json*	stateArray(std::ifstream& file)
{
#ifdef DEBUG
	std::cerr << "TEST entered ARRAY" << std::endl;
#endif

	Json*	array = new Json(Json::Array{});

	while (!file.eof())
	{
		skipWS(file);
		char c = file.peek();
		if (c == ']')
		{
			file.get();
#ifdef DEBUG
			std::cout << "TEST token = |]| delim" << std::endl;
#endif
			return (array);
		}
		if (array->getArray().size() > 0)
		{
			if (c == ',')
			{
				file.get();
#ifdef DEBUG
				std::cout << "TEST token = |,| delim" << std::endl;
#endif
				skipWS(file);
				c = file.peek();
			}
			else if (CHECK_COMMA)
			{
				std::cerr << "njson: [ERROR] bad array. (no ,)" << std::endl;
				delete array;
				return (NULL);
			}
		}
		array->getArray().emplace_back(runState(file, getStateFromChar(file.get())));
	}
	delete array;
	return (NULL);
}

static Json*	stateString(std::ifstream& file)
{
#ifdef DEBUG
	std::cerr << "TEST entered STRING" << std::endl;
#endif

	file.unget();
	std::string	str = getNextToken(file);

	if (str.back() != '\"')
	{
		std::cerr << "njson: [ERROR] string not encapsulated by '\"'." << std::endl;
		return (NULL);
	}
	trim(str);
	return (new Json(str));
}

static Json*	stateNumber(std::ifstream& file)
{
#ifdef DEBUG
	std::cerr << "TEST entered NUMBER" << std::endl;
#endif
	std::string	str;

	file.unget();
	str = getNextToken(file);

	if (str.empty())
	{
		std::cerr << "njson: [ERROR] bad number." << std::endl;
		return (NULL);
	}

	if (str.find('.'))
	{
		double	d;
		try
		{
			d = std::stod(str);
		}
		catch (std::invalid_argument& e) { std::cerr << e.what() << std::endl; }
		catch (std::out_of_range& e) { std::cerr << e.what() << std::endl; }
		return (new Json(d));
	}
	else
	{
		int	n;
		try
		{
			n = std::stoi(str);
		}
		catch (std::invalid_argument& e) { std::cerr << e.what() << std::endl; }
		catch (std::out_of_range& e) { std::cerr << e.what() << std::endl; }
		return (new Json(n));
	}
}

static Json*	stateWord(std::ifstream& file)
{
#ifdef DEBUG
	std::cerr << "TEST entered WORD" << std::endl;
#endif

	file.unget();
	skipWS(file);
	std::string word = getNextToken(file);
	if (word == "true")
		return (new Json(true));
	if (word == "false")
		return (new Json(false));
	if (word == "null")
		return (new Json());
	return (NULL);
}

static State	getStateFromChar(char c)
{
	switch (c)
	{
		case '{' :
			return (State::OBJECT);
		case '[' :
			return (State::ARRAY);
		case '\"' :
			return (State::STRING);
		default :
			if (c == '-' || (c >= '0' && c <= '9'))
				return (State::NUMBER);
			break ;
	}
	return (State::WORD);
}

static Json*	runState(std::ifstream& file, State state)
{
	switch (state)
	{
		case State::OBJECT :
			return (stateObject(file));
		case State::ARRAY :
			return (stateArray(file));
		case State::STRING :
			return (stateString(file));
		case State::NUMBER :
			return (stateNumber(file));
		case State::WORD :
			return (stateWord(file));
		default :
			std::cerr << "njson: [ERROR] Unknown State." << std::endl;
			return (NULL);
	}
}

Json*	parse(const char* fname)
{
	std::ifstream	file;

	file.open(fname);
	if (file.bad() || file.fail())
	{
		std::cerr << "njson: [ERROR] Bad file." << std::endl;
		return (NULL);
	}
	skipWS(file);
	return runState(file, getStateFromChar(file.get()));
}

Json*	parse(const std::string& fname)
{
	return parse(fname.c_str());
}

}
