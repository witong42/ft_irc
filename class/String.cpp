/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   String.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jegirard <jegirard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/19 18:50:54 by jegirard          #+#    #+#             */
/*   Updated: 2025/12/23 10:45:35 by jegirard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include <string>
#include <vector>
#include "String.hpp"

String ::String(std::string str) : std::string(str) {};
String ::String() : std::string() {};
String ::String(const String &other) : std::string(other) {};

std::vector<String> String ::split(const std::string &delimiter)
{
	_delimiter = delimiter;
	size_t start = 0;
	size_t end = std::string::find(_delimiter);
	while (end != std::string::npos)
	{
		_vector.push_back(std::string::substr(start, end - start));
		start = end + _delimiter.length();
		end = std::string::find(_delimiter, start);
	}
	_vector.push_back(std::string::substr(start, end));
	return _vector;
}
String String::getNpos(size_t pos)
{
	return (pos < _vector.size() ? _vector[pos] : String("")	);
	
}

std::string String ::join(const std::vector<String> &elements, const std::string &delimiter)
{
	std::string _str;

	for (size_t i = 0; i < elements.size(); ++i)
	{
		_str += elements[i];
		if (i < elements.size() - 1)
		{
			_str += delimiter;
		}
	}
	return _str;
}

String::~String()
{
}

