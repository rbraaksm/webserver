/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Location.hpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: renebraaksma <renebraaksma@student.42.f      +#+                     */
/*                                                   +#+                      */
/*   Created: 2021/06/11 10:33:58 by timvancitte   #+#    #+#                 */
/*   Updated: 2021/06/28 18:22:10 by rbraaksm      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
# define LOCATION_HPP

#include "Webserver.hpp"
#include "ParserUtils.hpp"

class Location
{

	public:

		typedef void	(Location::*setter)(const std::string&);
	private:

		bool							_autoIndex;
		bool							_ownAutoIndex;
		bool							_ownBodySize;
		bool							_isFileExtension;
		size_t							_maxBodySize;
		std::string						_match;
		std::string						_root;
		std::string						_errorPage;
		std::vector<std::string>		_methods;
		std::vector<std::string>		_indices;
		std::map<std::string, setter>	_typeFunctionMap;

		std::string							_authBasic;
		std::string							_authBasicUserFile;
		std::string							_cgiPath;
		std::string							_htpasswd_path;
		std::map<std::string, std::string>	_loginfo;

		Location(void);

	public:

		explicit Location(std::string &match);
		Location(Location const &src);
		~Location(void);

		Location&		operator=(Location const &obj);
		void			printLocation() const;

		void	setAutoIndex(const std::string &autoIndex);
		void	setMaxBodySize(const std::string &maxBodySize);
		void	setRoot(const std::string &root);
		void	setMethod(const std::string &method);
		void	setErrorPage(const std::string &errorPage);
		void	setIndices(const std::string &indices);
		void	setCgiPath(const std::string &cgiPath);
		void	setAuthBasic(const std::string &authBasic);
		void	setHTPasswordPath(const std::string &passwordpath);
		void	createParameter(std::string &key, std::string configLine);

		const bool&		hasOwnAutoIndex() const;
		const bool&		getAutoIndex() const;
		const bool&		hasOwnBodySize() const;
		const bool&		getIsFileExtension() const;
		const size_t&	getMaxBodySize() const;
		const std::string&	getMatch() const;
		const std::string&	getRoot() const;
		const std::string&	getErrorPage() const;
		const std::string&	getCGIPath() const;
		const std::string&	getHTPasswordPath() const;
		const std::string&	getAuthBasic() const;
		const std::string&	getAuthUserFile () const;
		const std::vector<std::string>&	getMethods() const;
		const std::vector<std::string>&	getIndices() const;
		const std::map<std::string, std::string>&	getLogInfo() const;

		const bool&			getAuthMatch(const std::string &username, const std::string &password);

		void				findKey(std::string &key, std::string line, int lineCount);
		bool				parameterCheck(int &lineCount) const;
		bool				checkAllowedMethods(const std::string method) const;
};

std::ostream&	operator<<(std::ostream &os, const Location &location);

#endif