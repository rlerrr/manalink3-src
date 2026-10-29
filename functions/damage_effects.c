#include "manalink.h"

void damage_effects(int player, int card, event_t event)
{
  bool bVar1;
  unsigned char bVar2;
  card_instance_t *pcVar3;
  card_instance_t *pcVar4;
  int iVar5;
  int iVar6;
  int32_t iVar7;
  unsigned int uVar8;
  card_instance_t *pcVar9;
  int iVar10;
  int amount;
  counter_t type;
  if (event != EVENT_DEAL_DAMAGE)
  {
    return;
  }
  pcVar3 = get_card_instance(player, card);
  pcVar4 = get_card_instance(affected_card_controller, affected_card);
  if (pcVar4->internal_card_id != damage_card)
  {
    return;
  }
  iVar5 = get_id(player, card);
  iVar6 = check_for_special_ability(player, card, 0x1000000);
  if ((iVar5 == 0x1c7a) || (bVar1 = false, iVar6 != 0))
  {
    bVar1 = true;
  }
  iVar6 = is_what(player, card, 0x80);
  bVar2 = 0;
  if (iVar6 != 0)
  {
    bVar2 = (pcVar3->targets[2].card != 0x1b2b) - 1U & 0x40;
  }
  iVar6 = (int)pcVar4->damage_source_player;
  if (((iVar6 == player) && (pcVar4->damage_source_card == card)) && (0 < pcVar4->info_slot))
  {
    if (pcVar4->damage_target_card == -1)
    {
      if (iVar5 != 0x1f2c)
      {
        return;
      }
      if ((pcVar4->token_status & 0x140000) == 0)
      {
        return;
      }
      add_counters(player, card, player, card, COUNTER_P1_P1, pcVar4->info_slot);
      mill((int)pcVar4->damage_target_player, pcVar4->info_slot);
      pcVar4->info_slot = 0;
      return;
    }
    if (!bVar1)
    {
      return;
    }
    pcVar9 = in_play((int)pcVar4->damage_target_player, pcVar4->damage_target_card);
    if (pcVar9 == NULL)
    {
      return;
    }
    iVar5 = pcVar3->targets[1].player;
    if (iVar5 < 2)
    {
      pcVar3->targets[1].player = 2;
      iVar5 = 2;
    }
    else if (9 < iVar5)
    {
      return;
    }
    pcVar3->targets[iVar5].player = (int)pcVar4->damage_target_player;
    pcVar3->targets[iVar5].card = pcVar4->damage_target_card;
    pcVar3->targets[1].player = pcVar3->targets[1].player + 1;
    return;
  }
  if (iVar5 == 0x214b)
  {
    if ((pcVar4->damage_target_player == player) && (pcVar4->damage_target_card == card))
    {
      iVar10 = pcVar4->info_slot;
      if (iVar10 < 1)
      {
        pcVar9 = get_card_instance(iVar6, pcVar4->damage_source_card);
        iVar10 = pcVar9->targets[0x10].player;
        if (0 < iVar10)
          goto LAB_026e1828;
      }
      else
      {
      LAB_026e1828:
        if (pcVar3->targets[9].player < 0)
        {
          pcVar3->targets[9].player = 0;
        }
        pcVar3->targets[9].player = pcVar3->targets[9].player + iVar10;
      }
      pcVar4->info_slot = 0;
    }
  }
  else if (iVar5 == 0x92)
  {
    if ((pcVar4->damage_target_player == player) && (pcVar4->damage_target_card == -1))
    {
      iVar7 = pcVar4->info_slot;
      type = COUNTER_VITALITY;
      goto LAB_026e17e9;
    }
  }
  else if (iVar5 == 0x1ab3)
  {
    if (((int)pcVar4->damage_target_player == 1 - player) && (pcVar4->damage_target_card == -1))
    {
      iVar7 = pcVar4->info_slot;
      type = COUNTER_P1_P1;
    LAB_026e17e9:
      add_counters(player, card, player, card, type, iVar7);
    }
  }
  else if (((iVar5 == 0x1c9c) && ((int)pcVar4->damage_target_player == 1 - player)) &&
           (iVar6 == player))
  {
    iVar7 = pcVar4->info_slot;
    type = COUNTER_THEFT;
    goto LAB_026e17e9;
  }
  iVar6 = is_what(player, card, 0x80);
  if ((((iVar6 != 0) && (pcVar3->targets[2].card == 0x44)) &&
       (iVar6 = (int)pcVar4->damage_target_player, iVar6 == pcVar3->targets[0].player)) &&
      (iVar10 = pcVar4->damage_target_card, iVar10 == pcVar3->targets[0].card))
  {
    amount = pcVar4->info_slot;
    if (iVar10 == -1)
    {
      iVar6 = life[iVar6];
      if (iVar6 < amount)
      {
      LAB_026e1a62:
        amount = iVar6;
      }
    }
    else
    {
      iVar6 = get_toughness(iVar6, iVar10);
      if (iVar6 < amount)
      {
        iVar6 = get_toughness(pcVar3->targets[0].player, pcVar3->targets[0].card);
        goto LAB_026e1a62;
      }
    }
    gain_life(player, amount);
    kill_card(player, card, KILL_EXILE);
  }
  iVar6 = is_what(player, card, 0x80);
  if (((iVar6 != 0) && (pcVar3->targets[2].card == 0x1e6a)) &&
      (iVar6 = get_id((int)pcVar4->damage_source_player, pcVar4->damage_source_card),
       iVar6 == 0x1e6a))
  {
    gain_life(player, pcVar4->info_slot);
  }
  iVar6 = is_what(player, card, 0x80);
  if (((iVar6 != 0) && (pcVar3->targets[1].card == 0x2059)) &&
      (((int)pcVar4->damage_source_player == pcVar3->targets[0].player &&
        (((pcVar4->damage_source_card == pcVar3->targets[0].card && (0 < pcVar4->info_slot)) &&
          (iVar6 = pcVar4->damage_target_card, iVar6 != -1))))))
  {
    iVar10 = pcVar3->targets[1].player;
    if (iVar10 < 2)
    {
      pcVar3->targets[1].player = 2;
      iVar10 = 2;
    }
    else if (9 < iVar10)
      goto LAB_026e15ce;
    pcVar3->targets[iVar10].player = (int)pcVar4->damage_target_player;
    pcVar3->targets[iVar10].card = iVar6;
    pcVar3->targets[1].player = pcVar3->targets[1].player + 1;
  }
LAB_026e15ce:
  if (((iVar5 == 0x2065 || iVar5 == 0x3245) || (iVar5 == 0x2525)) &&
      (pcVar4->damage_target_player == player))
  {
    iVar7 = pcVar4->damage_target_card;
    if ((iVar7 == card) && (0 < pcVar4->info_slot))
    {
      iVar10 = pcVar3->targets[1].player;
      iVar6 = 1;
      if (-1 < iVar10)
      {
        iVar6 = iVar10 + 1;
      }
      pcVar3->targets[1].player = iVar6;
    }
    if (iVar5 == 0x2234)
    {
    LAB_026e18f7:
      if (((iVar7 != -1) || (pcVar4->info_slot < 1)) ||
          (uVar8 = get_color((int)pcVar4->damage_source_player, pcVar4->damage_source_card),
           (pcVar3->info_slot & uVar8) == 0))
        goto LAB_026e1628;
      if (pcVar3->targets[1].player < 0)
      {
        pcVar3->targets[1].player = 0;
      }
    LAB_026e172a:
      pcVar3->targets[1].player = pcVar3->targets[1].player + 1;
    }
    else
    {
    LAB_026e1608:
      if (iVar5 == 0x2494)
      {
        if ((((pcVar4->damage_source_player == player) && (pcVar4->damage_target_card != -1)) &&
             (pcVar9 = in_play((int)pcVar4->damage_target_player, pcVar4->damage_target_card),
              pcVar9 != NULL)) &&
            (iVar5 = has_subtype((int)pcVar4->damage_source_player, pcVar4->damage_source_card,
                                 SUBTYPE_ARCHER),
             iVar5 != 0))
        {
          iVar5 = pcVar3->targets[1].player;
          if (iVar5 < 2)
          {
            pcVar3->targets[1].player = 2;
            iVar5 = 2;
          }
          else if (9 < iVar5)
            goto LAB_026e1628;
          pcVar3->targets[iVar5].player = (int)pcVar4->damage_target_player;
          pcVar3->targets[iVar5].card = pcVar4->info_slot;
          pcVar3->targets[1].player = pcVar3->targets[1].player + 1;
        }
      }
      else if (iVar5 == 0x24ba)
      {
        if (((pcVar4->damage_source_player == player) && (pcVar4->damage_target_card == -1)) &&
            ((pcVar9 = in_play(player, pcVar4->damage_source_card),
              pcVar9 != NULL && ((iVar5 = count_counters((int)pcVar4->damage_source_player,
                                                         pcVar4->damage_source_card, COUNTER_P1_P1),
                                  0 < iVar5 && ((pcVar4->token_status & 0x140000) != 0))))))
        {
          if (pcVar3->targets[1].player < 0)
          {
            pcVar3->targets[1].player = 0;
          }
          goto LAB_026e172a;
        }
      }
      else if ((((iVar5 == 0x2528) && (pcVar4->damage_source_player == player)) &&
                (pcVar4->damage_source_card == card)) &&
               ((pcVar4->damage_target_card == -1 && (5 < pcVar4->info_slot))))
      {
        pcVar3->targets[1].player = pcVar4->damage_target_player + 2;
      }
    }
  }
  else
  {
    if (iVar5 != 0x2234)
      goto LAB_026e1608;
    if (pcVar4->damage_target_player == player)
    {
      iVar7 = pcVar4->damage_target_card;
      goto LAB_026e18f7;
    }
  }
LAB_026e1628:
  iVar5 = is_what(player, card, 0x80);
  if ((((iVar5 != 0) && ((iVar5 = pcVar3->targets[2].card, iVar5 == 0x164 || (iVar5 == 0x755)))) &&
       (pcVar4->damage_source_player == pcVar3->damage_source_player)) &&
      (((pcVar4->damage_source_card == pcVar3->damage_source_card &&
         (-1 < pcVar4->damage_target_card)) &&
        (pcVar9 = in_play((int)pcVar4->damage_target_player, pcVar4->damage_target_card),
         pcVar9 != NULL))))
  {
    iVar5 = pcVar3->targets[2].player;
    if (iVar5 < 3)
    {
      pcVar3->targets[2].player = 3;
      iVar5 = 3;
    }
    else if (9 < iVar5)
      goto LAB_026e165f;
    pcVar3->targets[iVar5].player = (int)pcVar4->damage_target_player;
    pcVar3->targets[iVar5].card = pcVar4->damage_target_card;
    pcVar3->targets[2].player = pcVar3->targets[2].player + 1;
  }
LAB_026e165f:
  if (((bVar2 != 0) && (pcVar4->damage_target_card != -1)) &&
      (pcVar3 = in_play((int)pcVar4->damage_target_player, pcVar4->damage_target_card),
       pcVar3 != NULL))
  {
    kill_card((int)pcVar4->damage_target_player, pcVar4->damage_target_card, KILL_DESTROY);
    return;
  }
  return;
}

