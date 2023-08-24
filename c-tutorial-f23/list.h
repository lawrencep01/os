#ifndef LIST_H
#define LIST_H

#include <stdio.h>
#include <stdlib.h>

//[Exercise 2] define list_item struct (from c-tutorial-f23)
typedef struct list_item {
    struct list_item *pred, *next;
    void *datum;
} list_item_t;

//[Exercise 2] define list struct
typedef struct list {
    list_item_t *head, *tail; //pointer to head and tail of list
    int (*compare)(const void *key, const void *with); //Function ptr to compare list data
    void (*datum_delete)(void *datum); //Function ptr to delete list data
} list_t;


//[Exercise 2] list constructor prototype (from c-tutorial-f23)
void list_init(list_t *l, int (*compare)(const void *key, const void *with), void (*datum_delete)(void *datum));

//[Exercise 3] declare list_visit_items
void list_visit_items(list_t *l, void (*visitor)(void *v));

//[Exercise 4] declare list_insert_tail
void list_insert_tail(list_t *l, void *v);

//[Exercise 5] declare list_remove_head
void list_remove_head(list_t *l);



#endif
