#include "list.h"

//[Exercise 2] list constructor
void list_init(list_t *l, int (*compare)(const void *key, const void *with), void (*datum_delete)(void *datum)){
    l->head = NULL;
    l->tail = NULL;
    l->compare = compare; //compare points to passed in comparison function
    l->datum_delete = datum_delete; //datum_delete points to deletion function
}

//[Exercise 3] visit list items
void list_visit_items(list_t *l, void (*visitor)(void *v)){
    list_item_t *curr = l->head; //start @ head
    while (curr) { //iterate over each item
        visitor(curr->datum);  //call visitor function on current item
        curr = curr->next; //move to the next item
    }
}

//[Exercise 4] insert item @ list tail
void list_insert_tail(list_t *l, void *v){
    list_item_t *item = (list_item_t *)malloc(sizeof(list_item_t));
    item->datum = v;
    item->next = NULL; 

    //empty list edge case
    if(l->tail == NULL){
        item->pred = NULL;
        l->head = item;
        l->tail = item;
    }
    else{
        item->pred = l->tail;
        l->tail->next = item;
    }
    l->tail = item; //new item now tail of list
}

//[Exercise 5] remove item from list head
void list_remove_head(list_t *l){
    list_item_t *tmp = l->head;
    l->head = l->head->next;
    l->head->pred = tmp;
    l->datum_delete(tmp->datum);
    free(tmp);
}