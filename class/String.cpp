/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   String.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jegirard <jegirard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/19 18:50:54 by jegirard          #+#    #+#             */
/*   Updated: 2026/01/07 12:33:15 by jegirard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <vector>
#include "../header/String.hpp"
#include <iostream>

String ::String(std::string str) : std::string(str) {};
String ::String() : std::string() {};
String ::String(const String &other) : std::string(other) {};
String ::String(std::vector<String> vec, std::string delimiter) : _delimiter(delimiter)
{
	_vector = vec;
}

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
	return (pos < _vector.size() ? _vector[pos] : String(""));
}

std::string String ::join()
{
	std::string _str;

	for (size_t i = 0; i < this->_vector.size(); ++i)
	{
		_str += this->_vector[i];
		if (i < this->_vector.size() - 1)
		{
			_str += _delimiter;
		}
	}

	return _str;
}

std::vector<String> String ::get_vector()
{

	return this->_vector;
}
std::vector<String> String ::pop_front(size_t n)
{
	if (n > this->_vector.size())
		return this->_vector;
	std::vector<String> popped_elements;
	for (size_t i = 0; i < n; ++i)
	{
		popped_elements.push_back(this->_vector[i]);
	}
	for (size_t i = n; i < this->_vector.size(); ++i)
	{
		this->_vector[i - n] = this->_vector[i];
	}
	this->_vector.resize(this->_vector.size() - n);
	this->join();
	return popped_elements;
}
std::vector<String> String ::pop_back(size_t n)
{
	std::vector<String> popped_elements;
	if (n > _vector.size())
		n = _vector.size();
	for (size_t i = 0; i < n; ++i)
	{
		popped_elements.push_back(_vector[_vector.size() - n + i]);
	}
	_vector.erase(_vector.end() - n, _vector.end());
	return popped_elements;
}

String::~String()
{
}
