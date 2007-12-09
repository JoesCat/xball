/**********************************************************************
 * color_l.c - Color list object definition
 *
 * Copyright 1993, David Nedde
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

/* Functions */
static void get_rgb4_names();


/* Public object methods */

/* Constructs the color_list object */
color_list_type color_list__create(filename)
char *filename;
{
    color_list_type color_list;


    color_list = XtNew( color_list_struct);

    color_list->shaded_4_list = list__create();

    get_rgb4_names( color_list, filename);

    return color_list;
}


/* Destroys the color_list object */
void color_list__destroy(color_list)
color_list_type color_list;
{
    color_type            color;

    /* 2003-04-10 This doesn't work correctly, for
    ** now, I have just removed the code. It only gets
    ** called once, so we just waste a little memory.
    */
    return;
    
    for (color = color_list__get_last( color_list);
         color != (color_type)0;
         color = color_list__get_prev( color_list))
    {
        XtFree( color->name);
        XtFree( (char *)color);
    }

    list__destroy(color_list->shaded_4_list);

    XtFree( (char *)color_list);
}


/* Remove a specified node from the colored list */
color_type color_list__remove( color_list, ind)
color_list_type color_list;
int             ind;
{
    return (color_type)list__remove( color_list->shaded_4_list, ind);
}


/* Returns the number of nodes in the color list */
int color_list__get_count( color_list)
color_list_type color_list;
{
    return list__get_count( color_list->shaded_4_list);
}


/* Private object methods */

/**********************************************************************
/*
/* get_rgb4_colors - Extract the names of all colors from rgb.txt that
/*		     have 4 shades of the same color.
/*
/* void get_rgb4_colors( color_list, passed_file_name)
/*
/* list_type color_list   <-  a list of found colors
/* char * passed_file_name -> The path of the rgb.txt file.  If this parameter
/*                            is NULL, the RGB.TXT #define is used.
/*
/**********************************************************************/
static void get_rgb4_names(color_list, passed_file_name)
color_list_type color_list;
char *passed_file_name;
{
    FILE		       *rgb; /* File pointer for rgb.txt */
    char			colorname[50]; /* Temp color name string */
    int                   r,g,b;
    color_type            color;
    color_type            colors[SHADES];
    int                   color_index;
    int                   shade_index;
  


    /*** Find the number of system colors */
    if (!(rgb = fopen(passed_file_name, "r"))) 
    {
        /* Problem opening file */
        fprintf(stderr,"color_list::get_rgb4_names: Opening rgb.txt file: ");
        perror(passed_file_name);
        exit( EXIT_FAILURE);
    }

    /* Count the number of 4-shaded colors in the rgb.txt file */
    while (4 == fscanf(rgb, "%d %d %d %49[^\n]\n", &r, &g, &b, colorname))
    {
        color = XtNew( color_struct);
        color->r = r;
        color->g = g;
        color->b = b;
        color->name = XtNewString( colorname);
        color->last_char = strlen( color->name) - 1;

        list__add_end( color_list->shaded_4_list, (void *)color);
    }

    fclose(rgb);


    /* Now, remove all colors that do not have color, color2, color3, color4
       in the list */

    color_index = color_list__get_count( color_list) - 1; 
    while (color_index >= SHADES - 1)
    {
        for (shade_index = 0; shade_index < SHADES; shade_index++)
	{
	    colors[ shade_index] = 
	        color_list__get_cell( color_list, color_index - shade_index);

	    if (colors[ shade_index]->name[ colors[ shade_index]->last_char] !=
		'0' + SHADES - shade_index)
	    {
	        /* all 4 colors are not there */
	        int saveindex = color_index;

	        color_index -= shade_index + 1;
		while( shade_index >= 0)
		{
		    XtFree( colors[ shade_index]->name);
		    XtFree( (char *)colors[ shade_index]);
		    color_list__remove( color_list, saveindex + shade_index);
		    shade_index--;
		}
		break;
	    }
	}
	if (shade_index == SHADES)
	  color_index -= SHADES;
    }

    /* delete any remaning colors... */
    while (color_index >= 0)
    {
        color = color_list__get_cell( color_list, color_index);

        XtFree( color->name);
        XtFree( (char *)color);
        color_list__remove( color_list, color_index);

	color_index--;
    }
}



/* Gets the last node in the color list */
color_type color_list__get_last( color_list)
color_list_type color_list;
{
    return (color_type)list__get_last( color_list->shaded_4_list);
}


/* Get the previous node in the color list */
color_type color_list__get_prev( color_list)
color_list_type color_list;
{
    return (color_type)list__get_prev( color_list->shaded_4_list);
}


#ifdef NOT_USED

char *color__get_shade( color, shade_num)
color_type color;
int shade_num;
{
    color->name[ color->last_char] = '0' + shade_num;
    return color->name;
}

/**********************************************************************
/*
/* is_4_shaded_color - Returns True if the colorname passed is one of
/*		       the colors that has 4 shades.
/*
/* Bool is_4_shaded_color( colorname)
/*
/*  Bool return <- True if the colorname is a color with 4 shades
/*  char *colorname -> The colorname to check
/*
/**********************************************************************/
static Bool is_4_shaded_color( colorname)
char *colorname;
{
  /* A color with '4' as it's last character that isn't a 
  /* grey color (e.g. grey14) is how we determine that we
  /* have found a color that has 4 shades. */

  return (colorname[strlen(colorname)-1] == '4' && 
	  (strncmp(colorname,"grey",4) != 0 &&
	   strncmp(colorname,"gray",4) != 0));
}
#endif