int damage_creature(int tgt_player, int tgt_card, int32_t amt, int src_player, int src_card)
{
  unsigned short uVar1;
  uint16_t uVar2;
  uint16_t uVar3;
  uint16_t uVar4;
  uint32_t uVar5;
  bool bVar6;
  unsigned char bVar7;
  card_instance_t *pcVar8;
  card_instance_t *pcVar9;
  int iVar10;
  unsigned int uVar11;
  type_t tVar12;
  card_instance_t *pcVar13;
  card_instance_t *pcVar14;
  int iVar15;
  unsigned int uVar16;
  target_t *ptVar17;
  unsigned int uStack_24;
  type_t tStack_1c;
  char cStack_18;
  uint32_t uStack_14;

  if ((src_player == -1 || amt < 1) || (tgt_player == -1))
  {
    return -1;
  }
  cStack_18 = (char)tgt_player;
  if (tgt_card == -1)
  {
    uStack_14 = add_card_to_hand(tgt_player, damage_card);
    if (uStack_14 == 0xffffffff)
    {
      return -1;
    }
    land_can_be_played = land_can_be_played | 2;
    iVar10 = tgt_player;
  }
  else
  {
    uStack_14 = add_card_to_hand(src_player, damage_card);
    if (uStack_14 == 0xffffffff)
    {
      return -1;
    }
    land_can_be_played = land_can_be_played | 2;
    pcVar8 = get_card_instance(tgt_player, tgt_card);
    iVar10 = src_player;
    if (((pcVar8->token_status & 0x8000000) != 0) &&
        (cards_data[pcVar8->internal_card_id].code_pointer == (CardFunction)0x401010))
    {
      cStack_18 = pcVar8->damage_source_player;
      tgt_player = (int)cStack_18;
      tgt_card = pcVar8->damage_source_card;
    }
  }
  pcVar8 = get_card_instance(iVar10, uStack_14);
  pcVar8->damage_source_player = (int8_t)src_player;
  pcVar8->state = pcVar8->state | 2;
  hand_count[iVar10] = hand_count[iVar10] + -1;
  pcVar8->damage_target_player = cStack_18;
  pcVar8->damage_target_card = tgt_card;
  pcVar8->info_slot = amt;
  pcVar8->damage_source_card = src_card;
  if (src_card == -1)
  {
    pcVar8->display_pic_csv_id = 0xef;
    pcVar8->display_pic_num = 0;
    return uStack_14;
  }
  pcVar9 = get_card_instance(src_player, src_card);
  iVar10 = pcVar9->internal_card_id;
  if (iVar10 == -1)
  {
    uVar1 = pcVar9->backup_internal_card_id;
    tStack_1c = get_type_with_iid(src_player, src_card, (unsigned int)uVar1);
    uStack_24 = pcVar9->state;
    uVar11 =
        (uint16_t)cards_data[uVar1].id | ((uint32_t)(uint16_t)cards_data[uVar1].reserved2 << 16);
  LAB_026e1dec:
    uStack_24 = uStack_24 & 4;
    pcVar8->display_pic_csv_id = (uint16_t)uVar11;
    iVar10 = get_card_image_number(uVar11 & 0xffff, src_player, src_card);
    pcVar8->display_pic_num = (uint16_t)iVar10;
    iVar10 = get_color(src_player, src_card);
    bVar7 = (unsigned char)iVar10;
    bVar6 = true;
    pcVar14 = pcVar9;
    if ((tStack_1c & TYPE_ARTIFACT) != TYPE_NONE)
    {
      bVar7 = bVar7 | 0x40;
    }
  }
  else
  {
    if (iVar10 == activation_card)
    {
      uVar5 = pcVar9->original_internal_card_id;
      pcVar14 = get_card_instance(pcVar9->parent_controller, pcVar9->parent_card);
      tStack_1c = get_type_with_iid(pcVar9->parent_controller, pcVar9->parent_card, uVar5);
      uVar4 = pcVar9->display_pic_csv_id;
      pcVar8->display_pic_csv_id = uVar4;
      pcVar8->display_pic_num = pcVar9->display_pic_num;
      pcVar8->damage_source_player = (int8_t)pcVar9->parent_controller;
      pcVar8->damage_source_card = pcVar9->parent_card;
    }
    else
    {
      uVar2 = cards_data[iVar10].id;
      if ((1 < (unsigned short)(uVar2 - 0x386)) && (uVar2 != 0x38b))
      {
        tStack_1c = get_type_with_iid(src_player, src_card, iVar10);
        uStack_24 = pcVar9->state;
        uVar11 = (uint16_t)cards_data[iVar10].id |
                 ((uint32_t)(uint16_t)cards_data[iVar10].reserved2 << 16);
        goto LAB_026e1dec;
      }
      uVar5 = pcVar9->original_internal_card_id;
      pcVar14 = get_card_instance((int)pcVar9->damage_source_player, pcVar9->damage_source_card);
      tStack_1c =
          get_type_with_iid((int)pcVar9->damage_source_player, pcVar9->damage_source_card, uVar5);
      uVar3 = pcVar9->display_pic_csv_id;
      pcVar8->display_pic_csv_id = uVar3;
      pcVar8->display_pic_num = pcVar9->display_pic_num;
    }
    iVar10 = get_color(src_player, src_card);
    bVar7 = (unsigned char)iVar10;
    if ((tStack_1c & TYPE_ARTIFACT) == TYPE_NONE)
    {
      pcVar8->color = bVar7;
      pcVar8->initial_color = bVar7;
      pcVar8->eot_toughness = uStack_14;
      bVar6 = false;
      goto LAB_026e1e6b;
    }
    uStack_24 = 0;
    bVar6 = false;
    bVar7 = bVar7 | 0x40;
  }
  pcVar8->color = bVar7;
  pcVar8->initial_color = bVar7;
  pcVar8->eot_toughness = uStack_14;
  if (uStack_24 != 0)
  {
    uVar11 = pcVar8->token_status;
    if (current_phase == 0x19)
    {
      pcVar8->token_status = uVar11 | 0x100000;
    }
    else
    {
      if (current_phase == 0x1a)
      {
        uVar11 = uVar11 | 0x40000;
        pcVar8->token_status = uVar11;
      }
      if ((uVar11 & 0x140000) == 0)
        goto LAB_026e1e6b;
    }
    iVar10 = check_special_flags(src_player, src_card, 0x40);
    if (iVar10 != 0)
    {
      pcVar8->targets[4].player = 1 - src_player;
      iVar10 = planeswalker_its_attacking(src_player, src_card);
      pcVar8->targets[4].card = iVar10;
    }
  }
LAB_026e1e6b:
  if (pcVar8->damage_target_card < 0)
  {
    pcVar8->targets[3].card = 0;
    pcVar8->targets[3].player = tStack_1c;
    tVar12 = TYPE_NONE;
  }
  else
  {
    pcVar13 = get_card_instance((int)pcVar8->damage_target_player, pcVar8->damage_target_card);
    pcVar8->targets[3].player = tStack_1c;
    pcVar8->targets[3].card = 0;
    if (pcVar13 == NULL)
    {
      tVar12 = TYPE_NONE;
    }
    else
    {
      tVar12 = get_type((int)pcVar8->damage_target_player, pcVar8->damage_target_card);
    }
  }
  pcVar8->targets[5].card = tVar12;
  pcVar8->regen_status = pcVar14->regen_status & 0x60ffffff;
  pcVar8->ext->sp_keywords = pcVar14->ext->sp_keywords;
  if ((((tStack_1c & TYPE_PERMANENT) == TYPE_NONE) &&
       (iVar10 = is_planeswalker(src_player, src_card), iVar10 == 0)) &&
      (iVar10 = 0, 0 < active_cards_count[src_player]))
  {
    do
    {
      pcVar14 = in_play(src_player, iVar10);
      if ((pcVar14 != NULL) && (iVar15 = is_humiliated(src_player, iVar10, AL_STATIC), iVar15 == 0))
      {
        iVar15 = get_id(src_player, iVar10);
        if ((iVar15 == 0x3927) || (iVar15 = get_id(src_player, iVar10), iVar15 == 0x58f4))
        {
        LAB_026e2086:
          pcVar8->ext->sp_keywords = pcVar8->ext->sp_keywords | 2;
          break;
        }
        iVar15 = get_id(src_player, iVar10);
        if (iVar15 == 0x47e5)
        {
          uVar11 = get_color(src_player, src_card);
          uVar16 = get_sleighted_color_test(src_player, iVar10, 0x10);
          if ((uVar11 & uVar16) != 0)
            goto LAB_026e2086;
        }
      }
      iVar10 = iVar10 + 1;
    } while (iVar10 < active_cards_count[src_player]);
  }
  pcVar8->targets[7].player = 0;
  if (-1 < tgt_card)
  {
    uVar11 = (unsigned int)pcVar9->number_of_targets;
    if (uVar11 != 9 && 8 < pcVar9->number_of_targets)
    {
      uVar11 = 9;
    }
    ptVar17 = pcVar9->targets + uVar11;
    do
    {
      if ((ptVar17->player == tgt_player) && (ptVar17->card == tgt_card))
      {
        pcVar8->targets[7].player = 1;
        break;
      }
      ptVar17 = ptVar17 + -1;
    } while ((target_t *)&pcVar9->internal_card_id != ptVar17);
  }
  if ((bVar6) && (iVar10 = is_being_cast(src_player, src_card), iVar10 != 0))
  {
    pcVar8->targets[7].player = pcVar8->targets[7].player | 2;
    return uStack_14;
  }
  return uStack_14;
}

