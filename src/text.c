/* text.c

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

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include "text.h"

void add_char (char *str, const char *c);
void append_full_stop (char *str);
void prepend_blank (char *str);
void cap_word (char *word);
int get_random_int (int min, int max);

/* gen_text: This funtion returns an array of characters ready to print formatted
   accordingly to the specifications of the user.
   In order to do it it follows the following steps:
   
   1. The are 3 sources of data, the Cicero original text (DE_FINIBUS), the
   Original Lorem Ipsum (OG_LOREM), and the first 5 words of the Original Lorem
   Ipsum (OG_LOREM_MIN), who correspond with the 3 options c0(default), c2, c1.
   The DE_FINIBUS is an array of 4000+ words, all in lowercase and with no final
   stops. There are some '?' and ','. OG_LOREM is an array of 69 words, with full
   stops, uppercase and punctuations. This is, sentences are already formed.
   OG_LOREM_MIN is an array of just 5 words.
     If the user selects option c1 the only thing it do is check if the legth of
   these words exceedes the min_cpl and if so, insert a new line.
     If option c2 is selected, the user must accept that the values of min_wps,
   max_wps cannot be respected, because in the original text sentences are
   already formed. A random number for the first paragraph word length and we
   beginn formatting the text. If the paragraph_length > OG_LOREM we continue to
   step 2 when we finish formatting OG_LOREM and no words are left to complete
   the paragraph. If paragraph_length < OG_LOREM we stop formatting OG_LOREM when
   we reach the end of the paragraph. A full stop is inserted and continue to
   step 2 in a new paragraph.
   
   2. From DE_FINIBUS we add a random sequence of words and continue formatting
   until the number of paragraph introduced by the user is reached.
  
   Depends on: min_wps, max_wps, min_wpp, max_wpp
              indent, line_format, classic_mode  */
