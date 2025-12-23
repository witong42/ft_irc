/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   String.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jegirard <jegirard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 17:05:18 by jegirard          #+#    #+#             */
/*   Updated: 2025/12/23 13:47:53 by jegirard         ###   ########.fr       */
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
	std::string join();
	std::vector<String> get_vector();
	std::vector<String> split(const std::string &delimiter);
	String getNpos(size_t pos);
	std::vector<String> pop_front(size_t n=1);
	std::vector<String> pop_back(size_t n=1);
	~String();
};