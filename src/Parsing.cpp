#include "njson/njson.h"

#include <fstream>
#include <iostream>

namespace njson {

static bool	is_delimiter(char c)
{
	return (c == ',' || c == ':' || c == '}' || c == '{' || c == '[' || c == ']');
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

static std::string get_next_token(std::istream& stream)
{
	stream >> std::ws;
	
	std::string str = {(char)stream.get()};

	if (is_delimiter(str[0]))
		return (str);

	if (str[0] == '"')
	{
		while (!stream.eof())
		{
			char c = stream.get();
			str += c;
			if (c == '\"')
				return (str);
		}
	}

	while (!stream.eof() && !is_delimiter(stream.peek()) && !std::isspace(stream.peek()))
		str += stream.get();

	return (str);
}

// set error msg and return null-json
Json::pointer JsonParser::set_error(std::string const& str)
{
	errmsg = str;
	error = true;
	return (Json::null_ptr());
}

Json::pointer JsonParser::state_object(void)
{
	Json::pointer object = Json::pointer(new Json(Json::object{}));

	std::string	token;
	while (!stream.eof() && (token = get_next_token(stream)) != "}")
	{
		std::string	key;
		if (object->get<Json::object>().size() > 0)
		{
			if (token[0] == ',')
			{
				stream >> std::ws;
				token = get_next_token(stream);
			}
			else return set_error("bad object, elements not seperated by ','");
		}
		if (is_delimiter(token[0]))
			return set_error(std::string{"bad object, unexpected token: '"} + token[0] + '\'');
		key = token;
		if (key.back() == '\"' && key.front() != '\"')
			return set_error("bad object, key not encapsulated by quotes");
		trim(key);
		token = get_next_token(stream);
		if (token[0] != ':')
			return set_error("bad object, key not followed by ':'");
		stream >> std::ws;
		object->add_to_object(key, run_state(get_state_from_c(stream.get())));
		if (!object->get<Json::object>()[key])
			return (Json::pointer(new Json()));
	}
	if (token != "}")
		return set_error("object not closed by '}'");
	return (object);
}

Json::pointer JsonParser::state_array(void)
{
	Json::pointer array = Json::pointer(new Json(Json::array{}));

	while (!stream.eof())
	{
		stream >> std::ws;
		char c = stream.peek();
		if (c == ']')
		{
			stream.get();
			return (array);
		}
		if (array->get<Json::array>().size() > 0)
		{
			if (c == ',')
			{
				stream.get();
				stream >> std::ws;
				c = stream.peek();
			}
			else return set_error("bad array, elements not seperated by ','");
		}
		array->get<Json::array>().emplace_back(run_state(get_state_from_c(stream.get())));
	}
	return (set_error("array not closed by ']'"));
}

Json::pointer JsonParser::state_string(void)
{
	stream.unget();
	std::string	str = get_next_token(stream);

	if (str.back() != '\"')
		return set_error("string not encapsulated by '\"'");
	trim(str);
	return Json::pointer(new Json(str));
}

Json::pointer JsonParser::state_number(void)
{
	std::string	str;

	stream.unget();
	str = get_next_token(stream);
	if (str.find('.') != std::string::npos)
	{
		double d;
		try
		{
			d = std::stod(str);
		}
		catch (std::invalid_argument& e) { return set_error(std::string {"stod: "} + e.what()); }
		catch (std::out_of_range& e) { return set_error(std::string {"stod: "} + e.what()); }
		return (Json::pointer(new Json(d)));
	}
	else
	{
		int n;
		try
		{
			n = std::stoi(str);
		}
		catch (std::invalid_argument& e) { return set_error(std::string {"stoi: "} + e.what()); }
		catch (std::out_of_range& e) { return set_error(std::string {"stoi: "} + e.what()); }
		return (Json::pointer(new Json(n)));
	}
}

Json::pointer JsonParser::state_word(void)
{

	stream.unget();
	stream >> std::ws;
	std::string word = get_next_token(stream);
	if (word == "true")
		return (Json::pointer(new Json(true)));
	if (word == "false")
		return (Json::pointer(new Json(false)));
	if (word == "null")
		return (Json::pointer(new Json()));
	return (set_error("unrecognized token: " + word));
}

JsonParser::State JsonParser::get_state_from_c(char c)
{
	switch (c)
	{
		case '{' :
			return (OBJECT);
		case '[' :
			return (ARRAY);
		case '\"' :
			return (STRING);
		default :
			if (c == '-' || (c >= '0' && c <= '9'))
				return (NUMBER);
			break ;
	}
	return (WORD);
}

Json::pointer JsonParser::run_state(State state)
{
	switch (state)
	{
		case OBJECT :
			return (state_object());
		case ARRAY :
			return (state_array());
		case STRING :
			return (state_string());
		case NUMBER :
			return (state_number());
		default :
		case WORD :
			return (state_word());
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
	if (error)
		return Json::null_ptr();
	if (!stream)
		return set_error("bad input stream");
	stream >> std::ws;
	if (stream.eof())
		return Json::null_ptr();
	return run_state(get_state_from_c(stream.get()));
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
