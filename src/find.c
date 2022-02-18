/* find.c */

/* Copyright (C) 1994-2002, Jim Hall <jhall@freedos.org> */

/*
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
   */

/* This program locates a string in a text file and prints those lines
 * that contain the string.  Multiple files are clearly separated.
 */

#include <stdlib.h>			/* borland needs this for 'exit' */
#include <fcntl.h>			/* O_RDONLY */
#include <string.h>
#include <ctype.h>

#include <dos.h>			/* for findfirst, findnext */

#ifdef __TURBOC__
#include <dir.h>			/* for findfirst, findnext */
#else
#include <unistd.h>
/* redefine struct name */
#define ffblk find_t
/* rename one of the member of that struct */
#define ff_name name
#define findfirst(pattern, buf, attrib) \
  _dos_findfirst((pattern), (attrib), (struct find_t *)(buf))
#define findnext(buf) _dos_findnext((struct find_t *)(buf))
#endif

#if defined(__WATCOMC__)
#include <io.h>				/* for findfirst, findnext */
#endif

#if defined(__GNUC__)
#include <libi86/stdlib.h>
#define far __far
#endif

#include "find_str.h"			/* find_str() back-end */

#include "../kitten/kitten.h"		/* Kitten message library */
#include "../tnyprntf/tnyprntf.h"


/* Functions */

void usage (nl_catd cat);


/* Main program */

#ifndef MAXDIR
#define MAXDIR 256
#endif

