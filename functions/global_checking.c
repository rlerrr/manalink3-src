#include "manalink.h"

/* Canonical recovered helper declarations. */
int new_get_test_score(test_definition_2022_t *this_test);
void new_default_test_definition(test_definition_2022_t *this_test, int type, const char *msg);
void default_test_definition(test_definition_2022_t *this_test, int type);
int new_make_test_in_play(int player, int to_examine, int test_type,
                               test_definition_2022_t *this_test);
int new_make_test(int player, int to_examine, int test_type, test_definition_2022_t *this_test);
int make_test_in_play(int player, int to_examine, int test_type, int type, int flag1,
                           int subtype, int flag2, int color, int flag3, int id, int flag4, int cc,
                           int flag5);
int make_test(int player, int to_examine, int test_type, int type, int flag1, int subtype,
                   int flag2, int color, int flag3, int id, int flag4, int cc, int flag5);
int check_battlefield_for_special_card(int player, int card, int t_player, int mode,
                                            test_definition_2022_t *this_test);
int get_test_score(int type, int flag1, int subtype, int flag2, int color, int flag3, int id,
                        int flag4, int cc, int flag5);

int get_test_score(int type, int flag1, int subtype, int flag2, int color, int flag3, int id,
                        int flag4, int cc, int flag5)
{
  unsigned int uVar1;
  unsigned int uVar2;
  bool bVar3;
  bool bVar4;

  bVar3 = type != 0;
  uVar1 = (unsigned int)(flag1 == 0 && bVar3);
  if ((flag2 == 0) && (subtype != 0))
  {
    uVar1 = uVar1 + 2;
  }
  if ((flag3 == 0) && (color != 0))
  {
    uVar1 = uVar1 + 4;
  }
  bVar4 = id == 0;
  if ((flag4 == 0) && (!bVar4))
  {
    uVar1 = uVar1 + 8;
  }
  uVar2 = cc ^ 0xffffffff;
  if ((flag5 == 0) && ((int)uVar2 < 0))
  {
    uVar1 = uVar1 + 0x10;
  }
  if ((flag1 == 1) && (bVar3))
  {
    uVar1 = uVar1 + 0x20;
  }
  if ((0 < flag2) && (subtype != 0))
  {
    uVar1 = uVar1 + 0x40;
  }
  if ((flag3 == 1) && (color != 0))
  {
    uVar1 = uVar1 + 0x80;
  }
  if ((flag4 == 1) && (!bVar4))
  {
    uVar1 = uVar1 + 0x100;
  }
  if ((flag5 == 1) && ((int)uVar2 < 0))
  {
    uVar1 = uVar1 + 0x200;
  }
  else if ((flag5 == 2) && ((int)uVar2 < 0))
  {
    uVar1 = uVar1 + 0x400;
  }
  else if ((flag5 == 3) && ((int)uVar2 < 0))
  {
    uVar1 = uVar1 + 0x800;
  }
  if ((flag1 == 2) && (bVar3))
  {
    uVar1 = uVar1 + 0x1000;
  }
  else if ((flag1 == 3) && (bVar3))
  {
    uVar1 = uVar1 + 0x2000;
  }
  if ((flag4 != 2) || (bVar4))
  {
    if ((flag4 == 3) && (!bVar4))
    {
      uVar1 = uVar1 + 0x8000;
    }
  }
  else
  {
    uVar1 = uVar1 + 0x4000;
  }
  if ((flag1 == 4) && (bVar3))
  {
    uVar1 = uVar1 + 0x10000;
    if (color != 0 || flag3 != 2)
    {
      return uVar1;
    }
  }
  else if (color != 0 || flag3 != 2)
    goto LAB_026be62e;
  uVar1 = uVar1 + 0x20000;
LAB_026be62e:
  if ((flag1 == 5) && (bVar3))
  {
    uVar1 = uVar1 + 0x40000;
  }
  else if ((flag1 == 6) && (bVar3))
  {
    return uVar1 + 0x80000;
  }
  return uVar1;
}

int new_get_test_score(test_definition_2022_t *this_test)
{
  unsigned short uVar1;
  uint16_t uVar2;
  uint16_t uVar3;
  uint16_t uVar4;
  uint16_t uVar5;
  uint16_t uVar6;
  uint16_t uVar7;
  unsigned int uVar8;
  unsigned int uVar9;
  int result;
  unsigned int uVar11;

  uVar11 = 0;
  uVar8 = this_test->type;
  if (uVar8 != 0)
  {
    uVar11 = (unsigned int)(this_test->type_flag == 0);
  }
  if ((this_test->subtype != 0) && (this_test->subtype_flag == 0))
  {
    uVar11 = uVar11 + 2;
  }
  uVar9 = this_test->color;
  if ((uVar9 != 0) && (this_test->color_flag == 0))
  {
    uVar11 = uVar11 + 4;
  }
  result = this_test->id;
  if ((0 < result) && (this_test->id_flag == 0))
  {
    uVar11 = uVar11 + 8;
  }
  if ((-1 < this_test->cmc) && (this_test->cmc_flag == 0))
  {
    uVar11 = uVar11 + 0x10;
  }
  if ((uVar8 != 0) && (this_test->type_flag == 1))
  {
    uVar11 = uVar11 + 0x20;
  }
  if ((this_test->subtype != 0) && (this_test->subtype_flag == 1))
  {
    uVar11 = uVar11 + 0x40;
  }
  if ((uVar9 != 0) && (this_test->color_flag == 1))
  {
    uVar11 = uVar11 + 0x80;
  }
  if ((0 < result) && (this_test->id_flag == 1))
  {
    uVar11 = uVar11 + 0x100;
  }
  if (-1 < this_test->cmc)
  {
    uVar2 = this_test->cmc_flag;
    if (uVar2 == 1)
    {
      uVar11 = uVar11 + 0x200;
    }
    else if (uVar2 == 2)
    {
      uVar11 = uVar11 + 0x400;
    }
    else if (uVar2 == 3)
    {
      uVar11 = uVar11 + 0x800;
    }
  }
  if (uVar8 == 0)
  {
    if (0 < result)
      goto LAB_026be7d5;
  LAB_026be900:
    if (uVar9 == 0)
      goto LAB_026be908;
  }
  else
  {
    uVar3 = this_test->type_flag;
    if (uVar3 == 2)
    {
      uVar11 = uVar11 + 0x1000;
    }
    else if (uVar3 == 3)
    {
      uVar11 = uVar11 + 0x2000;
    }
    if (0 < result)
    {
    LAB_026be7d5:
      if (this_test->id_flag == 2)
      {
        uVar11 = uVar11 + 0x4000;
      }
      else if (this_test->id_flag == 3)
      {
        uVar11 = uVar11 + 0x8000;
      }
      if (uVar8 == 0)
        goto LAB_026be900;
    }
    if (this_test->type_flag == 4)
    {
      uVar11 = uVar11 + 0x10000;
    }
    if (uVar9 == 0)
    {
    LAB_026be908:
      uVar7 = this_test->color_flag;
      if (uVar7 == 2)
      {
        uVar11 = uVar11 + 0x20000;
      }
      else if (uVar7 == 3)
      {
        uVar11 = uVar11 + 0x40000;
      }
      if (uVar8 == 0)
        goto LAB_026be818;
      uVar1 = this_test->type_flag;
    }
    else
    {
      uVar1 = this_test->type_flag;
    }
    if (uVar1 == 6)
    {
      uVar8 = this_test->keyword;
      uVar11 = uVar11 + 0x80000;
      goto joined_r0x026be942;
    }
  }
LAB_026be818:
  uVar8 = this_test->keyword;
joined_r0x026be942:
  if ((uVar8 != 0) || (this_test->keyword_flag != 0))
  {
    uVar11 = uVar11 + 0x100000;
  }
  if (-1 < this_test->power)
  {
    uVar4 = this_test->power_flag;
    if (uVar4 == 0)
    {
      uVar11 = uVar11 + 0x400000;
    }
    else if (uVar4 == 1)
    {
      uVar11 = uVar11 + 0x800000;
    }
  }
  if (-1 < this_test->toughness)
  {
    if (this_test->toughness_flag == 0)
    {
      uVar11 = uVar11 + 0x1000000;
    }
    else if (this_test->toughness_flag == 1)
    {
      uVar11 = uVar11 + 0x2000000;
    }
  }
  if (this_test->state != 0)
  {
    uVar5 = this_test->state_flag;
    if (uVar5 == 0)
    {
      uVar11 = uVar11 + 0x4000000;
    }
    else if (uVar5 == 1)
    {
      uVar11 = uVar11 + 0x8000000;
    }
  }
  if (-1 < this_test->power)
  {
    uVar6 = this_test->power_flag;
    if (uVar6 == 2)
    {
      uVar11 = uVar11 + 0x10000000;
    }
    else if (uVar6 == 3)
    {
      uVar11 = uVar11 + 0x20000000;
    }
  }
  if (-1 < this_test->toughness)
  {
    if (this_test->toughness_flag == 2)
    {
      uVar11 = uVar11 + 0x40000000;
    }
    else if (this_test->toughness_flag == 3)
    {
      uVar11 = uVar11 + 0x80000000;
    }
  }
  if (this_test->owner != '\x02')
  {
    uVar11 = uVar11 | 0x80000000;
  }
  return uVar11;
}

