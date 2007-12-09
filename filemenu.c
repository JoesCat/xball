/**********************************************************************
 * filemenu.c
 * 
 * See SelFile() header for interface description.
 *
 * Copyright 1990, David Nedde
 *
 * Permission to use, copy, modify, and distribute this software
 * and its documentation for any purpose and without fee is granted
 * provided that the above copyright notice appears in all copies.
 * It is provided "as is" without express or implied warranty.
 *
 * This source is derived from xdbx source with the following disclaimer:
 **********************************************************************/
/*****************************************************************************
 *
 *  xdbx - X Window System interface to the dbx debugger
 *
 *  Copyright 1989 The University of Texas at Austin
 *  Copyright 1990 Microelectronics and Computer Technology Corporation
 *
 *  Permission to use, copy, modify, and distribute this software and its
 *  documentation for any purpose and without fee is hereby granted,
 *  provided that the above copyright notice appear in all copies and that
 *  both that copyright notice and this permission notice appear in
 *  supporting documentation, and that the name of The University of Texas
 *  and Microelectronics and Computer Technology Corporation (MCC) not be 
 *  used in advertising or publicity pertaining to distribution of
 *  the software without specific, written prior permission.  The
 *  University of Texas and MCC makes no representations about the 
 *  suitability of this software for any purpose.  It is provided "as is" 
 *  without express or implied warranty.
 *
 *  THE UNIVERSITY OF TEXAS AND MCC DISCLAIMS ALL WARRANTIES WITH REGARD TO
 *  THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 *  FITNESS, IN NO EVENT SHALL THE UNIVERSITY OF TEXAS OR MCC BE LIABLE FOR
 *  ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 *  RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 *  CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 *  CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 *  Author:  	Po Cheung
 *  Created:   	March 10, 1989
 *
 *****************************************************************************/

/* filemenu.c
 *
 *  Construct a file menu (directory browser) which allows a user to go
 *  up and down the directory tree, to select files.
 *  The file menu is popped up by the 'file' command button.
 *  Duane Voth (duanev@mcc.com) contributed to the layout of the file menu, 
 *  plus some code and ideas.
 *
 *  changeDir():	Record the current working directory.
 *  InList():		Select files to be displayed in the menu.
 *  ScanDir():		Scan the directory and record selected filenames.
 *  DisplayMenuFile():	Callback for the file menu.
 *  CancelFileMenu():	Pop down the file menu.
 *  SetUpFileMenu():	Create the file menu popupshell.
 *  UpdateFileMenu():	Update entries in the file menu.
 *  File():		Command callback for the 'file' command button.
 */

#include <ctype.h>
#include <X11/Xos.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Dialog.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/List.h>
#include <X11/Xaw/Paned.h>
#include <sys/stat.h>
#include <sys/param.h>
#ifdef SUNOS4
#include <dirent.h>
#else
#include <sys/dir.h>
#endif
/*#include "global.h"*/

#ifdef SUNOS4
typedef struct dirent 	Directory;
#else
typedef struct direct 	Directory;
#endif

#define MAXPATHLEN	1024
#define LINESIZ		512
#define LASTCH(s)	(s[strlen(s)-1])
#define SECLASTCH(s)	(s[strlen(s)-2])

#define MAXCOLUMNS      8 	/* max number of columns in file menu */
#define MAXARGS		20	/* max number of args */
#define FILES_PER_COL   10 	/* # of files per column in file menu */

#define	INVALID		(-2)
#define CANCELED	(-1)
#define OK		0
static int	glob_status;
static char	**glob_path;
static Widget	parent_w;


char		cwd[MAXPATHLEN] = "";	/* current working directory */
static char	fileMenuDir[MAXPATHLEN];/* current directory of file menu */
static char  	**filelist; 		/* list of file names in fileMenu */
static int	nfiles = 0;		/* number of files in filelist */
static Widget	popupshell,		/* parent of popup */
		popup, 			/* vpane widget containing file menu */
		fileMenu, 		/* list widget as file menu */
		fileMenuLabel; 		/* label widget as file menu label */

