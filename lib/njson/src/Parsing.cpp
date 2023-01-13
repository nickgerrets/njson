#include "njson/njson.h"

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

static State get_state_from_c(char c);
static Json::pointer run_state(std::istream& file, State state);

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

static Json::pointer stateObject(std::istream& file)
{
#ifdef DEBUG
	std::cerr << "TEST entered OBJECT" << std::endl;
#endif

	Json::pointer object = Json::pointer(new Json(Json::object{}));

	std::string	token;
	while (!file.eof() && (token = getNextToken(file)) != "}")
	{
		std::string	key;

		if (object->get<Json::object>().size() > 0)
		{
			if (token[0] == ',')
			{
				skipWS(file);
				token = getNextToken(file);
			}
			else if (CHECK_COMMA)
			{
				std::cerr << "njson: [ERROR] bad object. (no ,)" << std::endl;
				return (Json::pointer(new Json()));
			}
		}
		if (isDelim(token[0]))
		{
			std::cerr << "njson: [ERROR] bad object." << std::endl;
			return (Json::pointer(new Json()));
		}
		key = token;
		if (key.back() == '\"' && key.front() != '\"')
		{
			std::cerr << "njson: [ERROR] key not encapsulated by '\"'." << std::endl;
			return (Json::pointer(new Json()));
		}
		trim(key);
		token = getNextToken(file);
		if (token[0] != ':')
		{
			std::cerr << "njson: [ERROR] bad object (no :)." << std::endl;
			return (Json::pointer(new Json()));
		}
		skipWS(file);
		object->add_to_object(key, run_state(file, get_state_from_c(file.get())));
		// object->getObject()[key] = );
		if (!object->get<Json::object>()[key])
			return (Json::pointer(new Json()));
	}
	return (object);
}

static Json::pointer	stateArray(std::istream& file)
{
#ifdef DEBUG
	std::cerr << "TEST entered ARRAY" << std::endl;
#endif

	Json::pointer	array = Json::pointer(new Json(Json::array{}));

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
		if (array->get<Json::array>().size() > 0)
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
				return (Json::pointer(new Json()));
			}
		}
		array->get<Json::array>().emplace_back(run_state(file, get_state_from_c(file.get())));
	}
	return (Json::pointer(new Json()));
}

static Json::pointer	stateString(std::istream& stream)
{
#ifdef DEBUG
	std::cerr << "TEST entered STRING" << std::endl;
#endif

	stream.unget();
	std::string	str = getNextToken(stream);

	if (str.back() != '\"')
	{
		std::cerr << "njson: [ERROR] string not encapsulated by '\"'." << std::endl;
		return (Json::pointer(new Json()));
	}
	trim(str);
	return Json::pointer(new Json(str));
}

static Json::pointer	stateNumber(std::istream& stream)
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
		return (Json::pointer(new Json()));
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
		return (Json::pointer(new Json(d)));
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
		return (Json::pointer(new Json(n)));
	}
}

static Json::pointer	stateWord(std::istream& stream)
{
#ifdef DEBUG
	std::cerr << "TEST entered WORD" << std::endl;
#endif

	stream.unget();
	skipWS(stream);
	std::string word = getNextToken(stream);
	if (word == "true")
		return (Json::pointer(new Json(true)));
	if (word == "false")
		return (Json::pointer(new Json(false)));
	if (word == "null")
		return (Json::pointer(new Json()));
	return (Json::pointer(new Json()));
}

static State	get_state_from_c(char c)
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

static Json::pointer	run_state(std::istream& stream, State state)
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
			return (Json::pointer(new Json()));
	}
}

// CONSTRUCTORS
JsonParser::JsonParser() : holder(std::ifstream()), stream(holder), error(false), errmsg("") {}

JsonParser::JsonParser(std::string const& path) : holder(std::ifstream(path)), stream(holder), error(false), errmsg("")
{
	if (!stream)
	{
		error = true;
		errmsg = "bad input stream";
	}
}

JsonParser::JsonParser(char const* path) : JsonParser(std::string {path}) {}

JsonParser::JsonParser(std::istream& stream) : stream(stream), error(false), errmsg("")
{
	if (!stream)
	{
		error = true;
		errmsg = "bad input stream";
	}
}

Json::pointer	JsonParser::parse(void)
{
	if (!stream)
	{
		error = true;
		errmsg = "bad input stream";
		return Json::pointer(new Json());
	}
	stream >> std::ws;
	return run_state(stream, get_state_from_c(stream.get()));
}

void JsonParser::open(std::string const& path)
{
	this->holder.open(path);
}

void JsonParser::close(void)
{
	this->holder.close();
}

} // namespace njson