static int check_multisubtype_by_csvid(int csvid, int sub1, int sub2, int sub3, int sub4, int sub5){
	if( sub1 > 0 && has_subtype_by_id(csvid, sub1) ){
		return 1;
	}
	if( sub2 > 0 && has_subtype_by_id(csvid, sub2) ){
		return 1;
	}
	if( sub3 > 0 && has_subtype_by_id(csvid, sub3) ){
		return 1;
	}
	if( sub4 > 0 && has_subtype_by_id(csvid, sub4) ){
		return 1;
	}
	if( sub5 > 0 && has_subtype_by_id(csvid, sub5) ){
		return 1;
	}
	return 0;
}

static int check_multisubtype_all_by_csvid(int csvid, int sub1, int sub2, int sub3, int sub4, int sub5){
	if( sub1 > 0 && !has_subtype_by_id(csvid, sub1) ){
		return 0;
	}
	if( sub2 > 0 && !has_subtype_by_id(csvid, sub2) ){
		return 0;
	}
	if( sub3 > 0 && !has_subtype_by_id(csvid, sub3) ){
		return 0;
	}
	if( sub4 > 0 && !has_subtype_by_id(csvid, sub4) ){
		return 0;
	}
	if( sub5 > 0 && !has_subtype_by_id(csvid, sub5) ){
		return 0;
	}
	return 1;
}

static int check_multisubtype(int player, int card, int sub1, int sub2, int sub3, int sub4, int sub5){
	if( sub1 > 0 && has_subtype(player, card, sub1) ){
		return 1;
	}
	if( sub2 > 0 && has_subtype(player, card, sub2) ){
		return 1;
	}
	if( sub3 > 0 && has_subtype(player, card, sub3) ){
		return 1;
	}
	if( sub4 > 0 && has_subtype(player, card, sub4) ){
		return 1;
	}
	if( sub5 > 0 && has_subtype(player, card, sub5) ){
		return 1;
	}
	return 0;
}

static int check_multisubtype_all(int player, int card, int sub1, int sub2, int sub3, int sub4, int sub5){
	if( sub1 > 0 && !has_subtype(player, card, sub1) ){
		return 0;
	}
	if( sub2 > 0 && !has_subtype(player, card, sub2) ){
		return 0;
	}
	if( sub3 > 0 && !has_subtype(player, card, sub3) ){
		return 0;
	}
	if( sub4 > 0 && !has_subtype(player, card, sub4) ){
		return 0;
	}
	if( sub5 > 0 && !has_subtype(player, card, sub5) ){
		return 0;
	}
	return 1;
}

void new_default_test_definition(test_definition_2022_t *this_test, int type, const char *msg)
{
  if (ai_is_speculating == 1)
  {
    this_test->message[0] = '\0';
  }
  else
  {
    strcpy(this_test->message, msg);
  }
  this_test->subtype = 0;
  this_test->type = type;
  this_test->type_flag = 0;
  this_test->subtype_flag = 0;
  this_test->color = 0;
  this_test->id = 0;
  this_test->id2 = 0;
  this_test->color_flag = 0;
  this_test->id_flag = 0;
  this_test->cmc = -1;
  this_test->keyword = 0;
  this_test->cmc_flag = 0;
  this_test->keyword_flag = 0;
  this_test->sp_keyword = 0;
  this_test->sp_keyword_flag = 0;
  this_test->power = -1;
  this_test->toughness = -1;
  this_test->power_flag = 0;
  this_test->toughness_flag = 0;
  this_test->state = 0;
  this_test->zone = 0x200;
  this_test->state_flag = 0;
  this_test->not_me = 0;
  this_test->ai_selection_mode = 1;
  this_test->qty = 1;
  this_test->sub2 = 0;
  this_test->sub3 = 0;
  this_test->sub4 = 0;
  this_test->sub5 = 0;
  this_test->no_shuffle = 0;
  this_test->create_minideck = 0;
  this_test->special_selection_function = (test_selection_predicate_2022_t)0x0;
  this_test->value_for_special_selection_function = -1;
  this_test->painters_servant_hack[0] = -1;
  this_test->painters_servant_hack[1] = -1;
  this_test->has_mana_to_pay_cmc = '\0';
  this_test->can_legally_play = '\0';
  this_test->owner = '\x02';
  this_test->storage = -1;
  return;
}

