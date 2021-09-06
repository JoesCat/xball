/**********************************************************************
 * file - description
 *
 * Copyright 1993, David Nedde
 *
 * Permission to use, copy, modify, and distribute this software
 * and its documentation for any purpose and without fee is granted
 * provided that the above copyright notice appears in all copies.
 * It is provided "as is" without express or implied warranty.
 **********************************************************************/

#ifndef __TABLE_H__
#define __TABLE_H__

#include "list.h"

typedef struct {
   list_type list;
} table_struct_type, *table_type;


table_type table__create();
void       table__destroy(/* table */);
void       table__store(/* table, key, data */);
void *     table__retrieve(/* table, key */);
#ifdef DEBUG
void       table__dump(/* table */);
#endif


#endif
