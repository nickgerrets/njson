#include "Json.h"

#include <fstream>
#include <iostream>

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
static Json::pointer_t	runState(std::istream& file, State state);

static bool	isDelim(char c)
{
	return (c == ',' || c == ':' || c == '}' || c == '{' || c == '[' || c == ']');
}

static void	skipWS(std::istream& stream)
{
	while (std::isspace(stream.peek()))
		stream.get();
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

static std::string	getNextToken(std::istream& stream)
{
	stream >> std::ws;
	
	std::string str = {(char)stream.get()};

	if (isDelim(str[0]))
	{
#ifdef DEBUG
		std::cout << "TEST token = |" << str << "| delim" << std::endl;
#endif
		return (str);
	}

	if (str[0] == '"')
	{
		while (!stream.eof())
		{
			char c = stream.get();
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

	while (!stream.eof() && !isDelim(stream.peek()) && !std::isspace(stream.peek()))
		str += stream.get();
#ifdef DEBUG
	std::cout << "TEST token = |" << str << "| other" << std::endl;
#endif

	return (str);
}

static Json::pointer_t stateObject(std::istream& file)
{
#ifdef DEBUG
	std::cerr << "TEST entered OBJECT" << std::endl;
#endif

	Json::pointer_t object = Json::pointer_t(new Json(Json::object_t{}));

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
				return (Json::pointer_t(new Json()));
			}
		}
		if (isDelim(token[0]))
		{
			std::cerr << "njson: [ERROR] bad object." << std::endl;
			return (Json::pointer_t(new Json()));
		}
		key = token;
		if (key.back() == '\"' && key.front() != '\"')
		{
			std::cerr << "njson: [ERROR] key not encapsulated by '\"'." << std::endl;
			return (Json::pointer_t(new Json()));
		}
		trim(key);
		token = getNextToken(file);
		if (token[0] != ':')
		{
			std::cerr << "njson: [ERROR] bad object (no :)." << std::endl;
			return (Json::pointer_t(new Json()));
		}
		skipWS(file);
		object->addToObject(key, runState(file, getStateFromChar(file.get())));
		// object->getObject()[key] = );
		if (!object->getObject()[key])
			return (Json::pointer_t(new Json()));
	}
	return (object);
}

static Json::pointer_t	stateArray(std::istream& file)
{
#ifdef DEBUG
	std::cerr << "TEST entered ARRAY" << std::endl;
#endif

	Json::pointer_t	array = Json::pointer_t(new Json(Json::array_t{}));

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
				return (Json::pointer_t(new Json()));
			}
		}
		array->getArray().emplace_back(runState(file, getStateFromChar(file.get())));
	}
	return (Json::pointer_t(new Json()));
}

static Json::pointer_t	stateString(std::istream& stream)
{
#ifdef DEBUG
	std::cerr << "TEST entered STRING" << std::endl;
#endif

	stream.unget();
	std::string	str = getNextToken(stream);

	if (str.back() != '\"')
	{
		std::cerr << "njson: [ERROR] string not encapsulated by '\"'." << std::endl;
		return (Json::pointer_t(new Json()));
	}
	trim(str);
	return Json::pointer_t(new Json(str));
}

static Json::pointer_t	stateNumber(std::istream& stream)
{
#ifdef DEBUG
	std::cerr << "TEST entered NUMBER" << std::endl;
#endif
	std::string	str;

	stream.unget();
	str = getNextToken(stream);

	if (str.empty())
	{
		std::cerr << "njson: [ERROR] bad number." << std::endl;
		return (Json::pointer_t(new Json()));
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
		return (Json::pointer_t(new Json(d)));
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
		return (Json::pointer_t(new Json(n)));
	}
}

static Json::pointer_t	stateWord(std::istream& stream)
{
#ifdef DEBUG
	std::cerr << "TEST entered WORD" << std::endl;
#endif

	stream.unget();
	skipWS(stream);
	std::string word = getNextToken(stream);
	if (word == "true")
		return (Json::pointer_t(new Json(true)));
	if (word == "false")
		return (Json::pointer_t(new Json(false)));
	if (word == "null")
		return (Json::pointer_t(new Json()));
	return (Json::pointer_t(new Json()));
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

static Json::pointer_t	runState(std::istream& stream, State state)
{
	switch (state)
	{
		case State::OBJECT :
			return (stateObject(stream));
		case State::ARRAY :
			return (stateArray(stream));
		case State::STRING :
			return (stateString(stream));
		case State::NUMBER :
			return (stateNumber(stream));
		case State::WORD :
			return (stateWord(stream));
		default :
			std::cerr << "njson: [ERROR] Unknown State." << std::endl;
			return (Json::pointer_t(new Json()));
	}
}

Json::pointer_t	parse(const char* fname)
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

Json::pointer_t	parse(const std::string& fname)
{
	return parse(fname.c_str());
}

}