// Convenient frontend to damage_creature().  Deals dmg damage to targets[0].player/card.  Promotes to parent if an activation card.  Does not validate.
int damage_target0(int player, int card, int dmg)
{
  card_instance_t* instance = get_card_instance(player, card);
  if (instance->internal_card_id == activation_card)
	{
	  player = instance->parent_controller;
	  card = instance->parent_card;
	}
  return damage_creature(instance->targets[0].player, instance->targets[0].card, dmg, player, card);
}

static int remove_indestructible_until_eot(int player, int card, event_t event){

	if( get_card_instance(player, card)->damage_target_player > -1 ){
		card_instance_t* instance = get_card_instance(player, card);
		int p = instance->damage_target_player;
		int c = instance->damage_target_card;

		if( event == EVENT_ABILITIES && affect_me(p, c) ){
			remove_special_ability(p, c, SP_KEYWORD_INDESTRUCTIBLE);
		}
	}

	if( event == EVENT_CLEANUP ){
		kill_card(player, card, KILL_EXILE);
	}

	return 0;
}

int effect_damage(int player, int card, event_t event)
{
  uint16_t uVar1;
  uint16_t uVar2;
  short sVar3;
  uint16_t uVar4;
  int32_t iVar5;
  card_instance_t *pcVar6;
  unsigned int uVar7;
  int iVar8;
  int iVar9;
  int iVar10;
  int iVar11;
  unsigned int uVar12;
  int iVar13;
  int32_t iVar14;
  card_instance_t *pcStack_ec;

  /* DWARF locals share this frame offset; keep every typed view. */
  union
  {
    test_definition_t test;
    token_generation_t token;
    uint8_t bytes[216];
  } recovered_stack_token;

  iVar11 = affected_card_controller;
  if ((land_can_be_played & 4U) != 0)
  {
    if (card != affected_card)
    {
      return 0;
    }
    if (player != affected_card_controller)
    {
      return 0;
    }
    pcVar6 = get_card_instance(player, card);
    if (pcVar6 != NULL)
    {
      iVar9 = pcVar6->targets[4].player;
      if ((iVar9 < 0) && (pcVar6->targets[4].card < 0))
      {
      LAB_026e2736:
        iVar9 = (int)pcVar6->damage_target_player;
        iVar14 = pcVar6->damage_target_card;
      }
      else
      {
        if ((-1 < pcVar6->damage_target_player) && (-1 < pcVar6->damage_target_card))
        {
          iVar9 = is_planeswalker((int)pcVar6->damage_target_player, pcVar6->damage_target_card);
          if (iVar9 == 0)
            goto LAB_026e2736;
          iVar9 = pcVar6->targets[4].player;
        }
        iVar14 = pcVar6->targets[4].card;
      }
      if (((((iVar14 != -1) && (uVar7 = get_abilities(iVar9, iVar14, EVENT_ABILITIES, -1),
                                (uVar7 & 0x35f800) != 0)) &&
            ((pcVar6->state & 0x100000) == 0)) &&
           ((((((unsigned char)pcVar6->initial_color & 0x3e & (int)uVar7 >> 10) != 0 ||
               ((uVar12 = pcVar6->targets[3].player,
                 (uVar7 & 0x200000) != 0 && ((uVar12 & 2) != 0)))) ||
              (((uVar7 & 0x10000) != 0 && ((uVar12 & 0x40) != 0)))) ||
             ((((uVar7 & 0x100000) != 0 && ((uVar12 & 8) != 0)) ||
               (((uVar7 & 0x40000) != 0 && ((uVar12 & 0x30) != 0)))))))) ||
          ((iVar9 =
                has_protection_from_impl(iVar9, iVar14, iVar11, card, IPF_DAMAGE, KEYWORD_0),
            iVar9 != 0 && ((pcVar6->state & 0x100000) == 0))))
      {
        pcVar6->info_slot = 0;
        kill_card(iVar11, card, KILL_BURY);
      }
    }
  }
  if (event != EVENT_DEAL_DAMAGE)
  {
    return 0;
  }
  if (card != affected_card)
  {
    return 0;
  }
  if (player != affected_card_controller)
  {
    return 0;
  }
  pcVar6 = get_card_instance(player, card);
  if (pcVar6 == NULL)
  {
    return 0;
  }
  if ((pcVar6->state & 0x10) != 0)
  {
    return 0;
  }
  iVar11 = pcVar6->info_slot;
  pcVar6->state = pcVar6->state | 0x10;
  pcVar6->token_status = pcVar6->token_status | 0x10000;
  if (iVar11 < 1)
  {
    return 0;
  }
  iVar9 = pcVar6->targets[4].player;
  iVar8 = (int)pcVar6->damage_target_player;
  if ((iVar9 < 0) || (iVar10 = pcVar6->targets[4].card, iVar10 < 0))
  {
  LAB_026e2761:
    iVar9 = pcVar6->damage_target_card;
    pcVar6->targets[4].player = iVar8;
    pcVar6->targets[4].card = iVar9;
    pcStack_ec = NULL;
    if (iVar9 != -1)
    {
      pcStack_ec = in_play(iVar8, iVar9);
      if (pcStack_ec == NULL)
      {
        return 0;
      }
      pcVar6->damage_target_player = (int8_t)pcVar6->targets[4].player;
      pcVar6->damage_target_card = pcVar6->targets[4].card;
    }
  }
  else
  {
    if ((-1 < pcVar6->damage_target_player) && (-1 < pcVar6->damage_target_card))
    {
      iVar9 = is_planeswalker(iVar8, pcVar6->damage_target_card);
      if (iVar9 == 0)
      {
        iVar8 = (int)pcVar6->damage_target_player;
        goto LAB_026e2761;
      }
      iVar10 = pcVar6->targets[4].card;
      iVar9 = pcVar6->targets[4].player;
    }
    pcStack_ec = in_play(iVar9, iVar10);
    if (pcStack_ec == NULL)
    {
      return 0;
    }
    iVar9 = is_planeswalker(pcVar6->targets[4].player, pcVar6->targets[4].card);
    if (iVar9 == 0)
    {
      return 0;
    }
  }
  iVar9 = get_trap_condition(player, 0x205);
  if ((iVar9 < iVar11) || (iVar9 = get_trap_condition(1 - player, 0x205), iVar9 < iVar11))
  {
    set_trap_condition(player, 0x205, iVar11);
    set_trap_condition(1 - player, 0x205, iVar11);
  }
  uVar7 = pcVar6->ext->sp_keywords;
  if ((uVar7 & 2) != 0)
  {
    gain_life_from_lifelink((int)pcVar6->damage_source_player, iVar11);
    uVar7 = pcVar6->ext->sp_keywords;
  }
  if ((uVar7 & 0x20000000) != 0)
  {
    generate_tokens_by_id((int)pcVar6->damage_source_player, -1, 0x4991, iVar11);
  }
  iVar9 = pcVar6->targets[4].card;
  iVar8 = pcVar6->targets[4].player;
  if (iVar9 != -1)
  {
    iVar9 = is_what(iVar8, iVar9, 2);
    if (iVar9 == 0)
    {
      iVar9 = is_planeswalker(pcVar6->targets[4].player, pcVar6->targets[4].card);
      if (iVar9 != 0)
      {
        iVar9 = check_for_special_ability(player, card, 0x40000);
        iVar14 = 0;
        if (iVar9 != 0)
        {
          iVar9 =
              count_counters(pcVar6->targets[4].player, pcVar6->targets[4].card, COUNTER_LOYALTY);
          iVar14 = iVar11 - iVar9;
        }
        remove_counters(player, card, pcVar6->targets[4].player,
                                    pcVar6->targets[4].card, COUNTER_LOYALTY, iVar11);
        if ((pcVar6->targets[3].card & 0x80) != 0)
        {
          set_special_flags(pcVar6->targets[4].player, pcVar6->targets[4].card, 0x200);
          exile_if_would_be_put_into_graveyard(
              (int)pcVar6->damage_source_player, pcVar6->damage_source_card,
              pcVar6->targets[4].player, pcVar6->targets[4].card, 1);
        }
        iVar11 = check_for_special_ability(player, card, 0x40000);
        if (iVar11 != 0)
        {
          pcVar6->state = pcVar6->state & 0xffffffef;
          pcVar6->token_status = pcVar6->token_status & 0xfffeffff;
          land_can_be_played = land_can_be_played | 2;
          iVar5 = pcVar6->targets[4].player;
          pcVar6->damage_target_card = -1;
          pcVar6->damage_target_player = (int8_t)iVar5;
          pcVar6->targets[4].card = -1;
          pcVar6->targets[4].player = -1;
          pcVar6->targets[5].player = 0;
          pcVar6->info_slot = iVar14;
        }
        call_card_function(pcVar6->targets[4].player, pcVar6->targets[4].card,
                           EVENT_STATIC_EFFECTS);
      }
    }
    else
    {
      uVar7 = pcVar6->targets[3].card;
      if ((uVar7 & 0x100) != 0)
      {
        pump_ability_until_eot((int)pcVar6->damage_source_player, pcVar6->damage_source_card,
                               pcVar6->targets[4].player, pcVar6->targets[4].card, 0, 0, 0, 0x400);
        uVar7 = pcVar6->targets[3].card;
      }
      if ((uVar7 & 0x40) != 0)
      {
        tap_card(pcVar6->targets[4].player, pcVar6->targets[4].card);
        uVar7 = pcVar6->targets[3].card;
      }
      if ((uVar7 & 0x80) != 0)
      {
        set_special_flags(pcVar6->targets[4].player, pcVar6->targets[4].card, 0x200);
        exile_if_would_be_put_into_graveyard((int)pcVar6->damage_source_player,
                                             pcVar6->damage_source_card, pcVar6->targets[4].player,
                                             pcVar6->targets[4].card, 1);
        uVar7 = pcVar6->targets[3].card;
      }
      if ((uVar7 & 0x20) != 0)
      {
        cannot_regenerate_until_eot((int)pcVar6->damage_source_player, pcVar6->damage_source_card,
                                    pcVar6->targets[4].player, pcVar6->targets[4].card);
      }
      uVar7 = pcVar6->ext->sp_keywords;
      if ((uVar7 & 0x200) != 0)
      {
        uVar1 = pcVar6->display_pic_csv_id;
        if ((uVar1 == 0x1916) || (iVar9 = 1, uVar1 == 0x1b04))
        {
          iVar9 = (-(unsigned int)((pcVar6->token_status & 0x140000) == 0) & 0xfffffff9) + 8;
        }
        set_special_flags(pcVar6->targets[4].player, pcVar6->targets[4].card, iVar9);
        uVar7 = pcVar6->ext->sp_keywords;
      }
      if ((uVar7 & 0x100000) != 0)
      {
        set_special_flags(pcVar6->targets[4].player, pcVar6->targets[4].card, 0x10);
      }
      iVar14 = pcVar6->targets[10].card;
      if (iVar14 == 0x3c28)
      {
        iVar9 = get_color(pcVar6->targets[4].player, pcVar6->targets[4].card);
        if (iVar9 == 0)
        {
          tap_card(pcVar6->targets[4].player, pcVar6->targets[4].card);
        }
        iVar14 = pcVar6->targets[10].card;
      }
      if (iVar14 == 0x3d8f)
      {
        negate_ability_and_special_ability_until_eot(
            (int)pcVar6->damage_source_player, pcVar6->damage_source_card,
            pcVar6->targets[4].player, pcVar6->targets[4].card, KEYWORD_0, 0x800);
        iVar14 = pcVar6->targets[10].card;
      }
      if (iVar14 == 0x441d)
      {
        pump_ability_until_eot_no_repeat((int)pcVar6->damage_source_player,
                                         pcVar6->damage_source_card, pcVar6->targets[4].player,
                                         pcVar6->targets[4].card, 0, 0x20);
        iVar14 = pcVar6->targets[10].card;
      }
      if (iVar14 == 0x50eb)
      {
        uVar7 = get_color(pcVar6->targets[4].player, pcVar6->targets[4].card);
        uVar12 = get_sleighted_color_test((int)pcVar6->damage_source_player,
                                          pcVar6->damage_source_card, 0x10);
        if ((uVar7 & uVar12) == 0)
        {
          new_default_test_definition(&recovered_stack_token.test, 1,
                                      "Select a land to sacrifice.");
          new_sacrifice((int)pcVar6->damage_source_player, pcVar6->damage_source_card,
                        (int)pcVar6->damage_source_player, SAC_NO_CANCEL,
                        &recovered_stack_token.test);
        }
      }
      if (((pcVar6->ext->sp_keywords & 0x4000) == 0) && ((pcVar6->regen_status & 0x20000000) == 0))
      {
        uVar2 = pcStack_ec->damage_on_card;
        pcStack_ec->damage_on_card = uVar2 + (short)iVar11;
      }
      else
      {
        hack_silent_counters = hack_silent_counters + 1;
        add_counters(player, card, pcVar6->targets[4].player, pcVar6->targets[4].card,
                                 COUNTER_M1_M1, iVar11);
        hack_silent_counters = hack_silent_counters + -1;
      }
    }
    if (ai_is_speculating == 1)
    {
      return 0;
    }
    play_sound_effect(WAV_DAMAGE);
    EXE_FN(void, 0x437e20, void)();
    return 0;
  }
  iVar10 = (int)pcVar6->damage_source_player;
  iVar9 = pcVar6->damage_source_card;
  increase_trap_condition(iVar8, 0, iVar11);
  uVar7 = pcVar6->targets[3].player;
  if ((uVar7 & 0x40) != 0)
  {
    increase_trap_condition(iVar8, 3, iVar11);
    uVar7 = pcVar6->targets[3].player;
  }
  if ((uVar7 & 0x47) == 0)
    goto LAB_026e280f;
  if (iVar8 == 0)
  {
    iVar13 = 0x4000000;
  LAB_026e2de5:
    set_special_flags2(iVar10, iVar9, iVar13);
    uVar7 = pcVar6->targets[3].player;
  }
  else if (iVar8 == 1)
  {
    iVar13 = 0x8000000;
    goto LAB_026e2de5;
  }
  if (((uVar7 & 2) != 0) && ((pcVar6->token_status & 0x140000) != 0))
  {
    increase_trap_condition(iVar8, 0x605, 1);
    iVar13 = has_subtype(iVar10, iVar9, SUBTYPE_ROGUE);
    if (iVar13 != 0)
    {
      increase_trap_condition(iVar10, 0x601, 1);
    }
    iVar13 = has_subtype(iVar10, iVar9, SUBTYPE_GOBLIN);
    if (iVar13 != 0)
    {
      increase_trap_condition(iVar10, 0x602, 1);
    }
    iVar13 = has_subtype(iVar10, iVar9, SUBTYPE_FAERIE);
    if (iVar13 != 0)
    {
      increase_trap_condition(iVar10, 0x609, 1);
    }
    iVar9 = has_subtype(iVar10, iVar9, SUBTYPE_PIRATE);
    if (iVar9 != 0)
    {
      increase_trap_condition(iVar10, 0x40a, 1);
    }
  }
LAB_026e280f:
  if (((iVar10 != iVar8) && ((pcVar6->initial_color & 0x10U) != 0)) &&
      ((pcVar6->targets[3].player & 0x138U) != 0))
  {
    increase_trap_condition(iVar10, 0x603, 1);
  }
  *(int *)(iVar8 * 4 + 0x4ef1ec) = *(int *)(iVar8 * 4 + 0x4ef1ec) + iVar11;
  if ((pcVar6->regen_status & 0x20000000) == 0)
  {
    iVar9 = life[iVar8];
    iVar11 = iVar9 - iVar11;
    if (iVar11 < 7)
    {
      iVar11 =
          dispatch_event_with_initial_event_result(player, card, EVENT_DAMAGE_REDUCTION, iVar11);
      iVar9 = life[iVar8];
    }
    if (iVar11 < iVar9)
    {
      if ((card_flags[iVar8] & 0x10) == 0)
      {
        lose_life(iVar8, iVar9 - iVar11);
      }
    }
    else if (iVar9 < iVar11)
    {
      gain_life(iVar8, iVar11 - iVar9);
    }
  }
  else
  {
    poison(iVar8, iVar11);
  }
  sVar3 = pcVar6->display_pic_csv_id;
  if (sVar3 == 0x3af9)
  {
    if (pcVar6->targets[10].card != 0x3af9)
    {
      return 0;
    }
    default_token_definition(player, card, 0x3afa, &recovered_stack_token.token);
    recovered_stack_token.token.t_player = iVar8;
    generate_token(&recovered_stack_token.token);
    uVar4 = pcVar6->display_pic_csv_id;
    sVar3 = uVar4;
  }
  if ((sVar3 == 0x3257) && (pcVar6->targets[10].card == 0x3257))
  {
    iVar11 = generate_reserved_token_by_id(player, 0x6160);
    pcVar6 = get_card_instance(player, iVar11);
    pcVar6->targets[3].player = iVar8;
    pcVar6->targets[2].player = 0x80;
    pcVar6->targets[2].card = 0x3257;
    create_card_name_legacy(player, iVar11, 0x3257);
  }
  return 0;
}

