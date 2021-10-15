/**********************************************************************
 * color_l.c - Color list object definition
 *
 * Copyright 1993, David Nedde
 * Copyright 2021, Joe Da Silva - mods, fixes.
 *
 * Permission to use, copy, modify, and distribute this software
 * and its documentation for any purpose and without fee is granted
 * provided that the above copyright notice appears in all copies.
 * It is provided "as is" without express or implied warranty.
 **********************************************************************/

/* System Headers */
#include <stdlib.h>
#include <stdio.h>
#include <X11/X.h>
#include <X11/Intrinsic.h>

/* Local headers */
#include "color_l.h"
#include "misc.h"

/* Private Functions */

static Bool is_Nof4_shaded_color(char *colorname, char N) {
  /* A color with 'N' as it's last character that isn't a
  /* grey color (e.g. grey11) is how we determine that we
  /* have found a color that has 4 shades. */

  return (colorname[strlen(colorname)-1] == N &&
          (strncmp(colorname,"grey",4) != 0 &&
           strncmp(colorname,"gray",4) != 0));
}

/**********************************************************************
/*
/* get_rgb4_colors - Extract the names of all colors from rgb.txt that
/*                   have 4 shades of the same color.
/*
/* void get_rgb4_colors( color_list, passed_file_name)
/*
/* list_type color_list   <-  a list of found colors
/* char * passed_file_name -> The path of the rgb.txt file.  If this parameter
/*                            is NULL, the RGB.TXT #define is used.
/*
/**********************************************************************/
static int get_rgb4_names(color_list_type color_list, char *passed_file_name) {
    FILE       *rgb; /* File pointer for rgb.txt */
    char       colorname[50]; /* Temp color name string */
    int        r,g,b;
    color_type color;
    color_type colors[SHADES];
    int        color_index;
    int        shade_index;

    /*** Find the number of system colors */
    if (!(rgb = fopen(passed_file_name, "r"))) {
        /* Problem opening file */
        fprintf(stderr,"color_list::get_rgb4_names: Opening rgb.txt file: ");
        perror(passed_file_name);
        return 0;
    }

    /* Count the number of 4-shaded colors in the rgb.txt file */
    color_index = 0;
    while (4 == fscanf(rgb, "%d %d %d %49[^\n]\n", &r, &g, &b, colorname)) {
        color = XtNew(color_struct);
        color->r = r;
        color->g = g;
        color->b = b;
        color->name = XtNewString(colorname);
        color->last_char = strlen(color->name) - 1;

        list__add_end(color_list->shaded_4_list, (void *)color);
        ++color_index;

#ifdef DEBUG
        printf("color%d: %4d,%4d,%4d; %s %d\n", \
                color_index, r, g, b, color->name, color->last_char);
#endif
    }

    fclose(rgb);

    /* Now, remove all colors that do not have color1, color2, color3, color4
       in the list */

    color_index = color_list__get_count(color_list) - 1;
#ifdef DEBUG
        printf("color_index = %d\n", color_index);
#endif
    while (color_index >= 0) {
        color = color_list__get_cell(color_list, color_index);
        if (color_index >= SHADES - 1 && is_Nof4_shaded_color(color->name, '0' + SHADES)) {
            for (shade_index = 0; shade_index < SHADES; shade_index++) {
                colors[shade_index] =
                    color_list__get_cell(color_list, color_index - shade_index);
#ifdef DEBUG
                printf("  testing color%d: %4d,%4d,%4d; %s %d\n", \
                    color_index - shade_index, \
                    colors[shade_index]->r, colors[shade_index]->g, colors[shade_index]->b, \
                    colors[shade_index]->name, colors[shade_index]->last_char);
#endif
                if (colors[shade_index]->name[colors[shade_index]->last_char] !=
                    '0' + SHADES - shade_index) {
                    /* all 4 colors are not here. remove last one and keep looking. */
                    XtFree(color->name);
                    XtFree((char *)color);
                    color_list__remove(color_list, color_index);
                    color_index--;
                    break;
                }
            }
            color_index -= SHADES;
        } else {
#ifdef DEBUG
            printf("  removing color%d: %4d,%4d,%4d; %s %d\n", \
                color_index, color->r, color->g, color->b, color->name, color->last_char);
#endif
            XtFree(color->name);
            XtFree((char *)color);
            color_list__remove(color_list, color_index);
            color_index--;
        }
    }

#ifdef DEBUG
    printf("color count total = %d\n", color_list__get_count(color_list));
#endif
    return 1;
}


/* Public object methods */

/* Constructs the color_list object. Cleanup and return if failure. */
color_list_type color_list__create(char *filename) {
    color_list_type color_list;

    color_list = XtNew(color_list_struct);
    if (color_list) {
        color_list->shaded_4_list = list__create();
        if (color_list->shaded_4_list) {
            if (get_rgb4_names(color_list, filename))
                return color_list;
        }
        list__destroy(color_list->shaded_4_list);
        XtFree((char *)color_list);
        color_list = 0;
    }
    return color_list;
}


/* Destroys the color_list object */
void color_list__destroy(color_list_type color_list) {
    color_type            color;

    for (color = color_list__get_last(color_list);
         color != (color_type)0;
         color = color_list__get_prev(color_list))
    {
        XtFree(color->name);
        XtFree((char *)color);
    }

    list__destroy(color_list->shaded_4_list);

    XtFree((char *)color_list);
}


/* Remove a specified node from the colored list */
color_type color_list__remove(color_list_type color_list, int ind) {
    return (color_type)list__remove(color_list->shaded_4_list, ind);
}


/* Returns the number of nodes in the color list */
int color_list__get_count(color_list_type color_list) {
    return list__get_count(color_list->shaded_4_list);
}


/* Gets the last node in the color list */
color_type color_list__get_last(color_list_type color_list) {
    return (color_type)list__get_last(color_list->shaded_4_list);
}


/* Get the previous node in the color list */
color_type color_list__get_prev(color_list_type color_list) {
    return (color_type)list__get_prev(color_list->shaded_4_list);
}


#ifdef NOT_USED

char *color__get_shade(color_type color, int shade_num) {
    color->name[color->last_char] = '0' + shade_num;
    return color->name;
}

#endif
