/* NetHack 3.7	alloc.c	$NHDT-Date: 1706213795 2024/01/25 20:16:35 $  $NHDT-Branch: NetHack-3.7 $:$NHDT-Revision: 1.34 $ */
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/*-Copyright (c) Robert Patrick Rankin, 2012. */
/* NetHack may be freely redistributed.  See license for details. */

#define ALLOC_C /* comment line for pre-compiled headers */
/* since this file is also used in auxiliary programs, don't include all the
   function declarations for all of nethack */
#define EXTERN_H /* comment line for pre-compiled headers */

#include "config.h"
#ifndef LUA_INTEGER
#include "nhlua.h"
#endif


/*#define FITSint(x) FITSint_(x, __func__, __LINE__)*/
extern int FITSint_(LUA_INTEGER, const char *, int) NONNULLARG2;
/*#define FITSuint(x) FITSuint_(x, __func__, __LINE__)*/
extern unsigned FITSuint_(unsigned long long, const char *, int) NONNULLARG2;

char *fmt_ptr(const genericptr) NONNULL;

void *alloc(unsigned int) NONNULL;
void *re_alloc(void *, unsigned int) NONNULL;
ATTRNORETURN extern void panic(const char *, ...) PRINTF_F(1, 2) NORETURN;

void *
alloc(unsigned int bytes)
{
    void *p;

    if (!bytes)
        panic("Zero sized allocs not allowed");

    p = malloc(bytes);
    if (!p)
        panic("Memory allocation failure; cannot get %u bytes", bytes);

    return p;
}

/* realloc() call that might get substituted by nhrealloc(p,n,file,line) */
void *
re_alloc(void *old, unsigned int newlth)
{
    void *p;

    if (!newlth)
        panic("Zero sized reallocs not allowed");

    p = realloc(old, (size_t) newlth);
    /* "extend to":  assume it won't ever fail if asked to shrink */
    if (!p)
        panic("Memory allocation failure; cannot extend to %u bytes", newlth);

    return p;
}

#ifdef HAS_PTR_FMT
#define PTR_FMT "%p"
#define PTR_TYP genericptr_t
#else
#define PTR_FMT "%06lx"
#define PTR_TYP unsigned long
#endif

/* A small pool of static formatting buffers.
 * PTRBUFSIZ:  We assume that pointers will be formatted as integers in
 * hexadecimal, requiring at least 16+1 characters for each buffer to handle
 * 64-bit systems, but the standard doesn't mandate that encoding and an
 * implementation could do something different for %p, so we make some
 * extra room.
 * PTRBUFCNT:  Number of formatted values which can be in use at the same
 * time.  To have more, callers need to make copies of them as they go.
 */
#define PTRBUFCNT 4
#define PTRBUFSIZ 32
static char ptrbuf[PTRBUFCNT][PTRBUFSIZ];
static int ptrbufidx = 0;

/* format a pointer for display purposes; returns a static buffer */
char *
fmt_ptr(const genericptr ptr)
{
    char *buf;

    buf = ptrbuf[ptrbufidx];
    if (++ptrbufidx >= PTRBUFCNT)
        ptrbufidx = 0;

    Sprintf(buf, PTR_FMT, (PTR_TYP) ptr);
    return buf;
}

/* strdup() which uses our alloc() rather than libc's malloc(); */
char *
dupstr(const char *string)
{
    unsigned len = FITSuint_(strlen(string), __func__, (int) __LINE__);

    return strcpy((char *) alloc(len + 1), string);
}

/* similar for reasonable size strings, but return length of input as well */
char *
dupstr_n(const char *string, unsigned int *lenout)
{
    size_t len = strlen(string);

    if (len >= LARGEST_INT)
        panic("string too long");
    *lenout = (unsigned int) len;
    return strcpy((char *) alloc(len + 1), string);
}


/* cast to int or panic on overflow; use via macro */
int
FITSint_(LUA_INTEGER i, const char *file, int line)
{
    int iret = (int) i;

    if (iret != i)
        panic("Overflow at %s:%d", file, line);
    return iret;
}

unsigned
FITSuint_(unsigned long long ull, const char *file, int line)
{
    unsigned uret = (unsigned) ull;

    if (uret != ull)
        panic("Overflow at %s:%d", file, line);
    return uret;
}

/*alloc.c*/
