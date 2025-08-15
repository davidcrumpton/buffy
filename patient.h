/*
 * BSD Zero Clause License
 *
 * Copyright (c) 2025 David M Crumpton david.m.crumpton [at] gmail [dot] com
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR
 * IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * patient.h: patient management system
 *
 */


#ifndef PATIENT_H
#define PATIENT_H

#include "sys/types.h"
#include "playerio.h"

char *patient_reaction(const int *effort, int *patience, int *pain_tolerance, const int *fang_health, 
const int *tool_pain_factor, const char *patient_name, int *mood, int *pat_level);

// Track last mood per patient (or globally)
static int last_mood = -1;
static int last_turn = -1;

#define MOOD_HAPPY      0
#define MOOD_UNHAPPY    1
#define MOOD_ANGRY      2

#define PATIENCE_IMPATIENT    0
#define PATIENCE_CALM         1
#define PATIENCE_BLISS        2
    
#endif /* PATIENT_H */