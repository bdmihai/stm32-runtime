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

#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>

/**
 * Runtime initialization.
*/
void runtime_init(void) 
{
    // pre-init runs really early since we need it even for memcpy and divide!
    // (basically anything in aeabi that uses bootrom)

    // Start and end points of the constructor list,
    // defined by the linker script.
    extern void (*__preinit_array_start)(void);
    extern void (*__preinit_array_end)(void);

    // Call each function in the list.
    // We have to take the address of the symbols, as __preinit_array_start *is*
    // the first function pointer, not the address of it.
    for (void (**p)(void) = &__preinit_array_start; p < &__preinit_array_end; ++p) {
        (*p)();
    }

    // Start and end points of the constructor list,
    // defined by the linker script.
    extern void (*__init_array_start)(void);
    extern void (*__init_array_end)(void);

    // Call each function in the list.
    // We have to take the address of the symbols, as __init_array_start *is*
    // the first function pointer, not the address of it.
    for (void (**p)(void) = &__init_array_start; p < &__init_array_end; ++p) {
        (*p)();
    }
}

/**
 * The brk() and sbrk() functions are used to change dynamically the amount of space 
 * allocated for the calling process's data segment. The change is made by resetting 
 * the process's break value and allocating the appropriate amount of space. The break 
 * value is the address of the first location beyond the end of the data segment. 
 * The amount of allocated space increases as the break value increases. 
*/
void *_sbrk(int incr) {
    extern char __heap_start;
    extern char __heap_end;

    static char *heap_end;
    char *prev_heap_end;

    if (heap_end == 0)
        heap_end = &__heap_start;

    prev_heap_end = heap_end;
    char *next_heap_end = heap_end + incr;

    if (__builtin_expect(next_heap_end > (&__heap_end), false)) {
        if (heap_end == &__heap_end) {
            return (char *) -1;
        }
        next_heap_end = &__heap_end;
    }

    heap_end = next_heap_end;
    return (void *) prev_heap_end;
}

void __attribute__((noreturn)) _exit(__unused int status) 
{
    while (1) {
        __asm__("bkpt #0");
    }
}

void exit(int status) 
{
    _exit(status);
}

void __attribute__((noreturn)) __printflike(1, 0) panic(__unused const char *fmt, ...) 
{
    puts("\n*** PANIC ***\n");
    if (fmt) {
        va_list args;
        va_start(args, fmt);
        vprintf(fmt, args);
        va_end(args);
        puts("\n");
    }
    _exit(1);
}

void __assert_func(__unused const char *file, __unused int line, __unused const char *func, __unused const char *failedexpr) {
    printf("assertion \"%s\" failed: file \"%s\", line %d%s%s\n",
           failedexpr, file, line, func ? ", function: " : "",
           func ? func : "");

    _exit(1);
}
