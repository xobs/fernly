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

static int get_cuts(int base, unsigned long *cutoff, int *cutlim)
{
	switch(base) {
	case 1:
		*cutoff = 4294967295;
		*cutlim = 0;
		break;

	case 2:
		*cutoff = 2147483647;
		*cutlim = 1;
		break;

	case 3:
		*cutoff = 1431655765;
		*cutlim = 0;
		break;

	case 4:
		*cutoff = 1073741823;
		*cutlim = 3;
		break;

	case 5:
		*cutoff = 858993459;
		*cutlim = 0;
		break;

	case 6:
		*cutoff = 715827882;
		*cutlim = 3;
		break;

	case 7:
		*cutoff = 613566756;
		*cutlim = 3;
		break;

	case 8:
		*cutoff = 536870911;
		*cutlim = 7;
		break;

	case 9:
		*cutoff = 477218588;
		*cutlim = 3;
		break;

	case 10:
		*cutoff = 429496729;
		*cutlim = 5;
		break;

	case 11:
		*cutoff = 390451572;
		*cutlim = 3;
		break;

	case 12:
		*cutoff = 357913941;
		*cutlim = 3;
		break;

	case 13:
		*cutoff = 330382099;
		*cutlim = 8;
		break;

	case 14:
		*cutoff = 306783378;
		*cutlim = 3;
		break;

	case 15:
		*cutoff = 286331153;
		*cutlim = 0;
		break;

	case 16:
		*cutoff = 268435455;
		*cutlim = 15;
		break;

	case 17:
		*cutoff = 252645135;
		*cutlim = 0;
		break;

	case 18:
		*cutoff = 238609294;
		*cutlim = 3;
		break;

	case 19:
		*cutoff = 226050910;
		*cutlim = 5;
		break;

	case 20:
		*cutoff = 214748364;
		*cutlim = 15;
		break;

	case 21:
		*cutoff = 204522252;
		*cutlim = 3;
		break;

	case 22:
		*cutoff = 195225786;
		*cutlim = 3;
		break;

	case 23:
		*cutoff = 186737708;
		*cutlim = 11;
		break;

	case 24:
		*cutoff = 178956970;
		*cutlim = 15;
		break;

	case 25:
		*cutoff = 171798691;
		*cutlim = 20;
		break;

	case 26:
		*cutoff = 165191049;
		*cutlim = 21;
		break;

	case 27:
		*cutoff = 159072862;
		*cutlim = 21;
		break;

	case 28:
		*cutoff = 153391689;
		*cutlim = 3;
		break;

	case 29:
		*cutoff = 148102320;
		*cutlim = 15;
		break;

	case 30:
		*cutoff = 143165576;
		*cutlim = 15;
		break;

	case 31:
		*cutoff = 138547332;
		*cutlim = 3;
		break;

	case 32:
		*cutoff = 134217727;
		*cutlim = 31;
		break;

	case 33:
		*cutoff = 130150524;
		*cutlim = 3;
		break;

	case 34:
		*cutoff = 126322567;
		*cutlim = 17;
		break;

	case 35:
		*cutoff = 122713351;
		*cutlim = 10;
		break;

	case 36:
		*cutoff = 119304647;
		*cutlim = 3;
		break;

	case 37:
		*cutoff = 116080197;
		*cutlim = 6;
		break;

	case 38:
		*cutoff = 113025455;
		*cutlim = 5;
		break;

	case 39:
		*cutoff = 110127366;
		*cutlim = 21;
		break;

	case 40:
		*cutoff = 107374182;
		*cutlim = 15;
		break;

	case 41:
		*cutoff = 104755299;
		*cutlim = 36;
		break;

	case 42:
		*cutoff = 102261126;
		*cutlim = 3;
		break;

	case 43:
		*cutoff = 99882960;
		*cutlim = 15;
		break;

	case 44:
		*cutoff = 97612893;
		*cutlim = 3;
		break;

	case 45:
		*cutoff = 95443717;
		*cutlim = 30;
		break;

	case 46:
		*cutoff = 93368854;
		*cutlim = 11;
		break;

	case 47:
		*cutoff = 91382282;
		*cutlim = 41;
		break;

	case 48:
		*cutoff = 89478485;
		*cutlim = 15;
		break;

	case 49:
		*cutoff = 87652393;
		*cutlim = 38;
		break;

	case 50:
		*cutoff = 85899345;
		*cutlim = 45;
		break;

	case 51:
		*cutoff = 84215045;
		*cutlim = 0;
		break;

	case 52:
		*cutoff = 82595524;
		*cutlim = 47;
		break;

	case 53:
		*cutoff = 81037118;
		*cutlim = 41;
		break;

	case 54:
		*cutoff = 79536431;
		*cutlim = 21;
		break;

	case 55:
		*cutoff = 78090314;
		*cutlim = 25;
		break;

	case 56:
		*cutoff = 76695844;
		*cutlim = 31;
		break;

	case 57:
		*cutoff = 75350303;
		*cutlim = 24;
		break;

	case 58:
		*cutoff = 74051160;
		*cutlim = 15;
		break;

	case 59:
		*cutoff = 72796055;
		*cutlim = 50;
		break;

	case 60:
		*cutoff = 71582788;
		*cutlim = 15;
		break;

	case 61:
		*cutoff = 70409299;
		*cutlim = 56;
		break;

	case 62:
		*cutoff = 69273666;
		*cutlim = 3;
		break;

	case 63:
		*cutoff = 68174084;
		*cutlim = 3;
		break;

	case 64:
		*cutoff = 67108863;
		*cutlim = 63;
		break;

	default:
		break;
	}

	return 0;
}

unsigned long _strtoul(const void *nptr, void **endptr, int base)
{
	const char *s;
	unsigned long acc, cutoff;
	int c;
	int neg, any, cutlim;

	/*
	   * See strtol for comments as to the logic used.
	   */
	s = nptr;
	do {
		c = (unsigned char) *s++;
	} while (_isspace(c));
	if (c == '-') {
		neg = 1;
		c = *s++;
	} else {
		neg = 0;
		if (c == '+')
			c = *s++;
	}
	if ((base == 0 || base == 16) &&
			c == '0' && (*s == 'x' || *s == 'X')) {
		c = s[1];
		s += 2;
		base = 16;
	}
	if (base == 0)
		base = c == '0' ? 8 : 10;

	get_cuts(base, &cutoff, &cutlim);
	for (acc = 0, any = 0;; c = (unsigned char) *s++) {
		if (_isdigit(c))
			c -= '0';
		else if (_isalpha(c))
			c -= _isupper(c) ? 'A' - 10 : 'a' - 10;
		else
			break;
		if (c >= base)
			break;
		if (any < 0)
			continue;
		if (acc > cutoff || (acc == cutoff && c > cutlim)) {
			any = -1;
			acc = ULONG_MAX;
			//errno = ERANGE;
		} else {
			any = 1;
			acc *= (unsigned long)base;
			acc += c;
		}
	}
	if (neg && any > 0)
		acc = -acc;
	if (endptr != 0)
		*(char **)endptr = (char *) (any ? s - 1 : nptr);
	return (acc);
}

void _memset(void *dst0, char val, size_t length)
{
	uint8_t *ptr = dst0;
	while(length--)
		*ptr++ = val;
}

void __aeabi_memset(void *dst0, char val, size_t length)
{
	_memset(dst0, val, length);
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