void after_damage(void)
{
  unsigned short uVar1;
  uint32_t num;
  unsigned int uVar2;
  kill_t kill_mode;
  card_instance_t *instance;
  int iVar4;
  int iVar5;
  int iVar6;

  iVar6 = 0;
  do
  {
    iVar4 = 0;
    if (0 < active_cards_count[iVar6])
    {
      do
      {
        instance = in_play(iVar6, iVar4);
        if ((instance != NULL) &&
            ((iVar5 = is_what(iVar6, iVar4, 2),
              iVar5 != 0 || (iVar5 = is_planeswalker(iVar6, iVar4), iVar5 != 0))))
        {
          uVar2 = check_special_flags(iVar6, iVar4, 0x219);
          if (uVar2 == 0)
          {
          LAB_026e3050:
            if ((uVar2 & 8) == 0)
            {
              if ((uVar2 & 1) == 0)
              {
                iVar5 = is_what(iVar6, iVar4, 2);
                if (iVar5 != 0)
                {
                  instance = get_card_instance(iVar6, iVar4);
                  uVar1 = instance->damage_on_card;
                  iVar5 = get_toughness(iVar6, iVar4);
                  if (iVar5 <= (int)(unsigned int)uVar1)
                    goto LAB_026e316d;
                }
                iVar5 = check_special_flags3(iVar6, iVar4, 0x80);
                if ((iVar5 != 0) && (iVar5 = is_what(iVar6, iVar4, 2), iVar5 != 0))
                {
                  instance = get_card_instance(iVar6, iVar4);
                  num = instance->eot_toughness;
                  if (num != 0)
                  {
                    instance = get_card_instance(iVar6, iVar4);
                    instance->eot_toughness = 0;
                    add_counters(iVar6, iVar4, iVar6, iVar4, COUNTER_P1_P1, num);
                  }
                }
                goto LAB_026e3071;
              }
            LAB_026e316d:
              kill_mode = KILL_DESTROY;
            }
            else
            {
              kill_mode = KILL_BURY;
            }
          }
          else
          {
            remove_special_flags(iVar6, iVar4, 0x219);
            if ((uVar2 & 0x10) == 0)
            {
              if ((uVar2 & 0x200) != 0)
              {
                instance = get_card_instance(iVar6, iVar4);
                uVar1 = instance->damage_on_card;
                iVar5 = get_toughness(iVar6, iVar4);
                if (iVar5 <= (int)(unsigned int)uVar1)
                  goto LAB_026e30ed;
              }
              goto LAB_026e3050;
            }
          LAB_026e30ed:
            kill_mode = KILL_EXILE;
          }
          kill_card(iVar6, iVar4, kill_mode);
        }
      LAB_026e3071:
        iVar4 = iVar4 + 1;
      } while (iVar4 < active_cards_count[iVar6]);
    }
    iVar6 = iVar6 + 1;
    if (iVar6 == 2)
    {
      iVar4 = 1;
      iVar6 = 0;
      do
      {
        iVar5 = 0;
        if (0 < active_cards_count[iVar6])
        {
          do
          {
            instance = in_play(iVar6, iVar5);
            if (instance != NULL)
            {
              dispatch_event_with_attacker_to_one_card(iVar6, iVar5, EVENT_AFTER_DAMAGE, iVar4, -1);
            }
            iVar5 = iVar5 + 1;
          } while (iVar5 < active_cards_count[iVar6]);
        }
        iVar6 = iVar6 + 1;
        iVar4 = iVar4 + -1;
      } while (iVar6 != 2);
      return;
    }
  } while (true);
}

