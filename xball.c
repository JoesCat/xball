/**********************************************************************
/* XBall.c - Written by David Nedde (daven@ivy.wpi.edu) 5/93
/* Features: Motif or Athena widget interface
/*           Item collision detection
/*           Window resize -> boundary change
/*           Kind-of Randomly colored balls
/*           Shaded 3-D-looking balls
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
#include <stdlib.h>

/*** Start of main program ***/
int
main(argc,argv)
int argc;
char **argv;
{
    /* Use the app-defaults file as a set of strings for the fallback */
    static String fallback_resources[] = {
#include "fallback.h"
    };
    Widget            toplevel;
    XtAppContext      app_context;
    xball_system_type xball_system;

    #pragma pack()
    static char Mdelay[]      = "--delay";
    static char Odelay[]      = "*delay";
    static char Mgravity[]    = "--gravity";
    static char Ogravity[]    = "*gravity";
    static char Melasticity[] = "--elasticity";
    static char Oelasticity[] = "*elasticity";
    static char MitemWidth[]  = "--itemWidth";
    static char OitemWidth[]  = "*itemWidth";
    static char MitemHeight[] = "--itemHeight";
    static char OitemHeight[] = "*itemHeight";
    static char MrgbTxt[]     = "--rgbTxt";
    static char OrgbTxt[]     = "*rgbTxt";
    static char MrgbTxt2[]    = "--rgbTxt2";
    static char OrgbTxt2[]    = "*rgbTxt2";
    static char Mhelp[]       = "--help";
    static char Ohelp[]       = "*help";
    static char Mcollide[]    = "--collide";
    static char Ocollide[]    = "*collide";
    static char Mperpetual[]  = "--perpetual";
    static char Operpetual[]  = "*perpetual";
    static char MhaltSim[]    = "--haltSim";
    static char OhaltSim[]    = "*haltSim";
    static char MdemoFile[]   = "--demoFile";
    static char OdemoFile[]   = "*demoFile";

    static XrmOptionDescRec options[] = {
        {Mdelay,        Odelay,       XrmoptionSepArg, NULL },
        {Mdelay+1,      Odelay,       XrmoptionSepArg, NULL },
        {Mgravity,      Ogravity,     XrmoptionSepArg, NULL },
        {Mgravity+1,    Ogravity,     XrmoptionSepArg, NULL },
        {Melasticity,   Oelasticity,  XrmoptionSepArg, NULL },
        {Melasticity+1, Oelasticity,  XrmoptionSepArg, NULL },
        {MitemWidth,    OitemWidth,   XrmoptionSepArg, NULL },
        {MitemWidth+1,  OitemWidth,   XrmoptionSepArg, NULL },
        {MitemHeight,   OitemHeight,  XrmoptionSepArg, NULL },
        {MitemHeight+1, OitemHeight,  XrmoptionSepArg, NULL },
        {MrgbTxt,       OrgbTxt,      XrmoptionSepArg, NULL },
        {MrgbTxt+1,     OrgbTxt,      XrmoptionSepArg, NULL },
        {MrgbTxt2,      OrgbTxt2,     XrmoptionSepArg, NULL },
        {MrgbTxt2+1,    OrgbTxt2,     XrmoptionSepArg, NULL },
        {Mhelp,         Ohelp,        XrmoptionNoArg, "True"},
        {Mhelp+1,       Ohelp,        XrmoptionNoArg, "True"},
        {Mcollide,      Ocollide,     XrmoptionNoArg, "True"},
        {Mcollide+1,    Ocollide,     XrmoptionNoArg, "True"},
        {Mperpetual,    Operpetual,   XrmoptionNoArg, "True"},
        {Mperpetual+1,  Operpetual,   XrmoptionNoArg, "True"},
        {MhaltSim,      OhaltSim,     XrmoptionNoArg, "True"},
        {MhaltSim+1,    OhaltSim,     XrmoptionNoArg, "True"},
        {MdemoFile,     OdemoFile,    XrmoptionSepArg, NULL },
        {MdemoFile+1,   OdemoFile,    XrmoptionSepArg, NULL }
    };

    if (argc > 1 && /* -help or --help */ \
        (strncmp(argv[1],Mhelp+1, strlen(argv[1])) == 0 || \
         strncmp(argv[1],Mhelp  , strlen(argv[1])) == 0) ) {
        printf("XBall - version %s, patchlevel %d.\n\n", VERSION, PATCHLEVEL);
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
        puts(" -rgbTxt                  /etc/rgb.txt  Expected path to rgb file");
        puts(" -rgbTxt2       /usr/share/X11/rgb.txt  Alternate Path to another rgb file");

        exit(0);
    }

    toplevel = XtAppInitialize(&app_context,"XBall",
                               options, XtNumber(options),
                               &argc, argv,
                               fallback_resources,
                               /*args*/(ArgList)NULL, /*num_args*/(Cardinal)0);
    setupConverters();

    xball_system = xball_system__create( toplevel);

    XtRealizeWidget(toplevel);

    XtAppMainLoop(app_context);

    return 0;
}