void default_test_definition(test_definition_2022_t *this_test, int type)
{
  new_default_test_definition(this_test, type, "Select a card.");
  return;
}

int new_make_test_in_play(int player, int to_examine, int test_type,
                               test_definition_2022_t *this_test)
{
  uint16_t uVar1;
  uint16_t uVar2;
  short sVar3;
  uint16_t uVar4;
  uint16_t uVar5;
  uint16_t uVar6;
  uint16_t uVar7;
  uint16_t uVar8;
  uint16_t uVar9;
  uint16_t uVar10;
  uint16_t uVar11;
  uint16_t uVar12;
  short sVar13;
  uint16_t uVar14;
  short sVar15;
  subtype_t sVar16;
  test_selection_predicate_2022_t p_Var17;
  subtype_t subtype;
  subtype_t sVar18;
  subtype_t sVar19;
  subtype_t sVar20;
  int iVar21;
  card_instance_t *instance;
  unsigned int uVar23;
  unsigned int uVar24;
  type_t tVar25;
  int iVar26;
  unsigned short uVar27;

  iVar21 = get_id(player, to_examine);
  if (this_test->can_legally_play != '\0')
  {
    instance = get_card_instance(player, to_examine);
    iVar26 = can_legally_play_iid(player, instance->internal_card_id);
    if (iVar26 == 0)
    {
      return 0;
    }
  }
  if (this_test->type != 0)
  {
    uVar1 = this_test->type_flag;
    sVar13 = uVar1;
    if (uVar1 == 0)
    {
      iVar26 = is_what(player, to_examine, this_test->type);
      if (iVar26 == 0)
      {
        return 0;
      }
      uVar6 = this_test->type_flag;
      sVar13 = uVar6;
    }
    if (sVar13 == 1)
    {
      iVar26 = is_what(player, to_examine, this_test->type);
      if (iVar26 != 0)
      {
        return 0;
      }
      uVar7 = this_test->type_flag;
      sVar13 = uVar7;
    }
    if (sVar13 == 2)
    {
      iVar26 = is_what(player, to_examine, this_test->type);
      if (iVar26 == 0)
      {
        return 0;
      }
      iVar26 = is_what(player, to_examine, 2);
      if (iVar26 != 0)
      {
        return 0;
      }
      uVar8 = this_test->type_flag;
      sVar13 = uVar8;
    }
    if (sVar13 == 3)
    {
      iVar26 = is_what(player, to_examine, this_test->type);
      if (iVar26 == 0)
      {
        return 0;
      }
      iVar26 = is_token(player, to_examine);
      if (iVar26 != 0)
      {
        return 0;
      }
      uVar10 = this_test->type_flag;
      sVar13 = uVar10;
    }
    if (sVar13 == 4)
    {
      iVar26 = is_what(player, to_examine, this_test->type);
      if (iVar26 == 0)
      {
        return 0;
      }
      iVar26 = is_planeswalker(player, to_examine);
      if (iVar26 != 0)
      {
        return 0;
      }
      uVar11 = this_test->type_flag;
      sVar13 = uVar11;
    }
    if (sVar13 == 6)
    {
      iVar26 = is_what(player, to_examine, this_test->type);
      if (iVar26 == 0)
      {
        return 0;
      }
      iVar26 = is_token(player, to_examine);
      if (iVar26 == 0)
      {
        return 0;
      }
      uVar12 = this_test->type_flag;
      sVar13 = uVar12;
    }
    if (sVar13 == 9)
    {
      iVar26 = is_what(player, to_examine, 2);
      if (iVar26 == 0)
      {
        return 0;
      }
      iVar26 = is_what(player, to_examine, 0x40);
      if (iVar26 != 0)
      {
        return 0;
      }
      sVar13 = this_test->type_flag;
    }
    if ((sVar13 == 0xb) &&
        (tVar25 = get_type(player, to_examine), this_test->type != (tVar25 & this_test->type)))
    {
      return 0;
    }
  }
  sVar16 = this_test->subtype;
  if (sVar16 != SUBTYPE_INVALID)
  {
    uVar27 = this_test->subtype_flag;
    if (uVar27 == 3)
    {
      sVar18 = this_test->sub5;
      sVar19 = this_test->sub2;
      sVar20 = this_test->sub4;
      subtype = this_test->sub3;
      if ((((((int)sVar16 < 1) ||
             (iVar26 = has_subtype(player, to_examine, sVar16), iVar26 == 0)) &&
            (((int)sVar19 < 1 ||
              (iVar26 = has_subtype(player, to_examine, sVar19), iVar26 == 0)))) &&
           (((int)subtype < 1 ||
             (iVar26 = has_subtype(player, to_examine, subtype), iVar26 == 0)))) &&
          (((int)sVar20 < 1 || (iVar26 = has_subtype(player, to_examine, sVar20), iVar26 == 0))))
      {
        if ((int)sVar18 < 1)
        {
          return 0;
        }
        iVar26 = has_subtype(player, to_examine, sVar18);
        if (iVar26 == 0)
        {
          return 0;
        }
      }
      uVar27 = this_test->subtype_flag;
    }
    if (uVar27 == 4)
    {
      sVar16 = this_test->sub5;
      sVar18 = this_test->sub3;
      sVar19 = this_test->sub4;
      sVar20 = this_test->sub2;
      if ((0 < (int)this_test->subtype) &&
          (iVar26 = has_subtype(player, to_examine, this_test->subtype), iVar26 == 0))
      {
        return 0;
      }
      if ((0 < (int)sVar20) && (iVar26 = has_subtype(player, to_examine, sVar20), iVar26 == 0))
      {
        return 0;
      }
      if ((0 < (int)sVar18) && (iVar26 = has_subtype(player, to_examine, sVar18), iVar26 == 0))
      {
        return 0;
      }
      if ((0 < (int)sVar19) && (iVar26 = has_subtype(player, to_examine, sVar19), iVar26 == 0))
      {
        return 0;
      }
      if ((0 < (int)sVar16) && (iVar26 = has_subtype(player, to_examine, sVar16), iVar26 == 0))
      {
        return 0;
      }
      uVar27 = this_test->subtype_flag;
    }
    if (uVar27 == 0)
    {
      iVar26 = has_subtype(player, to_examine, this_test->subtype);
      if (iVar26 == 0)
      {
        return 0;
      }
      uVar27 = this_test->subtype_flag;
    }
    if ((uVar27 == 1) &&
        (iVar26 = has_subtype(player, to_examine, this_test->subtype), iVar26 != 0))
    {
      return 0;
    }
  }
  if (this_test->color == 0)
  {
    uVar2 = this_test->color_flag;
    sVar15 = uVar2;
  LAB_026beca5:
    if (sVar15 == 2)
    {
      iVar26 = count_colors(player, to_examine);
      if (iVar26 < 2)
      {
        return 0;
      }
      sVar15 = this_test->color_flag;
    }
    if ((sVar15 == 3) && (iVar26 = count_colors(player, to_examine), iVar26 != 1))
    {
      return 0;
    }
  }
  else
  {
    uVar24 = get_color(player, to_examine);
    uVar23 = this_test->color;
    uVar4 = this_test->color_flag;
    if (uVar23 == 1)
    {
      if (uVar4 == 0)
      {
        if (uVar24 != 0)
        {
          return 0;
        }
      }
      else if (uVar24 != 0)
        goto LAB_026becb9;
      if (uVar4 == 1)
      {
        return 0;
      }
    }
    else if (uVar4 == 0)
    {
      if ((uVar24 & uVar23) == 0)
      {
        return 0;
      }
    }
    else if (uVar4 == 1)
    {
      if ((uVar24 & uVar23) != 0)
      {
        return 0;
      }
    }
    else
    {
      sVar15 = uVar4;
      if (uVar23 == 0)
        goto LAB_026beca5;
    }
  }
LAB_026becb9:
  if (this_test->id == -1)
  {
    return 0;
  }
  if (this_test->id != 0)
  {
    uVar27 = this_test->id_flag;
    if (uVar27 == 3)
    {
      iVar26 = is_token(player, to_examine);
      if (iVar26 == 0)
      {
        return 0;
      }
      iVar26 = get_special_infos(player, to_examine);
      if (iVar26 != this_test->id)
      {
        return 0;
      }
      uVar27 = this_test->id_flag;
    }
    if (uVar27 == 4)
    {
      if (this_test->id != iVar21)
      {
        iVar26 = this_test->id2;
      joined_r0x026bece8:
        if (iVar26 != iVar21)
        {
          return 0;
        }
      }
    }
    else
    {
      if (uVar27 == 0)
      {
        iVar26 = this_test->id;
        goto joined_r0x026bece8;
      }
      if ((uVar27 == 1) && (this_test->id == iVar21))
      {
        return 0;
      }
    }
  }
  if (-1 < this_test->cmc)
  {
    iVar21 = get_cmc(player, to_examine);
    uVar5 = this_test->cmc_flag;
    if (uVar5 == 0)
    {
      if (this_test->cmc != iVar21)
      {
        return 0;
      }
    }
    else if (uVar5 == 1)
    {
      if (this_test->cmc == iVar21)
      {
        return 0;
      }
    }
    else if (uVar5 == 2)
    {
      if (iVar21 <= this_test->cmc)
      {
        return 0;
      }
    }
    else if ((uVar5 == 3) && (this_test->cmc <= iVar21))
    {
      return 0;
    }
  }
  if (this_test->keyword != 0)
  {
    uVar27 = this_test->keyword_flag;
    if (uVar27 == 0)
    {
      iVar21 = check_for_ability(player, to_examine, this_test->keyword);
      if (iVar21 == 0)
      {
        return 0;
      }
      uVar27 = this_test->keyword_flag;
    }
    if (uVar27 == 1)
    {
      iVar21 = check_for_ability(player, to_examine, this_test->keyword);
      if (iVar21 != 0)
      {
        return 0;
      }
      uVar27 = this_test->keyword_flag;
    }
    if (uVar27 == 2)
    {
      iVar21 = check_for_special_ability(player, to_examine, this_test->keyword);
      if (iVar21 == 0)
      {
        return 0;
      }
      uVar27 = this_test->keyword_flag;
    }
    if ((uVar27 == 3) &&
        (iVar21 = check_for_special_ability(player, to_examine, this_test->keyword),
         iVar21 != 0))
    {
      return 0;
    }
  }
  if (this_test->sp_keyword != 0)
  {
    sVar13 = (int16_t)this_test->sp_keyword_flag;
    if (sVar13 == 0)
    {
      iVar21 = check_for_special_ability(player, to_examine, this_test->sp_keyword);
      if (iVar21 == 0)
      {
        return 0;
      }
      sVar13 = (int16_t)this_test->sp_keyword_flag;
    }
    if ((sVar13 == 1) &&
        (iVar21 = check_for_special_ability(player, to_examine, this_test->sp_keyword),
         iVar21 != 0))
    {
      return 0;
    }
  }
  if (-1 < this_test->power)
  {
    iVar21 = get_power(player, to_examine);
    uVar9 = this_test->power_flag;
    if (uVar9 == 0)
    {
      if (this_test->power != iVar21)
      {
        return 0;
      }
    }
    else if (uVar9 == 1)
    {
      if (this_test->power == iVar21)
      {
        return 0;
      }
    }
    else
    {
      if (uVar9 != 2)
      {
        if (uVar9 == 3)
        {
          if (this_test->power <= iVar21)
          {
            return 0;
          }
          goto LAB_026bed5b;
        }
        if (uVar9 != 4)
          goto LAB_026bed5b;
        iVar21 = count_counters(player, to_examine, COUNTER_P1_P1);
      }
      if (iVar21 <= this_test->power)
      {
        return 0;
      }
    }
  }
LAB_026bed5b:
  if (-1 < this_test->toughness)
  {
    iVar21 = get_toughness(player, to_examine);
    uVar27 = this_test->toughness_flag;
    if (uVar27 == 0)
    {
      if (this_test->toughness != iVar21)
      {
        return 0;
      }
    }
    else if (uVar27 == 1)
    {
      if (this_test->toughness == iVar21)
      {
        return 0;
      }
    }
    else if (uVar27 == 2)
    {
      if (iVar21 <= this_test->toughness)
      {
        return 0;
      }
    }
    else if ((uVar27 == 3) && (this_test->toughness <= iVar21))
    {
      return 0;
    }
  }
  if (this_test->state != 0)
  {
    sVar3 = this_test->state_flag;
    if (sVar3 == 0)
    {
      iVar21 = check_state(player, to_examine, this_test->state);
      if (iVar21 == 0)
      {
        return 0;
      }
      uVar14 = this_test->state_flag;
      sVar3 = uVar14;
    }
    if ((sVar3 == 1) && (iVar21 = check_state(player, to_examine, this_test->state), iVar21 != 0))
    {
      return 0;
    }
  }
  p_Var17 = this_test->special_selection_function;
  if (p_Var17 != (test_selection_predicate_2022_t)0x0)
  {
    iVar21 = this_test->value_for_special_selection_function;
    instance = get_card_instance(player, to_examine);
    iVar21 = (*p_Var17)(instance->internal_card_id, iVar21, player, to_examine);
    if (iVar21 == 0)
    {
      return 0;
    }
  }
  if (this_test->has_mana_to_pay_cmc == '\x01')
  {
    iVar21 = get_cmc(player, to_examine);
    iVar21 = has_mana(player, 0, iVar21);
    if (iVar21 == 0)
    {
      return 0;
    }
  }
  if (this_test->owner == '\x02')
  {
    return 1;
  }
  uVar23 = get_owner(player, to_examine);
  return (unsigned int)(this_test->owner == uVar23);
}

