/*********************************************************************************************************//**
 * @file    printf.c
 * @version V1.0
 * @date    09/08/2011
 * @brief   Print functions.
 *************************************************************************************************************
 *
 * <h2><center > Copyright (C) 2011 Holtek Semiconductor Inc. All rights reserved</center></h2>
 *
 ************************************************************************************************************/

/* Includes ------------------------------------------------------------------------------------------------*/
#include "ht32f175x_275x.h"
#include <string.h>

/* Private typedef -----------------------------------------------------------------------------------------*/
/* Private define ------------------------------------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------------------------------------*/
#define vaStart(list, param) list = (char*)((int)&param + sizeof(param))
#define vaArg(list, type) ((type *)(list += sizeof(type)))[-1]

/* Private variables ---------------------------------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------------------------------------*/
static const char *FormatItem(const char *f, int a);
static void PutRepChar(const char c, int count);
static int PutString(const char *pString);
static int PutStringReverse(const char *pString, int index);
static void PutNumber(int value, int radix, int width, char fill);

/* Private functions ---------------------------------------------------------------------------------------*/

/*********************************************************************************************************//**
 * @brief Print function.
 * @param f: Format string.
 * @retval String length.
 ************************************************************************************************************/
signed int printf(const char *f, ...)
{
  char *argP;
  int i = 0;

  vaStart(argP, f);
  while (*f)
  {
    if (*f == '%')
    {
      f = FormatItem(f + 1, vaArg(argP, int));
    }
    else
    {
      fputc(*f++, (FILE *)1);
    }
    i++;
  }
  return i;
}

/*********************************************************************************************************//**
 * @brief Put string.
 * @param pString: String.
 * @retval String length.
 ************************************************************************************************************/
signed int puts(const char *pString)
{
  int i;
  i = PutString(pString);
  fputc('\n', (FILE *)1);
  return i;
}

/*********************************************************************************************************//**
 * @brief Format item for print function.
 * @param f: Format string.
 * @param a: Length of format string.
 * @retval Point of string.
 ************************************************************************************************************/
static const char *FormatItem(const char *f, int a)
{
  char c;
  int fieldwidth = 0;
  int leftjust = FALSE;
  int radix = 0;
  char fill = ' ';
  int i;

  if (*f == '0')
  {
    fill = '0';
  }

  while ((c = *f++) != 0)
  {
    if (c >= '0' && c <= '9')
    {
      fieldwidth = (fieldwidth * 10) + (c - '0');
    }
    else
    {
      switch (c)
      {
        case '\000':
        {
          return (--f);
        }
        case '%':
        {
          fputc('%', (FILE *)1);
          return (f);
        }
        case '-':
        {
          leftjust = TRUE;
          break;
        }
        case 'c':
        {
          if (leftjust)
          {
            fputc(a & 0x7f, (FILE *)f);
          }
          if (fieldwidth > 0)
          {
            PutRepChar(fill, fieldwidth - 1);
          }
          if (!leftjust)
          {
            fputc(a & 0x7f, (FILE *)f);
            return (f);
          }
        }
        case 's':
        {
          i = 0;
          while (*((char *)(a + i)) !='\0' )
          {
            i++;
          }

          if (leftjust)
          {
            PutString((char *)a);
          }

          if (fieldwidth > i )
          {
            PutRepChar(fill, fieldwidth - i);
          }

          if (!leftjust)
          {
            PutString((char *)a);
          }
          return (f);
        }
         case 'l':
        {
          radix = -10;
          f++;
          break;
        }
        case 'd':
        case 'i':
        {
          radix = -10;
          break;
        }
        case 'u':
        {
          radix = 10;
          break;
        }
        case 'x':
        case 'X':
        {
          radix = 16;
          break;
        }
        case 'o':
        {
          radix = 8;
          break;
        }
        default:
        {
          radix = 3;
          break;
        }
      }
    }
    if (radix)
    {
      break;
    }
  }

  if (leftjust)
  {
    fieldwidth = -fieldwidth;
  }

  PutNumber(a, radix, fieldwidth, fill);

  return (f);
}

/*********************************************************************************************************//**
 * @brief Put repeat character.
 * @param c: Character.
 * @param count: Repeat count
 ************************************************************************************************************/
static void PutRepChar(const char c, int count)
{
  while (count--)
  {
    fputc(c, (FILE *)1);
  }
}

/*********************************************************************************************************//**
 * @brief Put string.
 * @param pString: String.
 * @retval String length.
 ************************************************************************************************************/
static int PutString(const char *pString)
{
    int i = 0;
    while (*pString != '\0')
    {
        fputc(*pString, (FILE *)1);
        pString++;
        i++;
    }

    return i;
}

/*********************************************************************************************************//**
 * @brief Put srting in reversed order.
 * @param pString: String.
 * @param index: Srting length
 * @retval String length.
 ************************************************************************************************************/
static int PutStringReverse(const char *pString, int index)
{
    int i = 0;
    while ((index--) > 0)
    {
        fputc(pString[index], (FILE *)1);
        i++;
    }
    return i;
}

/*********************************************************************************************************//**
 * @brief Put number.
 * @param value: Value of number.
 * @param radix: Radix of number.
 * @param width: Width of number.
 * @param fill: fill character.
 ************************************************************************************************************/
static void PutNumber(int value, int radix, int width, char fill)
{
  char buffer[8];
  int bi = 0;
  unsigned int uvalue;
  unsigned short digit;
  unsigned short left = FALSE;
  unsigned short negative = FALSE;

  if (fill == 0)
  {
    fill = ' ';
  }

  if (width < 0)
  {
   width = -width;
   left = TRUE;
  }

  if (width < 0 || width > 80)
  {
    width = 0;
  }

  if (radix < 0)
  {
    radix = -radix;
    if (value < 0)
    {
      negative = TRUE;
      value = -value;
    }
  }

  uvalue = value;

  do
  {
    if (radix != 16)
    {
      digit = uvalue % radix;
      uvalue = uvalue / radix;
    }
    else
    {
      digit = uvalue & 0xf;
      uvalue = uvalue >> 4;
    }
    buffer[bi] = digit + ((digit <= 9) ? '0' : ('A' - 10));
    bi++;
  }
  while (uvalue != 0);

  if (negative)
  {
    buffer[bi] = '-';
    bi += 1;
  }

  if (width <= bi)
  {
    PutStringReverse(buffer, bi);
  }
  else
  {
    width -= bi;
    if (!left)
    {
      PutRepChar(fill, width);
    }

    PutStringReverse(buffer, bi);

    if (left)
    {
      PutRepChar(fill, width);
    }
  }
}
