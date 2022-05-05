#ifndef REQUESTBUF_HPP
# define REQUESTBUF_HPP

# include <iostream>
# include <list>

enum	check_values
{
	VALID = 7,
	INVALID = 8
};

class requestBuf {
	private:
		std::string				_headers;
		std::list<std::string>	_body;
		size_t					_bodySize;

	public:
		requestBuf();
		~requestBuf();
		
		void	addHeaderData(const std::string& requestData);
		void	addBodyData(const std::string& requestData);
		void	setBodySize(int size);
		
		int	headersReceived();
		int	validateRequest();
		int	getBodySize();
		
		std::string	getHeaders();
		
		std::list<std::string>	getBody();
};

#endif