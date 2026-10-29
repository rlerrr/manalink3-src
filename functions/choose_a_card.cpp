#include "manalink.h"

#ifndef SHANDALAR
/* TODO(recovery-ui): Built-in Wish/card-picker dialogs are OUT OF SCOPE for
 * decompilation for now, by user request. These boundaries return no selection.
 * They are stubs, not recovered native implementations. Restore the UI before
 * relying on interactive Wish/card-list selection. */

/* Stub for native dialog entry 0x26ef820. */
extern "C" int choose_a_card_test_ct(char *prompt, test_definition_t *constraints,
                                     color_test_t colors, type_t types)
{
  (void)prompt;
  (void)constraints;
  (void)colors;
  (void)types;
  return -1;
}

/* Stub for native dialog entry 0x26ef860. */
extern "C" int choose_a_card_test_ct_allow_cancel(char *prompt, test_definition_t *constraints,
                                                  color_test_t colors, type_t types)
{
  (void)prompt;
  (void)constraints;
  (void)colors;
  (void)types;
  return -1;
}

/* TODO(recovery-ui): native 0x26ef7a0. Deferred IID card-picker boundary.
 * This is a stub, not a recovered implementation. */
extern "C" int choose_a_card_iid(char *prompt, color_test_t colors, type_t types)
{
  (void)prompt;
  (void)colors;
  (void)types;
  return -1;
}

/* TODO(recovery-ui): native 0x26ef760. Deferred card-picker boundary.
 * This returns no selection and is not counted as a recovered implementation. */
extern "C" int choose_a_card_allow_cancel_with_defaults(char *prompt, color_test_t allowable_colors,
                                                       type_t allowable_types, color_test_t initial_color,
                                                       type_t initial_type)
{
  (void)prompt;
  (void)allowable_colors;
  (void)allowable_types;
  (void)initial_color;
  (void)initial_type;
  return -1;
}

/* TODO(recovery-ui): native 0x26ef8a0. Deferred card-picker state query.
 * Keep caller defaults while the built-in picker is stubbed. No recovery credit. */
extern "C" void choose_a_card_get_last_options(color_test_t *cols, type_t *typs)
{
  (void)cols;
  (void)typs;
}

#endif

