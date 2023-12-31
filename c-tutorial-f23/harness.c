#include <stdio.h>
#include <string.h>
#include "list.h"

//[Exercise 3] visitor function for list_visit_items
void display(void *str){ 
    printf("%s\n", (char *)str);
}

//[Exercise 1] display input one word per line, skipping empty lines
void echo(FILE *file){
    char line[41]; //each string no more than 40 char + newline, account for buffer overflow
    int empty = 1;
    while(fgets(line, sizeof(line), file)){ //read until EOF
        if (line[0] != '\n'){
            empty = 0;
            printf("%s", line);
        }
    }
    if(empty){
        printf("<EMPTY>\n");
    }
}

//[Exercise 4] insert input into list & print
void tail(FILE *file){
    char line[41];
    list_t list;
    int empty = 1;
    list_init(&list, NULL, free); //'NULL' compare & 'free' datum_delete
    while(fgets(line, sizeof(line), file)){
        if (line[0] != '\n'){
            empty = 0;
            line[strcspn(line, "\n")] = 0;
            char *inserted_line = strdup(line); //allocate memory & duplicate 
            list_insert_tail(&list, inserted_line);
        }
    }
    if(empty){
        printf("<EMPTY>\n");
        return;
    }
    list_visit_items(&list, display); //print list

}

// [Exercise 5] insert input into list, remove three items from head, & print
void tail_remove(FILE *file){
    char line[41];
    list_t list;
    int empty = 1;
    list_init(&list, NULL, free); //'NULL' compare & 'free' datum_delete
    while(fgets(line, sizeof(line), file)){
        if (line[0] != '\n'){
            empty = 0;
            line[strcspn(line, "\n")] = 0;
            char *inserted_line = strdup(line); //allocate memory & duplicate 
            list_insert_tail(&list, inserted_line);
        }
    }
    if(empty){
        printf("<EMPTY>\n");
        return;
    }

    while (!list_empty(&list)){
        for (int i = 0; i<3; i++){
            list = *list_remove_head(&list);
        }
        list_visit_items(&list, display);
        printf("---------------------------\n");
    }

}

int main(int argc, char const *argv[]){
    FILE *file = fopen(argv[1], "r"); //open file in read mode
    if(strcmp(argv[2], "echo") == 0){ //echo option
        echo(file);
    }
    else if(strcmp(argv[2], "tail") == 0){ //tail option
        tail(file);
    }
    else if(strcmp(argv[2], "tail-remove") == 0){
        tail_remove(file);
    }
    else{
        printf("Invalid input\n");
    }
    fclose(file);
    return 0;
}

