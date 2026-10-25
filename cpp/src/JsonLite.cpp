#include "JsonLite.hpp"
JsonLite::JsonLite(char *argv[] = nullptr)
{
    val = argv ;
    jsonFunctionInit();
}

JsonLite::~JsonLite()
{
}

void JsonLite::parse(char* argv[])
{

}

void JsonLite::jsonFunctionInit(char *argv[])
{
    using Handler = std::function<void()>;
    static const std::unordered_map<std::string, Handler> handlers = {
        {"check",checkJsonType()},
        {"parse", parse()},
    };
    
    auto it = handlers.find(argv[0]);
    if (it == handlers.end()) {
        throw std::invalid_argument("Unknown command");
    }
    
    it->second();

}