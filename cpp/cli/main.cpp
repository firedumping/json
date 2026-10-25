#include "JsonLite.hpp"

int main(int argc, char* argv[])
{
    
    
    std::cout << argc << std::endl;
    std::cout << argv[1]<<std::endl;
    std::cout << "hello "<< std::endl;
    JsonLite jsonlite;
    if(argc < 2) {
        std::cerr <<"The command-line" 
           " argument is less than 2\n";
    }

}

