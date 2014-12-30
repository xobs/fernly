/*	$OpenBSD: strcasecmp.c,v 1.6 2005/08/08 08:05:37 espie Exp $	*/

/*
 * Copyright (c) 1987, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * These functions were taken from libbionic.  They replace GNU functions
 * that weren't getting correctly linked.
 */
#include <stdint.h>
#include <string.h>
#include "bionic.h"
#include "printf.h"
typedef unsigned char u_char;

/*
 * This array is designed for mapping upper and lower case letter
 * together for a case independent comparison.  The mappings are
 * based upon ascii character sequences.
 */
static const u_char charmap[] = {
        '\000', '\001', '\002', '\003', '\004', '\005', '\006', '\007',
        '\010', '\011', '\012', '\013', '\014', '\015', '\016', '\017',
        '\020', '\021', '\022', '\023', '\024', '\025', '\026', '\027',
        '\030', '\031', '\032', '\033', '\034', '\035', '\036', '\037',
        '\040', '\041', '\042', '\043', '\044', '\045', '\046', '\047',
        '\050', '\051', '\052', '\053', '\054', '\055', '\056', '\057',
        '\060', '\061', '\062', '\063', '\064', '\065', '\066', '\067',
        '\070', '\071', '\072', '\073', '\074', '\075', '\076', '\077',
        '\100', '\141', '\142', '\143', '\144', '\145', '\146', '\147',
        '\150', '\151', '\152', '\153', '\154', '\155', '\156', '\157',
        '\160', '\161', '\162', '\163', '\164', '\165', '\166', '\167',
        '\170', '\171', '\172', '\133', '\134', '\135', '\136', '\137',
        '\140', '\141', '\142', '\143', '\144', '\145', '\146', '\147',
        '\150', '\151', '\152', '\153', '\154', '\155', '\156', '\157',
        '\160', '\161', '\162', '\163', '\164', '\165', '\166', '\167',
        '\170', '\171', '\172', '\173', '\174', '\175', '\176', '\177',
        '\200', '\201', '\202', '\203', '\204', '\205', '\206', '\207',
        '\210', '\211', '\212', '\213', '\214', '\215', '\216', '\217',
        '\220', '\221', '\222', '\223', '\224', '\225', '\226', '\227',
        '\230', '\231', '\232', '\233', '\234', '\235', '\236', '\237',
        '\240', '\241', '\242', '\243', '\244', '\245', '\246', '\247',
        '\250', '\251', '\252', '\253', '\254', '\255', '\256', '\257',
        '\260', '\261', '\262', '\263', '\264', '\265', '\266', '\267',
        '\270', '\271', '\272', '\273', '\274', '\275', '\276', '\277',
        '\300', '\301', '\302', '\303', '\304', '\305', '\306', '\307',
        '\310', '\311', '\312', '\313', '\314', '\315', '\316', '\317',
        '\320', '\321', '\322', '\323', '\324', '\325', '\326', '\327',
        '\330', '\331', '\332', '\333', '\334', '\335', '\336', '\337',
        '\340', '\341', '\342', '\343', '\344', '\345', '\346', '\347',
        '\350', '\351', '\352', '\353', '\354', '\355', '\356', '\357',
        '\360', '\361', '\362', '\363', '\364', '\365', '\366', '\367',
        '\370', '\371', '\372', '\373', '\374', '\375', '\376', '\377',
};

/*
 * Span the string s2 (skip characters that are in s2).
 */
size_t _strspn(const char *s1, const char *s2)
{
        const char *p = s1, *spanp;
        char c, sc;

        /*
         * Skip any characters in s2, excluding the terminating \0.
         */
cont:
        c = *p++;
        for (spanp = s2; (sc = *spanp++) != 0;)
                if (sc == c)
                        goto cont;
        return (p - 1 - s1);
}


/*
 * Find the first occurrence in s1 of a character in s2 (excluding NUL).
 */
char * _strpbrk(const char *s1, const char *s2)
{
        const char *scanp;
        int c, sc;

        while ((c = *s1++) != 0) {
                for (scanp = s2; (sc = *scanp++) != 0;)
                        if (sc == c)
                                return ((char *)(s1 - 1));
        }
        return (NULL);
}

char *_strtok(char *str, const char *delim, char **saveptr) {
  char *token;
  if (str)
    *saveptr = str;
  token = *saveptr;

  if (!token)
    return NULL;

  token += _strspn(token, delim);
  *saveptr = _strpbrk(token, delim);
  if (*saveptr)
    *(*saveptr)++ = '\0';

  return *token ? token : NULL;
}


int _strcasecmp(const char *s1, const char *s2)
{
        const u_char *cm = charmap;
        const u_char *us1 = (const u_char *)s1;
        const u_char *us2 = (const u_char *)s2;

        while (cm[*us1] == cm[*us2++])
                if (*us1++ == '\0')
                        return (0);
        return (cm[*us1] - cm[*--us2]);
}