int
main (int argc, char **argv)
{
  char *s, *needle;
  int c, i, done, ret;

  unsigned drive /* , thisdrive */ ;	/* used to store, change drive */
  char cwd[MAXDIR], thiscwd[MAXDIR];	/* used to store, change cwd */
  char cwd2[MAXDIR];			/* store cwd of other drive */

  /* char drv[MAXDRIVE]; */		/* temporary buffer */
  unsigned drv;				/* drive found in argument */
  unsigned maxdrives;

  int invert_search = 0;		/* flag to invert the search */
  int count_lines = 0;			/* flag to whether/not count lines */
  int number_output = 0;		/* flag to print line numbers */
  int ignore_case = 0;			/* flag to be case insensitive */

  /* FILE *pfile; */			/* file pointer */
  int thefile;				/* file handler */
  nl_catd cat;				/* message catalog */
  struct ffblk ffblk;			/* findfirst, findnext block */

  /* Message catalog */

  cat = catopen ("find", 0);

  /* Scan the command line */
  c = 1; /* argv[0] is the path of the exectutable! */


  /* first, expect all slashed arguments */
  while ((c < argc) && (argv[c][0] == '/') ) {
      /* printf("arg: %s\n",argv[c]); */
      switch (argv[c][1]) {
	  case 'c':
	  case 'C':		/* Count */
	    count_lines = 1;
	    break;

	  case 'i':
	  case 'I':		/* Ignore */
	    ignore_case = 1;
	    break;

	  case 'n':
	  case 'N':		/* Number */
	    number_output = 1;
	    break;

	  case 'v':
	  case 'V':		/* Not with */
	    invert_search = 1;
	    break;

	  default:
	    usage (cat);
	    catclose (cat);
	    exit (2);		/* syntax error .. return errorlevel 2 */
	    break;
	    
      } /* end case */
      c++;	/* next argument */
  } /* end while */

  /* Get the string */

  if (c >= argc)
    {
      /* No string? */
      /* printf("no string"); */
      usage (cat);
      catclose (cat);
      exit (1);
    }
  else
    {
      /* Assign the string to find */
      needle = argv[c];
      c++; /* next argument(s), if any: file name(s) */
      /* printf("needle: %s\n",needle); */
    }



  /* Store the drive and cwd */

  /* findfirst/findnext do not return drive and cwd information, so we
     have to store the drive & cwd at the beginning, then chdir for
     each file we scan using findfirst, then chdir back to the initial
     drive & cwd at the end.  This is probably not the most efficient
     way of doing it, but it works.  -jh */

  _dos_getdrive (&drive); 		/* 1 for A, 2 for B, ... */
  getcwd (cwd, MAXDIR);			/* also stores drive letter */

#if 0 /* debugging */
  /* printf ("drive=%c\n", (drive+'A'-1)); */
  /* printf ("cwd=%s\n", cwd); */
#endif /* debugging */

  /* Scan the files for the string */

  if ((argc - c) <= 0)
    {
      /* No files on command line - scan stdin */
      ret = find_str (needle, 0 /* stdin */,
        invert_search, count_lines, number_output, ignore_case);
    }

  else
    {
      for (i = c; i < argc; i++)
	{
	  /* find drive and wd for each file when using findfirst */

	  /* fnsplit (argv[i], drv, thiscwd, NULL, NULL); */
	  /* fnsplit is "expensive", so replace it... */
	  
	  if (argv[i][1] == ':') {
	    drv = toupper(argv[i][0]) - 'A';
	    strcpy(thiscwd,argv[i]+2);
	  } else {
	    drv = drive - 1; /* default drive */
	    strcpy(thiscwd,argv[i]);
	  }

	  if (strrchr(thiscwd,'\\') == NULL) {
	    strcpy(thiscwd,"."); /* no dir given */
	  } else {
	    if (strrchr(thiscwd,'\\') != thiscwd) {
	      strrchr(thiscwd,'\\')[0] = '\0'; /* end string at last \\ */
	    } else {
	      strcpy(thiscwd,"\\"); /* dir is root dir */
	    }
	  }

          /* printf("drive (0=A:)=%d dir=%s\n", drv, thiscwd); */

	  /* use findfirst/findnext to expand the filemask */

	  done = findfirst (argv[i], &ffblk, 0);

	  if (done)
	    {
	      /* We were not able to find a file. Display a message and
		 set the exit status. */

	      s = catgets (cat, 2, 1, "No such file");
	      PRINTF("FIND: %s: %s\n", argv[i], s);
	    }

	  while (!done)
	    {
	      /* We have found a file, so try to open it */

	      /* set cwd to the filemask */

	      _dos_setdrive (drv + 1, &maxdrives);
	      getcwd(cwd2,MAXDIR); /* remember cwd here, too */

	      if (chdir (thiscwd) < 0) {
		  s = catgets (cat, 2, 2, "Cannot change to directory");
		  PRINTF("FIND: %s: %s\n", argv[i], s);
	      };

	      /* open the file, or not */

	      if ((thefile = open (ffblk.ff_name, O_RDONLY)) != -1)
		{
		  PRINTF("---------------- %s\n", ffblk.ff_name);
		  ret = find_str (needle, thefile, invert_search, count_lines, number_output, ignore_case);
		  close (thefile);
		}

	      else
		{
		  s = catgets (cat, 2, 0, "Cannot open file");
		  PRINTF("FIND: %s: %s\n", argv[i], s);
		}

	      /* return the cwd */

	      chdir (cwd2); /* restore cwd on THAT drive */

	      _dos_setdrive (drive, &maxdrives);
	      chdir (cwd);

	      /* find next file to match the filemask */

	      done = findnext (&ffblk);
	    } /* while !done */
	} /* for each argv */
    } /* else */

  /* Done */

  catclose (cat);


 /* RETURN: If the string was found at least once, returns 0.
  * If the string was not found at all, returns 1.
  * (Note that find_str.c returns the exact opposite values.)
  */

  exit ( (ret ? 0 : 1) );
  return (ret ? 0 : 1);

}

/* Show usage */

void
usage (nl_catd cat)
{
  char *s, *s2;

  (void)cat; /* avoid unused argument error message in kitten */

  PRINTF("FreeDOS Find, version 2.9\n");
  PRINTF("GNU GPL - copyright 1994-2002 Jim Hall <jhall@freedos.org>\n");
  PRINTF("          copyright 2003 Eric Auer <eric@coli.uni-sb.de>\n\n");

  s = catgets (cat, 0, 0, "Prints all lines of a file that contain a string");
  PRINTF("FIND: %s\n", s);

  s = catgets (cat, 1, 1, "string");
  s2 = catgets (cat, 1, 0, "file");
  PRINTF("FIND [ /C ] [ /I ] [ /N ] [ /V ] \"%s\" [ %s ... ]\n", s, s2);

  s = catgets (cat, 0, 1, "Only count the matching lines");
  PRINTF("  /C  %s\n", s);

  s = catgets (cat, 0, 2, "Ignore case");
  PRINTF("  /I  %s\n", s);

  s = catgets (cat, 0, 3, "Show line numbers");
  PRINTF("  /N  %s\n", s);

  s = catgets (cat, 0, 4, "Print lines that do not contain the string");
  PRINTF("  /V  %s\n", s);
}

