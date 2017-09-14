/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef TEST_H
#define TEST_H

#define FIXMATH_NO_OVERFLOW 1
#define FIXMATH_NO_ROUNDING 1

#include <yaul.h>
#include <tga.h>

#include "globals.h"
#include "fs.h"
#include "state_machine.h"

#define TEST_PROTOTYPE_DECLARE(name, event)                                    \
    void                                                                       \
    __CONCAT(__CONCAT(test_, name),                                            \
        __CONCAT(_,__CONCAT(event, )))(void)

#define MATH_SIN(x)     (lut_sin[(x) & 0x1FF])
#define MATH_COS(x)     (lut_cos[(x) & 0x1FF])

void test_init(void);
void test_exit(void);

extern int32_t lut_cos[512];
extern int32_t lut_sin[512];

extern char *text;
extern size_t text_len;

#endif /* !TEST_H */
