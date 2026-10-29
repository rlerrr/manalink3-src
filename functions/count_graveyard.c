#include "manalink.h"

/* Canonical recovered helper declarations. */
int new_special_count_grave(int player, test_definition_t *this_test);

int count_graveyard_by_color(int player, int selected_color)
{
  unsigned int timestamp = 0;
  int changer = -1, hack = 0;
  for (int p = 0; p < 2; ++p)
    for (int c = 0; c < active_cards_count[p]; ++c)
      if (in_play(p, c))
      {
        if (get_id(p, c) == CARD_ID_PAINTERS_SERVANT)
        {
          card_instance_t *instance = get_card_instance(p, c);
          if (instance->targets[1].card >= 0 && instance->timestamp > timestamp)
          {
            hack |= instance->targets[1].card;
            changer = CARD_ID_PAINTERS_SERVANT;
            timestamp = instance->timestamp;
          }
        }
        if (p == player && get_id(player, c) == CARD_ID_CELESTIAL_DAWN)
        {
          card_instance_t *instance = get_card_instance(player, c);
          unsigned int next_timestamp = instance->timestamp;
          if (next_timestamp > timestamp)
          {
            hack = get_sleighted_color_test(player, c, COLOR_TEST_WHITE);
            changer = CARD_ID_CELESTIAL_DAWN;
            timestamp = next_timestamp;
          }
        }
        if (get_id(p, c) == CARD_ID_MYCOSYNTH_LATTICE)
        {
          card_instance_t *instance = get_card_instance(p, c);
          if (instance->timestamp > timestamp)
          {
            hack = COLOR_TEST_COLORLESS;
            changer = CARD_ID_MYCOSYNTH_LATTICE;
            timestamp = instance->timestamp;
          }
        }
      }
  const int *grave = get_grave(player);
  int result = 0;
  for (int i = 0; grave[i] != -1; ++i)
  {
    if (changer == CARD_ID_CELESTIAL_DAWN)
    {
      if (!is_what(-1, grave[i], TYPE_LAND) && (selected_color & hack))
        ++result;
    }
    else if ((changer == CARD_ID_PAINTERS_SERVANT || changer == CARD_ID_MYCOSYNTH_LATTICE) &&
             (selected_color & hack))
      return count_graveyard(player);
    // Native has no fallback that counts printed colors.
  }
  return result;
}


int count_graveyard_by_id(int player, int id){
	int i;
	int result = 0;
	for(i=0; i<2; i++){
		if( i == player || player == 2 ){
			const int *grave = get_grave(i);
			int count = 0;
			while( grave[count] != -1 ){
					if( cards_data[ grave[count] ].id == id ){
						result++;
					}
					count++;
			}
		}
	}
	return result;
}

// Count the number of cards in a zone
int count_zone(int player, const int* zone){
  int i;

  for (i = 0; i < 500; i++)
	if (zone[i] == -1)
	  return i;

  return 500;
}

int count_deck(int player){
	return count_zone(player, deck_ptr[player]);
}

int count_graveyard(int player){
	int result = 0;
	int i;
	for(i=0; i<2; i++){
		if( i == player || player == 2 ){
			result += count_zone(i, get_grave(i));
		}
	}
	return result;
}

int count_rfg(int player){
	return count_zone(player, rfg_ptr[player]);
}

void purge_rfg(int player){
	int *rfg = rfg_ptr[player];
	rfg[count_rfg(player) - 1] = -1;
}

/* Native zone counting has two physical arguments; the unused player predecessor is retired. */
static int count_zone_by_type(int type, const int *zone);

int count_graveyard_by_type(int player, int type){
	int i;
	int result = 0;
	for(i=0; i<2; i++){
		if( i == player || player == 2 ){
			result += count_zone_by_type(type, get_grave(i));
		}
	}
	return result;
}

// Equivalent to count_graveyard_by_type(player, type) > 0, but stops when it finds the first qualifying card.
int any_in_graveyard_by_type(int player, int type)
{
  int p, i;
  for (p = 0; p <= 1; ++p)
	if (p == player || player == ANYBODY)
	  for (i = 0; i < 500 && get_grave(p)[i] != -1; ++i)
		if (is_what(-1, get_grave(p)[i], type))
		  return 1;
  return 0;
}

int count_graveyard_by_subtype(int player, subtype_t subtype)
{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;

  iVar2 = 0;
  iVar4 = 0;
  do
  {
    if ((player == iVar4) || (player == 2))
    {
      iVar3 = 0;
      do
      {
        if (*(int *)((int)graveyard_ptr_mutable[iVar4] + iVar3) == -1)
          break;
        iVar1 = has_subtype_by_id(
            player,
            (unsigned int)cards_data[*(int *)((int)graveyard_ptr_mutable[iVar4] + iVar3)].id,
            subtype);
        iVar2 = (iVar2 + 1) - (unsigned int)(iVar1 == 0);
        iVar3 = iVar3 + 4;
      } while (iVar3 != 2000);
    }
    iVar4 = iVar4 + 1;
    if (iVar4 == 2)
    {
      return iVar2;
    }
  } while (true);
}

