#include<iostream>
#include<unordered_map>
#include <functional>
#include <string_view>
#include <fstream>

class JsonLite
{
private:

public:

    JsonLite();
    void parse(char* argv[]) ;
    void checkJsonType(std::string_view content);
    void checkJsonType(std::istream *file);
    ~JsonLite();
};