int make_test_in_play(int player, int to_examine, int test_type, int type, int flag1,
                           int subtype, int flag2, int color, int flag3, int id, int flag4, int cc,
                           int flag5)
{
  int result;
  test_definition_2022_t this_test;
  new_default_test_definition(&this_test, type, "Select a card.");
  this_test.type_flag = (unsigned short)flag1;
  this_test.subtype = subtype;
  this_test.subtype_flag = (unsigned short)flag2;
  this_test.color = color;
  this_test.color_flag = (unsigned short)flag3;
  this_test.id = id;
  this_test.id_flag = (unsigned short)flag4;
  this_test.cmc = cc;
  this_test.cmc_flag = (unsigned short)flag5;
  this_test.type = type;
  result = new_make_test_in_play(player, to_examine, test_type, &this_test);
  return result;
}

int new_make_test(int player, int to_examine, int test_type, test_definition_2022_t *this_test)
{
  uint16_t uVar1;
  uint16_t uVar2;
  uint16_t uVar3;
  uint16_t uVar4;
  uint16_t uVar5;
  uint16_t uVar6;
  uint16_t uVar7;
  uint16_t uVar8;
  uint16_t uVar9;
  uint16_t uVar10;
  short sVar11;
  short sVar12;
  subtype_t sVar13;
  unsigned int uVar14;
  subtype_t subtype;
  subtype_t sVar15;
  subtype_t sVar16;
  subtype_t sVar17;
  unsigned int uVar18;
  unsigned int clr;
  int result;
  type_t tVar20;
  unsigned short uVar21;

  uVar18 = (uint32_t)cards_data[to_examine].id | ((uint32_t)cards_data[to_examine].reserved2 << 16);
  if ((this_test->can_legally_play != '\0') &&
      (result = can_legally_play_iid(player, to_examine), result == 0))
  {
    return 0;
  }
  if (this_test->type != 0)
  {
    uVar1 = this_test->type_flag;
    sVar11 = uVar1;
    if (uVar1 == 0)
    {
      result = is_what(-1, to_examine, this_test->type);
      if (result == 0)
      {
        return 0;
      }
      uVar6 = this_test->type_flag;
      sVar11 = uVar6;
    }
    if (sVar11 == 1)
    {
      result = is_what(-1, to_examine, this_test->type);
      if (result != 0)
      {
        return 0;
      }
      uVar7 = this_test->type_flag;
      sVar11 = uVar7;
    }
    if (sVar11 == 2)
    {
      result = is_what(-1, to_examine, this_test->type);
      if (result == 0)
      {
        return 0;
      }
      result = is_what(-1, to_examine, 2);
      if (result != 0)
      {
        return 0;
      }
      uVar8 = this_test->type_flag;
      sVar11 = uVar8;
    }
    if (sVar11 == 4)
    {
      result = is_what(-1, to_examine, this_test->type);
      if (result == 0)
      {
        return 0;
      }
      result = is_planeswalker(-1, to_examine);
      if (result != 0)
      {
        return 0;
      }
      uVar10 = this_test->type_flag;
      sVar11 = uVar10;
    }
    if (sVar11 == 9)
    {
      result = is_what(-1, to_examine, 2);
      if (result == 0)
      {
        return 0;
      }
      result = is_what(-1, to_examine, 0x40);
      if (result != 0)
      {
        return 0;
      }
      sVar11 = this_test->type_flag;
    }
    if ((sVar11 == 0xb) &&
        (tVar20 = get_type(-1, to_examine), this_test->type != (tVar20 & this_test->type)))
    {
      return 0;
    }
  }
  uVar18 = uVar18 & 0xffff;
  sVar13 = this_test->subtype;
  if (sVar13 != SUBTYPE_INVALID)
  {
    uVar21 = this_test->subtype_flag;
    if (uVar21 == 3)
    {
      sVar15 = this_test->sub5;
      sVar16 = this_test->sub3;
      sVar17 = this_test->sub4;
      subtype = this_test->sub2;
      if ((((((int)sVar13 < 1) ||
             (result = has_subtype_by_id_2022(-1, uVar18, sVar13), result == 0)) &&
            (((int)subtype < 1 ||
              (result = has_subtype_by_id_2022(-1, uVar18, subtype), result == 0)))) &&
           (((int)sVar16 < 1 ||
             (result = has_subtype_by_id_2022(-1, uVar18, sVar16), result == 0)))) &&
          (((int)sVar17 < 1 || (result = has_subtype_by_id_2022(-1, uVar18, sVar17), result == 0))))
      {
        if ((int)sVar15 < 1)
        {
          return 0;
        }
        result = has_subtype_by_id_2022(-1, uVar18, sVar15);
        if (result == 0)
        {
          return 0;
        }
      }
      uVar21 = this_test->subtype_flag;
    }
    if (uVar21 == 4)
    {
      sVar13 = this_test->sub5;
      sVar15 = this_test->sub3;
      sVar16 = this_test->sub4;
      sVar17 = this_test->sub2;
      if ((0 < (int)this_test->subtype) &&
          (result = has_subtype_by_id_2022(-1, uVar18, this_test->subtype), result == 0))
      {
        return 0;
      }
      if ((0 < (int)sVar17) && (result = has_subtype_by_id_2022(-1, uVar18, sVar17), result == 0))
      {
        return 0;
      }
      if ((0 < (int)sVar15) && (result = has_subtype_by_id_2022(-1, uVar18, sVar15), result == 0))
      {
        return 0;
      }
      if ((0 < (int)sVar16) && (result = has_subtype_by_id_2022(-1, uVar18, sVar16), result == 0))
      {
        return 0;
      }
      if ((0 < (int)sVar13) && (result = has_subtype_by_id_2022(-1, uVar18, sVar13), result == 0))
      {
        return 0;
      }
      uVar21 = this_test->subtype_flag;
    }
    if (uVar21 == 0)
    {
      result = has_subtype_by_id_2022(player, uVar18, this_test->subtype);
      if (result == 0)
      {
        return 0;
      }
      uVar21 = this_test->subtype_flag;
    }
    if ((uVar21 == 1) &&
        (result = has_subtype_by_id_2022(player, uVar18, this_test->subtype), result != 0))
    {
      return 0;
    }
  }
  if ((this_test->color == 0) && (uVar3 = this_test->color_flag, 1 < (unsigned short)(uVar3 - 2)))
  {
    clr = 0;
    sVar12 = uVar3;
  LAB_026bf834:
    if (sVar12 == 2)
    {
      result = real_count_colors(clr);
      if (result < 2)
      {
        return 0;
      }
      sVar12 = this_test->color_flag;
    }
    if ((sVar12 == 3) && (result = real_count_colors(clr), result != 1))
    {
      return 0;
    }
  }
  else
  {
    if (this_test->painters_servant_hack[player] == -1)
    {
      result = get_global_color_hack(player);
      this_test->painters_servant_hack[player] = (int16_t)result;
    }
    clr = get_color_by_internal_id(player, to_examine);
    uVar14 = this_test->color;
    if (uVar14 == 0)
    {
      uVar2 = this_test->color_flag;
      sVar12 = uVar2;
      goto LAB_026bf834;
    }
    uVar4 = this_test->color_flag;
    if (uVar4 == 0)
    {
      if ((clr & uVar14) == 0)
      {
        return 0;
      }
    }
    else if ((uVar4 == 1) && ((clr & uVar14) != 0))
    {
      return 0;
    }
  }
  uVar14 = this_test->id;
  if (uVar14 == 0xffffffff)
  {
    return 0;
  }
  if (uVar14 != 0)
  {
    uVar21 = this_test->id_flag;
    if ((uVar21 != 4) || (uVar14 == uVar18))
    {
      if ((uVar21 == 0) && (uVar14 != uVar18))
      {
        return 0;
      }
      if ((uVar14 == uVar18) && (uVar21 == 1))
      {
        return 0;
      }
    }
    else if (this_test->id2 != uVar18)
    {
      return 0;
    }
  }
  if (-1 < this_test->cmc)
  {
    result = get_cmc_by_id((unsigned int)cards_data[to_examine].id);
    uVar5 = this_test->cmc_flag;
    if (uVar5 == 0)
    {
      if (this_test->cmc != result)
      {
        return 0;
      }
    }
    else if (uVar5 == 1)
    {
      if (this_test->cmc == result)
      {
        return 0;
      }
    }
    else if (uVar5 == 2)
    {
      if (result <= this_test->cmc)
      {
        return 0;
      }
    }
    else if ((uVar5 == 3) && (this_test->cmc <= result))
    {
      return 0;
    }
  }
  uVar18 = this_test->keyword;
  if (uVar18 != 0)
  {
    uVar21 = this_test->keyword_flag;
    if (uVar21 == 0)
    {
      if ((cards_data[to_examine].static_ability & uVar18) == 0)
      {
        return 0;
      }
    }
    else
    {
      if (uVar21 == 1)
      {
        uVar18 = cards_data[to_examine].static_ability & uVar18;
      }
      else
      {
        if (uVar21 == 2)
        {
          result = check_for_special_ability(player, to_examine, uVar18);
          if (result == 0)
          {
            return 0;
          }
          uVar21 = this_test->keyword_flag;
        }
        if (uVar21 != 3)
          goto LAB_026bf8ac;
        uVar18 = check_for_special_ability(player, to_examine, this_test->keyword);
      }
      if (uVar18 != 0)
      {
        return 0;
      }
    }
  }
LAB_026bf8ac:
  if (this_test->sp_keyword != 0)
  {
    sVar11 = (int16_t)this_test->sp_keyword_flag;
    if (sVar11 == 0)
    {
      result = check_for_special_ability(-1, to_examine, this_test->sp_keyword);
      if (result == 0)
      {
        return 0;
      }
      sVar11 = (int16_t)this_test->sp_keyword_flag;
    }
    if ((sVar11 == 1) &&
        (result = check_for_special_ability(-1, to_examine, this_test->sp_keyword),
         result != 0))
    {
      return 0;
    }
  }
  if (-1 < this_test->power)
  {
    result = get_base_power_iid(player, to_examine);
    uVar9 = this_test->power_flag;
    if (uVar9 == 0)
    {
      if (this_test->power != result)
      {
        return 0;
      }
    }
    else if (uVar9 == 1)
    {
      if (this_test->power == result)
      {
        return 0;
      }
    }
    else if (uVar9 == 2)
    {
      if (result <= this_test->power)
      {
        return 0;
      }
    }
    else if (uVar9 == 3)
    {
      if (this_test->power <= result)
      {
        return 0;
      }
    }
    else
    {
      if (uVar9 == 4)
      {
        return 0;
      }
    }
  }
  if (-1 < this_test->toughness)
  {
    result = get_base_toughness_iid(player, to_examine);
    uVar21 = this_test->toughness_flag;
    if (uVar21 == 0)
    {
      if (this_test->toughness != result)
      {
        return 0;
      }
    }
    else if (uVar21 == 1)
    {
      if (this_test->toughness == result)
      {
        return 0;
      }
    }
    else if (uVar21 == 2)
    {
      if (result <= this_test->toughness)
      {
        return 0;
      }
    }
    else if ((uVar21 == 3) && (this_test->toughness <= result))
    {
      return 0;
    }
  }
  if ((this_test->special_selection_function != (test_selection_predicate_2022_t)0x0) &&
      (result = (*this_test->special_selection_function)(
           to_examine, this_test->value_for_special_selection_function, player, -1),
       result == 0))
  {
    return 0;
  }
  if (this_test->has_mana_to_pay_cmc == '\x01')
  {
    result = get_cmc_by_id((unsigned int)cards_data[to_examine].id);
    result = has_mana(player, 0, result);
    if (result == 0)
    {
      return 0;
    }
  }
  if (this_test->owner == 2)
  {
    return 1;
  }
  return (unsigned int)((unsigned int)this_test->owner == player);
}