void		File();
static int	UpdateFileMenu();
static void 	CancelFileMenu();

/*  Change working directory to 'dir'.
 *  Modify static global variable, cwd, to keep track of 
 *  current working directory.
 */
static void changeDir(dir)
char *dir;
{
    int i;

    if (strcmp(dir, "./") == NULL) 
	return;
    if (dir[0] == '/' || dir[0] == '~') 
	strcpy(cwd, dir);
    if (strcmp(dir, "../") == NULL) {
	for (i=strlen(cwd); cwd[i] != '/' && i > 0; i--);
	cwd[i] = '\0';
	if (strcmp(cwd, "") == NULL)
	    strcpy(cwd, "/");
    }
    else {
	sprintf(cwd, "%s/%s", cwd, dir);
	LASTCH(cwd) = '\0';
    }
}


/*  Determines if a directory entry should appear in the file menu. 
 *  The files included in the menu are :
 *    ..  (parent directory)
 *    directories
 *    text files 
 *    executable files
 */
static int InList(entry)
Directory *entry;
{
    char pathname[LINESIZ];
    struct stat statbuf;

    if (strcmp(entry->d_name, ".") == NULL || 	/* ignore current directory */
	LASTCH(entry->d_name) == '~' ||		/* ignore Emacs backup files */
	entry->d_name[0] == '#' ||		/* Ignore emacs backup files */
	(LASTCH(entry->d_name) == 'o' && SECLASTCH(entry->d_name) == '.'))
						/* ignore object files */
	return False;
    if (entry->d_name[0] == '.' && entry->d_name[1] != '.')
	return False;				/* ignore hidden files */
    if (strcmp(cwd, "")) 			/* give full path name */
    	sprintf(pathname, "%s/%s", cwd, entry->d_name);
    else
    	strcpy(pathname, entry->d_name);
    if (stat(pathname, &statbuf) == -1) 
	return False;
    if (statbuf.st_mode & S_IFDIR) {		/* is directory */
	strcat(entry->d_name, "/");
	++(entry->d_namlen);
	return True;
    }
    if (statbuf.st_mode & S_IEXEC) {		/* is executable */
	strcat(entry->d_name, "*");
	++(entry->d_namlen);
	return True;
    }
    return True;
}


/*  Scans the working directory for files selected by InList(), sorted
 *  alphabetically, and stored in an array of pointers to directory
 *  entries called namelist.
 *  The names of the selected files are stored in filelist.
 */
static int ScanDir(dir)
char *dir;
{
    extern 	alphasort();
    Directory   **namelist;
    int		i, j;
    char	*format_static();

    nfiles = scandir(dir, &namelist, InList, alphasort);
    if (nfiles <= 0) {
      XBell(XtDisplay(popupshell),0);
      return(INVALID);
    }
    if (filelist) {
	for (i=0; filelist[i]; i++)
	    XtFree(filelist[i]);
	XtFree((char *)filelist);
    }
    filelist = (char **) XtMalloc((nfiles+1) * sizeof(char *));
    i = 0;
    for (j=0; j<nfiles; j++) {
	filelist[i++] = XtNewString(namelist[j]->d_name);
    	XtFree((char *)namelist[j]);
    }
    filelist[i++] = NULL;
    XtFree((char *)namelist);
    return(OK);
}
    

/* Delete the current directory prefix from the passed path */
char *remove_current_prefix(path)
char *path;
{
  char	wd[MAXPATHLEN];
  char	*temp_str;


  getwd(wd);
  if (!strncmp(wd,path,strlen(wd)))
  {
    /* wd matches path prefix */
    temp_str = &path[strlen(wd)];
    if (temp_str[0] == '/')
      /* If first char of prefix is a '/',	 *
       * remove it since we want a relative path */
      temp_str++;
    return(temp_str);
  }

  /* Couldn't find the prefix -- return the whole path */
  return(path);
}


/*  Callback for the fileMenu list widget.
 *  >  if the selected item is a directory, display contents of that directory.
 *  >  if the selected item is a readable file, return the file name
 */
