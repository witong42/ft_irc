/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   String.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jegirard <jegirard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/19 18:50:54 by jegirard          #+#    #+#             */
/*   Updated: 2025/12/19 20:48:13 by jegirard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <vector>

class String
{
private:
	std::string _str;
	std::string _delimiter;
	std::vector<std::string> _vector;
	/* data */
public:
	String(std::string str) : _str(str) {};
	
	std::vector<std::string> split(const std::string &delimiter)
	{
		_delimiter = delimiter;	
		size_t start = 0;
		size_t end = _str.find(_delimiter);
		while (end != std::string::npos)
		{
			_vector.push_back(_str.substr(start, end - start));
			start = end + _delimiter.length();
			end = _str.find(_delimiter, start);
		}
		_vector.push_back(_str.substr(start, end));
		return _vector;
	}
	std::string getNpos(size_t pos)
	{
		return (pos<_vector.size() ? _vector[pos] : "");
	}
	
	~String();
};
std::string join (const std::vector<std::string> &elements, const std::string &delimiter)
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