int make_test(int player, int to_examine, int test_type, int type, int flag1, int subtype,
                   int flag2, int color, int flag3, int id, int flag4, int cc, int flag5)
{
  int result;
  test_definition_2022_t this_test;
  new_default_test_definition(&this_test, type, "Select a card.");
  this_test.type_flag = (unsigned short)flag1;
  this_test.subtype = subtype;
  this_test.subtype_flag = (unsigned short)flag2;
  this_test.color = color;
  this_test.color_flag = (unsigned short)flag3;
  this_test.id = id;
  this_test.id_flag = (unsigned short)flag4;
  this_test.cmc = cc;
  this_test.cmc_flag = (unsigned short)flag5;
  result = new_make_test(player, to_examine, test_type, &this_test);
  return result;
}

int my_base_value_by_id( int csvid ){
	card_ptr_t* c = cards_ptr[ csvid ];
	int result = c->ai_base_value;

	if( result < 1 ){
		result = 0;
		int iid = get_internal_card_id_from_csv_id(csvid);
		if( is_what(-1, iid, TYPE_CREATURE) ){
			result += get_base_power_iid(-1, iid);
			result += get_base_toughness_iid(-1, iid);
			result -= get_cmc_by_id(csvid)*2;
			int i;
			for(i=0; i<32; i++){
				if( cards_data[iid].static_ability & (1<<i) ){
					result+=4;
				}
				if( cards_data[iid].extra_ability & (1<<i) ){
					result+=8;
				}
			}
			// Flash
			if(cards_data[iid].type & TYPE_INSTANT){
				result+=10;
			}
		}
		if( is_what(-1, iid, TYPE_ARTIFACT)){
			int i;
			for(i=0; i<32; i++){
				if( cards_data[iid].extra_ability & (1<<i) ){
					result+=4;
				}
			}
			if( cards_data[iid].extra_ability & EA_MANA_SOURCE ){
				result+=real_count_colors(c->mana_source_colors)*2;
			}
			result-=get_cmc_by_id(csvid);
		}
		if( is_what(-1, iid, TYPE_ENCHANTMENT) || is_planeswalker(-1, iid) ){
			int i;
			for(i=0; i<32; i++){
				if( cards_data[iid].extra_ability & (1<<i) ){
					result+=4;
				}
			}
			if( cards_data[iid].extra_ability & EA_MANA_SOURCE ){
				result+=real_count_colors(c->mana_source_colors)*2;
			}
			result-=get_cmc_by_id(csvid);
			if( is_planeswalker(-1, iid) ){
				result+=20;
			}
		}
		if( is_what(-1, iid, TYPE_LAND)){
			int i;
			for(i=0; i<32; i++){
				if( cards_data[iid].extra_ability & (1<<i) ){
					result+=2;
				}
			}
			if( cards_data[iid].extra_ability & EA_MANA_SOURCE ){
				result+=real_count_colors(c->mana_source_colors)*2;
				result+=10;
			}
		}
		if( is_what(-1, iid, TYPE_SPELL) && ! is_what(-1, iid, TYPE_CREATURE) ){
			int i;
			for(i=0; i<32; i++){
				if( cards_data[iid].extra_ability & (1<<i) ){
					result+=4;
				}
			}
			result-=get_cmc_by_id(csvid);
		}
		result*=5;
		if( result < 0 ){
			result = 0;
		}
	}
	return result;
}

