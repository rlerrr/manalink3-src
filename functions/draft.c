#include "manalink.h"

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2662100; original functions/draft.c:32. */
static void pick_card(int *picks, int *pack, int selected)
{
  int result;
  int *piVar2;
  result = *picks;
  while (-1 < result)
  {
    picks = picks + 1;
    result = *picks;
  }
  *picks = pack[selected];
  result = pack[selected + 1];
  pack[selected] = result;
  piVar2 = pack + selected + 1;
  while (-1 < result)
  {
    result = piVar2[1];
    *piVar2 = result;
    piVar2 = piVar2 + 1;
  }
  return;
}
