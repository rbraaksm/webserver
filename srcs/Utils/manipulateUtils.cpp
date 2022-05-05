#include "../../includes/manipulateUtils.hpp"

const std::string WHITESPACE =" \n\r\t\f\v";

namespace manipulateUtils
{
	std::vector<std::string>     strToVector(std::string request)
	{
		std::vector<std::string> request_elements;
		std::string str;
		size_t      pos;

		while ((int)(pos = request.find_first_of('\r')) != -1)
		{
			str = request.substr(0, pos);
			if (!str.empty())
				request_elements.push_back(str);
			request = request.substr(pos + 2);
		}
		request_elements.push_back(request);
		return request_elements;
	}
	
	size_t		ft_strlen(const char *s)
	{
		size_t len;

		len = 0;
		while (s[len] != '\0')
		{
			len++;
		}
		return len;
	}

	char	*ft_strjoin(char const *s1, char const *s2)
	{
		size_t	i1;
		size_t	i2;
		char	*dst;

		if (s1 == 0 || s2 == 0)
			return 0;
		i1 = 0;
		i2 = 0;
		dst = (char *)malloc(sizeof(char) * (ft_strlen(s1) + ft_strlen(s2) + 1));
		if (!dst)
			throw (std::string("Malloc failed"));
		while (s1[i1] != '\0')
		{
			dst[i1] = s1[i1];
			i1++;
		}
		while (s2[i2] != '\0')
		{
			dst[i1 + i2] = s2[i2];
			i2++;
		}
		dst[i1 + i2] = '\0';
		return dst;
	}

    void	removeComments(std::string &line)
	{
		line.erase(std::find(line.begin(), line.end(), '#'), line.end());
	}

	void	removeSpacesBeforeAfter(std::string *line)
	{
		size_t n = line->find_last_not_of(WHITESPACE);
		size_t x = line->find_first_not_of(WHITESPACE);

		if (n != std::string::npos)
			line->assign(line->substr(0, n + 1));
		if (x != std::string::npos)
			line->assign(line->substr(x));
	}

	void	deleteSpacesBeforeAndAfter(std::map<int, std::string> &lines)
	{
		std::map<int, std::string>::iterator it = lines.begin();
		for (; it != lines.end(); ++it)
			removeSpacesBeforeAfter(&it->second);
	}

	void	createPath(std::string &path)
	{
		size_t x;
		size_t y;
		x = path.find_first_of('/');
		while (x != std::string::npos)
		{
			y = path.find_first_of('/', x + 1);
			if (x + 1 == y)
				path.erase(y, 1);
			else
				x = path.find_first_of('/', y);
		}
	}
}