int my_base_value(int player, int card ){
	int result = 0;
	if( player == -1 ){
		result = my_base_value_by_id(cards_data[card].id);
	}
	else{
		int cmc = in_play(player, card) ? -get_cmc(player, card) : get_cmc(player, card);

		card_instance_t *instance = get_card_instance( player, card );
		int iid = instance->internal_card_id;
		int csvid = cards_data[iid].id;
		card_ptr_t* cp = cards_ptr[csvid];

		result = 0;
		if( is_what(player, card, TYPE_CREATURE) ){
			result+=get_power(player, card);
			result+=get_toughness(player, card);
			result-=cmc*2;
			int i;
			for(i=0; i<32; i++){
				if( cards_data[iid].static_ability & (1<<i) ){
					result+=4;
				}
				if( cards_data[iid].extra_ability & (1<<i) ){
					result+=8;
				}
			}
			// Flash
			if( (get_card_data(player, card)->type & TYPE_INSTANT) && !in_play(player, card) ){
				result+=10;
			}
		}
		if( is_what(player, card, TYPE_ARTIFACT)){
			int i;
			for(i=0; i<32; i++){
				if( cards_data[iid].extra_ability & (1<<i) ){
					result+=4;
				}
			}
			if( cards_data[iid].extra_ability & EA_MANA_SOURCE ){
				result+=real_count_colors(cp->mana_source_colors)*2;
			}
			result-=cmc;
		}
		if( is_what(player, card, TYPE_ENCHANTMENT) || is_planeswalker(player, card) ){
			int i;
			for(i=0; i<32; i++){
				if( cards_data[iid].extra_ability & (1<<i) ){
					result+=4;
				}
			}
			if( cards_data[iid].extra_ability & EA_MANA_SOURCE ){
				result+=real_count_colors(cp->mana_source_colors)*2;
			}
			result-=cmc;
			if( is_planeswalker(player, card) ){
				result+=20;
			}
		}
		if( is_what(player, card, TYPE_LAND)){
			int i;
			for(i=0; i<32; i++){
				if( cards_data[iid].extra_ability & (1<<i) ){
					result+=2;
				}
			}
			if( cards_data[iid].extra_ability & EA_MANA_SOURCE ){
				result+=real_count_colors(cp->mana_source_colors)*2;
				result+=10;
			}
		}
		if( is_what(player, card, TYPE_SPELL) && ! is_what(player, card, TYPE_CREATURE) ){
			int i;
			for(i=0; i<32; i++){
				if( cards_data[iid].extra_ability & (1<<i) ){
					result+=4;
				}
			}
			result-=cmc;
		}
		result*=5;
		if( result < 0 ){
			result = 0;
		}
	}
	return result;
}

