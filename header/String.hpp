/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   String.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jegirard <jegirard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 17:05:18 by jegirard          #+#    #+#             */
/*   Updated: 2025/12/22 18:37:41 by jegirard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <string>
#include <vector>
class String : public std::string
{
private:
	// std::string _str;
	std::string _delimiter;
	std::vector<String> _vector;
	/* data */
public:
	String();
	String(std::string str);
	String(const String &other);
	std::string join(const std::vector<String> &elements, const std::string &delimiter);
	std::vector<String> split(const std::string &delimiter);
	String getNpos(size_t pos);
	~String();
};