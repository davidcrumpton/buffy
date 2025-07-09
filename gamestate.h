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
#ifndef GAMESTATE_H
#define GAMESTATE_H

#define MAJOR 1
#define MINOR 0
#define PATCH 0

#define GAMECODE 0x62746664	/* buffy the flouride dispenser */

void
load_game_state(const char *load_path, game_state_type * gamestate_g, size_t gs_len,
	    creature_type * patient_g, size_t plen, char *character_name_g);
int		save_game_state(const char *save_path, const game_state_type * gamestate, size_t gs_len, const creature_type * patient, size_t plen);
int		validate_game_file(const char *file);

#endif				/* GAMESTATE_H */
