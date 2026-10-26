"""Typed reconstructions of mana availability, choice, production and Wild Growth helpers."""

BODIES = {
'266a640': r'''
void choose_combination_of_colors_of_mana_to_produce_into_arr_2022(int* cols, int player, color_test_t available, int num_mana, const char* prompt){
	char buf[300];
	int num_left = num_mana;
	for (; num_left > 0; --num_left){
		if (prompt || player == 1 || ai_is_speculating == 1){
			buf[0] = 0;	
		} else {
			if (num_left == num_mana){
				if (num_left == 1){
					prompt = "What kind of mana?";
					buf[0] = 0;
				} else {
					sprintf(buf, "%d mana left", num_left);
				}
			} else {
				char* p = buf + sprintf(buf, "%d mana left (producing ", num_left);
				int i;
				if (cols[COLOR_COLORLESS] > 0){
					p += sprintf(p, "%d", cols[COLOR_COLORLESS]);
				}
				for (i = 0; i < cols[COLOR_WHITE]; ++i){
					*p++ = 'W';
				}
				for (i = 0; i < cols[COLOR_BLUE]; ++i){
					*p++ = 'U';
				}
				for (i = 0; i < cols[COLOR_BLACK]; ++i){
					*p++ = 'B';
				}
				for (i = 0; i < cols[COLOR_RED]; ++i){
					*p++ = 'R';
				}
				for (i = 0; i < cols[COLOR_GREEN]; ++i){
					*p++ = 'G';
				}
				for (i = 0; i < cols[COLOR_ARTIFACT]; ++i){
					*p++ = 'A';
				}
				*p++ = ')';
				*p = 0;
			}
		}
		int result;
        {
          int choice_budget = num_left;
          const char *choice_prompt = prompt ? prompt : buf;

	int i, highest = -1, num_available_needed = 0;
	color_test_t needed = 0;
	int charging[COLOR_ARTIFACT + 1] = { 0 };

	if (needed_mana_colors > 0){	
		
		for (i = COLOR_COLORLESS; i <= COLOR_ARTIFACT; ++i){
			if ((*charge_mana_addr_of_pay_mana_xbugrw)[i] == -1){
				charging[i] = 1000;
			} else {
				charging[i] = ((*charge_mana_addr_of_pay_mana_xbugrw)[i]	
							   - cols[i]									
							   + (charge_mana_addr_of_pre_mana_pool ?		
								  (*charge_mana_addr_of_pre_mana_pool)[i] - mana_pool[player][i] : 0));	
			}
		}

		
		
		for (i = COLOR_COLORLESS; i <= COLOR_ARTIFACT; ++i){
			if (charging[i] < 0){
				if (charging[COLOR_ARTIFACT] > 0 && i != COLOR_ARTIFACT){
					int amt_to_apply = MIN(-charging[i], charging[COLOR_ARTIFACT]);
					charging[COLOR_ARTIFACT] -= amt_to_apply;
					charging[i] += amt_to_apply;
					if (charging[i] >= 0)
						continue;
					
				}
				if (charging[COLOR_COLORLESS] > 0 && i != COLOR_COLORLESS){
					int amt_to_apply = MIN(-charging[i], charging[COLOR_COLORLESS]);
					charging[COLOR_COLORLESS] -= amt_to_apply;
					charging[i] += amt_to_apply;
				}
			} else if (charging[i] > 0){
				needed |= 1<<i;

				if (i != COLOR_COLORLESS && i != COLOR_ARTIFACT && (available & (1<<i))){
					++num_available_needed;

					choice_budget -= charging[i];

					if (highest == -1
						|| charging[i] > charging[highest]){
						highest = i;
					}
				}
			}
		}

		if (choice_budget >= 0){	
			if (highest >= 0){
				{ result = highest; goto mana_choice_done; }
			} else if (always_prompt_for_color){
				highest = choose_any_color_from_available(available);
				if ((charging[COLOR_COLORLESS] > 0 || charging[COLOR_ARTIFACT] > 0) && !always_prompt_for_color){
					{ result = highest; goto mana_choice_done; }
				} else {
					{ result = choose_a_color_exe(player, choice_prompt, 1, highest, available); goto mana_choice_done; }
				}
			}
		}
	}

	if (num_available_needed == 1){	
		{ result = highest; goto mana_choice_done; }
	}

	if (highest == -1 && !always_prompt_for_color){	
		highest = choose_any_color_from_available(available);

		if (needed_mana_colors > 0
			&& (charging[COLOR_COLORLESS] > 0 || charging[COLOR_ARTIFACT] > 0)){	
			{ result = highest; goto mana_choice_done; }
		}
	}

	
	{ result = choose_a_color_exe(player, choice_prompt, 1, highest, available); goto mana_choice_done; }

        }
      mana_choice_done:
		if (cancel == 1 || result == -1){
			cancel = 1;
			return;
		}
		++cols[result];
		prompt = NULL;	
	}
}
''',

'266ab80': r'''
void declare_mana_available_any_combination_of_colors_impl_2022(int player, color_test_t available, int amount)
{
  unsigned int num_bits = num_bits_set(available & (COLOR_TEST_ANY | COLOR_TEST_ARTIFACT));
  for (unsigned int i = 0; i < num_bits; ++i)
    if ((unsigned int)amount + i >= num_bits)
      declare_mana_available_hex(player, available, ((unsigned int)amount + i) / num_bits);
}
''',

'266abf0': r'''
void declare_mana_available_any_combination_of_colors_2022(int player, color_test_t available, int amount)
{
  if (amount > 0)
    declare_mana_available_any_combination_of_colors_impl_2022(player, available, amount);
}
''',

'266ac10': r'''
void produce_mana_multi_2022(int player, int cless, int black, int blue, int green, int red, int white){

	produce_mana(player, COLOR_COLORLESS, cless);
	produce_mana(player, COLOR_BLACK, black);
	produce_mana(player, COLOR_BLUE, blue);
	produce_mana(player, COLOR_GREEN, green);
	produce_mana(player, COLOR_RED, red);
	produce_mana(player, COLOR_WHITE, white);
}
''',

'266acc0': r'''
int single_color_test_bit_to_color_2022(color_test_t col){
	return (col & COLOR_TEST_BLACK ? COLOR_BLACK
			: col & COLOR_TEST_BLUE ? COLOR_BLUE
			: col & COLOR_TEST_GREEN ? COLOR_GREEN
			: col & COLOR_TEST_RED ? COLOR_RED
			: col & COLOR_TEST_WHITE ? COLOR_WHITE
			: col & COLOR_TEST_ARTIFACT ? COLOR_ARTIFACT
			: COLOR_COLORLESS);
}
''',

'266ad20': r'''
void declare_mana_available_maybe_hex_2022(int player, color_test_t colors, int amount)
{
  if (num_bits_set(colors) == 1)
	declare_mana_available(player, single_color_test_bit_to_color_2022(colors), amount);
  else
	declare_mana_available_hex(player, colors, amount);
}
''',

'266ad70': r'''
int produce_mana_any_combination_of_colors_2022(int player, color_test_t available, int num_mana, const char* prompt){
	chosen_colors = 0;
	cancel = 0;
	int cols[COLOR_ARTIFACT + 1] = { 0 };
	choose_combination_of_colors_of_mana_to_produce_into_arr_2022(cols, player, available, num_mana, prompt);
	if (cancel == 1){
		return 0;
	} else {
		int i;
		for (i = COLOR_COLORLESS; i <= COLOR_ARTIFACT; ++i){
			if (cols[i] > 0){
				produce_mana(player, i, cols[i]);
				chosen_colors |= 1 << i;
			}
		}
		return 1;
	}
}
''',

'266b160': r'''
int produce_mana_all_one_color_impl_2022(int player, color_test_t available, int num_mana)
{
  int choice = choose_mana_all_one_color_2022(player, available, num_mana,
                                              "What kind of mana?", COLOR_TEST_COLORLESS);
  if (cancel == 1 || choice == -1)
  {
    cancel = 1;
    return 0;
  }
  produce_mana(player, choice, num_mana);
  chosen_colors |= 1 << choice;
  return 1;
}
''',

'266b520': r'''
int produce_mana_all_one_color_2022(int player, color_test_t available, int num_mana)
{
  chosen_colors = 0;
  if (num_mana <= 0)
    return 1;
  return produce_mana_all_one_color_impl_2022(player, available, num_mana);
}
''',

'266b1f0': r'''
int wga_backend_2022(int player, int card, event_t event, int subtype, produce_mana_t what_to_produce, int colors, int amount){
	target_definition_t td;
	default_target_definition(player, card, &td, TYPE_LAND);
	td.preferred_controller = player;
	if (subtype > 0){
		td.required_subtype = subtype;
	}

	card_instance_t *instance = get_card_instance(player, card);

	switch (event){
		case EVENT_CAN_CAST:
			return can_target(&td);

		case EVENT_CAST_SPELL:
			if (affect_me(player, card)){
				char buffer[100];
				scnprintf(buffer, 100, "Select target %s.", subtype > 0 ? get_hacked_land_text(player, card, "%s", subtype) : "land");
				if (new_pick_target(&td, buffer, 0, 1 | GS_LITERAL_PROMPT)){
					if (player == AI && instance->targets[0].player == player){
						ai_modifier += 30;
						
					}
				} else {
					cancel = 1;
				}
			}
			break;

		case EVENT_RESOLVE_SPELL:
			if (valid_target(&td)){
				instance->damage_target_player = instance->targets[0].player;
				instance->damage_target_card = instance->targets[0].card;
			} else {
				kill_card(player, card, KILL_BURY);
				spell_fizzled = 1;
			}
			instance->number_of_targets = 0;
			break;

		case EVENT_TAP_CARD:
			if (instance->damage_target_card != -1 && affect_me(instance->damage_target_player, instance->damage_target_card)
				&& tapped_for_mana_color >= 0 && amount > 0){
				if (what_to_produce == PM_ALL_ONE)
              {
                chosen_colors = 0;
                produce_mana_all_one_color_impl_2022(instance->damage_target_player, colors, amount);
              }
              else
                produce_mana_any_combination_of_colors_2022(instance->damage_target_player, colors, amount, NULL);
			}
			break;

		case EVENT_COUNT_MANA:
			if (instance->damage_target_card != -1 && affect_me(instance->damage_target_player, instance->damage_target_card)
				&& !is_tapped(instance->damage_target_player, instance->damage_target_card)
				&& !is_animated_and_sick(instance->damage_target_player, instance->damage_target_card)
				&& can_produce_mana(instance->damage_target_player, instance->damage_target_card)
				&& amount > 0){
				if (what_to_produce == PM_ALL_ONE)
                declare_mana_available_maybe_hex_2022(instance->damage_target_player, colors, amount);
              else
                declare_mana_available_any_combination_of_colors_impl_2022(instance->damage_target_player, colors, amount);
			}
			break;

		default: break;
	}
	return 0;
}
''',

'266b550': r'''
void produce_mana_of_any_type_tapped_for_2022(int player, int card, int amount){
	chosen_colors = 0;
	switch (tapped_for_mana_color){
		case 0x100:{	
						
			color_test_t colors = 0;
			color_t col, first_found = 0x100;
			int num_found = 0;
			for (col = COLOR_COLORLESS; col < COLOR_ARTIFACT; ++col){
				if (tapped_for_mana[col] > 0){
					++num_found;
					colors |= (1 << col);
					if (first_found == 0x100)
						first_found = col;
				}
			}

			
			if (tapped_for_mana[COLOR_ARTIFACT] > 0 && !(colors & COLOR_TEST_COLORLESS)){
				++num_found;
				colors |= COLOR_TEST_COLORLESS;
				if (first_found == 0x100)
					first_found = COLOR_COLORLESS;
			}

			if (num_found == 0){		
				break;
			} else if (num_found == 1){	
				produce_mana(affected_card_controller, first_found, amount);
				chosen_colors |= 1 << first_found;
			} else {					
				char prompt[300];
				sprintf(prompt, "%s (%s): What kind of mana?",
						cards_ptr[cards_at_7c7000[get_card_instance(player, card)->internal_card_id]->id]->full_name,
						cards_ptr[cards_at_7c7000[get_card_instance(affected_card_controller, affected_card)->internal_card_id]->id]->full_name);

				produce_mana_any_combination_of_colors_2022(player, colors, amount, prompt);
				if (cancel == 1){
					
					produce_mana(affected_card_controller, first_found, amount);
					chosen_colors |= 1 << first_found;
				}
			}
			break;
		}

		case COLOR_ARTIFACT:
			
			produce_mana(affected_card_controller, COLOR_COLORLESS, amount);
			chosen_colors = 1 << COLOR_COLORLESS;
			break;

		case COLOR_COLORLESS:
		case COLOR_BLACK:
		case COLOR_BLUE:
		case COLOR_GREEN:
		case COLOR_RED:
		case COLOR_WHITE:
			
			produce_mana(affected_card_controller, tapped_for_mana_color, amount);
			chosen_colors = 1 << tapped_for_mana_color;
			break;
	}
}
''',

'266b770': r'''
void produce_mana_when_subtype_is_tapped_2022(int allowed_player, event_t event, int type, int subtype, color_t color, int amount){
	if (event == EVENT_TAP_CARD && tapped_for_mana_color >= 0
		&& (allowed_player == 2 || allowed_player == affected_card_controller)
		&& ((type & TYPE_PERMANENT) || is_what(affected_card_controller, affected_card, type))
		&& (subtype <= 0 || has_subtype(affected_card_controller, affected_card, subtype))){
		produce_mana(affected_card_controller, color, amount);
	}

	if (event == EVENT_COUNT_MANA
		&& (allowed_player == 2 || allowed_player == affected_card_controller)
		&& !is_tapped(affected_card_controller, affected_card) && !is_animated_and_sick(affected_card_controller, affected_card)
		&& can_produce_mana(affected_card_controller, affected_card)
		&& ((type & TYPE_PERMANENT) || is_what(affected_card_controller, affected_card, type))
		&& (subtype <= 0 || has_subtype(affected_card_controller, affected_card, subtype))){
		declare_mana_available(affected_card_controller, color, amount);
	}
}
''',

'266b930': r'''
int wild_growth_aura_2022(int player, int card, event_t event, int subtype, color_t color, int amount){
	return wga_backend_2022(player, card, event, subtype, PM_ALL_ONE, 1 << color, amount);
}
''',

'266b970': r'''
int wild_growth_aura_all_one_color_2022(int player, int card, event_t event, int subtype, color_test_t available, int num_mana){
	return wga_backend_2022(player, card, event, subtype, PM_ALL_ONE, available, num_mana);
}
''',

'266b9a0': r'''
int wild_growth_aura_any_combination_of_colors_2022(int player, int card, event_t event, int subtype, color_test_t available, int num_mana){
	return wga_backend_2022(player, card, event, subtype, PM_ANY_COMBO, available, num_mana);
}
''',

}