extern int hack_illusionary_mask_event;	// must set to event when setting test_definition_t::has_mana_to_pay_cmc to 2
int check_battlefield_for_special_card(int player, int card, int t_player, int mode,
                                            test_definition_2022_t *this_test)
{
  card_instance_t *pcVar1;
  int result;
  unsigned char uVar3;
  unsigned int card_00;
  int player_00;
  bool bVar4;
  card_instance_t *pcStack_7c;
  unsigned int uStack_70;
  unsigned int uStack_6c;
  target_definition_2022_t td;

  pcStack_7c = NULL;
  if ((mode & 8U) != 0)
  {
    default_target_definition(player, card, &td, this_test->type);
    td.allowed_controller = t_player;
    td.preferred_controller = t_player;
    if (this_test->type_flag == 1)
    {
      td.required_type = ((this_test->zone != 0x200) - 1 & 0xffffffc8) + 0x7f;
      td.illegal_type = this_test->type | 0x80;
    }
    pcStack_7c = get_card_instance(player, card);
  }
  uStack_70 = mode & 0x402;
  if (uStack_70 != 0)
  {
    uStack_70 = 1000;
  }
  uStack_6c = -(unsigned int)((mode & 3U) != 0);
  player_00 = 0;
  do
  {
    if (((t_player == player_00) || (t_player == 2)) &&
        (card_00 = active_cards_count[player_00] - 1, -1 < (int)card_00))
    {
      do
      {
        pcVar1 = in_play(player_00, card_00);
        if ((((((pcVar1 != NULL) && (this_test->zone == 0x200)) &&
               (result = check_state(player_00, card_00, 0x100000), result == 0)) ||
              ((pcVar1 = in_hand(player_00, card_00),
                pcVar1 != NULL && (this_test->zone == 0x100)))) &&
             (result = new_make_test_in_play(player_00, card_00, -1, this_test),
              result != 0)) &&
            ((this_test->not_me == 0 ||
              ((this_test->not_me == 1 && ((player != player_00 || (card != card_00))))))))
        {
          bVar4 = true;
          if ((mode & 8U) == 0)
          {
            uVar3 = this_test->has_mana_to_pay_cmc;
          }
          else
          {
            pcStack_7c->targets[0].player = player_00;
            pcStack_7c->targets[0].card = card_00;
            pcStack_7c->number_of_targets = '\x01';
            result = valid_target(&td);
            uVar3 = this_test->has_mana_to_pay_cmc;
            bVar4 = result != 0;
          }
          if (uVar3 == '\x01')
          {
            result = get_cmc(player_00, card_00);
            result = has_mana(player, 0, result);
            uVar3 = this_test->has_mana_to_pay_cmc;
            if (result == 0)
            {
              bVar4 = false;
            }
          }
          if (uVar3 == '\x02')
          {
            result = get_original_internal_card_id(player_00, card_00);
            result = has_mana_to_cast_iid(player, hack_illusionary_mask_event, result);
            if (result == 0)
              goto LAB_026c08e8;
          }
          if (((player != player_00 || card != card_00) || ((mode & 0x20U) == 0)) && (bVar4))
          {
            if (mode == 0)
            {
              return 1;
            }
            uStack_6c = (uStack_6c + 1) - (unsigned int)((mode & 4U) == 0);
            if (((mode & 0x10U) != 0) && (result = is_what(player_00, card_00, 2), result != 0))
            {
              result = get_toughness(player_00, card_00);
              uStack_6c = uStack_6c + result;
            }
            if (((mode & 0x40U) != 0) && (result = is_what(player_00, card_00, 2), result != 0))
            {
              result = get_power(player_00, card_00);
              uStack_6c = uStack_6c + result;
            }
            if ((mode & 0x800U) != 0)
            {
              result = get_cmc(player_00, card_00);
              uStack_6c = uStack_6c + result;
            }
            if ((((mode & 0x100U) != 0) &&
                 (result = is_what(player_00, card_00, 2), result != 0)) &&
                (result = get_toughness(player_00, card_00), (int)uStack_70 < result))
            {
              uStack_70 = get_toughness(player_00, card_00);
              uStack_6c = uStack_70;
            }
            if ((((mode & 0x80U) != 0) && (result = is_what(player_00, card_00, 2), result != 0)) &&
                (result = get_power(player_00, card_00), (int)uStack_70 < result))
            {
              uStack_70 = get_power(player_00, card_00);
              uStack_6c = uStack_70;
            }
            if (((mode & 0x200U) != 0) &&
                (result = get_cmc(player_00, card_00), (int)uStack_70 < result))
            {
              uStack_70 = get_cmc(player_00, card_00);
              uStack_6c = uStack_70;
            }
            if (((mode & 0x400U) != 0) &&
                (result = get_cmc(player_00, card_00), result < (int)uStack_70))
            {
              uStack_70 = get_cmc(player_00, card_00);
              uStack_6c = uStack_70;
            }
            if ((mode & 1U) != 0)
            {
              result = is_token(player_00, card_00);
              if (result == 0)
              {
                result = my_base_value(player_00, card_00);
                if ((int)uStack_70 < result)
                {
                  uStack_70 = my_base_value(player_00, card_00);
                  uStack_6c = card_00;
                }
              }
              else
              {
                result = get_power(player_00, card_00);
                if ((int)uStack_70 < result)
                {
                  uStack_70 = get_power(player_00, card_00);
                  uStack_6c = card_00;
                }
              }
            }
            if ((mode & 2U) != 0)
            {
              result = is_token(player_00, card_00);
              if (result == 0)
              {
                result = my_base_value(player_00, card_00);
                if (result < (int)uStack_70)
                {
                  uStack_70 = my_base_value(player_00, card_00);
                  uStack_6c = card_00;
                }
              }
              else
              {
                result = get_power(player_00, card_00);
                if (result < (int)uStack_70)
                {
                  uStack_70 = get_power(player_00, card_00);
                  uStack_6c = card_00;
                }
              }
            }
          }
        }
      LAB_026c08e8:
        card_00 = card_00 - 1;
      } while (card_00 != 0xffffffff);
    }
    player_00 = player_00 + 1;
    if (player_00 == 2)
    {
      return uStack_6c;
    }
  } while (true);
}

