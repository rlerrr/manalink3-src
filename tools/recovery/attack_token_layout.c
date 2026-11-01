/* Native offsets independently read from the complete October 2022 card bodies.
 * Compile with GCC -m32 -fsyntax-only; this does not build a gameplay test. */
#include <stddef.h>
#include "manalink.h"

_Static_assert(offsetof(card_instance_t, ext) == 0x2c, "native damage extension pointer");
_Static_assert(offsetof(card_ext_t, sp_keywords) == 0, "native extension keyword word");
_Static_assert(offsetof(card_instance_t, info_slot) == 0x38, "native attacker count");
_Static_assert(offsetof(card_instance_t, targets[2].player) == 0x84, "native packed attacker bytes");
_Static_assert(sizeof(((card_instance_t*)0)->info_slot) == 4, "native attacker count width");
_Static_assert(offsetof(token_generation_t, s_key_plus) == 0x24, "native token special keywords");
_Static_assert(offsetof(token_generation_t, color_forced) == 0x28, "native token color");
_Static_assert(offsetof(token_generation_t, qty) == 0x2b, "native token quantity");
_Static_assert(sizeof(((token_generation_t*)0)->qty) == 1, "native token quantity width");
_Static_assert(GVC_PLAYER_COUNTERS(PCOUNTER_ENERGY, 8) == 0x02020800, "native energy cost descriptor");
