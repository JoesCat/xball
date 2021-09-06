/**********************************************************************
 * table.c - table object definition
 *
 * Copyright 1993, David Nedde
 *
 * Permission to use, copy, modify, and distribute this software
 * and its documentation for any purpose and without fee is granted
 * provided that the above copyright notice appears in all copies.
 * It is provided "as is" without express or implied warranty.
 **********************************************************************/

/* System Headers */
#include <malloc.h>
#include <string.h>

/* Local headers */
#include "misc.h"
#include "table.h"

/* Structures */
typedef struct {
    char *key;
    void *data;
} table_node_struct_type, *table_node_type;


/* Public object methods */

/* Create a table object */
table_type table__create()
{
    table_type table = (table_type)malloc( sizeof(table_struct_type));

    table->list = list__create();

    return table;
}


/* Free table but not the data contained by the table */
void table__destroy( table)
table_type table;
{
    table_node_type table_node;

    while ((table_node = 
            (table_node_type)list__remove_last(table->list)) != 
                                                            (table_node_type)0)
    {
        free( table_node->key);
        free( table_node);
    }

    list__destroy( table->list);
    free( table);
}


/* Store data in the table by key */
void table__store( table, key, data)
table_type table;
char *     key;
void *     data;
{
    table_node_type table_node = 
        (table_node_type)malloc( sizeof(table_node_struct_type));

    table_node->key = my_strdup( key);
    table_node->data = data;

    list__add_end( table->list, (void *)table_node);
}


/* Get a value out of the table, given its key */
void *table__retrieve( table, key)
table_type table;
char *     key;
{
    table_node_type table_node;

    for (table_node = (table_node_type)list__get_first( table->list);
         table_node != (table_node_type)0;
         table_node = (table_node_type)list__get_next( table->list))
    {
        if (strcmp( table_node->key, key) == 0)
            return table_node->data;
    }

    return (void *)0;
}


#ifdef DEBUG

void table__dump( table)
table_type table;
{
    table_node_type table_node;

    for (table_node = (table_node_type)list__get_first( table->list);
         table_node != (table_node_type)0;
         table_node = (table_node_type)list__get_next( table->list))
    {
        printf("key = %s\n", table_node->key);
    }
    printf("------\n");
}

#endif
