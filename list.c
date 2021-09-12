/**********************************************************************
 * list.c - defines a void * list object
 *
 * Copyright 1993, David Nedde
 * Copyright 2021, Joe Da Silva
 *
 * Permission to use, copy, modify, and distribute this software
 * and its documentation for any purpose and without fee is granted
 * provided that the above copyright notice appears in all copies.
 * It is provided "as is" without express or implied warranty.
 **********************************************************************/

/* System Headers */
#include <malloc.h>

/* Local headers */
#include "list.h"

#define LIST_INITIAL_SIZE 100
#define LIST_SIZE_ADD_MORE 100

/* Private Functions */

/* Return the specified object at a specified position */
static void *get_item(list_type list, int ind) {
    if (ind < list->count)
        return list->data[ind];
    else
        return (void *)0;
}

/* Increase size of list data. 1==done, 0==fail */
static int list_more_data(list_type list) {
    int i = 0;
    int n = list->max_count + LIST_SIZE_ADD_MORE;
    void **tmp_data;

    tmp_data = (void **)malloc( sizeof(void *) * n);
    if (tmp_data) {
        for (i = 0; i < list->count; i++)
            tmp_data[i] = list->data[i];
        free(list->data);
        list->data = tmp_data;
        list->max_count = n;
        i = 1;
#ifdef DEBUG
        printf("list_more = %d\n", n);
    } else {
        printf("list_more failed\n");
#endif
    }

    return i;
}


/* Public object methods */

/* Create list object. Return NULL if fail. */
list_type list__create() {
    list_type list;
    int n;

    list = (list_type)malloc(sizeof(list_struct));
    if (list) {
        list->count     = 0;
        list->curr_item = 0;
        list->max_count = n = LIST_INITIAL_SIZE;
        list->data = (void **)malloc(sizeof(void *) * list->max_count);
        if (!list->data) {
            free(list);
            list = (void *)0;
            n = 0;
        }
#ifdef DEBUG
        printf("list__create ");
        if (n) printf("= %d\n", n); else printf("failed\n");
    } else {
        printf("list__create failed\n");
#endif
    }

    return list;
}


/* Destroy the list object. Items in the list are not freed. */
/* Items in the list need to be deleted by another function. */
void list__destroy(list_type list) {
    free(list->data);
    free(list);
}


/* Add an item to the start of the list */
void *list__add_start(list_type list, void *data_item) {
    int i;

    if (list->count >= list->max_count)
        if (!list_more_data(list))
            return (void *)0;

    /* Copy all item up by one */
    for (i = list->count; i > 0; i--)
        list->data[i] = list->data[i - 1];

    list->data[0] = data_item;

    list->count++;
    list->curr_item = 0;

    return data_item;
}


/* Add an item to the end of the list */
void *list__add_end(list_type list, void *data_item) {
    if (list->count >= list->max_count)
        if (!list_more_data(list))
            return (void *)0;

    list->data[list->count] = data_item;
    list->curr_item = list->count;

    list->count++;

    return data_item;
}


/* Get the first item in the list */
void *list__get_first(list_type list) {
    if (list->count > 0) {
        list->curr_item = 0;
        return get_item(list, list->curr_item);
    } else
        return (void *)0;
}


/* Get the next item in the list.  NULL is returned if there are no more
   items in the list */
void *list__get_next(list_type list) {
    if (list->curr_item >= 0 && list->curr_item <= list->count - 1) {
      list->curr_item++;
      return get_item(list, list->curr_item);
    } else
        return (void *)0;
}


/* Return the last item in the list */
void *list__get_last(list_type list) {
    if (list->count > 0) {
        list->curr_item = list->count - 1;
        return get_item(list, list->curr_item);
    } else
        return (void *)0;
}


/* Return the previous item in the list.  If there are no more items,
   return NULL. */
void *list__get_prev(list_type list) {
    if (list->curr_item > 0 && list->curr_item < list->count) {
        list->curr_item--;
        return get_item(list, list->curr_item);
    } else
        return (void *)0;
}


/* Remove the last item in the list.  The item removed is returned. */
void *list__remove_last(list_type list) {
    void *data_item = (void *)0;

    if (list->count > 0) {
        list->count--;
        data_item = list->data[list->count];
        list->data[list->count] = (void *)0;
    }

    return data_item;
}


/* Remove the first item in the list.  The item removed is returned. */
void *list__remove_first(list_type list) {
    int i;
    void *data_item = (void *)0;

    if (list->count > 0) {
        data_item = list->data[0];

        /* Shift data pointers down one */
        list->count--;
        for (i = 0; i < list->count; i++)
            list->data[i] = list->data[i + 1];

        list->data[list->count] = (void *)0;
    }

    return data_item;
}


/* Remove the current item in the list, as positioned by get_first, next, 
   last, prev.   The current item is now the item after the one removed */
void *list__remove_curr(list_type list) {
    int i;
    void *data_item = (void *)0;

    if (list->curr_item >= 0 && list->curr_item < list->count) {
        data_item = list->data[list->curr_item];

        list->count--;
        for (i = list->curr_item; i < list->count; i++)
            list->data[i] = list->data[i+1];

        list->data[list->count] = (void *)0;
    }

    return data_item;
}


/* Remove an item at the specified position in the list */
void *list__remove(list_type list, int ind) {
    int i;
    void *data_item = (void *)0;

    if (ind >= 0 && ind < list->count) {
        data_item = list->data[ind];

        list->count--;
        for (i = ind; i < list->count; i++)
            list->data[i] = list->data[i+1];

        list->data[list->count] = (void *)0;
    }

    return data_item;
}
