#include "JsonLite.hpp"
JsonLite::JsonLite()
{
}

JsonLite::~JsonLite()
{
}

void JsonLite::checkJsonType(std::string_view content)
{
    if(content.empty()) {
        std::cerr<<"str is null\n";
        return;
    }
    
}