// Returns a damage card during EVENT_PREVENT_DAMAGE if it's combat damage and doing more than 0.
card_instance_t* combat_damage_being_prevented(event_t event)
{
  card_instance_t* damage;
  if (event == EVENT_PREVENT_DAMAGE
	  && (damage = get_card_instance(affected_card_controller, affected_card))
	  && damage->internal_card_id == damage_card
	  && (damage->token_status & (STATUS_COMBAT_DAMAGE|STATUS_FIRST_STRIKE_DAMAGE))
	  && damage->info_slot > 0)
	return damage;

  return NULL;
}

// Returns a damage card during EVENT_PREVENT_DAMAGE if it's noncombat damage and doing more than 0.
card_instance_t* noncombat_damage_being_prevented(event_t event)
{
  card_instance_t* damage;
  if (event == EVENT_PREVENT_DAMAGE
	  && (damage = get_card_instance(affected_card_controller, affected_card))
	  && damage->internal_card_id == damage_card
	  && !(damage->token_status & (STATUS_COMBAT_DAMAGE|STATUS_FIRST_STRIKE_DAMAGE))
	  && damage->info_slot > 0)
	return damage;

  return NULL;
}

// Returns a damage card during EVENT_PREVENT_DAMAGE if it's doing more than 0.
card_instance_t* damage_being_prevented(event_t event)
{
  card_instance_t* damage;
  if (event == EVENT_PREVENT_DAMAGE
	  && (damage = get_card_instance(affected_card_controller, affected_card))
	  && damage->internal_card_id == damage_card
	  && damage->info_slot > 0)
	return damage;

  return NULL;
}

