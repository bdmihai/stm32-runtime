/*_____________________________________________________________________________
 │                                                                            |
 │ COPYRIGHT (C) 2023 Mihai Baneu                                             |
 │                                                                            |
 | Permission is hereby  granted,  free of charge,  to any person obtaining a |
 | copy of this software and associated documentation files (the "Software"), |
 | to deal in the Software without restriction,  including without limitation |
 | the rights to  use, copy, modify, merge, publish, distribute,  sublicense, |
 | and/or sell copies  of  the Software, and to permit  persons to  whom  the |
 | Software is furnished to do so, subject to the following conditions:       |
 |                                                                            |
 | The above  copyright notice  and this permission notice  shall be included |
 | in all copies or substantial portions of the Software.                     |
 |                                                                            |
 | THE SOFTWARE IS PROVIDED  "AS IS",  WITHOUT WARRANTY OF ANY KIND,  EXPRESS |
 | OR   IMPLIED,   INCLUDING   BUT   NOT   LIMITED   TO   THE  WARRANTIES  OF |
 | MERCHANTABILITY,  FITNESS FOR  A  PARTICULAR  PURPOSE AND NONINFRINGEMENT. |
 | IN NO  EVENT SHALL  THE AUTHORS  OR  COPYRIGHT  HOLDERS  BE LIABLE FOR ANY |
 | CLAIM, DAMAGES OR OTHER LIABILITY,  WHETHER IN AN ACTION OF CONTRACT, TORT |
 | OR OTHERWISE, ARISING FROM,  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR  |
 | THE USE OR OTHER DEALINGS IN THE SOFTWARE.                                 |
 |____________________________________________________________________________|
 |                                                                            |
 |  Author: Mihai Baneu                           Last modified: 05.Feb.2023  |
 |                                                                            |
 |___________________________________________________________________________*/

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

extern void *__real_malloc(size_t size);
extern void *__real_calloc(size_t count, size_t size);
extern void *__real_realloc(void *mem, size_t size);
extern void __real_free(void *mem);
extern void panic(__unused const char *fmt, ...);

extern char __heap_end; /* Set by linker.  */

static inline void check_alloc(__unused void *mem, __unused uint32_t size) 
{
    if (!mem || (((char *)mem) + size) > &__heap_end) {
        panic("Out of memory");
    }
}

void *__wrap_malloc(size_t size) 
{
    void *rc = __real_malloc(size);
    check_alloc(rc, size);
    return rc;
}

void *__wrap_calloc(size_t count, size_t size) 
{
    void *rc = __real_calloc(count, size);
    check_alloc(rc, size);
    return rc;
}

void *__wrap_realloc(void *mem, size_t size) 
{
    void *rc = __real_realloc(mem, size);
    check_alloc(rc, size);
    return rc;
}

void __wrap_free(void *mem) 
{
    __real_free(mem);
}
