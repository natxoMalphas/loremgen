/* loremgen -- generate custom lorem ipsum text

   Copyright 2023 Natxo Malphas 
      
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#include <config.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>

#include "text.h"

const char *program_name;

/* Options */
static _Bool indent = 0;
/* Maximum words per sentence. */
static int max_wps = 20;
/* Minimum words per sentence. Absolut minum is 5.  */
static int min_wps = 7;
/* Maximum words per paragraph.  */
static int max_wpp = 90;
/* Minimum words per paragraph.  */
static int min_wpp = 40;
/* Maximum characters per line.  */
static int max_cpl = 79;
/* Minimum characters per line.  */
static int min_cpl = 10;

static long paragraphs = 1;
static long classic_mode = 0;
/* Lines are formatted to include new line.  */
static _Bool line_format = 0;

static void write_file (char *str, const char *file_name);
static void print_help (void);
static void print_version (void);

static const struct option longopts[] = {
  {"classic", optional_argument, NULL, 'c'},
  {"help", no_argument, NULL, 'h'},
  {"indent", no_argument, NULL, 't'},
  {"line", optional_argument, NULL, 'l'},
  {"output", required_argument, NULL, 'o'},
  {"paragraphs", required_argument, NULL, 'p'},
  {"version", no_argument, NULL, 'v'},
  {NULL, 0, NULL, 0}
};

int
main (int argc, char *argv[])
{
  int optc;
  int lose = 0;
  const char *file_name = NULL;

  program_name = argv[0];

  while ((optc =
          getopt_long (argc, argv, "c::htl::o:p:v", longopts, NULL)) != -1)
    {
      switch (optc)
        {
        case 'c':
          if (optarg != NULL)
            {
              classic_mode = atoi (optarg);
              if ((classic_mode == 0) || (classic_mode > 2))
                {
                  fprintf (stderr,
                           "%s: Invalid value for parameter --classic.\n", 
                           program_name);
                  exit (EXIT_FAILURE);
                }
            }
          else
            classic_mode = 1;
          break;
        case 'h':
          print_help ();
          exit (EXIT_SUCCESS);
          break;
        case 't':
          indent = 1;
          break;
        case 'l':
          line_format = 1;
          if (optarg != NULL)
            {
              max_cpl = atoi (optarg);
              if (max_cpl < min_cpl)
                {
                  fprintf (stderr, "%s: Invalid value for parameter --line.\n",
                           program_name);
                  exit (EXIT_FAILURE);
                }
            }
          break;
        case 'o':
          file_name = optarg;
          break;
        case 'p':
          paragraphs = atoi (optarg);
          if (paragraphs == 0)
            {
              fprintf (stderr,
                       "%s: Invalid value for parameter --paragraphs.\n",
                       program_name);
              exit (EXIT_FAILURE);
            }
          break;
        case 'v':
          print_version ();
          exit (EXIT_SUCCESS);
          break;
        case '?':
          exit (EXIT_FAILURE);
          break;
        default:
          lose = 1;
          break;
        }
    }

  if (lose || optind < argc)
    {
      /* Print error message and exit.  */
      if (optind < argc)
        {
          fprintf (stderr, "%s: Extra operand: %s\n", program_name, argv[optind]);
          exit (EXIT_FAILURE);
        }
    }

  char *lorem_text = gen_text (paragraphs, classic_mode, line_format, indent,
                               min_wps, max_wps, min_wpp, max_wpp, max_cpl);

  if (file_name)
    write_file (lorem_text, file_name);
  else
    printf ("%s", lorem_text);

  free (lorem_text);

  exit (EXIT_SUCCESS);
}

static void
write_file (char *str, const char *file_name)
{

  FILE *stream;

  stream = fopen (file_name, "w");

  if (stream == NULL)
    {
      fprintf (stderr, "%s: Cannot open file %s", program_name, file_name);
      exit (EXIT_FAILURE);
    }

  fprintf (stream, str);
  fclose (stream);
}

static void
print_help (void)
{
  printf ("\
Usage: %s [OPTION]...\n", program_name);
  printf ("\
Generate Lorem Ipsum text. \n");
  printf ("\
-c, --classic[=M]     begin your text with the classic \"Lorem ipsum...\n\
                      Optional value \"mode\":\n\
                      1 - Default. Minimal mode, only the first five words:\n\
                          \"Lorem ipsum, dolor sit amet...\", the rest is\n\
                          random generated.\n\
                      2 - Generates the whole classic text of 69 words. If\n\
                          needed more, random generated. In order to do\n\
                          this, option values of min. and max. words per\n\
                          sentence logically cannot be respected.\n\
-h, --help            display this help and exit\n\
-l, --line[=N]        text is generated with formatted lines and N maximal\n\
                      number of characters per line.\n\
-t, --indent          add indentation to the paragraphs\n\
-o, --output=FILE     generate text into file\n\
-p, --paragraphs=N    generate N number of paragraphs\n\
-v, --version         display version info and exit\n");

  printf ("\n");
  printf ("Report bugs to %s\n", PACKAGE_BUGREPORT);
}

static void
print_version (void)
{
  printf ("loremgen %s\n", PACKAGE_VERSION);
  printf ("\
Copyright (C) 2023 Natxo Malphas\n\
License GPLv3+: GNU GPL version 3 or later\
<http://gnu.org/licenses/gpl.html>\n\
This is free software: you are free to change and redistribute it.\n\
There is NO WARRANTY, to the extent permitted by law.\n");
}
