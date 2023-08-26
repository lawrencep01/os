#include "list.h"

//[Exercise 2] list constructor
void list_init(list_t *l, int (*compare)(const void *key, const void *with), void (*datum_delete)(void *datum)){
    l->head = (list_item_t *)malloc(sizeof(list_item_t));
    l->tail = (list_item_t *)malloc(sizeof(list_item_t));

    l->head->next = l->tail;
    l->tail->pred = l->head;
    l->head->pred = NULL;
    l->tail->next = NULL;

    l->compare = compare; //compare points to passed in comparison function
    l->datum_delete = datum_delete; //datum_delete points to deletion function
}

//[Exercise 3] visit list items
void list_visit_items(list_t *l, void (*visitor)(void *v)){
    list_item_t *curr = l->head; //start @ head
    while (curr) { //iterate over each item
        if (curr != l->head && curr != l->tail){
            visitor(curr->datum);  //call visitor function on current item
        }
        curr = curr->next; //move to the next item
    }
}

//[Exercise 4] insert item @ list tail
void list_insert_tail(list_t *l, void *v){
    list_item_t *item = (list_item_t *)malloc(sizeof(list_item_t)); //allocate memory on heap
    item->datum = v;
    item->next = l->tail;
    item->pred = l->tail->pred;
    l->tail->pred->next = item;
    l->tail->pred = item;

    /*
    //empty list edge case
    if(l->tail == NULL){
        item->pred = NULL;
        l->head = item; //head and tail are the new item
        l->tail = item;
    }
    else{
        item->pred = l->tail;
        l->tail->next = item;
    }
    l->tail = item; //set l tail to item
    */
}

//[Exercise 5] remove item from list head
list_t* list_remove_head(list_t *l){
    if (!list_empty(l)){
        l->head->next->next->pred = l->head;
        l->head->next = l->head->next->next;
    }
    return l;
}

//helper function to check if list is empty
bool list_empty(list_t *l){
    return l->head->next == l->tail;
}