/* ARGSUSED */
static void DisplayMenuFile(w, popupshell, call_data)
    Widget w;
    Widget popupshell;
    XawListReturnStruct *call_data;
{
    char *filename;

    XtPopdown(popupshell);
    filename = call_data->string;
    if (filename == NULL) return;
    if (LASTCH(filename) == '/') {
	changeDir(filename);
	XtDestroyWidget(popupshell);

	if (UpdateFileMenu() == INVALID)
	  changeDir("../");

	/* create new menu */
	File(w,(XtPointer )0,(XtPointer )0);			/* pop it up */
    }
    else {
      *glob_path = XtMalloc(strlen(filename)+strlen(cwd)+2);
      strcpy(*glob_path, remove_current_prefix(cwd));
      if (strlen(*glob_path) != 0)
	strcat(*glob_path, "/");
      strcat(*glob_path, filename);
      glob_status = OK;
    }
}


/*  Callback to popdown the file menu
 */
/* ARGSUSED */
static void CancelFileMenu(w, popupshell, call_data)
    Widget w;
    Widget popupshell;
    XtPointer call_data;
{
    XtPopdown(popupshell);
    glob_status = CANCELED;
}



void DisableWindowResize(w)
Widget w;
{
    Arg args[MAXARGS];
    Cardinal n;
    Dimension height;

    n = 0;
    XtSetArg(args[n], XtNheight, &height);                       n++;
    XtGetValues(w, args, n);
    XawPanedSetMinMax(w, height, height);
    XawPanedAllowResize(w, False);
}


/*  Creates a popup shell with its child being a vpane widget containing
 *  a file menu label, a file menu containing file names returned from 
 *  ScanDir(), and a cancel command button.
 *  When an item in the list is selected, DisplayMenuFile is called.
 */
static int SetUpFileMenu(dir) 
char *dir;
{
    Widget	cancelButton;
    Arg 	args[MAXARGS];
    Cardinal	n;
    char	menulabel[LINESIZ];
    int		ncolumns;

    n = 0;
    popupshell = XtCreatePopupShell("File Directory", transientShellWidgetClass,
				    parent_w, args, n);

    n = 0;
    popup = XtCreateManagedWidget("popup", panedWidgetClass, popupshell,
				  args, n);
    if (ScanDir(dir) == OK)
      /* It worked OK */
      strcpy(fileMenuDir, dir);
    else
      return(INVALID);

    n = 0;
    sprintf(menulabel, "   %s   ", dir);
    XtSetArg(args[n], XtNlabel, (XtArgVal) menulabel); 			n++;
    XtSetArg(args[n], XtNjustify, (XtArgVal) XtJustifyCenter);          n++;
    fileMenuLabel = XtCreateManagedWidget("fileMenuLabel", labelWidgetClass,
                                          popup, args, n);

    n = 0;
    ncolumns = nfiles/FILES_PER_COL + 1;
    ncolumns = MIN(ncolumns, MAXCOLUMNS);
    XtSetArg(args[n], XtNlist, filelist);				n++;
    XtSetArg(args[n], XtNverticalList, True);				n++;
    XtSetArg(args[n], XtNdefaultColumns, (XtArgVal) ncolumns);		n++;
    fileMenu = XtCreateManagedWidget("fileMenu", listWidgetClass, 
				     popup, args, n);
    XtAddCallback(fileMenu, XtNcallback, DisplayMenuFile,(XtPointer)popupshell);

    n = 0;
    XtSetArg(args[n], XtNresize, False);				n++;
    XtSetArg(args[n], XtNlabel, "CANCEL");				n++;
    cancelButton = XtCreateManagedWidget("cancelButton", commandWidgetClass,
					 popup, args, n);
    XtAddCallback(cancelButton, XtNcallback, CancelFileMenu, 
                  (XtPointer)popupshell);

    DisableWindowResize(fileMenuLabel);
    DisableWindowResize(cancelButton);
    return(OK);
}


/*  This routine is called when there is a a change in current directory.
 *  It destroys the existing popup shell and creates a new file menu based
 *  on the new current directory.  A new directory list is created.
 */
