std::vector<std::string> tokens;
std::string token;

tokens.clear();
std::cout << ">>> ";
std::getline(std::cin, cmd, '\n');

std::stringstream ss(cmd);
while(std::getline(ss, token, ' ')) {
    tokens.push_back(token);
}
if(tokens[0] == "pos") {
    std::cout << e_ptr->getPosition(0) << std::endl;
} else if(tokens[0] == "cmd") {
    char msg[4] = {0, 0, 0, 0};
    try{
        for(int i = 0; i < 4; ++i) {
            msg[i] = std::stoi(tokens[i+1]);
        }
    } catch(const std::exception& e) {
        std::cout << e.what() << std::endl;
    }
     
    e_ptr->elevControl(msg);
} else if(tokens[0] == "q" || tokens[0] == "quit") {
    break;
}
