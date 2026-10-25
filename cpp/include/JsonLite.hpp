#include<iostream>
#include<unordered_map>
#include <functional>
#include <string_view>
class JsonLite
{
private:
    void parse(char* argv[]) ;
    void checkJsonType(char* argv[]);
    void jsonFunctionInit(char* argv[]);
    char** val = nullptr;
public:

    JsonLite(char *argv[] = nullptr);
    ~JsonLite();
};