/*
 * sizeof(word) MUST BE A POWER OF TWO
 * SO THAT wmask BELOW IS ALL ONES
 */
typedef	long word;		/* "word" used for optimal copy speed */

#define	wsize	sizeof(word)
#define	wmask	(wsize - 1)

#define MEMCOPY
/*
 * Copy a block of memory, handling overlap.
 * This is the routine that actually implements
 * (the portable versions of) bcopy, memcpy, and memmove.
 */
#ifdef MEMCOPY
void *
_memcpy(void *dst0, const void *src0, size_t length)
#else
#ifdef MEMMOVE
void *
_memmove(void *dst0, const void *src0, size_t length)
#else
void
_bcopy(const void *src0, void *dst0, size_t length)
#endif
#endif
{
	char *dst = dst0;
	const char *src = src0;
	size_t t;

	if (length == 0 || dst == src)		/* nothing to do */
		goto done;

	/*
	 * Macros: loop-t-times; and loop-t-times, t>0
	 */
#define	TLOOP(s) if (t) TLOOP1(s)
#define	TLOOP1(s) do { s; } while (--t)

	if ((unsigned long)dst < (unsigned long)src) {
		/*
		 * Copy forward.
		 */
		t = (long)src;	/* only need low bits */
		if ((t | (long)dst) & wmask) {
			/*
			 * Try to align operands.  This cannot be done
			 * unless the low bits match.
			 */
			if ((t ^ (long)dst) & wmask || length < wsize)
				t = length;
			else
				t = wsize - (t & wmask);
			length -= t;
			TLOOP1(*dst++ = *src++);
		}
		/*
		 * Copy whole words, then mop up any trailing bytes.
		 */
		t = length / wsize;
		TLOOP(*(word *)dst = *(word *)src; src += wsize; dst += wsize);
		t = length & wmask;
		TLOOP(*dst++ = *src++);
	} else {
		/*
		 * Copy backwards.  Otherwise essentially the same.
		 * Alignment works as before, except that it takes
		 * (t&wmask) bytes to align, not wsize-(t&wmask).
		 */
		src += length;
		dst += length;
		t = (long)src;
		if ((t | (long)dst) & wmask) {
			if ((t ^ (long)dst) & wmask || length <= wsize)
				t = length;
			else
				t &= wmask;
			length -= t;
			TLOOP1(*--dst = *--src);
		}
		t = length / wsize;
		TLOOP(src -= wsize; dst -= wsize; *(word *)dst = *(word *)src);
		t = length & wmask;
		TLOOP(*--dst = *--src);
	}
done:
#if defined(MEMCOPY) || defined(MEMMOVE)
	return (dst0);
#else
	return;
#endif
}

int _isspace(char c)
{
	return (c == ' '
		|| c == '\f'
		|| c == '\n'
		|| c == '\r'
		|| c == '\t'
		|| c == '\v');
}

int _isdigit(char c)
{
	return (c >= '0' && c <= '9');
}

int _isxdigit(char c)
{
	return ((c >= '0' && c <= '9') ||
		(c >= 'a' && c <= 'f') ||
		(c >= 'A' && c <= 'F'));
}

int _isupper(char c)
{
	return (c >= 'A' && c <= 'Z');
}

int _islower(char c)
{
	return (c >= 'a' && c <= 'z');
}

int _isalpha(char c)
{
	return _isupper(c) || _islower(c);
}

int _isalnum(char c)
{
	return _isalpha(c) || _isdigit(c);
}

int _toupper(char c)
{
	if (!_islower(c))
		return c;
	return c + ('a' - 'A');
}

void *memset (void *dst0, int val, size_t length)
{
	uint8_t *ptr = dst0;
	while(length--)
		*ptr++ = val;
	return dst0;
}

void __aeabi_memset(void *dst0, char val, size_t length)
{
	memset(dst0, val, length);
}

int _strlen(const char *s)
{
	int i = 0;
	while(s[i++]);
	return i;
}

int _strnlen(const char *s, uint32_t maxlen)
{
	int i = 0;
	while(s[i++] && i < maxlen);
	return i;
}

void _usleep(uint32_t usecs)
{
	uint32_t i, j;
	for (i = 0; i < usecs; i++) {
		for (j = 0; j < 73; j++) {
			asm("nop");
		}
	}
}

void _msleep(uint32_t msecs)
{
	uint32_t i, j;
	for (i = 0; i < msecs; i++) {
		for (j = 0; j < 73000; j++) {
			asm("nop");
		}
	}
}

int puts(const char *str)
{
	printf("%s", str);
	return 0;
}

int putchar(int c)
{
	printf("%c", c);
	return 0;
} 