/* Returns a damage card during EVENT_DEAL_DAMAGE if it's combat damage and doing more than 0.  damage->targets[16].player will hold the damage dealt if it's
 * determinable, even in the case of wither/infect damage; this is no more robust than the previous workarounds were. */
card_instance_t* combat_damage_being_dealt(event_t event)
{
  card_instance_t* damage;
  if (event == EVENT_DEAL_DAMAGE
	  && (damage = get_card_instance(affected_card_controller, affected_card))
	  && damage->internal_card_id == damage_card
	  && (damage->token_status & (STATUS_COMBAT_DAMAGE|STATUS_FIRST_STRIKE_DAMAGE)))
	{
	  if (damage->info_slot > 0)
		{
		  damage->targets[16].player = damage->info_slot;
		  return damage;
		}

	  card_instance_t* source = get_card_instance(damage->damage_source_player, damage->damage_source_card);
	  if (source->targets[16].player > 0)
		{
		  damage->targets[16].player = source->targets[16].player;
		  return damage;
		}
	}

  return NULL;
}

/* Returns a damage card during EVENT_DEAL_DAMAGE if it's noncombat damage and doing more than 0.  damage->targets[16].player will hold the damage dealt if it's
 * determinable, even in the case of wither/infect damage; this is no more robust than the previous workarounds were. */
