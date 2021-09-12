/**********************************************************************
 * list-tests.c - test all functions in file list.c
 *
 * Copyright 2021, Joe Da Silva
 *
 * Permission to use, copy, modify, and distribute this software
 * and its documentation for any purpose and without fee is granted
 * provided that the above copyright notice appears in all copies.
 * It is provided "as is" without express or implied warranty.
 **********************************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "../list.c"

/* do nothing much, but valgrind should == 0 */
static int simple_list(void) {
    list_type list;

    printf("Simple_empty_list. Open, close, done.\n");
    list = list__create();
    list__destroy(list);
    return 0;
}

/* create something to store as data */
static int *create_value(int val) {
    int *p;
    p = malloc(sizeof(int));
    if (p) {
	*p = val;
    }
    return p;
}

/* test all existing functions in list.c */
static int fun_list(void) {
    list_type list;
    int *item, *item1, *item2, *item3, *item4, *item5;
    int c, i, n, max;

    printf("Create an empty list.\n");
    list = list__create();
    c   = list->count;
    n   = list->curr_item;
    max = list->max_count;
    printf("  max=%d count=%d curr=%d\n", list->max_count, list->count, list->curr_item);
    if (c != 0 || n != 0 || max < 100) return -1;

    printf("  add_start() 1 data item to list.\n");
    item1 = list__add_start(list, create_value(100));
    printf("    item1=%d\n", *item1);
    if (*item1 != 100) return -2;

    printf("  add_start() 2 more data items to list.\n");
    item2 = list__add_start(list, create_value(200));
    item3 = list__add_start(list, create_value(300));
    printf("    item3=%d, item2=%d, item1=%d\n", *item3, *item2, *item1);
    if (*item1 != 100 || *item2 != 200 || *item3 != 300) return -3;
    printf("    count=%d curr=%d\n", list->count, list->curr_item);
    if (list->count != 3 || list->curr_item != 0) return -4;

    printf("  add_end() 2 more data items to list.\n");
    item4 = list__add_end(list, create_value(400));
    item5 = list__add_end(list, create_value(500));
    printf("    item4=%d, item5=%d\n", *item4, *item5);
    if (*item4 != 400 || *item5 != 500) return -5;
    printf("    count=%d curr=%d\n", list->count, list->curr_item);
    if (list->count != 5 || list->curr_item != 4) return -6;

    printf("  get_first() data item from list.\n");
    item = list__get_first(list);
    printf("    item=%d, curr=%d\n", *item, list->curr_item);
    if (*item != 300 || list->curr_item != 0) return -7;

    printf("  get_next() data item from list.\n");
    item = list__get_next(list);
    printf("    item=%d, curr=%d\n", *item, list->curr_item);
    if (*item != 200 || list->curr_item != 1) return -8;

    printf("  get_last() data item from list.\n");
    item = list__get_last(list);
    printf("    item=%d, curr=%d\n", *item, list->curr_item);
    if (*item != 500 || list->curr_item != 4) return -9;

    printf("  get_prev() data item from list.\n");
    item = list__get_prev(list);
    printf("    item=%d, curr=%d\n", *item, list->curr_item);
    if (*item != 400 || list->curr_item != 3) return -10;

    printf("  remove_last() data item from list.\n");
    item = list__remove_last(list);
    printf("    item=%d, count=%d\n", *item, list->count);
    if (*item != 500 || list->count != 4) return -11;
    free(item);

    printf("  remove_first() data item from list.\n");
    item = list__remove_first(list);
    printf("    item=%d, count=%d\n", *item, list->count);
    if (*item != 300 || list->count != 3) return -12;
    free(item);

    list->curr_item = 1;
    printf("  remove_curr() data item from list. curr=%d\n", list->curr_item);
    item = list__remove_curr(list);
    printf("    item=%d, count=%d, curr=%d\n", *item, list->count, list->curr_item);
    if (*item != 100 || list->curr_item != 1 || list->count != 2) return -13;
    free(item);

    printf("  remove_curr() data item from list (at 0)\n");
    item = list__remove(list, 0);
    printf("    item=%d, count=%d, curr=%d\n", *item, list->count, list->curr_item);
    if (*item != 200 || list->curr_item != 1 || list->count != 1) return -14;
    free(item);

    printf("  remove_last() data item from list.\n");
    item = list__remove_last(list);
    printf("    item=%d, count=%d\n", *item, list->count);
    if (*item != 400 || list->count != 0) return -15;
    free(item);

    printf("  Done. Cleanup.\n");
    list__destroy(list);
    return 0;
}

int main(int argc, char **argv) {
    int e;

    e  = simple_list();
    e |= fun_list();
    return e;
}
