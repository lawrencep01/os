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

std::vector<std::string> validate_command(const std::string &s){

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
        words.clear();
        return words;
        
    }
    words.push_back(input);
    words.push_back(output);
    return words;
}

void process_command(const std::string &s){

    std::vector<std::string> words = validate_command(s);

    if (words.size() == 0){
        std::cerr << "Invalid command" << std::endl;
        return;
    }
    std::string output = words.back();
    words.pop_back();
    std::string input = words.back();
    words.pop_back();

    // getting ready for exec
    const char **args = new const char* [words.size()+2];
    int i = 0;
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

void process_pipe(std::string &s){
    std::string delim = " | ";
    size_t pos = 0;
    std::vector<std::string> commands;
    std::string command;

    if (s.back() == '|'){
        std::cerr << "Invalid command" << std::endl;
        return;
    }

    while ((pos = s.find(delim)) != std::string::npos){
        command = trim(s.substr(0, pos));
        s.erase(0, pos + delim.length());
        commands.push_back(command);
    }
    commands.push_back(trim(s));

    std::vector<std::vector<std::string>> validated;
    std::vector<std::string> words;
    for (auto s: commands){
        words = validate_command(s);
        if (words.size() == 0){
            std::cerr << "Invalid command in pipeline" << std::endl;
            return;
        }
        validated.push_back(words);
    }

    int prev[2];
    int fd[2];
    for (unsigned int i = 0; i < validated.size(); i++){
        words = validated[i];
        std::string output = words.back();
        words.pop_back();
        std::string input = words.back();
        words.pop_back();

        // getting ready for exec
        const char **args = new const char* [words.size()+2];
        int k = 0;
        for (unsigned int j = 0; j < words.size(); j++){
            if (words[j] != "<" && words[j] != ">" && words[j] != input && words[j] != output && words[j] != "|"){ 
                args[k] = words[j].c_str();
                k++;
            }
        }
        args[k] = NULL;

        if (i != validated.size() - 1){
            if (pipe2(fd, 0) < 0){
                perror("Failed");
            }
        }

        //max out the pipe size
        fcntl(fd[0], F_SETPIPE_SZ, 1048576);
        fcntl(fd[1], F_SETPIPE_SZ, 1048576);

        int status;
        pid_t pid = fork();
        if (pid == 0){
            if (i != 0){
                dup2(prev[0], 0);
                close(prev[0]);
                close(prev[1]);
            }
            if (i != validated.size() - 1){
                close(fd[0]);
                dup2(fd[1], 1);
                close(fd[1]);
            }
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
            if (i != 0){
                close(prev[0]);
                close(prev[1]);
            }
            if (i != validated.size() - 1){
                prev[0] = fd[0];
                prev[1] = fd[1];
            }
            wait(&status);
            int es = WEXITSTATUS(status);
            std::cout << args[0] << " exit status: " << es << std::endl;
        } else {
            std::cerr << "Error while forking" << std::endl;
        }
        delete[](args);

    }
    close(prev[0]);
    close(prev[1]);
    return;

}

void parse_and_run_command(const std::string &command) {

    // trim up leading and trailing whitespaces
    std::string new_command = trim(command);

    if (new_command == "exit") {
        exit(0);
    } else if (new_command.find('|') != std::string::npos){
        process_pipe(new_command);
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