card_instance_t* noncombat_damage_being_dealt(event_t event)
{
  card_instance_t* damage;
  if (event == EVENT_DEAL_DAMAGE
	  && (damage = get_card_instance(affected_card_controller, affected_card))
	  && damage->internal_card_id == damage_card
	  && !(damage->token_status & (STATUS_COMBAT_DAMAGE|STATUS_FIRST_STRIKE_DAMAGE)))
	{
	  if (damage->info_slot > 0)
		{
		  damage->targets[16].player = damage->info_slot;
		  return damage;
		}

	  card_instance_t* source = get_card_instance(damage->damage_source_player, damage->damage_source_card);
	  if (source->targets[16].player > 0)
		{
		  damage->targets[16].player = source->targets[16].player;
		  return damage;
		}
	}

  return NULL;
}

/* Returns a damage card during EVENT_DEAL_DAMAGE if it's doing more than 0.  damage->targets[16].player will hold the damage dealt if it's determinable, even
 * in the case of wither/infect damage; this is no more robust than the previous workarounds were. */
card_instance_t* damage_being_dealt(event_t event)
{
  card_instance_t* damage;
  if (event == EVENT_DEAL_DAMAGE
	  && (damage = get_card_instance(affected_card_controller, affected_card))
	  && damage->internal_card_id == damage_card)
	{
	  if (damage->info_slot > 0)
		{
		  damage->targets[16].player = damage->info_slot;
		  return damage;
		}

	  card_instance_t* source = get_card_instance(damage->damage_source_player, damage->damage_source_card);
	  if (source->targets[16].player > 0)
		{
		  damage->targets[16].player = source->targets[16].player;
		  return damage;
		}
	}

  return NULL;
}

