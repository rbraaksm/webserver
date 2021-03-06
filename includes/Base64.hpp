#ifndef BASE64_HPP
# define BASE64_HPP

# include <iostream>

std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len);
std::string base64_decode(std::string const& encoded_string);

#endif