static int UpdateFileMenu()
{
    return(SetUpFileMenu(cwd));
}


/*  File command button callback.
 */
/* ARGSUSED */
void File(w, client_data, call_data)
    Widget w;
    XtPointer client_data;
    XtPointer call_data;
{
    Arg 	args[MAXARGS];
    Cardinal	n;
    Dimension	fileMenu_width, fileMenuLabel_width, border_width;
    Dimension	fileMenu_height, fileMenuLabel_height;
    Dimension   dialog_width, dialog_height;
    int         width, height;
    int         x_offset, y_offset;
    Position    x,y;

/*    if (strcmp(fileMenuDir, cwd))*/
      if (UpdateFileMenu() != OK)
	return;


    n = 0;
    XtSetArg(args[n], XtNwidth, &fileMenu_width);		n++;
    XtSetArg(args[n], XtNheight, &fileMenu_height);		n++;
    XtSetArg(args[n], XtNborderWidth, &border_width);		n++;
    XtGetValues(fileMenu, args, n);

    n = 0;
    XtSetArg(args[n], XtNwidth, &fileMenuLabel_width);		n++;
    XtSetArg(args[n], XtNheight, &fileMenuLabel_height);	n++;
    XtGetValues(fileMenuLabel, args, n);

    
    /* Place the popup window centered on parent */
    n = 0;
    XtSetArg(args[n], XtNwidth,  &dialog_width);		n++;
    XtSetArg(args[n], XtNheight, &dialog_height);		n++;
    XtGetValues(parent_w, args, n);

    width = MAX(fileMenu_width, fileMenuLabel_width);
    x_offset = (Position) (dialog_width - width - border_width) / 2;

    height = fileMenu_height + fileMenuLabel_height;
    y_offset = (Position) (dialog_height - height - border_width) / 2;

    XtTranslateCoords(parent_w, x_offset, y_offset, &x, &y);

    x = MAX(0, x);
    y = MAX(0, y);

    n = 0;
    XtSetArg(args[n], XtNx, x);					n++;
    XtSetArg(args[n], XtNy, y);					n++;
    XtSetValues(popupshell, args, n);


    /*XtManageChild( popupshell);*/
    XtPopup(popupshell, XtGrabNonexclusive);

    /*UpdateMessageWindow("Select a file or directory");*/
}


/**********************************************************************
 * SelFile
 *
 * Pop up a window, letting the user select a file.  When the file
 * is selected, the string is returned to the caller.
 *
 * If the init_path is NULL and this is the first call, the current
 *   directory is used, else the previous call's directory is used.
 * If the init_path is an empty string, the current directory is used.
 * A passed init path will have the current dir prepended if the path
 *   doesn't start with a '/'.
 * If the popup is canceled, -1 is returned, otherwise 0 is returned.
 **********************************************************************/
int SelFile(w,prompt,cancel,init_path,show_entry,path_return)
Widget		w;
char		*prompt;		/* Not used yet */
char		*cancel;		/* Not used yet */
char		*init_path;
int		*(*show_entry)();	/* Not used yet */
char		**path_return;
{
  XtAppContext app_context;
  XEvent event;


  parent_w = w;
  glob_path = path_return;

  if (init_path == (char *)0 || init_path[0] == '\0')
  {
    /* No init_path -- Use the current working directory */
    if (cwd[0] == '\0' || init_path == (char *)0 || init_path[0] == '\0')
      getwd(cwd);
  }
  else
  if (strcmp(cwd,init_path))
  {
    /* They are different -- copy requested starting dir in */
    /* Prefix with wd if init_path is a relative path */
    if (init_path[0] != '/')
    {
      getwd(cwd);
      strcat(cwd,"/");
    }
    else
      cwd[0] = '\0';
    strcat(cwd,init_path);
  }

  glob_status = INVALID;
  File(w, (XtPointer )0, (XtPointer )0);

  app_context = XtWidgetToApplicationContext( w);
  while (glob_status == INVALID)
  {
    XtAppNextEvent(app_context, &event);
    XtDispatchEvent(&event);
  }

  return(glob_status);
}

