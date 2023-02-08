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
#include <stdarg.h>
#include <string.h>
#include "printf.h"
#include "stm32f4xx.h"

#define WRAPPER_FUNC(x) __wrap_ ## x
#define REAL_FUNC(x) __real_ ## x

void send_char(char c)
{
    (void)c;
    __asm volatile("svc 55":::);
}

uint32_t check_received_char()
{
    uint32_t ret;
    __asm volatile("svc 56": "=r" (ret) ::);
    return ret;
}

char receive_char()
{
    char ret;
    __asm volatile("svc 57": "=r" (ret) ::);
    return ret;
}

int WRAPPER_FUNC(putchar)(int c) {
    send_char(c);
    return c;
}

int WRAPPER_FUNC(puts)(const char *s) {
    int len = (int)strlen(s);
    for (int i = 0; i < len; i++) {
        send_char(s[i]);
    }
    send_char('\n');
    return len;
}

int WRAPPER_FUNC(vprintf)(const char *format, va_list va) {
    return vprintf_(format, va);
}

int __printflike(1, 0) WRAPPER_FUNC(printf)(const char* format, ...)
{
    va_list va;
    va_start(va, format);
    int ret = vprintf(format, va);
    va_end(va);
    return ret;
}

int WRAPPER_FUNC(getchar)(void) 
{
    /* wait for a char */
    while (!check_received_char()) {}
    return receive_char();
}
