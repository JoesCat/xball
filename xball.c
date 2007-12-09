/**********************************************************************
/* XBall.c - Written by David Nedde (daven@ivy.wpi.edu) 5/93
/* Features: Motif or Athena widget interface
/*           Item collision detection
/*           Window resize -> boundry change
/*           Kind-of Randomly colored balls
/*	     Shaded 3-D-looking balls
/*           Different ball creation algorithms
/*
/* Copyright 1991, 1993 David Nedde
/*
 * Permission to use, copy, modify, and distribute this software
 * and its documentation for any purpose and without fee is granted
 * provided that the above copyright notice appears in all copies.
 * It is provided "as is" without express or implied warranty.
/**********************************************************************/

/* System Headers */
#include <stdio.h>
#include <X11/Intrinsic.h>

/* Local headers */
#include "xball_sys.h"
#include "miscx.h"
#include "patchlevel.h"

/*** Start of main program ***/
main(argc,argv)
unsigned argc;
char **argv;
{
    /* Use the app-defaults file as a set of strings for the fallback */
    static String fallback_resources[] = {
#include "fallback.h"
    };
    Widget            toplevel;
    XtAppContext      app_context;
    xball_system_type xball_system;


    static XrmOptionDescRec options[] = {
        {"-delay",      "*delay",      XrmoptionSepArg, NULL },
        {"-gravity",    "*gravity",    XrmoptionSepArg, NULL },
        {"-elasticity", "*elasticity", XrmoptionSepArg, NULL },
        {"-itemWidth",  "*itemWidth",  XrmoptionSepArg, NULL },
        {"-itemHeight", "*itemHeight", XrmoptionSepArg, NULL },
        {"-rgbTxt",     "*rgbTxt",     XrmoptionSepArg, NULL },
        {"-help",       "*help",       XrmoptionNoArg, "True"},
        {"-collide",    "*collide",    XrmoptionNoArg, "True"},
        {"-perpetual",  "*perpetual",  XrmoptionNoArg, "True"},
        {"-haltSim",    "*haltSim",    XrmoptionNoArg, "True"},
        {"-demoFile",   "*demoFile",   XrmoptionSepArg, NULL}
    };


    if (argc > 1 && strncmp(argv[1],"-help", strlen(argv[1])) == 0)
    {
        printf("XBall - version %d, patchlevel %d.\n\n", VERSION, PATCHLEVEL);
        puts("Usage:\n\txball [-options ...]\n\nWhere options include:");
        puts(" Option          Default  Resource      Meaning");
        puts(" -help                                  Print out this message");
        puts(" -delay number      0     delay         Default delay setting to slow program");
        puts(" -gravity          10     gravity       Default gravity setting");
        puts(" -elasticity       90     elasticity    Default elasticity value");
        puts(" -itemWidth        10     itemWidth     Width of the drawn item");
        puts(" -itemHeight       10     itemHeight    Height of the drawn item");
        puts(" -collide                 collide       Balls hit each other if specified");
        puts(" -perpetual               perpetual     Balls do not die if specified");
        puts(" -haltSim                 haltSim       Starts w/simulation halted if specified");
        puts(" -demoFile                demoFile      A demo file to run at startup");
        puts(" -rgbTxt           /etc/X11/rgb.txt     Path to rgb file");

        exit(0);
    }

    toplevel = XtAppInitialize(&app_context,"XBall",
                               options, XtNumber( options),
                               &argc, argv,     
                               fallback_resources,
                               /*args*/(ArgList)NULL, /*num_args*/(Cardinal)0);
    setupConverters();

    xball_system = xball_system__create( toplevel);

    XtRealizeWidget(toplevel);

    XtAppMainLoop(app_context);

    return 0;
}
