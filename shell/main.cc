#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

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

void exec_command(char **args, std::string input, std::string output){

    pid_t pid;
    int status;

    pid = fork();
    if (pid == 0){
        //setup redirection here (child before exec)
        int r = execvp(args[0], args);
        if (r < 0) {
            std::cout << "fuck, error" << std::endl;
            perror("Failed");
        }
    } else if (pid > 0) {
        wait(&status);
    } else {
        std::cout << "Error forking" << std::endl;
    }
    return;
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

    if (flag == true || num_words == 0 || input == ">" || input == "<" || output == ">" || output == "<"){
        std::cout << "Invalid command" << std::endl;
        return;
    }

    // getting ready for exec
    std::vector<char*> cleaned_words;
    for (auto s: words){
        if (s != "<" && s != ">"){ cleaned_words.push_back(const_cast<char*>(s.c_str())); }
    }
    cleaned_words.push_back(NULL);

    exec_command(&cleaned_words[0], input, output);
    // clean it up (done)
    // setup redirection (almost done)
    // figure out error handling (read more I suppose)

}

void parse_and_run_command(const std::string &command) {
    /* TODO: Implement this. */
    /* Note that this is not the correct way to test for the exit command.
       For example the command "   exit  " should also exit your shell.
     */

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