int any_in_graveyard_by_subtype(int player, subtype_t subtype)
{
  int iVar1;
  int iVar2;
  int iVar3;

  iVar2 = 0;
  do
  {
    if ((player == iVar2) || (player == 2))
    {
      iVar3 = 0;
      do
      {
        if (*(int *)((int)graveyard_ptr_mutable[iVar2] + iVar3) == -1)
          break;
        iVar1 = has_subtype_by_id(
            player,
            (unsigned int)cards_data[*(int *)((int)graveyard_ptr_mutable[iVar2] + iVar3)].id,
            subtype);
        if (iVar1 != 0)
        {
          return 1;
        }
        iVar3 = iVar3 + 4;
      } while (iVar3 != 2000);
    }
    iVar2 = iVar2 + 1;
    if (iVar2 == 2)
    {
      return 0;
    }
  } while (true);
}

int count_deck_by_type(int player, int type){
	return count_zone_by_type(type, deck_ptr[player]);
}

int new_special_count_grave(int player, test_definition_t *this_test)
{
  int test_type;
  int iVar1;
  int *piVar2;
  int iVar3;
  int iStack_14;

  test_type = new_get_test_score(this_test);
  iStack_14 = 0;
  iVar3 = 0;
  do
  {
    if ((player == iVar3) || (player == 2))
    {
      piVar2 = graveyard_ptr_mutable[iVar3];
      iVar1 = *piVar2;
      while (iVar1 != -1)
      {
        piVar2 = piVar2 + 1;
        iVar1 = new_make_test(player, iVar1, test_type, this_test);
        iStack_14 = (iStack_14 + 1) - (unsigned int)(iVar1 == 0);
        iVar1 = *piVar2;
      }
    }
    iVar3 = iVar3 + 1;
  } while (iVar3 != 2);
  return iStack_14;
}

int special_count_grave(int player, int type, int flag1, int subtype, int flag2, int color, int flag3, int id, int flag4, int cc, int flag5){

	int count = count_graveyard(player)-1;
	int result = 0;
	const int *grave = get_grave(player);
	int test_type = get_test_score(type, flag1, subtype, flag2, color, flag3, id, flag4, cc, flag5);
	while( count > - 1 ){
			if( make_test(player, grave[count], test_type, type, flag1, subtype, flag2, color, flag3, id,
						  flag4, cc, flag5)
			  ){
				result++;
			}
			count--;
	}
	return result;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x26e6370; original functions/count_graveyard.c:211. */
int special_count_zone(int player, int *zone, test_definition_t *this_test)
{
  int *piVar1;
  int test_type;
  int iVar2;
  int iVar3;

  iVar3 = 0;
  test_type = new_get_test_score(this_test);
  piVar1 = zone + 500;
  do
  {
    if (*zone == -1)
    {
      return iVar3;
    }
    iVar2 = new_make_test(player, *zone, test_type, this_test);
    iVar3 = (iVar3 + 1) - (unsigned int)(iVar2 == 0);
    zone = zone + 1;
  } while (zone != piVar1);
  return iVar3;
}

/* ManalinkEh 2022: 0x26e5c70; original functions/count_graveyard.c:119. */
/* Concrete compiler clone: only EAX/EDX are ABI arguments. */
static int count_zone_by_type(int type, const int *zone)
{
  const int *piVar1;
  int iVar2;
  int iVar3;

  piVar1 = zone + 500;
  iVar3 = 0;
  do
  {
    if (*zone == -1)
    {
      return iVar3;
    }
    iVar2 = is_what(-1, *zone, type);
    iVar3 = (iVar3 + 1) - (unsigned int)(iVar2 == 0);
    zone = zone + 1;
  } while (zone != piVar1);
  return iVar3;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
int count_rfg_by_type(int player, int type);

/* ManalinkEh 2022: 0x26e60e0; original functions/count_graveyard.c:143. */
int count_rfg_by_type(int player, int type)
{
  int iVar1;
  int iVar2;

  iVar2 = 0;
  if ((player & 0xfffffffdU) == 0)
  {
    iVar2 = count_zone_by_type(type, rfg_ptr[player]);
  }
  if (player - 1U < 2)
  {
    iVar1 = count_zone_by_type(type, rfg_ptr[player]);
    return iVar2 + iVar1;
  }
  return iVar2;
}


#ifndef SHANDALAR
/* Recovered October 2022 typed consumers and compiler parts. */

/* ManalinkEh 2022: 0x26e62e0; original functions/count_graveyard.c:192. */


#endif