char *
gen_text (int paragraphs, long classic_mode, _Bool line_format, _Bool indent,
          int min_wps, int max_wps, int min_wpp, int max_wpp, int max_cpl)
{
  int i = 0;
  int total_words = 0;
  int total_lines = 0;
  int total_sentences = 0;
  int total_paragraphs = 0;

  int max_words_left;

  char *text;
  text = malloc (0);
  int len_text = 0;
  char *word;
  int length;
  int char_count = 0;           /* Counter for characters in line */
  int s_word_count = 0;         /* Counter for words in sentence */
  int p_word_count = 0;         /* Counter for words in paragraph */

  int sentence_words = get_random_int (min_wps, max_wps);
  int paragraph_words = get_random_int (min_wpp, max_wpp);

  if (indent && (classic_mode != 0))
    {
      add_char (text, TAB);
      char_count += TAB_LEN;
    }

  if (classic_mode == 1)
    {
      for (i = 0; i < OG_LOR_MIN_WORDS; i++)
        {
          length = strlen (OG_LOREM_MIN[i]);
          word = malloc ((length + 1) * sizeof (char));
          strcpy (word, OG_LOREM_MIN[i]);

          if (line_format && ((char_count + length + 1) > max_cpl))
            {
              add_char (text, NEW_LINE);
              char_count = 0;
            }

          if ((char_count != 0) && (i != 0))
            {
              prepend_blank (word);
              length++;
            }

          len_text = strlen (text);
          text = realloc (text, (len_text + length + 1) * sizeof (char));
          strcat (text, word);

          free (word);
          char_count += length;
        }

      s_word_count = 5;
      p_word_count = 5;
      total_words = 5;
    }
  else if (classic_mode == 2)
    {
      i = 0;

      while ((p_word_count < paragraph_words)
             && (p_word_count < OG_LOR_WORDS))
        {
          length = strlen (OG_LOREM[i]);
          word = malloc ((length + 1) * sizeof (char));
          strcpy (word, OG_LOREM[i]);

          if (line_format && ((char_count + length + 1) > max_cpl))
            {
              add_char (text, NEW_LINE);
              char_count = 0;
            }

          if ((char_count != 0) && (i != 0))
            {
              prepend_blank (word);
              length++;
            }

          char_count += length;

          if (*(word + length - 1) == '.')
            {
              total_sentences++;
              s_word_count = 0;
            }
          else
            s_word_count++;

          len_text = strlen (text);
          text = realloc (text, (len_text + length + 1) * sizeof (char));
          strcat (text, word);

          free (word);
          p_word_count++;
          total_words++;
          i++;
        }

      if (p_word_count == paragraph_words)
        {
          append_full_stop (text);

          total_sentences++;
          s_word_count = 0;

          total_paragraphs++;
          p_word_count = 0;
          char_count = 0;
        }
    }

  while (total_paragraphs < paragraphs)
    {
      i += get_random_int (1, 8);

      if (i > CICERO_WORDS)
        i = i - CICERO_WORDS;

      length = strlen (DE_FINIBUS[i]);
      word = malloc ((length + 1) * sizeof (char));
      strcpy (word, DE_FINIBUS[i]);

      /* Case: the word is the last of a sentence.
         Action:  a full stop must be appended and the number of words of the
         next sentence reseted.  */
      if (s_word_count == sentence_words)
        {
          append_full_stop (word);
          s_word_count = 0;
          total_sentences++;

          sentence_words = get_random_int (min_wps, max_wps);
          max_words_left = paragraph_words - p_word_count;

          /* Check if the next sentence doesn't violete max words per
             paragraph, and if so, fix it.  */
          if (sentence_words > max_words_left)
            {
              if (max_words_left >= min_wps)
                sentence_words = max_words_left;
              else
                paragraph_words = p_word_count; //TODO: min_wpp?
            }

          /* Case: the word is the last of a paragraph
             Action: the number of words of the next paragraph must be reseted.
             Full stop was added on the last conditional.  */
          if (p_word_count == paragraph_words)
            {
              p_word_count = 0;
              total_paragraphs++;
              paragraph_words = get_random_int (min_wpp, max_wpp);
            }
          else
            p_word_count++;
        }
      else
        {
          if (s_word_count == 0)
            {
              cap_word (word);

              /* Case: the word is at the beginning of a paragraph.
                 Action: add a new line (if its not the first paragraph)
                 and insert tabulation if necessary.  */
              if (p_word_count == 0)
                {
                  char_count = 0;

                  if (total_words != 0)
                    {
                      add_char (text, NEW_LINE);
                    }

                  if (indent)
                    {
                      add_char (text, TAB);
                      char_count += TAB_LEN;
                    }
                }
            }
          s_word_count++;
          p_word_count++;
        }

      length = strlen (word);

      /* Case: line formatting is defined and current word would surpass the
         maximum characters per line.
         Action: insert a new line in the text.  */
      if (line_format && ((char_count + length + 1) > max_cpl))
        {
          add_char (text, NEW_LINE);
          char_count = 0;
        }

      /* Case: the word is not at the beginning of the line and is not the
         beginning of a paragraph.
         Action: prepare the wor prepending a blank space.  */
      if ((char_count != 0) && (p_word_count != 1))
        {
          prepend_blank (word);
          length++;
        }

      len_text = strlen (text);
      text = realloc (text, (len_text + length + 1) * sizeof (char));
      strcat (text, word);
      char_count += length;

      free (word);
      total_words++;
    }

  add_char (text, NEW_LINE);

  return text;
}

void
add_char (char *str, const char *c)
{
  int len_str = strlen (str);
  str = realloc (str, (len_str + 2) * sizeof (char));
  strcat (str, c);
}

/* append_full: Smart appends a full stop to a given string.
   If the string already has a full stop at the end does nothing.
   If the string has a punctuation sign at the end, it replaces it whith a
   full stop character.  */
void
append_full_stop (char *str)
{
  const char *full_stop_c = ".";
  const char comma_c = ',';
  const char interr_c = '?';
  size_t len_str = strlen (str);

  char last_c = *(str + len_str - 1);

  if ((last_c == comma_c) || (last_c == interr_c))
    {
      *(str + len_str - 1) = '.';
    }
  else if (last_c != '.')
    {
      str = realloc (str, (len_str + 2) * sizeof (char));
      strcat (str, full_stop_c);
    }
}

/* prepend_blank: Prepends a blank space into a given string  */
void
prepend_blank (char *str)
{
  const char *blank_c = " ";

  size_t len_str = strlen (str);
  str = realloc (str, (len_str + 2) * sizeof (char));

  memmove (str + 1, str, len_str + 1);
  memcpy (str, blank_c, 1);
}

/* cap_word: Capitalizes given word.  */
void
cap_word (char *word)
{
  char first_char;

  first_char = *word;
  int length = strlen (word);
  first_char = toupper (first_char);
  *word = first_char;
}

/* get_random_int: Returns a random integer between given parameters.
   Note: I was not sure where to declare srand(seed), here or at the beginning
   of the program. It seems to work both ways, so I decided to declare here
   each time the function is called, albeit it seems to be not neccesary.  */
int
get_random_int (int min, int max)
{
  srand (time (NULL));
  int num = (rand () % (max - min + 1)) + min;
  return num;
}
