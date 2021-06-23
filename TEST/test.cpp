#include <iostream>
#include <fstream>

int main(int argc, char **argv)
{
    std::string line;
    std::fstream file;

   file.open(argv[1]);
    while (std::getline(file, line))
    {
        std::cout << line << std::endl;
    }
    return (0);
    (void)argc;
}