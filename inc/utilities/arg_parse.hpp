#ifndef ARG_PARSER_HPP
#define ARG_PARSER_HPP
#include <vector>
#include <string>
#include <map>

enum class ArgOption {
    VALUE,
    FLAG
};

class ArgParser {
    public:
        ArgParser(int argc, char* argv[]);
        void addArgument(std::string arg, ArgOption opt);
        std::map<std::string, int> getArgs();
    private:
        void parseArgs();
    private:
        std::vector<std::string> args;
        std::map<std::string, ArgOption> arg_list;
        std::map<std::string, int> parsed_args;
};


#endif //ARG_PARSER_HPP
