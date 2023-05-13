#include "../include/webserv.hpp"

bool isAlnumStr(const std::string& input)
{
	for (std::string::const_iterator it = input.begin(); it != input.end(); it++)
		if (!isalnum(*it))
			return false;
	return true;
}

std::string trim(std::string& input)
{
	size_t	start = input.find_first_not_of(WHITESPACE);
	size_t	end = input.find_last_not_of(WHITESPACE);
	if (start == std::string::npos)
		input = "";
	else
		input = input.substr(start, end - start + 1);
	return input;
}

std::string splitEraseStr(std::string& input, std::string targetString)
{
	std::string	element;
	size_t 		len;

	len = input.find(targetString);
	if (len == std::string::npos)
	{
		element = input;
		input.erase();
	}
	else
	{
		element = input.substr(0, len);
		input.erase(0, len);
	}
	return element;
}

std::string splitEraseChars(std::string& input, std::string targetChars)
{
	std::string element;
	size_t 		len;

	len = input.find_first_of(targetChars);
	if (len == std::string::npos)
	{
		element = input;
		input.erase();
	}
	else
	{
		element = input.substr(0, len);
		input.erase(0, len);
		trim(input);
	}
	trim(element);
	return element;
}

strMap createHeaderMap(std::string& input, std::string endOfKey, std::string endOfValue, std::string endOfMap)
{
	strMap 		stringMap;
	std::string key, value;

	while (!input.empty())
	{
		if (input.find(endOfMap) == 0)
		{
			input = input.substr(endOfMap.size());
			return stringMap;
		}
		key = splitEraseStr(input, endOfKey);
		value = splitEraseStr(input, endOfValue);
		stringMap.insert(std::make_pair(strToLower(key), value));
	}
	return stringMap;
}

strVec splitEraseStrVec(std::string& input, std::string targetChars, std::string endOfParsing)
{
	strVec 		stringVector;
	std::string	parseRegion, element;
	
	parseRegion = splitEraseChars(input, endOfParsing);
	input.erase(0, endOfParsing.size());
	while (!parseRegion.empty())
	{
		element = splitEraseChars(parseRegion, targetChars);
		stringVector.push_back(element);
	}
	return stringVector;
}

contentType	extensionType(const std::string & filePath)
{
	size_t	pos;

	if ((pos = filePath.find_last_of('.')) == std::string::npos)
		return (OCTETSTREAM);
	if (filePath.find(".txt", pos) != std::string::npos)
		return (PLAINTEXT);
	else if (filePath.find(".css", pos) != std::string::npos)
		return (CSS);
	else if (filePath.find(".html", pos) != std::string::npos)
		return (HTML);
	else if (filePath.find(".zip", pos) != std::string::npos)
		return (ZIP);
	else if (filePath.find(".png", pos) != std::string::npos)
		return (PNG);
	else if (filePath.find(".jpeg", pos) != std::string::npos || filePath.find(".jpg", pos) != std::string::npos)
		return (JPEG);
	else if (filePath.find(".pdf", pos) != std::string::npos)
		return (PDF);
	else if (filePath.find(".xml", pos) != std::string::npos)
		return (XML);
	else if (filePath.find(".json", pos) != std::string::npos)
		return (JSON);
	else if (filePath.find(".avif", pos) != std::string::npos)
		return (AVIF);
	return (OCTETSTREAM);
}

std::vector<std::string>	splitString(std::string str, const std::string & del)
{
	std::vector<std::string>	out;
	out.reserve(100);
	if (str.find(del) == std::string::npos)
	{
		out.push_back(str);
		return (out);
	}
	size_t end = str.find(del);
	while (end != std::string::npos)
	{
		out.push_back(str.substr(0, end));
		str.erase(str.begin(), str.begin() + end + 1);
		end = str.find(del);
	}
	size_t	endPos = str.find(*(str.end() - 1));
	out.push_back(str.substr(0, endPos));
	return (out);
}

bool isSameNoCase(std::string str_a, std::string str_b)
{
	return strToLower(str_a) == strToLower(str_b);
}

std::string strToLower(std::string str)
{
	for (std::string::iterator it = str.begin(); it != str.end(); it++)
		*it = tolower(*it);
	return str;
}

std::string getInstruction(std::string& inputStr)
{
	std::string	instruction;
	bool		braced = false;
	int			i = 0, braces = 0;

	while (inputStr[i] && braces > -1 && braces < 3)
	{
		if (inputStr[i] == ';' && !braces)
			break;
		else if (inputStr[i] == '{')
		{
			braces++;
			braced = true;
		}
		else if (inputStr[i] == '}')
			braces--;
		i++;
	}
	if (braces != 0)
		throw std::runtime_error(E_INVALIDBRACE + inputStr + '\n');
	instruction = inputStr.substr(0, i);
	inputStr.erase(0, i + 1);
	if (braced)
	{
		instruction.replace(instruction.find("{"), 1, " ");
		instruction.replace(instruction.find_last_of("}"), 1, " ");
	}
	trim(instruction);
	return instruction;
}