// Returns nonzero if this damage card is being dealt to a planeswalker.
int damage_is_to_planeswalker(card_instance_t* damage)
{
  return (damage->damage_target_card == -1
		  ? ((damage->token_status & (STATUS_FIRST_STRIKE_DAMAGE|STATUS_COMBAT_DAMAGE))
			 && check_special_flags(damage->damage_source_player, damage->damage_source_card, SF_ATTACKING_PWALKER))
		  : is_planeswalker(damage->damage_target_player, damage->damage_target_card));
}

static int if_attached_creature_dies_do_something(int player, int card, event_t event)
{

  card_instance_t *instance = get_card_instance(player, card);
  int p = instance->damage_target_player;
  int c = instance->damage_target_card;

  int mode = instance->targets[1].card;
  // modes
  // 1<<0 --> Controller of source gains life equal to attached creature toughness [Abattoir Ghoul]
  // 1<<1 --> Controller of source may draw a card [Rot Wolf]

  if (p > -1)
  {
    if (event == EVENT_GRAVEYARD_FROM_PLAY)
    {
      if (affect_me(p, c) && in_play(p, c) && get_card_instance(p, c)->kill_code > 0 &&
          get_card_instance(p, c)->kill_code < KILL_EXILE &&
          !check_special_flags2(p, c, SF2_WILL_BE_EXILED_IF_PUTTED_IN_GRAVE) &&
          !check_for_death_trigger_removal(p, c))
      {
        if (mode && (mode & 1))
        {
          int toughness = get_toughness(p, c);
          instance->targets[1].player = (card_counts[player][5] + 1) * toughness;
        }
        instance->targets[11].player = 1; // Otherwise "resolve_gfp_ability" won't trigger
        instance->damage_target_player = instance->damage_target_card = -1;
      }
    }
  }

  // This check is mandatory or the legacy card will be removed before EVENT_GRAVEYARD_ABILITY if
  // source is killed in combat
  if (instance->targets[0].player > -1 && current_phase != PHASE_NORMAL_COMBAT_DAMAGE &&
      current_phase != PHASE_FIRST_STRIKE_DAMAGE)
  {
    other_leaves_play(player, card, instance->targets[0].player, instance->targets[0].card, event,
                      0, 0);
    if (event == EVENT_LEAVES_PLAY_ABILITY)
    {
      kill_card(player, card, KILL_EXILE);
    }
  }

  if (resolve_gfp_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY))
  {
    if (mode && (mode & 1) && instance->targets[1].player > 0)
    {
      gain_life(instance->targets[0].player, instance->targets[1].player);
    }
    if (mode && (mode & 2))
    {
      int amount = card_counts[player][5] + 1;
      do
      {
        if (do_dialog(instance->targets[0].player, player, card, -1, -1, " Draw a card\n Pass",
                      0) == 0)
          draw_cards(instance->targets[0].player, 1);
      } while (--amount != 0);
    }
    kill_card(player, card, KILL_EXILE);
  }

  if (current_phase == PHASE_CLEANUP)
  {
    kill_card(player, card, KILL_EXILE);
  }

  return 0;
}

void if_a_creature_damaged_by_me_dies_do_something(int player, int card, event_t event, int mode){
	// modes
	// 1<<0 --> Controller of source gains life equal to attached creature toughness [Abattoir Ghoul]
	// 1<<1 --> Controller of source may draw a card [Rot Wolf]

	card_instance_t *instance = get_card_instance(player, card);

	if( ! in_play(player, card) || is_humiliated(player, card, AL_TRIG) ){
		return;
	}

	if( event == EVENT_DEAL_DAMAGE ){
		card_instance_t *damage= get_card_instance( affected_card_controller, affected_card);
		if( damage->internal_card_id == damage_card ){
			if( damage->damage_target_card != -1 && damage->damage_source_player == player &&
				damage->damage_source_card == card && damage->info_slot > 0
			  ){
				if( instance->info_slot < 0 ){
					instance->info_slot = 0;
				}
				if( instance->info_slot < 10 ){
					instance->targets[instance->info_slot].player = damage->damage_target_player;
					instance->targets[instance->info_slot].card = damage->damage_target_card;
					instance->info_slot++;
				}
			}
		}
	}

	if( instance->info_slot > 0 && trigger_condition == TRIGGER_DEAL_DAMAGE && affect_me(player, card) && reason_for_trigger_controller == player ){
		int i;
		for(i=0; i<instance->info_slot; i++){
			int legacy = create_targetted_legacy_effect(player, card, &if_attached_creature_dies_do_something,
														instance->targets[i].player, instance->targets[i].card);
			card_instance_t *leg = get_card_instance(player, legacy);
			leg->targets[0].player = player;
			leg->targets[0].card = card;
			leg->number_of_targets = 1;
			leg->targets[1].card = mode;
		}
		instance->info_slot = 0;
	}
}

