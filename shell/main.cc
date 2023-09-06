#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <cstring>
#include <fcntl.h>


const std::string WHITESPACE = " \n\r\t\f\v";

std::string left_trim(const std::string &s){
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}

std::string right_trim(const std::string &s){
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string trim(const std::string &s) {
    return right_trim(left_trim(s));
}

void process_command(const std::string &s){

    std::vector<std::string> words;
    // split by delimiters
    std::size_t prev = 0, pos;
    while ((pos = s.find_first_of(WHITESPACE, prev)) != std::string::npos)
    {
        if (pos > prev)
            words.push_back(s.substr(prev, pos-prev));
        prev = pos+1;
    }
    if (prev < s.length())
        words.push_back(s.substr(prev, std::string::npos));

    // takes care of redirection
    // looks for < or > or words. Says its valid if word >= 0 and < || > are valid
    std::string input = "";
    std::string output = "";
    unsigned int num_words = 0;
    unsigned int i = 0;
    bool flag = false;

    while (i < words.size()){
        std::string word = words[i];
        if (word == "<"){
            if (input != "" || i == words.size() - 1){
                flag = true;
                break;
            } else {
                i ++;
                input = words[i];
            }
        } else if (word == ">"){
            if (output != "" || i == words.size() - 1){
                flag = true;
                break;
            } else {
                i ++;
                output = words[i];
            }
        } else { num_words++; }

        i ++;
    }

    //std::cout << flag << num_words << input << output << std::endl;
    if (flag == true || num_words == 0 || input == ">" || input == "<" || output == ">" || output == "<"){
        std::cerr << "Invalid command" << std::endl;
        return;
    }

    // getting ready for exec
    const char **args = new const char* [words.size()+2];
    i = 0;
    for (unsigned int j = 0; j < words.size(); j++){
        if (words[j] != "<" && words[j] != ">" && words[j] != input && words[j] != output){ 
            args[i] = words[j].c_str();
            i++;
        }
    }
    args[i] = NULL;
    pid_t pid;
    int status;

    pid = fork();
    if (pid == 0){
        //setup redirection here (child before exec)
        if (input != ""){
            close(0);
            if (open(input.c_str(), O_RDONLY) < 0) {
                perror("Failed on <");
                exit(255);
            }
        }
        if (output != ""){
            close(1);
            if (open(output.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644) < 0){
                perror("Failed on >");
                exit(255);
            }
        }
        int r = execv(args[0], (char **)args);
        if (r < 0) {
            perror("Failed");
            exit(255);
        }
    } else if (pid > 0) {
        wait(&status);
        int es = WEXITSTATUS(status);
        std::cout << args[0] << " exit status: " << es << std::endl;
    } else {
        std::cerr << "Error while forking" << std::endl;
    }

    delete[](args);
    return;

}

void parse_and_run_command(const std::string &command) {

    // trim up leading and trailing whitespaces
    std::string new_command = trim(command);

    if (new_command == "exit") {
        exit(0);
    } else if (new_command.find('|') != std::string::npos){
        std::cout << "To be implemented in Part 2. Exiting" << "\n";
        exit(0);
    } else {
        process_command(new_command);
    }   
}

int main(void) {
    std::string command;
    std::cout << "> ";
    while (std::getline(std::cin, command)) {
        parse_and_run_command(command);
        std::cout << "> ";
    }
    return 0;
}
