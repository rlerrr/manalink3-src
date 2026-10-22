#include "manalink.h"

#ifndef SHANDALAR
/* TODO(recovery-ui): Built-in Wish/card-picker dialogs are OUT OF SCOPE for
 * decompilation for now, by user request. These boundaries return no selection.
 * They are stubs, not recovered native implementations. Restore the UI before
 * relying on interactive Wish/card-list selection. */

/* Stub for native dialog entry 0x26ef820. */
extern "C" int choose_a_card_test_ct(char *prompt, test_definition_2022_t *constraints,
                                     color_test_t colors, type_t types)
{
  (void)prompt;
  (void)constraints;
  (void)colors;
  (void)types;
  return -1;
}

/* Stub for native dialog entry 0x26ef860. */
extern "C" int choose_a_card_test_ct_allow_cancel(char *prompt, test_definition_2022_t *constraints,
                                                  color_test_t colors, type_t types)
{
  (void)prompt;
  (void)constraints;
  (void)colors;
  (void)types;
  return -1;
}

#endif

