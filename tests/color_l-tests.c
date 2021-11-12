/**********************************************************************
 * color_l-tests.c - test all functions in file color_l.c
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
#include <Xm/Xm.h>

#include "../list.c"
#include "../rgbtxt_loc.h"
#include "../color_l.c"

/* fail to load rgb.txt file on purpose */
static int test_fail_to_load(void) {
    color_list_type color_list;

    printf("Fail to load rgb.txt file (on purpose).\n");
    color_list = color_list__create("no_file.txt");
    if (color_list)
	return -1;
    return 0;
    /* valgrind should be zero */
}

/* load rgb.txt file and test functions */
static int fun_color_l(void) {
    color_list_type color_list;
    color_type color, color1, color2, color3, color4;
    list_type list;
    int c;

    printf("Load '%s' file.\n", RGB1_TXT);
    color_list = color_list__create(RGB2_TXT);
    if (!color_list) {
	printf("  error - Try load secondary-site '%s' file.\n", RGB2_TXT);
	color_list = color_list__create(RGB2_TXT);
	if (!color_list)
	    return -2;
    }

    printf("  color_list__get_count(color_list)\n");
    c = color_list__get_count(color_list);
    printf("    count = %d.\n", c);
    if (c < 300 || c > 350) return -3;

    printf("  color_list__get_last(color_list)\n");
    color4 = color_list__get_last(color_list);
    printf("    color%d: %4d,%4d,%4d; %s %d\n", \
	    c, color4->r, color4->g, color4->b, color4->name, color4->last_char);
    if (color4->name[color4->last_char] != '4') return -4;

    printf("  color_list__get_prev(color_list)\n");
    color3 = color_list__get_prev(color_list);
    printf("    color%d: %4d,%4d,%4d; %s %d\n", \
	    c-1, color3->r, color3->g, color3->b, color3->name, color3->last_char);
    if (color3->name[color3->last_char] != '3') return -5;

    printf("  color_list__get_prev(color_list)\n");
    color2 = color_list__get_prev(color_list);
    printf("    color%d: %4d,%4d,%4d; %s %d\n", \
	    c-2, color2->r, color2->g, color2->b, color2->name, color2->last_char);
    if (color2->name[color2->last_char] != '2') return -6;

    printf("  color_list__get_prev(color_list)\n");
    color1 = color_list__get_prev(color_list);
    printf("    color%d: %4d,%4d,%4d; %s %d\n", \
	    c-1, color1->r, color1->g, color1->b, color1->name, color1->last_char);
    if (color1->name[color1->last_char] != '1') return -7;

    printf("  color_list__destroy(color_list)\n");
    color_list__destroy(color_list);
    /* valgrind should be zero */

    return 0;
}

int main(int argc, char **argv) {
    int e;

    e = test_fail_to_load();
    if (!e) e = fun_color_l();
    return e;
}
