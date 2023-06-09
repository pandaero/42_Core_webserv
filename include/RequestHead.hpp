/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wmardin <wmardin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/01 22:04:57 by pandalaf          #+#    #+#             */
/*   Updated: 2023/06/09 19:04:30 by wmardin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTHEAD_HPP
# define REQUESTHEAD_HPP

# include "webserv.hpp"

# include <utility>
# include <string>
# include <map>
# include <cstdlib>

// Forward declaration necessary here, even though included in webserv.hpp
typedef std::map<std::string, std::string> 				strMap;

class	RequestHead
{
	public:
		RequestHead();
		RequestHead(std::string);
		std::string	headerValue(std::string);

		std::string	getFilename() const;
		std::string	getMethod() const;
		std::string getPath() const;
		std::string	getProtocol() const;
		int			getContentLength() const;

	private:
		strMap		createHeaderMap(std::string &, std::string, std::string, std::string);

		std::string	_method;
		std::string	_path;
		std::string	_protocol;
		int			_contentLength;
		strMap		_headers;
};
#endif