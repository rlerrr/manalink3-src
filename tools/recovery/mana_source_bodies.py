"""Typed reconstructions of land/creature mana sources and replacement queries."""

BODIES = {
'266bc20': r'''
int all_lands_are_basiclandtype_2022(int player, int card, int event, int whose_lands,
                                    int land_color, int land_subtype)
{
  if (event == EVENT_MODIFY_MANA_PROD && in_play(player, card))
  {
    for (int p = 0; p < 2; ++p)
      if (p == whose_lands || whose_lands == ANYBODY)
      {
        mana_colors_added[p] |= basic_subtype_to_color_test(
            get_hacked_subtype(player, card, land_subtype));
        mana_colors_added[p] |= color_to_color_test(get_hacked_color(player, card, land_color)) & 0x7f;
      }
  }
  else if (event == EVENT_CHANGE_TYPE && in_play(player, card) &&
           (affected_card_controller == whose_lands || whose_lands == ANYBODY))
  {
    card_instance_t *instance = get_card_instance(player, card);
    /* Native code shifts by the converted color-test value, not the color index. */
    unsigned int col = 1U << (color_to_color_test(get_hacked_color(player, card, land_color)) & 31);
    if ((unsigned int)instance->info_slot != col)
    {
      instance->info_slot = col;
      int subtype = get_hacked_subtype(player, card, land_subtype);
      for (int p = 0; p < 2; ++p)
        if (p == whose_lands || whose_lands == ANYBODY)
          for (int c = 0; c < active_cards_count[p]; ++c)
            if (in_play(p, c) && is_what(p, c, TYPE_LAND))
              add_a_subtype(p, c, subtype);
      col = 1U << (color_to_color_test(get_hacked_color(player, card, land_color)) & 31);
      get_card_instance(player, card)->info_slot = col;
    }
  }
  return 0;
}
''',

'266f670': r'''
int get_colors_of_mana_land_could_produce_ignoring_costs_2022(int player,int card)

{
  uint id_word;
  card_data_t *card_data;
  card_instance_t *instance;
  int count;
  uint colors;
  ushort id16;
  
                    
  card_data = get_card_data(player,card);
  if ((card_data->extra_ability & 0x1000) == 0) {
    return mana_colors_added[player];
  }
  STATIC_ASSERT(offsetof(card_data_t, id) == 0x24, mana_card_data_id_offset);
  memcpy(&id_word, (const unsigned char *)card_data + offsetof(card_data_t, id), sizeof(id_word));
  id16 = (ushort)id_word;
  if (multi_mana_transmutation[player][0] != 0) {
    count = mana_doublers(player,TYPE_LAND);
    if (((1 < count) && ((card_data->type & 1) != 0)) &&
       (((card_data->extra_ability & 0x1000) != 0 || (mana_colors_added[player] != 0))))
    goto LAB_0266f8d8;
    colors = multi_mana_transmutation[player][0];
    if (colors == 0) goto LAB_0266f6a9;
    if (id16 == 0xf36) {
      count = count_subtype(player,4,-1);
      goto joined_r0x0266fd01;
    }
    if (id16 < 0xf37) {
      if (0x6ea < id16) {
        if (id16 < 0x723) {
          if (0x6fb < id16) {
            switch(id_word - 0x6fc & 0xffff) {
            case 0:
            case 3:
code_r0x0266faef:
              return colors | 8;
            case 0x10:
            case 0x1e:
code_r0x0266fb10:
              return colors | 0x20;
            case 0x1f:
            case 0x26:
code_r0x0266fb53:
              return colors | 4;
            }
            goto LAB_0266f6a9;
          }
        }
        else {
          if (id16 == 0xa83) {
            return colors;
          }
          if (id16 < 0xa84) {
            if (id16 == 0x92e) {
              return colors;
            }
            if (id16 < 0x92f) {
              if (id16 == 0x8d4) {
                return colors;
              }
              if (id16 == 0x927) {
                return colors;
              }
            }
            else {
              if (id16 == 0xa47) {
                return colors;
              }
              if (id16 == 0xa7b) {
                return colors;
              }
            }
          }
          else if (id16 < 0xaab) {
            if ((0xa8b < id16) && ((1 << ((char)id_word + 0x74U & 0x1f) & 0x50000001U) != 0)) {
              return colors;
            }
          }
          else {
            if (id16 == 0xbba) {
              return colors;
            }
            if (id16 == 0xf0e) {
              return colors;
            }
          }
        }
        goto LAB_0266f6b9;
      }
      if (0x6bd < id16) {
        switch(id_word - 0x6be & 0xffff) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
          goto LAB_0266f82c;
        case 0x1b:
        case 0x2c:
          goto LAB_0266fcd6;
        case 0x27:
        case 0x29:
LAB_0266fcde:
          return colors | 0x10;
        }
        goto LAB_0266f6a9;
      }
      if (id16 != 0x5e8) {
        if (id16 < 0x5e9) {
          if (id16 == 0x5e1) {
            count = count_subtype(player,1,0x5009);
            goto joined_r0x0266fd01;
          }
        }
        else {
          if (id16 == 0x5e9) {
            count = count_subtype(player,2,-1);
            goto joined_r0x0266fd01;
          }
          if (id16 == 0x60f) {
            return colors;
          }
        }
        goto LAB_0266f6b9;
      }
      count = count_subtype(player,0x40,-1);
joined_r0x0266fd01:
      if (1 < count) {
LAB_0266f8d8:
        return multi_mana_transmutation[player][0];
      }
    }
    else {
      if (id16 != 0x4413) {
        if (id16 < 0x4414) {
          if (id16 == 0x1410) {
LAB_0266fcd6:
            return colors | 2;
          }
          if (id16 < 0x1411) {
            if (id16 == 0x138b) goto LAB_0266fcde;
            if (id16 < 0x138c) {
              if (id16 == 0x1340) goto code_r0x0266fb53;
              if (id16 < 0x11f7) {
                if (id16 < 0x11d8) {
                  if (id16 == 0x1188) {
                    return colors;
                  }
                  if (id16 == 0x11c6) {
                    return colors;
                  }
                }
                else if ((1 << ((char)id_word + 0x28U & 0x1f) & 0x40400001U) != 0) {
                  return colors;
                }
              }
            }
            else if (id16 == 0x1399) goto code_r0x0266fb10;
          }
          else {
            if (id16 == 0x15f7) {
              return colors;
            }
            if (id16 < 0x15f8) {
              if (id16 == 0x1420) goto code_r0x0266faef;
              if (0x141f < id16) {
                if (id16 == 0x1595) {
                  return colors;
                }
                if (id16 == 0x1598) {
                  return colors;
                }
                if (id16 == 0x1538) {
                  return colors;
                }
              }
            }
            else {
              if (id16 == 0x25fd) {
                return colors;
              }
              if (id16 < 0x25fe) {
                if (id16 == 0x22ea) {
                  return colors;
                }
                if (0x22ea < id16) {
                  if (id16 == 0x258e) {
                    return colors;
                  }
                  if (id16 == 0x25d8) {
                    return colors;
                  }
                  if (id16 == 0x253c) {
                    return colors;
                  }
                  if ((ushort)(id16 + 0xdbae) < 5) {
                    return 0x3e;
                  }
                  goto LAB_0266f6d5;
                }
                if (((ushort)(id_word - 0x163d) < 0x1e) &&
                   ((1 << ((byte)(id_word - 0x163d) & 0x1f) & 0x20000101U) != 0)) {
                  return colors;
                }
              }
              else {
                if (id16 == 0x2684) {
                  return colors;
                }
                if (id16 < 0x2685) {
                  if (id16 == 0x2637) {
                    return colors;
                  }
                  if (id16 == 0x263d) {
                    return colors;
                  }
                  if (id16 == 0x2615) {
                    return colors;
                  }
                }
                else {
                  if (id16 == 0x41df) {
                    return colors;
                  }
                  if (id16 == 0x43f6) {
                    return colors;
                  }
                  if (id16 == 0x26a6) {
                    return colors;
                  }
                }
              }
            }
          }
        }
        else {
          if (id16 == 0x477e) {
            count = count_subtype(player,0x40,-1);
            if (1 < count) {
              colors = multi_mana_transmutation[player][0];
              goto LAB_0266fcd6;
            }
            goto LAB_0266f928;
          }
          if (id16 < 0x477f) {
            if (id16 == 0x46f1) {
              if (1 < life[player]) {
                return colors;
              }
              goto LAB_0266f928;
            }
            if (id16 == 0x4728) {
              return colors;
            }
          }
          else if (id16 == 0x47bd) {
            count = count_basic_lands(player,0x5009);
            goto joined_r0x0266fd01;
          }
        }
        goto LAB_0266f6b9;
      }
      count = count_subtype(player,2,-1);
      if (1 < count) {
        return multi_mana_transmutation[player][0] | 3;
      }
    }
LAB_0266f928:
    colors = 0xffffffff;
LAB_0266f82c:
    return colors;
  }
LAB_0266f6a9:
  if ((ushort)(id16 + 0xdbae) < 5) {
    return 0x3e;
  }
LAB_0266f6b9:
  if (id16 == 0x1d96) {
    return 0x3e;
  }
  if ((id_word & 0xffff) == 0x1b06) {
    return 0x3e;
  }
LAB_0266f6d5:
  colors = id_word & 0xffff;
  if ((((colors == 0x2429) || (colors == 0x3028)) || (colors == 0x3515 || colors == 0x41df)) ||
     (colors == 0x927)) {
    return 0x3e;
  }
  if (((((colors != 0x2df9) && (colors != 0x1340)) &&
       ((colors != 0x138b && ((colors != 0x1399 && ((id_word - 0x1410 & 0xffef) != 0)))))) &&
      (colors != 0x2170)) &&
     (((((colors != 0x2318 && (3 < (ushort)(id16 + 0xe901))) && (colors != 0x232d)) &&
       ((colors != 0x366 && (colors != 0x34b6)))) &&
      (((colors != 0x22a6 && ((colors != 0x353d && (colors != 0x386f)))) &&
       ((colors != 0x5020 && (colors != 0x43a9)))))))) {
    if ((colors != 0xcac) && (colors != 0x64e)) {
      if (colors != 0x2697) {
                    
        instance = get_card_instance(player,card);
        return (instance->field_26).info_slot | mana_colors_added[player] | (uint)instance->mana_color;
      }
      return 0x3f;
    }
                    
    instance = get_card_instance(player,card);
    return mana_colors_added[player] | (uint)instance->mana_color;
  }
                    
  instance = get_card_instance(player,card);
  return (instance->field_26).info_slot | mana_colors_added[player];
}
''',

'266b9d0': r'''
int tap_a_permanent_you_control_for_mana_2022(int player, int card, event_t event, type_t type, int32_t subtype, color_t color, int amount){
	if (event == EVENT_CAN_ACTIVATE && can_use_activated_abilities(player, card)){
		return count_untapped_subtype(player, type, subtype) > 0;
	}

	if (event == EVENT_ACTIVATE ){

		target_definition_t td;
		default_target_definition(player, card, &td, type);
		td.allowed_controller = player;
		td.preferred_controller = player;
		td.illegal_state = TARGET_STATE_TAPPED;
		td.illegal_abilities = 0;
		if (subtype > 0){
			td.required_subtype = subtype;
		}

		if( pick_target(&td, type == TYPE_CREATURE ? "TARGET_CREATURE" : type == TYPE_LAND ? "TARGET_LAND" : type == TYPE_ARTIFACT ? "TARGET_ARTIFACT" : "TARGET_PERMANENT") ){
			card_instance_t* instance = get_card_instance(player, card);
			instance->number_of_targets = 1;
			
			produce_mana(player, color, amount);
			tapped_for_mana_color = -2;	
			if (instance->targets[0].player == player && instance->targets[0].card == card){
				
				instance->state |= STATE_TAPPED;
			} 
			else {
				get_card_instance(instance->targets[0].player, instance->targets[0].card)->state |= STATE_TAPPED;
				dispatch_event(instance->targets[0].player, instance->targets[0].card, EVENT_TAP_CARD);
			}
		} 
		else {
			cancel = 1;
		}
	}

	if (event == EVENT_COUNT_MANA && affect_me(player, card)){
		
		int count = amount * count_untapped_subtype(player, type, subtype);
		if (count > 0){
			declare_mana_available(player, color, count);
		}
	}

	return 0;
}
''',

'266ca00': r'''
int permanents_you_control_can_tap_for_mana_all_one_color_2022(int player, int card, event_t event, type_t type, int32_t subtype, color_test_t available, int num_mana){
	if ((available & (COLOR_TEST_ANY | COLOR_TEST_ARTIFACT)) == 0){
		
		return 0;
	}
	
	if (event == EVENT_CAN_ACTIVATE && !is_humiliated_for_ability(player, card, AL_STATIC) ){
		return check_for_untapped_nonsick_subtype(player, type, subtype);
	}

	if (event == EVENT_ACTIVATE ){

		target_definition_t td;
		default_target_definition(player, card, &td, type);
		td.allowed_controller = player;
		td.preferred_controller = player;
		td.illegal_state = TARGET_STATE_TAPPED | TARGET_STATE_SUMMONING_SICK;
		td.illegal_abilities = 0;
		if (subtype > 0){
			td.required_subtype = subtype;
		}

		if( pick_target(&td, type == TYPE_CREATURE ? "TARGET_CREATURE" : type == TYPE_LAND ? "TARGET_LAND" : "TARGET_PERMANENT") ){
			card_instance_t* instance = get_card_instance(player, card);
			instance->number_of_targets = 1;
			produce_mana_tapped_all_one_color_with_default_impl(instance->targets[0].player, instance->targets[0].card, available, num_mana, COLOR_TEST_COLORLESS, 1, 1);
			if (!(instance->targets[0].player == player && instance->targets[0].card == card)){
				
				dispatch_event(instance->targets[0].player, instance->targets[0].card, EVENT_TAP_CARD);
			}
		} else {
			cancel = 1;
		}
	}

	if (event == EVENT_COUNT_MANA && affect_me(player, card)){
		
		int count = count_untapped_nonsick_subtype(player, type, subtype) * num_mana;
		if (count > 0)
            declare_mana_available_any_combination_of_colors_impl_2022(player, available, count);
	}

	return 0;
}
''',

'266d250': r'''
int permanents_you_control_can_tap_for_mana_2022(int player, int card, event_t event, type_t type, int32_t subtype, color_t color, int amount){
	if (event == EVENT_CAN_ACTIVATE){
		return check_for_untapped_nonsick_subtype(player, type, subtype);
	}

	if (event == EVENT_ACTIVATE ){

		target_definition_t td;
		default_target_definition(player, card, &td, type);
		td.allowed_controller = player;
		td.preferred_controller = player;
		td.illegal_state = TARGET_STATE_TAPPED | TARGET_STATE_SUMMONING_SICK;
		td.illegal_abilities = 0;
		if (subtype > 0){
			td.required_subtype = subtype;
		}

		if( pick_target(&td, type == TYPE_CREATURE ? "TARGET_CREATURE" : type == TYPE_LAND ? "TARGET_LAND" : "TARGET_PERMANENT") ){
			card_instance_t* instance = get_card_instance(player, card);
			instance->number_of_targets = 1;
			produce_mana_tapped_impl(instance->targets[0].player, instance->targets[0].card, color, amount, 1, 1);
			if (!(instance->targets[0].player == player && instance->targets[0].card == card)){
				
				dispatch_event(instance->targets[0].player, instance->targets[0].card, EVENT_TAP_CARD);
			}
		} else {
			cancel = 1;
		}
	}

	if (event == EVENT_COUNT_MANA && affect_me(player, card)){
		
		int count = amount * count_untapped_nonsick_subtype(player, type, subtype);
		if (count > 0){
			declare_mana_available(player, color, count);
		}
	}

	return 0;
}
''',

'266de60': r'''
int sac_land_tapped_2022(int player, int card, event_t event, int base_color, int color1, int color2)
{
  if (!is_humiliated_for_ability(player, card, AL_TRIG))
    comes_into_play_tapped(player, card, event);
  return sac_land_2022(player, card, event, base_color, color1, color2);
}
''',

'266d9b0': r'''
int two_mana_land_2022(int player, int card, event_t event, color_t color1, color_t color2)
{
  switch (event)
  {
    case EVENT_RESOLVE_SPELL:
      play_land_sound_effect(player, card);
      return 0;
    case EVENT_CAN_ACTIVATE:
      return !is_tapped(player, card) && !is_animated_and_sick(player, card) &&
             can_produce_mana(player, card);
    case EVENT_ACTIVATE:
      if (check_special_flags2(player, card, SF2_CONTAMINATION))
      {
        declare_mana_available(player, mana_color_override[player][color2], -1);
        declare_mana_available(player, mana_color_override[player][color1], -1);
      }
      else
      {
        declare_mana_available(player, color2, -1);
        declare_mana_available(player, color1, -1);
      }
      produce_mana_tapped2_impl(player, card, color1, 1, color2, 1, 1);
      return 0;
    case EVENT_COUNT_MANA:
      if (!affect_me(player, card) ||
          !two_mana_land_2022(player, card, EVENT_CAN_ACTIVATE, color1, color2))
        return 0;
      if (mana_colors_added[player])
      {
        declare_mana_available_maybe_hex_2022(player, color_to_color_test(color1) | mana_colors_added[player], 1);
        declare_mana_available_maybe_hex_2022(player, color_to_color_test(color2) | mana_colors_added[player], 1);
      }
      else if (multi_mana_transmutation[player][0])
        declare_mana_available(player,
            single_color_test_bit_to_color_2022(multi_mana_transmutation[player][0]),
            multi_mana_transmutation[player][1]);
      else if (check_special_flags2(player, card, SF2_CONTAMINATION))
      {
        declare_mana_available(player, mana_color_override[player][color2], 1);
        declare_mana_available(player, mana_color_override[player][color1], 1);
      }
      else
      {
        declare_mana_available(player, color2, 1);
        declare_mana_available(player, color1, 1);
      }
      return 0;
    default:
      return 0;
  }
}
''',

'266e5e0': r'''
int mana_producing_creature_multi_2022(int player, int card, event_t event, int reluctance_to_fight, int colorless, int black, int blue, int green, int red, int white){
	if (event == EVENT_CAN_ACTIVATE){
		return !is_tapped(player, card) && !is_sick(player, card) && can_produce_mana(player, card)
			&& (player != AI || colorless > 0 || black > 0 || blue > 0 || green > 0 || red > 0 || white > 0) && !check_special_flags2(player, card, SF2_KATABATIC_WINDS);
	} else if (event == EVENT_ACTIVATE){
		produce_mana_tapped_multi_impl(player, card, colorless, black, blue, green, red, white, 1);
	} else if (event == EVENT_COUNT_MANA){
		if (affect_me(player, card) && mana_producing_creature_multi_2022(player, card, EVENT_CAN_ACTIVATE, reluctance_to_fight, colorless, black, blue, green, red, white)){
			declare_mana_available(player, COLOR_COLORLESS, colorless);
			declare_mana_available(player, COLOR_BLACK, black);
			declare_mana_available(player, COLOR_BLUE, blue);
			declare_mana_available(player, COLOR_GREEN, green);
			declare_mana_available(player, COLOR_RED, red);
			declare_mana_available(player, COLOR_WHITE, white);
		}
	} else if (reluctance_to_fight && (event == EVENT_ATTACK_RATING || event == EVENT_BLOCK_RATING) && affect_me(player, card)){
		if (event == EVENT_ATTACK_RATING){
			ai_defensive_modifier += reluctance_to_fight / (landsofcolor_controlled[player][COLOR_ANY] + 2);
		} else {	
			ai_defensive_modifier -= 4*reluctance_to_fight / (landsofcolor_controlled[player][COLOR_ANY] + 2);
		}
	}
	return 0;
}
''',

'266d050': r'''
int mana_producing_creature_2022(int player, int card, event_t event, int reluctance_to_fight, color_t color, int amount){
	if (event == EVENT_CAN_ACTIVATE){
		return !is_tapped(player, card) && !is_sick(player, card) && can_produce_mana(player, card) && (player != AI || amount > 0) && !check_special_flags2(player, card, SF2_KATABATIC_WINDS);
	} else if (event == EVENT_ACTIVATE){
		produce_mana_tapped_impl(player, card, color, amount, 1, 1);
	} else if (event == EVENT_COUNT_MANA){
		if (affect_me(player, card) && mana_producing_creature_2022(player, card, EVENT_CAN_ACTIVATE, reluctance_to_fight, color, amount)){
			declare_mana_available(player, color, amount);
		}
	} else if (reluctance_to_fight && (event == EVENT_ATTACK_RATING || event == EVENT_BLOCK_RATING) && affect_me(player, card)){
		if (color == COLOR_COLORLESS || color == COLOR_ARTIFACT){
			color = COLOR_ANY;	
		}
		if (event == EVENT_ATTACK_RATING){
			ai_defensive_modifier += reluctance_to_fight / (landsofcolor_controlled[player][color] + 2);
		} else {	
			ai_defensive_modifier -= 4*reluctance_to_fight / (landsofcolor_controlled[player][color] + 2);
		}
	}
	return 0;
}
''',

'266f1a0': r'''
int enchanted_land_has_T_add_n_mana_of_any_one_color_2022(int player, int card, event_t event, int amt)
{
  if (in_play(player, card) && get_card_instance(player, card)->damage_target_player >= 0)
  {
    card_instance_t *instance = get_card_instance(player, card);
    int p = instance->damage_target_player;
    int c = instance->damage_target_card;
    if (event == EVENT_CAN_ACTIVATE)
      return mana_producer_2022(p, c, event);
    if (event == EVENT_ACTIVATE)
    {
      produce_mana_tapped_all_one_color_with_default_impl(
          p, c, COLOR_TEST_ANY_COLORED, amt, COLOR_TEST_COLORLESS, 1, 1);
      if (spell_fizzled != 1)
        dispatch_event(p, c, EVENT_TAP_CARD);
    }
    if (event == EVENT_COUNT_MANA && affect_me(player, card) &&
        mana_producer_2022(p, c, EVENT_CAN_ACTIVATE))
    {
      /* Native conditions use the Aura's player; output can use its land's player. */
      if (multi_mana_transmutation[player][0] &&
          (amt > 1 || (amt == 1 && mana_doublers(player, TYPE_LAND) > 1)))
      {
        int quantity = multi_mana_transmutation[p][1];
        color_test_t colors;
        if (check_special_flags2(p, c, SF2_CONTAMINATION))
          colors = mana_color_override[p][multi_mana_transmutation[p][0]];
        else
          colors = multi_mana_transmutation[p][0];
        declare_mana_available(p, single_color_test_bit_to_color_2022(colors), quantity);
      }
      else if (check_special_flags2(p, c, SF2_CONTAMINATION))
      {
        color_test_t colors = 0;
        for (int i = 0; i < 7; ++i)
        {
          if (mana_quantity_override[p][i] >= 0)
          {
            declare_mana_available(p, mana_color_override[p][i], mana_quantity_override[p][i]);
            return 0;
          }
          colors |= color_to_color_test(mana_color_override[player][i]);
        }
        declare_mana_available_maybe_hex_2022(player, colors, amt);
      }
      else
        declare_mana_available_hex(p, COLOR_TEST_ANY_COLORED, amt);
    }
  }

  if (!((event == EVENT_CAST_SPELL && affect_me(player, card)) || event == EVENT_CAN_CAST ||
        event == EVENT_RESOLVE_SPELL || event == EVENT_CHANGE_TARGET ||
        event == EVENT_CAN_CHANGE_TARGET || event == EVENT_CAN_MOVE_AURA ||
        event == EVENT_MOVE_AURA || event == EVENT_RESOLVE_MOVING_AURA ||
        event == EVENT_POWER || event == EVENT_TOUGHNESS || event == EVENT_ABILITIES ||
        event == EVENT_CHANGE_TYPE || event == EVENT_SET_COLOR ||
        event == EVENT_GET_SELECTED_CARD || event == EVENT_STATIC_EFFECTS))
    return 0;
  target_definition_t td;
  default_target_definition(player, card, &td, TYPE_LAND);
  td.preferred_controller = player;
  td.allowed_controller = player;
  card_instance_t *instance = get_card_instance(player, card);
  if (event == EVENT_CAN_CAST)
    return targeted_aura(player, card, event, &td, NULL);
  if (event == EVENT_CAST_SPELL && affect_me(player, card))
  {
    instance->number_of_targets = 0;
    ai_modifier += 100;
    new_pick_target(&td, "Select target land you control.", 0, 1 | GS_LITERAL_PROMPT);
  }
  if (event == EVENT_RESOLVE_SPELL)
    return targeted_aura(player, card, event, &td, NULL);
  return 0;
}
''',

'266f100': r'''
int mana_producer_tapped_2022(int player, int card, event_t event)
{
  if (!is_humiliated_for_ability(player, card, AL_TRIG))
    comes_into_play_tapped(player, card, event);
  return mana_producer_2022(player, card, event);
}
''',

'266ed00': r'''
int mana_producer_2022(int player,int card,event_t event)

{
  byte bVar1;
  card_instance_t *pcVar2;
  int iVar3;
  color_test_t cVar4;
  uint uVar5;
  color_t color;
  int amount;
  color_test_t cVar6;
  int iStack_14;
  
                    
  pcVar2 = get_card_instance(player,card);
  if (event == EVENT_CHANGE_TYPE) {
    if ((((card == affected_card) && (player == affected_card_controller)) &&
        (iVar3 = pcVar2->targets[0xc].card, iVar3 != -1)) && (pcVar2->parent_card < 0)) {
      event_result = iVar3;
      return 0;
    }
  }
  else {
    switch(event) {
    case EVENT_CAST_SPELL:
      if ((card == affected_card) && (player == affected_card_controller)) {
        ai_modifier = ai_modifier + 100;
        return 0;
      }
      break;
    case EVENT_ACTIVATE:
      iVar3 = can_produce_mana(player,card);
      if (iVar3 == 0) {
        spell_fizzled = 1;
        return 0;
      }
                    
      bVar1 = pcVar2->card_color;
      iVar3 = is_what(player,card,1);
      cVar6 = bVar1 & (COLOR_TEST_ARTIFACT|COLOR_TEST_ANY);
      if (iVar3 != 0) {
        cVar6 = cVar6 | mana_colors_added[player];
      }
      uVar5 = num_bits_set(cVar6);
      if ((uVar5 != 1) && (uVar5 = num_bits_set(cVar6), uVar5 != 0)) {
        produce_mana_tapped_all_one_color_with_default_impl
                  (player,card,cVar6,1,COLOR_TEST_COLORLESS,1,1);
        return 0;
      }
      amount = num_bits_set(cVar6);
      color = single_color_test_bit_to_color_2022(cVar6);
      produce_mana_tapped_impl(player,card,color,amount,0,0);
      return 0;
    case EVENT_RESOLVE_SPELL:
      play_land_sound_effect(player,card);
      iVar3 = get_id(player,card);
      pcVar2->targets[0xd].player = iVar3;
      return 0;
    case EVENT_CAN_ACTIVATE:
      iVar3 = is_tapped(player,card);
      if ((iVar3 == 0) && (iVar3 = is_animated_and_sick(player,card), iVar3 == 0)) {
        iVar3 = can_produce_mana(player,card);
        return (uint)(iVar3 != 0);
      }
      break;
    case EVENT_COUNT_MANA:
      if (((card == affected_card) && (player == affected_card_controller)) &&
         ((iVar3 = is_tapped(player,card), iVar3 == 0 &&
          ((iVar3 = is_animated_and_sick(player,card), iVar3 == 0 &&
           (iVar3 = can_produce_mana(player,card), iVar3 != 0)))))) {
                    
        cVar6 = (color_test_t)pcVar2->card_color;
        iVar3 = is_what(player,card,1);
        if (iVar3 != 0) {
          cVar6 = cVar6 | mana_colors_added[player];
        }
        cVar4 = check_special_flags2(player,card,0x800000);
        if ((cVar4 & cVar6 & COLOR_TEST_WHITE) != COLOR_TEST_0) {
          cVar6 = cVar6 & ~COLOR_TEST_WHITE | COLOR_TEST_COLORLESS;
        }
        iVar3 = check_special_flags2(player,card,0x20000000);
        if (iVar3 != 0) {
          cVar6 = COLOR_TEST_BLUE;
        }
        iVar3 = check_special_flags2(player,card,0x40);
        if (iVar3 != 0) {
                    
          iVar3 = 0;
          do {
            cVar4 = color_to_color_test(iVar3);
            if ((cVar4 & cVar6) != COLOR_TEST_0) {
              uVar5 = color_to_color_test(iVar3);
              cVar4 = color_to_color_test(mana_color_override[player][iVar3]);
              cVar6 = cVar6 & ~uVar5 | cVar4;
            }
            iVar3 = iVar3 + 1;
          } while (iVar3 != 7);
        }
        iStack_14 = 1;
        if (((multi_mana_transmutation[player][0] != 0) &&
            (iVar3 = mana_doublers(player,TYPE_LAND), 1 < iVar3)) &&
           (iVar3 = is_what(player,card,1), iVar3 != 0)) {
          iStack_14 = multi_mana_transmutation[player][1];
          cVar6 = multi_mana_transmutation[player][0];
        }
        if ((cVar6 & (COLOR_TEST_ARTIFACT|COLOR_TEST_ANY)) != COLOR_TEST_0) {
          declare_mana_available_maybe_hex_2022
                    (player,cVar6 & (COLOR_TEST_ARTIFACT|COLOR_TEST_ANY),iStack_14);
          return 0;
        }
      }
    }
  }
  return 0;
}
''',

'266dca0': r'''
int sac_land_2022(int player, int card, event_t event, int base_color, int color1, int color2)
{
  if (event == EVENT_RESOLVE_SPELL)
    get_card_instance(player, card)->info_slot = (1 << base_color) | (1 << color1) | (1 << color2);
  else if (event == EVENT_ACTIVATE)
  {
    int choice = 0;
    if (player != AI && ai_is_speculating != 1 && !ldoubleclicked)
      choice = do_dialog(player, player, card, -1, -1, " One Mana Ability\n Sac Ability\n Cancel", 0);
    if (choice == 1)
    {
      produce_mana_tapped2_impl(player, card, color1, 1, color2, 1, 0);
      kill_card(player, card, KILL_SACRIFICE);
    }
    else if (choice == 2)
      spell_fizzled = 1;
    else
    {
      color_test_t colors = color_to_color_test(base_color) | mana_colors_added[player];
      if (num_bits_set(colors) == 1)
        mana_producer_fixed_2022(player, card, event, base_color);
      else
        produce_mana_tapped_all_one_color_with_default_impl(
            player, card, colors, 1, COLOR_TEST_COLORLESS, 0, 1);
    }
    return 0;
  }
  return mana_producer_fixed_2022(player, card, event, base_color);
}
''',

}
