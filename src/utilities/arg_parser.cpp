#include <utilities/arg_parse.hpp>

#include <algorithm>

ArgParser::ArgParser(int argc, char* argv[]) {
    for(int i = 0; i < argc; ++i) {
        args.push_back(std::string(args[i]));
    }
}

std::map<std::string, int> ArgParser::getArgs() {
    parseArgs();
    return parsed_args;
}

void ArgParser::addArgument(std::string arg, ArgOption opt) {
    arg_list.insert(std::pair<std::string, ArgOption>(arg, opt));
}

void ArgParser::parseArgs() {
    std::for_each(arg_list.cbegin(), arg_list.cend(), 
        [=](const std::pair<std::string, ArgOption> &arg) {
            auto s = std::find(args.cbegin(), args.cend(), arg.first);
            if(arg.second == ArgOption::VALUE) {
                parsed_args.insert(std::pair<std::string, int>(*s, std::stoi(*(s++))));
            } else if(arg.second == ArgOption::FLAG) {
                parsed_args.insert(std::pair<std::string, int>(*s, 1));
            }
        });
}