int select_card_to_play_from_zone(int player, int card, event_t event, const int *zone, test_definition_t *test, int mode){
	int ng[count_zone(player, zone)];
	int ngc = 0;
	int count = 0;
	int result = -1;
	while( zone[count] != -1 ){
			if( new_make_test(player, zone[count], -1, test)  ){
				int good = 0;
				if( ! mode && has_mana_to_cast_iid(player, event, zone[count]) ){
					good = 1;
				}
				if( mode == 1 ){
					int cless = get_updated_casting_cost(player, -1, event, zone[count], -1);
					if( has_mana(player, COLOR_COLORLESS, cless) ){
						good = 1;
					}
				}
				if( good ){
					ng[ngc] = zone[count];
					ngc++;
				}
			}
			count++;
	}
	int selected = select_card_from_zone(player, player, ng, ngc, 0, AI_MAX_CMC, -1, test);
	if( selected != -1 ){
		count = 0;
		while( zone[count] != -1 ){
				if( zone[count] == ng[selected] ){
					result = count;
					break;
				}
				count++;
		}
	}
	return result;
}

int check_battlefield_for_name(int player, int type, int name){
	APNAP(p,{ 
				if( player == ANYBODY || p == player ){
					int c;
					for(c=0; c<active_cards_count[p]; c++){
						if( in_play(p, c) && is_what(p, c, type) && get_card_name(p, c) == name ){
							return 1;
						}
					}
				}
			};
	);
	return 0;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x26c1000; original functions/global_checking.c:1212. */
int test_not_in_array(int iid, int arrptr, int player, int card)
{
  int result;

  result = 0;
  do
  {
    if (*(int *)arrptr == iid)
    {
      return 0;
    }
    result = result + 1;
    arrptr = arrptr + 4;
  } while ((result < 4) && (result != -1));
  return 1;
}


#ifndef SHANDALAR

/* ManalinkEh 2022: 0x26be510; original functions/global_checking.c:3. */



/* ManalinkEh 2022: 0x26be6e0; original functions/global_checking.c:90. */



/* ManalinkEh 2022: 0x26bea60; original functions/global_checking.c:374. */



/* ManalinkEh 2022: 0x26bebb0; original functions/global_checking.c:421. */



/* ManalinkEh 2022: 0x26bebe0; original functions/global_checking.c:425. */



/* ManalinkEh 2022: 0x26bf6c0; original functions/global_checking.c:638. */



/* ManalinkEh 2022: 0x26bf750; original functions/global_checking.c:654. */



/* ManalinkEh 2022: 0x26c0130; original functions/global_checking.c:848. */


#endif


#ifndef SHANDALAR
/* Recovered October 2022 split card helpers. */

/* ManalinkEh 2022: 0x26c0860; original functions/global_checking.c:1033. */


#endif
