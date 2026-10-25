#include "JsonLite.hpp"

#include <optional>
#include <functional>
#include <unordered_map>

void check_js(int argc, char* argv[]);
void parase_js(char* argv[]);
int main(int argc, char* argv[])
{   
    if(argc < 2) {
        std::cerr << "用法: jsonlite check [FILE|-]\n";
        return 2;
    }
    
    using Handler = std::function<void()>;
    const std::unordered_map<std::string_view, Handler> handlers = {
        {"check",[argc, argv]{return check_js(argc, argv);} },
    };
   
    auto it = handlers.find(argv[1]);
    
    if(it == handlers.end()) {
        throw std::invalid_argument("Unknown command");
    }
    it->second();

}

void check_js(int argc, char* argv[])
{
    std::optional<std::string_view> path;
    if(argc == 2) {
        path = argv[2];
    }
    if(!path) {
        throw std::invalid_argument("用法: jsonlite check [FILE|-]\n");
    }
    std::ifstream file ;
    std::istream* input ;
    JsonLite jsonlite;
    if(*path != "-") {
        file.open(std::string(*path), std::ios::binary);
        if(!file) {
            std::cerr<<"file can not open\n";
            return;
        }
        jsonlite.checkJsonType(&file);
    } else {
        jsonlite.checkJsonType(argv[2]);
    }
}

