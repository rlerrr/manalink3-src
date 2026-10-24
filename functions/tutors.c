// Interactive searching and manipulation of libraries/graveyards/exile.

#include "manalink.h"

int show_deck_exe(int player, const int* deck, int num_cards, const char* prompt, int suppress_done_label, /* const char* */int done_label);

int show_deck(int player, const int* deck, int num_cards, const char* prompt, int suppress_done_label, /* const char* */int done_label)
{
	call_sub_437E20_unless_ai_is_speculating();	// force redisplay
	return show_deck_exe(player, deck, num_cards, prompt, suppress_done_label, done_label);
}

int get_average_value_from_zone(int targ_player, const int *s_location, int zone_count, test_definition_t *this_test, int my_test, int mode){
	int total = 0;
	int found = 0;
	int i;
	for(i=0; i<zone_count; i++){
		if( new_make_test(targ_player, s_location[i], my_test, this_test) ){
			found++;
			if( mode == AI_MAX_CMC || mode == AI_MIN_CMC || mode == AI_GOOD_TO_PUT_IN_GRAVE){
				total+=get_cmc_by_internal_id(s_location[i]);
			}
			if( mode == AI_MAX_VALUE || mode == AI_MIN_VALUE){
				total+=get_base_value(-1, s_location[i]);
			}
		}
	}

	return found ? total / found : 0;
}

const char* hack_override_show_deck_done_button_label = NULL;
int select_card_from_zone(int player, int targ_player, const int *s_location, int zone_count, int must_select, int ai_selection_mode, int test_type, test_definition_t *this_test){
	int selected = -1;
	if( player != AI ){
		while( 1 ){
				const char* label = hack_override_show_deck_done_button_label;
				if (!label){
					label = EXE_STR(0x7375B0);	// DIALOGBUTTONS[2] - "Done"
				}
				selected = show_deck( player, s_location, zone_count, this_test->message, 0, (int)label );
				if( must_select == 0 ){
					if( selected != -1 ){
						if( ! new_make_test( targ_player, s_location[selected], test_type, this_test)
						  ){
							selected = -1;
						}
						break;
					}
					else{
						 break;
					}
				}
				else{
					if( selected != -1 && new_make_test( targ_player, s_location[selected], test_type, this_test) ){
						break;
					}
				}
		}
		if( selected == -1 ){
			return -1;
		}
	}

	else{
		int count = 0;
		int av_value = get_average_value_from_zone(targ_player, s_location, zone_count, this_test, test_type, ai_selection_mode);
		int ai_pick[zone_count];
		int apc = 0;

		while( count < zone_count ){
				if( new_make_test( targ_player, s_location[count], test_type, this_test) ){
					if( ai_selection_mode == AI_FIRST_FOUND || (ai_selection_mode == AI_RANDOM && internal_rand(100)+1 > 50) ){
						selected = count;
						break;
					}

					if( ai_selection_mode == AI_GOOD_TO_PUT_IN_GRAVE ){
						if( good_to_put_in_grave(-1, s_location[count]) ){
							selected = count;
							break;
						}
					}

					int amount = -1;

					if( ai_selection_mode == AI_MAX_VALUE || ai_selection_mode == AI_MIN_VALUE ){
						if( this_test->has_mana_to_pay_cmc == 0 ||
							(this_test->has_mana_to_pay_cmc == 1 && has_mana(player, COLOR_COLORLESS, get_cmc_by_internal_id(s_location[count])))
						  ){
							amount = get_base_value(-1, s_location[count]);
						}
					}

					if( ai_selection_mode == AI_MAX_CMC || ai_selection_mode == AI_MIN_CMC || ai_selection_mode == AI_GOOD_TO_PUT_IN_GRAVE ){
						if( this_test->has_mana_to_pay_cmc == 0 ||
							(this_test->has_mana_to_pay_cmc == 1 && has_mana(player, COLOR_COLORLESS, get_cmc_by_internal_id(s_location[count])))
						  ){
							amount = get_cmc_by_internal_id(s_location[count]);
						}
					}

					if( (ai_selection_mode == AI_MAX_VALUE || ai_selection_mode == AI_MAX_CMC || ai_selection_mode == AI_GOOD_TO_PUT_IN_GRAVE) &&
						amount >= av_value
					  ){
						ai_pick[apc] = count;
						apc++;
					}
					if( (ai_selection_mode == AI_MIN_VALUE || ai_selection_mode == AI_MIN_CMC) && amount <= av_value + 1 ){
						ai_pick[apc] = count;
						apc++;
					}
				}
				count++;
		}

		if( selected == -1 && apc > 0 ){
			selected = ai_pick[internal_rand(apc)];
		}
	}
	return selected;
}

int new_select_a_card(int player, int targ_player, int search_location, int must_select, int ai_selection_mode, int test_type, test_definition_t *this_test){

	if( search_location < 1 ){
	   return -1;
	}
	int zone_count = 0;

	const int *s_location = deck_ptr[targ_player];

	if( this_test->create_minideck > 0 ){
		zone_count = this_test->create_minideck;
	}

	if( search_location == TUTOR_FROM_DECK && this_test->create_minideck > 0 && check_battlefield_for_id(1-player, CARD_ID_AVEN_MINDCENSOR) ){
		zone_count = 4;
		if( zone_count > count_deck(targ_player) ){
			zone_count = count_deck(targ_player);
		}
	}

	if( search_location == TUTOR_FROM_GRAVE && graveyard_has_shroud(targ_player) ){
	   return -1;
	}

	if( search_location == TUTOR_FROM_GRAVE || search_location == TUTOR_FROM_GRAVE_NOTARGET ){
		s_location = get_grave(targ_player);
	}

	if( search_location == TUTOR_FROM_RFG ){
	   s_location = rfg_ptr[targ_player];
	}

	int hand_index[150];
	if( search_location == TUTOR_FROM_HAND ){
		int i;
		for(i=0; i<150; i++){
			hand_index[i] = -1;
		}
		int hand_c = 0;
		int count = 0;
		while( count < active_cards_count[targ_player] ){
				if( in_hand(targ_player, count) && ! check_state(targ_player, count, STATE_CANNOT_TARGET) ){
					card_instance_t *this = get_card_instance(targ_player, count);
					hand_index[hand_c] = this->internal_card_id;
					hand_c++;
					zone_count++;
				}
				count++;
		}
		s_location = hand_index;
	}

	if( s_location[0] == -1 ){
		return -1;
	}

	if( zone_count < 1 ){
		while( s_location[zone_count] != -1){
				zone_count++;
		}
	}

	int selected = select_card_from_zone(player, targ_player, s_location, zone_count, must_select, ai_selection_mode, -1, this_test);

	if( selected != -1 && search_location == TUTOR_FROM_HAND ){
		int my_id = cards_data[s_location[selected]].id;
		int count = 0;
		while( count < active_cards_count[targ_player] ){
				if( in_hand(targ_player, count) && get_id(targ_player, count) == my_id){
					selected = count;
					break;
				}
				count++;
		}
	}

	return selected;
}

int select_a_card(int player, int targ_player, int search_location, int must_select, int ai_selection_mode, int test_type, int type, int flag1, int subtype, int flag2, int color, int flag3, int id, int flag4, int cc, int flag5){
	test_definition_t this_test;
	default_test_definition(&this_test, type);
	this_test.type = type;
	this_test.type_flag = flag1;
	this_test.subtype = subtype;
	this_test.subtype_flag = flag2;
	this_test.color = color;
	this_test.color_flag = flag3;
	this_test.id = id;
	this_test.id_flag = flag4;
	this_test.cmc = cc;
	this_test.cmc_flag = flag5;
	return new_select_a_card(player, targ_player, search_location, must_select, ai_selection_mode, test_type, &this_test);
}

int get_tutoring_denial(int player){
		int i;
		for(i=0; i<2; i++){
			int count = 0;
			while(count < active_cards_count[i] ){
					if( in_play(i, count) ){
						if( is_what(i, count, TYPE_PERMANENT) ){
							if( get_id(i, count) == CARD_ID_MINDLOCK_ORB ){
								return 1;
							}
							if( get_id(i, count) == CARD_ID_STRANGLEHOLD && i != player ){
								return 1;
							}
							if( get_id(i, count) == CARD_ID_LEONIN_ARBITER ){
								card_instance_t *instance = get_card_instance( i, count);
								if( instance->targets[i+2].player != 66 ){
									if( has_mana(player, COLOR_COLORLESS, 2) ){
										int choice = do_dialog(player, i, count, -1, -1, " Deactivate Leonin Arbiter until EOT\n Pass", 0);
										if( choice == 0 ){
											charge_mana(player, COLOR_COLORLESS, 2);
											if( spell_fizzled != 1 ){
												instance->targets[i+2].player = 66;
											}
											else{
												return 1;
											}
										}
										else{
											return 1;
										}
									}
									else{
										return 1;
									}
								}
							}
						}
						if( is_what(i, count, TYPE_EFFECT) ){
							card_instance_t *instance = get_card_instance( i, count);
							if( instance->targets[2].card == CARD_ID_SHADOW_OF_DOUBT ){
								return 1;
							}
						}
					}
					count++;
			}
		}
	return 0;
}

int new_global_tutor(int player, int targ_player, int search_location, int destination, int must_select, int ai_selection_mode, test_definition_t *this_test){

	if( search_location == TUTOR_FROM_DECK && this_test->create_minideck < 1 ){
		if( get_tutoring_denial(player) ){
			return -1;
		}
	}

	int avoid_shuffle = this_test->no_shuffle;

	// This is inane, but can't be removed because of the ridiculous number of cards that do the shuffling *themselves* afterwards
	if( this_test->qty > 1 && !avoid_shuffle ){
		avoid_shuffle = 1;
	}

	int test_type = new_get_test_score(this_test);

	int q;
	int tutored[this_test->qty];
	int tc = 0;
	int grafdiggers_cage_flag = ((destination == TUTOR_PLAY || destination == TUTOR_PLAY_TAPPED || destination == TUTOR_PLAY_ATTACKING)
								 && check_battlefield_for_id(ANYBODY, CARD_ID_GRAFDIGGERS_CAGE));
	int nsl = search_location;
	if (nsl == TUTOR_FROM_GRAVE_NOTARGET){
		nsl = TUTOR_FROM_GRAVE;
	}
	for(q=0; q<this_test->qty; q++){
		int selected = new_select_a_card(player, targ_player, search_location, must_select, ai_selection_mode, test_type, this_test);
		if( selected != -1 ){
			const int *s_location = deck_ptr[targ_player];

			if( nsl == TUTOR_FROM_GRAVE ){
				s_location = get_grave(targ_player);
			}

			if( nsl == TUTOR_FROM_RFG ){
				s_location = rfg_ptr[targ_player];
			}

			if( nsl == TUTOR_FROM_HAND ){
				if( destination == TUTOR_GET_ID ){
					tutored[tc] = get_id(targ_player, selected);
					tc++;
				}
				else{
					tutored[tc] = selected;
					tc++;
				}
			}

			if( nsl == TUTOR_FROM_DECK ){
				if( player == targ_player ){
					set_trap_condition(targ_player, TRAP_DECK_WAS_SEARCHED, 1);
				}
				if( grafdiggers_cage_flag && is_what(-1, s_location[selected], TYPE_CREATURE) ){
					if( destination == TUTOR_PLAY || destination == TUTOR_PLAY_TAPPED || destination == TUTOR_PLAY_ATTACKING ){
						tutored[tc] = -1;
					}
				}
				else{
					tutored[tc] = s_location[selected];
					remove_card_from_deck(targ_player, selected);
					tc++;
				}
			}

			if( nsl == TUTOR_FROM_GRAVE ){
				if( grafdiggers_cage_flag && is_what(-1, s_location[selected], TYPE_CREATURE) ){
					if( destination == TUTOR_PLAY || destination == TUTOR_PLAY_TAPPED || destination == TUTOR_PLAY_ATTACKING ){
						tutored[tc] = -1;
					}
				}
				else{
					tutored[tc] = s_location[selected];
					remove_card_from_grave(targ_player, selected);
					tc++;
				}
			}

			if( nsl == TUTOR_FROM_RFG ){
				tutored[tc] = s_location[selected];
				remove_card_from_rfg(targ_player, cards_data[s_location[selected]].id);
				tc++;
			}

			if( this_test->create_minideck > 1 ){
				this_test->create_minideck--;
				if( this_test->create_minideck == 0 ){
					break;
				}
			}
		}
		else{
			break;
		}
	}

	int card_added = -1;
	int really_tutored = 0;
	int storage_loc = 0;
	for(q=0; q<tc; q++){
		if( tutored[q] != -1 ){
			really_tutored++;
			switch( destination ){
					case TUTOR_RFG:
					{
						if( nsl == TUTOR_FROM_HAND ){
							card_added = get_id(targ_player, tutored[q]);
							rfg_card_in_hand(targ_player, tutored[q]);
						}
						else{
							card_added = cards_data[tutored[q]].id;
							add_card_to_rfg(targ_player, tutored[q]);
						}
						play_sound_effect(WAV_DESTROY);
					}
					break;

					case TUTOR_BOTTOM_OF_DECK:
					case TUTOR_BOTTOM_OF_DECK_RANDOM:
					{
						if( nsl == TUTOR_FROM_HAND ){
							int *t_deck = deck_ptr[targ_player];
							t_deck[count_deck(targ_player)] = get_original_internal_card_id(targ_player, tutored[q]);
							obliterate_card(targ_player, tutored[q]);
						}
						else{
							card_added = cards_data[tutored[q]].id;
							int *t_deck = deck_ptr[targ_player];
							t_deck[count_deck(targ_player)] = tutored[q];
						}
					}
					break;

					case TUTOR_GRAVE:
					{
						// It's currently in targ_player's hand as card_added
						card_added = nsl == TUTOR_FROM_HAND ? tutored[q] : add_card_to_hand(targ_player, tutored[q]);
						if (nsl == TUTOR_FROM_DECK){
							put_on_top_of_deck(targ_player, card_added);
							mill(targ_player, 1);	// so cards specifically triggering on library-to-graveyard, or replacing that, work correctly
						}
						else if (nsl == TUTOR_FROM_HAND){
								new_discard_card(targ_player, card_added, player, 0);	// So cards specifically triggering on discard work correctly
						}
						else{
							kill_card(targ_player, card_added, KILL_DESTROY);
						}
					}
					break;

					case TUTOR_HAND:
					case TUTOR_TPLAYER_HAND:
					case TUTOR_HAND_AND_MARK:
					{
						int dest_player = destination == TUTOR_TPLAYER_HAND ? targ_player : player;
						card_added = add_card_to_hand(dest_player, tutored[q]);
						// avoid_shuffle==2 suppresses the automatic reveal, for reveal_top_cards_of_library_and_choose().  It's not part of the public interface.
						if( player == (destination == TUTOR_HAND ? AI : HUMAN) && avoid_shuffle != 2
							&& (nsl == TUTOR_FROM_GRAVE
								|| nsl == TUTOR_FROM_RFG
								|| !(test_type == 0
									 || (test_type == 1<<0 && this_test->type == TYPE_ANY)))
						  ){
							reveal_card(player, card_added, -1, -1);
						}
						if( destination == TUTOR_HAND_AND_MARK ){
							add_state(dest_player, card_added, STATE_TARGETTED);
						}
						if( nsl == TUTOR_FROM_GRAVE ){
							from_grave_to_hand_triggers(dest_player, card_added);
						}
					}
					break;

					case TUTOR_PLAY:
					case TUTOR_PLAY_TAPPED:
					case TUTOR_PLAY_ATTACKING:
					{
						card_added = nsl == TUTOR_FROM_HAND ? tutored[q] : add_card_to_hand(player, tutored[q]);
						card_instance_t *instance = get_card_instance(player, card_added);
						if( player != targ_player ){
							if( player == HUMAN ){
								add_state(player, card_added, STATE_OWNED_BY_OPPONENT);
							}
							else{
								remove_state(player, card_added, STATE_OWNED_BY_OPPONENT);
							}
						}
						if( destination == TUTOR_PLAY_TAPPED || destination == TUTOR_PLAY_ATTACKING ){
							instance->state |= STATE_TAPPED;
						}
						if( nsl == TUTOR_FROM_GRAVE ){
							set_special_flags3(player, card_added, SF3_REANIMATED);
						}
						put_into_play(player, card_added);
						if( destination == TUTOR_PLAY_ATTACKING ){
							instance->state |= STATE_ATTACKING;
							choose_who_attack(player, card_added);
						}
					}
					break;

					case TUTOR_DECK:
					{
						card_added = nsl == TUTOR_FROM_HAND ? tutored[q] : add_card_to_hand(targ_player, tutored[q]);
						if( test_type != 0 && this_test->type > 0 && this_test->type != TYPE_ANY && player == AI ){
							reveal_card(player, card_added, -1, -1);
						}
						put_on_top_of_deck(targ_player, card_added);
					}
					break;

					default:
					break;
			}
			if( this_test->storage > -1 ){
				get_card_instance(player, this_test->storage)->targets[storage_loc].card = tutored[q];
				storage_loc++;
			}
		}
	}

	if( nsl == TUTOR_FROM_DECK ){
		if( player == targ_player ){
			int amount = count_cards_by_id(1-player, CARD_ID_OB_NIXILIS_UNSHACKLED);
			if( amount ){
				lose_life(player, amount*10);
				int result = locate_id(1-player, CARD_ID_OB_NIXILIS_UNSHACKLED);
				impose_sacrifice(1-player, result, player, amount, TYPE_CREATURE, 0, 0, 0, 0, 0, 0, 0, -1, 0);
			}
		}
	}
	if( search_location == TUTOR_FROM_DECK && ! avoid_shuffle ){
		shuffle(targ_player);
	}

	return this_test->qty > 1 ? really_tutored : card_added;
}

int global_tutor(int player, int targ_player, int search_location, int destination, int must_select, int ai_selection_mode, int type, int flag1, int subtype, int flag2, int color, int flag3, int id, int flag4, int cc, int flag5){
	test_definition_t this_test;
	default_test_definition(&this_test, type);
	this_test.type = type;
	this_test.type_flag = flag1;
	this_test.subtype = subtype;
	this_test.subtype_flag = flag2;
	this_test.color = color;
	this_test.color_flag = flag3;
	this_test.id = id;
	this_test.id_flag = flag4;
	this_test.cmc = cc;
	this_test.cmc_flag = flag5;
	return new_global_tutor(player, targ_player, search_location, destination, must_select, ai_selection_mode, &this_test);
}

int tutor_from_grave_to_hand_except_for_dying_card(int player, int card, int must_select, int ai_selection_mode, test_definition_t* test)
{
  /* Find card in graveyard and make it unselectable without disturbing its placement.
   * If not there - maybe it was stolen or a token or has already been removed by something else - this won't do anything.
   *
   * It's conceivable that there's some sort of effect somewhere that can trigger during the new_global_tutor() call to shuffle graveyard into library or do
   * some other shenanigans that will move the iid_draw_a_card elsewhere ("Whenever a player has more than 4 cards in his hand, he shuffles his graveyard into
   * his library"?  "While Yixlid Jailer Variant is in a graveyard, all other cards in all graveyards lose all abilities" + an Eldrazi?), but I doubt it.
   *
   * It would be much more of a concern if the chosen card was put on the battlefield or played for free; either of those could very easily cause a reshuffling.
   * (Consider reanimating Barishi with a Pandemonium in play, or Timetwister.)  If such a case were needed, it would need support within new_global_tutor to
   * change the iid_draw_a_card's back into their original iids before removing the selected card. */

  if (graveyard_has_shroud(player))
	return 0;

  int owner, position, iid = -1;
  if (find_in_owners_graveyard(player, card, &owner, &position)
	  && owner == player)	// If it's in the other player's graveyard, then don't even bother - we aren't searching that one
	iid = turn_card_in_grave_face_down(player, position);

  int rval = -1;
  if (new_special_count_grave(player, test))
	rval = new_global_tutor(player, player, TUTOR_FROM_GRAVE, TUTOR_HAND, must_select, ai_selection_mode, test);

  // Restore the unselectable card, if indeed there was one.  Have to search, since its position may have changed.
  if (iid != -1)
	for (position = 0; get_grave(player)[position] != -1; ++position)
	  if (get_grave(player)[position] == iid_draw_a_card)
		{
		  turn_card_in_grave_face_up(player, position, iid);
		  break;
		}

  return rval;
}

// [player] reveals the top [num_cards] of his library.
int reveal_top_cards_of_library(int player, int num_cards)
{
  int count = count_deck(player);
  num_cards = MIN(count, num_cards);
  if (num_cards <= 0)
	return 0;

  if (ai_is_speculating != 1 && (player == AI || (trace_mode & 2)))
	show_deck(1-player, deck_ptr[player], num_cards, "Opponent reveals", 0, 0x78F0A8);	// Localized "OK"

  return num_cards;
}

/* [reveal_rest or Look at] the top [num_cards] of your library.  Choose a [test] card from among them, [reveal_chosen], and move it to [destination_chosen].
 * Move the rest to [destination_rest]. */
int reveal_top_cards_of_library_and_choose(int src_player, int src_card, int t_player, int num_cards, int must_select, tutor_t destination_chosen, int reveal_chosen, tutor_t destination_rest, int reveal_rest, test_definition_t* test)
{
  if (reveal_rest)
	num_cards = reveal_top_cards_of_library(t_player, num_cards);
  else
	{
	  int count = count_deck(t_player);
	  num_cards = MIN(count, num_cards);
	}
  if (num_cards <= 0)
	return -1;

  int num_to_pick = MIN(num_cards, test->qty);
  if (num_to_pick <= 0)
	return -1;

  test->qty = 1;
  test->create_minideck = num_cards;

  int i, selected = -1;
  int chosen_iids[num_to_pick], num_chosen = 0;	// only used for destinations that have to be dealt with all at once, after moving the unchosen cards

  int grafdiggers_cage = ((destination_chosen == TUTOR_PLAY || destination_chosen == TUTOR_PLAY_TAPPED || destination_chosen == TUTOR_PLAY_ATTACKING)
						  && check_battlefield_for_id(ANYBODY, CARD_ID_GRAFDIGGERS_CAGE));

  for (i = 0; i < num_to_pick; ++i)
	{
	  selected = select_card_from_zone(src_player, t_player, deck_ptr[t_player], num_cards, must_select, test->ai_selection_mode, -1, test);
	  if (selected == -1)
		break;

	  --num_cards;

	  int iid = deck_ptr[t_player][selected];

	  if (reveal_chosen)
		reveal_card_iid(src_player, src_card, iid);

	  switch (destination_chosen)
		{
		  case TUTOR_HAND:
		  case TUTOR_HAND_AND_MARK:
			remove_card_from_deck(t_player, selected);
			int ca = add_card_to_hand(src_player, iid);
			if( destination_chosen == TUTOR_HAND_AND_MARK && ca > -1 ){
				add_state(t_player, ca, STATE_TARGETTED);
			}
			break;

		  case TUTOR_GRAVE:
			remove_card_from_deck(t_player, selected);
			chosen_iids[num_chosen++] = iid;
			break;

		  case TUTOR_PLAY:
		  case TUTOR_PLAY_TAPPED:
		  case TUTOR_PLAY_ATTACKING:
			if (!(grafdiggers_cage && is_what(-1, deck_ptr[t_player][selected], TYPE_CREATURE)))
			  {
				remove_card_from_deck(t_player, selected);
				chosen_iids[num_chosen++] = iid;
				break;
			  }
			// else fall through
		  case TUTOR_DECK:	// Have to make it n'th from the top, where n is num_cards before the above decrement.
			;int n;
			for (n = selected; n < num_cards; ++n)
			  SWAP(deck_ptr[t_player][n], deck_ptr[t_player][n + 1]);
			break;

		  case TUTOR_RFG:
			rfg_card_in_deck(t_player, selected);
			break;

		  case TUTOR_GET_ID:
			selected = cards_data[iid].id;
			// intentionally fall through
		  case TUTOR_GET_POSITION:
			if (num_to_pick > 1)
			  {
				char buf[200];
				sprintf(buf, "%s:%d\nillegal destination_chosen: %d with qty=%d", __FILE__, __LINE__, destination_chosen, num_to_pick);
				backtrace_and_abort(buf);
				break;
			  }
			break;

		  case TUTOR_BOTTOM_OF_DECK:
			put_card_in_deck_to_bottom(t_player, selected);
			break;

		  case TUTOR_TPLAYER_HAND:
			remove_card_from_deck(t_player, selected);
			add_card_to_hand(t_player, iid);
			break;

		  case TUTOR_BOTTOM_OF_DECK_RANDOM:
			{
				char buf[200];
				sprintf(buf, "%s:%d\nillegal destination_chosen: %d", __FILE__, __LINE__, destination_chosen);
				backtrace_and_abort(buf);
			}
			break;
		}
	}

  if (num_cards > 0)
	switch (destination_rest)
	  {
		case TUTOR_HAND_AND_MARK:
		case TUTOR_HAND:
			for (i = 0; i < num_cards; ++i){
				int ca = add_card_to_hand(t_player, deck_ptr[t_player][i]);
				if( ca > -1 && destination_rest == TUTOR_HAND_AND_MARK ){
					add_state(t_player, ca, STATE_TARGETTED);
				}
			}
			obliterate_top_n_cards_of_deck(t_player, num_cards);
		break;

		case TUTOR_DECK:			rearrange_top_x(t_player, t_player, num_cards);	break;
		case TUTOR_GRAVE:			mill(t_player, num_cards);	break;
		case TUTOR_RFG:			rfg_top_n_cards_of_deck(t_player, num_cards);	break;
		case TUTOR_BOTTOM_OF_DECK:put_top_x_on_bottom(t_player, t_player, num_cards);	break;

		case TUTOR_TPLAYER_HAND:
		  for (i = 0; i < num_cards; ++i)
			add_card_to_hand(1-t_player, deck_ptr[t_player][i]);
		  obliterate_top_n_cards_of_deck(t_player, num_cards);
		  break;

		case TUTOR_BOTTOM_OF_DECK_RANDOM:	put_top_x_on_bottom_in_random_order(t_player, num_cards);	break;

		case TUTOR_PLAY:
		case TUTOR_PLAY_TAPPED:
		case TUTOR_GET_ID:
		case TUTOR_PLAY_ATTACKING:
		case TUTOR_GET_POSITION:;
		  char buf[200];
		  sprintf(buf, "%s:%d\nillegal destination_rest: %d", __FILE__, __LINE__, destination_rest);
		  backtrace_and_abort(buf);
		  break;
	  }

  if (num_chosen > 0)
	switch (destination_chosen)
	  {
		case TUTOR_GRAVE:
		  for (i = num_chosen - 1; i >= 0; --i)
			raw_put_iid_on_top_of_deck(t_player, chosen_iids[i]);
		  mill(t_player, num_chosen);
		  break;

		case TUTOR_PLAY:
		case TUTOR_PLAY_TAPPED:
		case TUTOR_PLAY_ATTACKING:
		  for (i = 0; i < num_chosen; ++i)
			{
			  int c = add_card_to_hand(src_player, chosen_iids[i]);
			  card_instance_t* inst = get_card_instance(src_player, c);
			  if (t_player != src_player)
				inst->state ^= STATE_OWNED_BY_OPPONENT;

			  if (destination_chosen == TUTOR_PLAY_ATTACKING)
				{
				  inst->state |= STATE_TAPPED|STATE_ATTACKING;
				  put_into_play(src_player, c);
				  choose_who_attack(src_player, c);
				}
			  else
				{
				  if (destination_chosen == TUTOR_PLAY_TAPPED)
					inst->state |= STATE_TAPPED;
				  put_into_play(src_player, c);
				}
			}
		  break;

		default:
		  // Should be impossible
		  ;char buf[200];
		  sprintf(buf, "%s:%d\nillegal destination_chosen: %d with num_chosen=%d", __FILE__, __LINE__, destination_chosen, num_chosen);
		  backtrace_and_abort(buf);
		  break;
	  }

  return selected;	// position of last card chosen; or its csvid if destination_chosen was TUTOR_GET_ID
}

// Identical to reveal_top_cards_of_library_and_choose(), but specify a type bitfield instead of a full test_definition_t.  Constructs an appropriate prompt.
int reveal_top_cards_of_library_and_choose_type(int src_player, int src_card, int t_player, int num_cards, int must_select, tutor_t destination_chosen, int reveal_chosen, tutor_t destination_rest, int reveal_rest, type_t legal_types)
{
  test_definition_t test;
  if (legal_types == 0 || legal_types == TYPE_ANY || legal_types == TYPE_NONEFFECT)
	default_test_definition(&test, TYPE_ANY);
  else
	{
	  new_default_test_definition(&test, legal_types, "Select a ");

	  if (ai_is_speculating != 1)
		{
		  int l = strlen(test.message);
		  type_text(test.message + l, 100 - l, legal_types, 0);
		  l = strlen(test.message);	// Go, Schlemiel, go!
		  scnprintf(test.message + l, 100 - l, " card.");
		}
	}

  return reveal_top_cards_of_library_and_choose(src_player, src_card, t_player, num_cards, must_select, destination_chosen, reveal_chosen, destination_rest, reveal_rest, &test);
}

int select_one_and_mill_the_rest(int player, int t_player, int number, int type){

	int card_added = -1;
	int *deck = deck_ptr[t_player];
	if( number > count_deck(t_player) ){
		 number = count_deck(t_player);
	}
	if( number > 0 ){
		int selected = -1;
		while( selected == -1 ){
				selected = show_deck( player, deck, number, "Pick a card", 0, 0x7375B0 );
		}
		if( is_what(-1, deck[selected], type) ){
			card_added = add_card_to_hand(t_player, deck[selected]);
			remove_card_from_deck(t_player, selected);
			number--;
		}
		mill(player, number);
	}

	return card_added;
}

int select_one_and_put_the_rest_on_bottom(int player, int t_player, int number){

	int selected = -1;
	int *deck = deck_ptr[t_player];
	while( selected == -1 ){
			selected = show_deck( player, deck, number, "Pick a card", 0, 0x7375B0 );
	}
	int card_added = add_card_to_hand(t_player, deck[selected]);
	remove_card_from_deck(t_player, selected);
	put_top_x_on_bottom(player, t_player, number-1);

	return card_added;
}

int reveal_x_and_choose_a_card_type( int player, int card, int to_reveal, int selected_type ){

	if( count_deck(player) < to_reveal ){
		to_reveal = count_deck(player);
	}

	test_definition_t this_test;
	char buffer[100];
	int pos = scnprintf(buffer, 100, " Select a");
	if( selected_type == TYPE_CREATURE ){
		pos += scnprintf(buffer + pos, 100-pos, " creature");
	}
	if( selected_type == TYPE_LAND ){
		pos += scnprintf(buffer + pos, 100-pos, " land");
	}
	if( selected_type == TYPE_ARTIFACT ){
		pos += scnprintf(buffer + pos, 100-pos, "n artifact");
	}
	if( selected_type == TYPE_ENCHANTMENT ){
		pos += scnprintf(buffer + pos, 100-pos, " enchantment");
	}
	pos += scnprintf(buffer + pos, 100-pos, " card.");
	new_default_test_definition(&this_test, selected_type, buffer);
	this_test.create_minideck = to_reveal;
	this_test.no_shuffle = 1;

	int card_added = -1;

	if( to_reveal > 0 ){
		card_added = new_global_tutor(player, player, TUTOR_FROM_DECK, TUTOR_HAND, 0, AI_MAX_VALUE, &this_test);
		if( card_added != -1 ){
			to_reveal--;
		}
		if( to_reveal > 0 ){
			put_top_x_on_bottom(player, player, to_reveal);
		}
	}

	return card_added;
}

void tutor_lands(int player, tutor_t destination, int num){
	test_definition_t test;
	new_default_test_definition(&test, TYPE_LAND, "Choose a land card.");
	test.qty = num;
	new_global_tutor(player, player, TUTOR_FROM_DECK, destination, 0, AI_MAX_VALUE, &test);
}

void tutor_basic_land(int player, int put_in_play, int tap_it){

	int mode = TUTOR_HAND;
	if( put_in_play == 1 ){
		mode = TUTOR_PLAY;
	}
	if( tap_it == 1 ){
		mode = TUTOR_PLAY_TAPPED;
	}
	tutor_basic_lands(player, mode, 1);
}

void tutor_basic_lands(int player, tutor_t destination, int num){
	test_definition_t test;
	new_default_test_definition(&test, TYPE_LAND, "Choose a basic land card.");
	test.subtype = SUBTYPE_BASIC;
	test.subtype_flag = MATCH;
	test.qty = num;
	test.no_shuffle = 1;
	new_global_tutor(player, player, TUTOR_FROM_DECK, destination, 0, AI_MAX_VALUE, &test);
	shuffle(player);
}

// Tutor two basic lands, one onto bf tapped, one into hand, then shuffle
void cultivate(int player){
	test_definition_t test;
	new_default_test_definition(&test, TYPE_LAND, "Choose a basic land card to put on the battlefield tapped.");
	test.subtype = SUBTYPE_BASIC;
	test.subtype_flag = MATCH;
	test.no_shuffle = 1;

	if (new_global_tutor(player, player, TUTOR_FROM_DECK, TUTOR_PLAY_TAPPED, 0, AI_MAX_VALUE, &test) != -1){
		strcpy(test.message, "Choose a basic land card to put in your hand.");
		new_global_tutor(player, player, TUTOR_FROM_DECK, TUTOR_HAND, 0, AI_MAX_VALUE, &test);
	}

	shuffle(player);
}

// Simple n^2 insertion sort on array[].player, descending.  Poorly suited if number is greater than about 30.
void descending_insertion_sort_on_player(int number, target_t* array){
	target_t value;
	int i, j;
	for (i = 1; i < number; ++i){
		value = array[i];	// struct copy
		for (j = i; j > 0 && value.player > array[j - 1].player; --j){
			array[j] = array[j - 1];	// struct copy
		}
		array[j] = value;	// struct copy
	}
}

int select_multiple_cards_from_graveyard(int player, int targ_player, int must_select_all /*if < 0, not enforced; but failing to select all is considered cancelling */, int ai_selection_mode, test_definition_t* this_test /*optional*/, int max_targets, target_t* ret_location){
	// no reason this won't work on higher except that 1) the sort will take forever, and 2) it won't be storable in a card's target array.
	ASSERT(max_targets > 0 && max_targets <= 19);

	test_definition_t default_test;
	if (this_test){
		if (this_test->id){
			ASSERT(this_test->id == 904/*draw a card*/ && this_test->id_flag == DOESNT_MATCH);
		} else {
			this_test->id = 904;/*draw a card*/
			this_test->id_flag = DOESNT_MATCH;
		}
	} else {
		new_default_test_definition(&default_test, 0, "");
		if (ai_is_speculating != 1){
			if (max_targets == 1){
				strcpy(default_test.message, "Select a card.");
			} else if (must_select_all) {
				sprintf(default_test.message, "Select %d cards.", max_targets);
			} else {
				sprintf(default_test.message, "Select up to %d cards.", max_targets);
			}
		}
		default_test.id = 904;/*draw a card*/
		default_test.id_flag = DOESNT_MATCH;
		this_test = &default_test;
	}

	int i;
	// Clear targets first, so we don't have to worry about it later if too few cards are chosen
	for (i = 0; i < max_targets; ++i){
		ret_location[i].player = ret_location[i].card = -1;
	}

	if (graveyard_has_shroud(targ_player)){
		if (must_select_all) {
			spell_fizzled = 1;
		}
		return 0;
	}

	int count = count_graveyard(targ_player);
	if (count == 0){
		if (must_select_all){
			spell_fizzled = 1;
		}
		return 0;
	} else if (count < max_targets){
		if (must_select_all){
			spell_fizzled = 1;
			return 0;
		} else {
			max_targets = count;
		}
	}

	// Select cards, changing each chosen card to a face-down creature so it can't be chosen again and is visually distinct
	int num_chosen = max_targets;
	for (i = 0; i < max_targets; ++i){
		int selected = new_select_a_card(player, targ_player, TUTOR_FROM_GRAVE, must_select_all > 0 ? 1 : 0, ai_selection_mode, -1, this_test);

		if (selected == -1){
			num_chosen = i;
			break;
		} else {
			ret_location[i].player = selected;
			ret_location[i].card = turn_card_in_grave_face_down(targ_player, selected);
		}
	}

	// Change the face-down cards back to what they were
	for (i = 0; i < num_chosen; ++i){
		turn_card_in_grave_face_up(targ_player, ret_location[i].player, ret_location[i].card);
	}

	// Sort so earliest targets have the highest indices.  Simple n^2 insertion sort, since n is always 19 or below.
	descending_insertion_sort_on_player(num_chosen, ret_location);

	if (must_select_all && num_chosen < max_targets){
		spell_fizzled = 1;
	}

	return num_chosen;
}

// test only needs to be defined during EVENT_CAST_SPELL.
int spell_return_up_to_n_target_cards_from_your_graveyard_to_your_hand(int player, int card, event_t event, int max_cards, test_definition_t* test, int dont_kill_self)
{
  if (event == EVENT_CAN_CAST)
	return 1;

  if (event == EVENT_CAST_SPELL && affect_me(player, card))
	{
	  card_instance_t* instance = get_card_instance(player, card);
	  int num_chosen = select_multiple_cards_from_graveyard(player, player, 0, AI_MAX_VALUE, test, max_cards, &instance->targets[0]);
	  if (num_chosen < max_cards)
		ai_modifier += (player == AI ? -24 : 24) * (max_cards - num_chosen);
	}

  if (event == EVENT_RESOLVE_SPELL)
	{
	  card_instance_t* instance = get_card_instance(player, card);
	  int i, num_validated = 0, num_targeted = 0, selected;
	  for (i = 0; i < max_cards; ++i)
		if (instance->targets[i].card != -1)
		  {
			++num_targeted;
			if ((selected = validate_target_from_grave(player, card, player, i)) != -1)
			  {
				from_grave_to_hand(player, selected, TUTOR_HAND);
				++num_validated;
			  }
		  }

	  if (num_validated == 0 && num_targeted > 0)
		spell_fizzled = 1;

	  if (!dont_kill_self)
		kill_card(player, card, KILL_DESTROY);
	}

  return 0;
}

/* Choose one or both - Return target [type1] card from your graveyard to your hand; and/or return target [type2] card from your graveyard to your hand.
 * Not really appropriate if type1 == type2, for which you want spell_return_up_to_n_target_cards_from_your_graveyard_to_your_hand(). */
int spell_return_one_or_two_cards_from_gy_to_hand(int player, int card, event_t event, type_t type1, type_t type2)
{
  if (event == EVENT_CAN_CAST)
	{
	  if (((type1 | type2) & (TYPE_PERMANENT | TARGET_TYPE_PLANESWALKER)) == TYPE_PERMANENT)
		{
		  // have to check separately, or else a planeswalker in the graveyard will be a false positive
		  if (!any_in_graveyard_by_type(player, type1) && !any_in_graveyard_by_type(player, type2))
			return 0;
		}
	  else if (!any_in_graveyard_by_type(player, type1 | type2))	// Otherwise, can just iterate through graveyard once
		return 0;

	  return !graveyard_has_shroud(player);
	}

  if (event == EVENT_CAST_SPELL && affect_me(player, card))
	{
	  card_instance_t* instance = get_card_instance(player, card);

	  instance->targets[0].player = instance->targets[0].card = -1;
	  instance->targets[1].player = instance->targets[1].card = -1;

	  int iid = -1, i;
	  type_t types[2] = { type1, type2 };
	  test_definition_t test;
	  for (i = 0; i <= 1; ++i)
		if (any_in_graveyard_by_type(player, types[i]))
		  {
			new_default_test_definition(&test, types[i], "");
			char type_txt[100];
			type_text(type_txt, 100, types[i], 0);
			scnprintf(test.message, 100, "Select target %s card.", type_txt);

			if (select_target_from_grave_source(player, card, player, 0, AI_MAX_VALUE, &test, i) == -1)
			  cancel = 0;	// Might still choose the other type; or might have done so already.
			else if (i == 0)
			  iid = turn_card_in_grave_face_down(player, instance->targets[i].player);
		  }

	  if (iid != -1)
		turn_card_in_grave_face_up(player, instance->targets[0].player, iid);

	  if (instance->targets[0].player == -1 && instance->targets[1].player == -1)	// chose neither
		cancel = 1;
	}

  if (event == EVENT_RESOLVE_SPELL)
	{
	  card_instance_t* instance = get_card_instance(player, card);

	  int i, num_validated = 0;;
	  for (i = 0; i <= 1; ++i)
		if (validate_target_from_grave_source(player, card, player, i) >= 0)
		  {
			from_grave_to_hand(player, instance->targets[i].player, TUTOR_HAND);
			++num_validated;
		  }

	  if (num_validated == 0)
		spell_fizzled = 1;

	  kill_card(player, card, KILL_DESTROY);
	}

  return 0;
}

// Separates the first number cards in source_deck[] into two piles.  AI attempts to divide evenly by base_value.
// Preconditions: piles is an array of int[2][number].  Elements of source_deck[] are internal_card_ids (such as from deck_ptr[], get_grave(), or rfg_ptr[]).
// Postcondition: For each i in [0..number), one of piles[0][i] and piles[1][i] will be set to source_deck[i], and the other iid_draw_a_card.
void separate_into_two_piles(int who_separates, const int* source_deck, int number, void* piles_)
{
  int (*piles)[number] = piles_;
  int i;
  for (i = 0; i < number; ++i)
	{
	  piles[0][i] = source_deck[i];
	  piles[1][i] = iid_draw_a_card;
	}

  if (number <= 1)
	return;

  if (who_separates == AI || ai_is_speculating == 1)
	{
	  /* Evenly separating a pile of things with various scores into two piles of roughly equal total scores is a variant of the Knapsack Problem, and is NP
	   * complete.  On the other hand, we could solve it exactly, since we know number is always very small (5 or under).  But, it's not worth even that much
	   * effort.  What we're going to do instead is sort on score, then for each card, put it into the pile with the lowest total score so far. */
	  int scores[2] = {0,0};

	  target_t vals[number];	// vals[].player = score, vals[].card = index into source deck
	  for (i = 0; i < number; ++i)
		{
		  vals[i].player = my_base_value_by_id(source_deck[i]);
		  vals[i].player = MAX(1, vals[i].player);
		  vals[i].card = i;
		}

	  descending_insertion_sort_on_player(number, vals);

	  for (i = 0; i < number; ++i)
		if (scores[0] <= scores[1])
		  // Leave in pile 0
		  scores[0] += vals[i].player;
		else
		  {
			// Move to pile 1
			scores[1] += vals[i].player;
			SWAP(piles[0][vals[i].card], piles[1][vals[i].card]);
		  }
	}
  else
	while (1)
	  {
		int selected = show_deck(who_separates, piles[0], number, "Choose pile 1.", 0, 0x7375B0);
		if (selected == -1)
		  break;

		SWAP(piles[0][selected], piles[1][selected]);
	  }
}

// Chooses between two piles of cards each with number cards.
// Preconditions: piles is an array of int[2][number].  Elements of piles[][] are internal_card_ids.  ai_selection_mode is AI_MAX_VALUE or AI_MIN_VALUE.  choice_txt is 11 chars or less.
// Returns chosen pile, 0 or 1.
int choose_between_two_piles(int chooser, int number, void* piles_, int ai_selection_mode, const char* choice_txt)
{
  ASSERT(ai_selection_mode == AI_MIN_VALUE || ai_selection_mode == AI_MAX_VALUE);

  int (*piles)[number] = piles_;

  if (chooser == AI || ai_is_speculating == 1)
	{
	  int scores[2] = {0, 0};

	  int i, j;
	  for (i = 0; i < 2; ++i)
		for (j = 0; j < number; ++j)
		  if (piles[i][j] != -1
			  && piles[i][j] != iid_draw_a_card)
			{
			  int val = my_base_value_by_id(piles[i][j]);
			  val = MAX(1, val);
			  scores[i] += val;
			}

	  int chosen;
	  if (scores[0] > scores[1])
		chosen = ai_selection_mode == AI_MAX_VALUE ? 0 : 1;
	  else
		chosen = ai_selection_mode == AI_MAX_VALUE ? 1 : 0;

	  show_deck(HUMAN, piles[chosen], number, "Opponent chose this pile.", 0, (int)"OK");

	  return chosen;
	}
  else
	{
	  int selected, chosen = 1;
	  do
		{
		  chosen = 1 - chosen;
		  selected = show_deck(chooser, piles[chosen], number, "Click a card to see the other pile.", 0, (int)choice_txt);
		} while (selected != -1);

	  return chosen;
	}
}

// 1-who_chooses separates the top number cards of player's deck into two piles.  who_chooses picks a pile to put in player's hand.
// The rest are put in put_rest_where, which must currently be TUTOR_GRAVE or TUTOR_BOTTOM_OF_DECK.
void effect_fof(int player, int who_chooses, int number, int put_rest_where)
{
  int num_cards_in_deck = count_deck(player);
  number = MIN(number, num_cards_in_deck);

  ASSERT((put_rest_where == TUTOR_GRAVE || put_rest_where == TUTOR_BOTTOM_OF_DECK) && "others unimplemented");

  if (number > 0)
	{
	  int piles[2][number];

	  separate_into_two_piles(1 - who_chooses, deck_ptr[player], number, piles);

	  int chosen = choose_between_two_piles(who_chooses, number, piles, who_chooses == player ? AI_MAX_VALUE : AI_MIN_VALUE, "Put in hand");

	  // Obliterate the existing cards on top of the deck
	  obliterate_top_n_cards_of_deck(player, number);

	  // Put the ones from the chosen pile into hand one at a time
	  int i;
	  for (i = 0; i < number; ++i)
		if (piles[chosen][i] != iid_draw_a_card && piles[chosen][i] != -1)
		  add_card_to_hand(player, piles[chosen][i]);

	  // Put the ones from the unchosen pile back on top of the library
	  int num_unchosen = 0;
	  for (i = number - 1; i >= 0; --i)
		if (piles[1 - chosen][i] != iid_draw_a_card && piles[1 - chosen][i] != -1)
		  {
			raw_put_iid_on_top_of_deck(player, piles[1 - chosen][i]);
			++num_unchosen;
		  }

	  if (num_unchosen > 0)
		switch (put_rest_where)
		  {
			case TUTOR_GRAVE:
			  mill(player, num_unchosen);
			  break;

			case TUTOR_BOTTOM_OF_DECK:
			  for (i = 0; i < num_unchosen; ++i)
				put_top_card_of_deck_to_bottom(player);
			  if (num_unchosen > 1)
				rearrange_bottom_x(player, player, num_unchosen);
			  break;
		  }
	}
}

void alphabetize_deck(int player, int* deck, int num_cards){
	int count = count_deck(player);
	if( num_cards < count || deck != deck_ptr[player] ){ return; }
	//set_trap_condition(player, TRAP_DECK_WAS_SEARCHED, 1);
	if( count < 10 || ! get_setting(SETTING_ALPHABETIZE_DECK) ){ return; }
	if( player == AI ){ return; }

	//FILE *file = fopen("sort.txt", "w");
	//int *deck = deck_ptr[player];

	int i;
	int deck2[500];
	for(i=0;i<500;i++){
		deck2[i] = -1;
	}
	for(i=0;i<count;i++){
		int card = deck[i];
		card_ptr_t* c = cards_ptr[ cards_data[ deck[i] ].id ];
		//fprintf(file,"%d) Card=%s\n", i, c->name );

		int j;
		int index = i;
		for(j=0;j<i;j++){
			card_ptr_t* c1 = cards_ptr[ cards_data[ deck2[j] ].id ];
			if( strcmp( c1->name, c->name  ) > 0  ){
				index = j;
				//fprintf(file," --Fitting before %s\n", c1->name );
				break;
			}
			else{
				//fprintf(file," Skipping %s\n", c1->name );
			}
		}
		for(j=i+1;j>=index;j--){
			deck2[j] = deck2[j-1];
		}
		deck2[index] = card;

		// print out the deck
		//for(j=0;j<=i;j++){
		//    card_ptr_t* c1 = cards_ptr[ cards_data[ deck2[j] ].id ];
		//    fprintf(file,"   %d) %s\n", j, c1->name );
		//}

	}
	for(i=0;i<500;i++){
		deck[i] = deck2[i];
	}
	//fclose(file);
}

int get_total_available_lands(int player){

	int total_lands = 0;

	CYCLE_ALL_CARDS(p, c, 	{
								if( p == player ){
									if( (in_hand(p, c) || in_play(p, c)) && is_what(p, c, TYPE_LAND) ){
										total_lands++;
									}
								}
							}
	);

	return total_lands;
}

void scrylike_effect(int player, int t_player, int remaining){
	int *deck = deck_ptr[t_player];
	if( remaining > count_deck(t_player) ){
		remaining = count_deck(t_player);
	}
	if( remaining > 0 ){
		if( IS_AI(player) ){
			int lands = get_total_available_lands(t_player);
			if( player != t_player ){
				if( lands <= 3 ){
					test_definition_t test;
					default_test_definition(&test, TYPE_LAND);
					test.create_minideck = remaining;
					test.no_shuffle = 1;
					test.qty = remaining;
					int result = new_global_tutor(player, t_player, TUTOR_FROM_DECK, TUTOR_BOTTOM_OF_DECK, 0, AI_MAX_VALUE, &test);
					remaining -= result;
				}
				if( lands >= 6 ){
					test_definition_t test;
					default_test_definition(&test, TYPE_LAND);
					test.type = DOESNT_MATCH;
					test.create_minideck = remaining;
					test.no_shuffle = 1;
					test.qty = remaining;
					int result = new_global_tutor(player, t_player, TUTOR_FROM_DECK, TUTOR_BOTTOM_OF_DECK, 0, AI_MAX_VALUE, &test);
					remaining -= result;
				}
			}
			else{
				if( lands <= 3 ){
					test_definition_t test;
					default_test_definition(&test, TYPE_LAND);
					test.type = DOESNT_MATCH;
					test.create_minideck = remaining;
					test.no_shuffle = 1;
					test.qty = remaining;
					int result = new_global_tutor(player, t_player, TUTOR_FROM_DECK, TUTOR_BOTTOM_OF_DECK, 0, AI_MAX_VALUE, &test);
					remaining -= result;
				}
				if( lands >= 6 ){
					test_definition_t test;
					default_test_definition(&test, TYPE_LAND);
					test.create_minideck = remaining;
					test.no_shuffle = 1;
					test.qty = remaining;
					int result = new_global_tutor(player, t_player, TUTOR_FROM_DECK, TUTOR_BOTTOM_OF_DECK, 0, AI_MAX_VALUE, &test);
					remaining -= result;
				}
			}
		} 
		else{
			// put cards to bottom first
			while( remaining > 0 ){
					int selected = show_deck( player, deck, remaining, "Choose a card to put on the bottom", 0, 0x7375B0 );
					if( selected != -1 ){
						deck[ count_deck(t_player) ] = deck[selected];
						remove_card_from_deck( t_player, selected );
						remaining--;
					}
					else{
						break;
					}
			}
		}

		// put cards to the top now
		if( remaining > 1 ){
			rearrange_top_x(t_player, player, remaining);
		}
	}
}

void scry(int player, int number_of_cards){
	scrylike_effect(player, player, number_of_cards);
	dispatch_xtrigger2(player, XTRIGGER_SCRY, "Scrying", 0, player, number_of_cards);
}

void rearrange_top_x(int t_player, int who_chooses, int number){
	// t_player = target player
	// who_chooses = player who rearranges

	int *deck = deck_ptr[t_player];
	number = MIN(number, count_deck(t_player));
	if( number == 1 ){
		show_deck( who_chooses, deck, 1, "Top card of library", 0, 0x7375B0 );
	}
	else{
		int backup_deck[number];
		int i;
		for(i=0; i<number; i++){
			backup_deck[i] = deck_ptr[t_player][0];
			remove_card_from_deck(t_player, 0);
		}
		if( ! IS_AI(who_chooses) ){
			test_definition_t test;
			new_default_test_definition(&test, TYPE_ANY, "Pick a card to put on top.");
			while( 1 ){
					int selected = select_card_from_zone(who_chooses, who_chooses, backup_deck, number, 0, AI_MAX_VALUE, -1, &test);
					if( selected != -1 ){
						int transit = backup_deck[0];
						backup_deck[0] = backup_deck[selected];
						backup_deck[selected] = transit;
					}
					else{
						break;
					}
			}
			for(i=number-1; i > -1; i--){
				raw_put_iid_on_top_of_deck(t_player, backup_deck[i]);
			}
		}
		else{
			int lands = get_total_available_lands(t_player);
			int remaining = number;
			if( who_chooses != t_player ){
				if( lands <= 3 ){
					for(i=0; i<number; i++){
						if( is_what(-1, backup_deck[i], TYPE_LAND) ){
							raw_put_iid_on_top_of_deck(t_player, backup_deck[i]);
							backup_deck[i] = -1;
							remaining--;
						}
					}
					while( remaining ){
							int max_cmc = -1;
							int selected = -1;
							for(i=0; i<number; i++){
								if( backup_deck[i] > -1 ){
									int this_cmc = get_cmc_by_internal_id(backup_deck[i]);
									if( this_cmc > max_cmc ){
										max_cmc = this_cmc;
										selected = i;
									}
								}
							}
							raw_put_iid_on_top_of_deck(t_player, backup_deck[selected]);
							backup_deck[selected] = -1;
							remaining--;
					}
				}
				if( lands >= 6 ){
					for(i=0; i<number; i++){
						if( ! is_what(-1, backup_deck[i], TYPE_LAND) ){
							raw_put_iid_on_top_of_deck(t_player, backup_deck[i]);
							backup_deck[i] = -1;
							remaining--;
						}
					}
					while( remaining ){
							int min_cmc = 1000;
							int selected = -1;
							for(i=0; i<number; i++){
								if( backup_deck[i] > -1 ){
									int this_cmc = get_cmc_by_internal_id(backup_deck[i]);
									if( this_cmc < min_cmc ){
										min_cmc = this_cmc;
										selected = i;
									}
								}
							}
							raw_put_iid_on_top_of_deck(t_player, backup_deck[selected]);
							backup_deck[selected] = -1;
							remaining--;
					}
				}
			}
			else{
				if( lands <= 3 ){
					for(i=0; i<number; i++){
						if( ! is_what(-1, backup_deck[i], TYPE_LAND) ){
							raw_put_iid_on_top_of_deck(t_player, backup_deck[i]);
							backup_deck[i] = -1;
							remaining--;
						}
					}
					while( remaining ){
							int min_cmc = 1000;
							int selected = -1;
							for(i=0; i<number; i++){
								if( backup_deck[i] > -1 ){
									int this_cmc = get_cmc_by_internal_id(backup_deck[i]);
									if( this_cmc < min_cmc ){
										min_cmc = this_cmc;
										selected = i;
									}
								}
							}
							raw_put_iid_on_top_of_deck(t_player, backup_deck[selected]);
							backup_deck[selected] = -1;
							remaining--;
					}
				}
				if( lands >= 6 ){
					for(i=0; i<number; i++){
						if( is_what(-1, backup_deck[i], TYPE_LAND) ){
							raw_put_iid_on_top_of_deck(t_player, backup_deck[i]);
							backup_deck[i] = -1;
							remaining--;
						}
					}
					while( remaining ){
							int max_cmc = -1;
							int selected = -1;
							for(i=0; i<number; i++){
								if( backup_deck[i] > -1 ){
									int this_cmc = get_cmc_by_internal_id(backup_deck[i]);
									if( this_cmc > max_cmc ){
										max_cmc = this_cmc;
										selected = i;
									}
								}
							}
							raw_put_iid_on_top_of_deck(t_player, backup_deck[selected]);
							backup_deck[selected] = -1;
							remaining--;
					}
				}
			}
		}
	}
}

int show_bottom_of_deck(int player, int* deck, int num_cards, const char* prompt, int count_in_deck){
	// This is a moderate hack - instead of passing deck[] to show_deck, we find the exact position so that the bottom num_cards cards are shown
	return show_deck(player, &deck[count_in_deck - num_cards], num_cards, prompt, 0, 0x7375B0);
}

void rearrange_bottom_x(int t_player, int who_chooses, int number){
	// t_player = target player
	// who_chooses = player who rearranges

	int* deck = deck_ptr[t_player];
	int count = count_deck(t_player);
	if (count < number){
		number = count;
	}

	if (number == 1){
		show_bottom_of_deck(who_chooses, deck, 1, "Bottom card of library", count);
	} else {
		for (; number > 1; --number){
			int selected;
			do {
				selected = show_bottom_of_deck(who_chooses, deck, number, "Pick a card to put on bottom", count);
			} while (selected == -1);

			SWAP(deck[count - number], deck[count - number + selected]);
		}
	}
}

int select_best_card_from_top_x(int player, int t_player, int amount){

	int result = -1;
	int *deck = deck_ptr[t_player];
	int count = 0;
	int par = 0;
	if( player != t_player ){
		par = 1000;
	}

	while( count < amount ){
			card_ptr_t* c = cards_ptr[ cards_data[deck[count]].id ];
			if( player == t_player ){
				if( c->ai_base_value > par ){
					par = c->ai_base_value;
					result = count;
				}
			}
			else{
				if( c->ai_base_value < par ){
					par = c->ai_base_value;
					result = count;
				}
			}
			count++;
	}
	return result;
}

void put_top_x_on_bottom(int player, int t_player, int remaining ){

	int *deck = deck_ptr[t_player];

	while( remaining > 0 ){
			if( remaining == 1 ){
				deck[count_deck(t_player)] = deck[0];
				remove_card_from_deck(t_player, 0);
				remaining--;
			}
			else{
				char msg[100] = "Select a card to put on bottom.";
				test_definition_t this_test;
				new_default_test_definition(&this_test, TYPE_ANY, msg);
				this_test.create_minideck = remaining;
				this_test.no_shuffle = 1;
				int mode = AI_MAX_VALUE;
				if( player != t_player ){
					mode = AI_MIN_VALUE;
				}
				int selected = new_select_a_card(player, t_player, TUTOR_FROM_DECK, 1, mode, -1, &this_test);
				if( selected != -1 ){
					deck[count_deck(t_player)] = deck[selected];
					remove_card_from_deck(t_player, selected);
					remaining--;
				}
			}
	}
}

void put_top_x_on_bottom_in_random_order(int player, int amount)
{
  if (amount <= 0)
	return;

  int* deck = deck_ptr[player];

  // Store top amount cards of deck
  int i, cards[amount];
  for (i = 0; i < amount; ++i)
	cards[i] = deck[i];

  // Remove them from deck
  obliterate_top_n_cards_of_deck(player, amount);

  int pos = count_deck(player);

  for (; amount > 0; --amount, ++pos)
	{
	  // Put a random card from array on bottom of deck
	  int selected = internal_rand(amount);
	  ASSERT(deck[pos] == -1);
	  ASSERT(pos == 0 || deck[pos - 1] != -1);
	  deck[pos] = cards[selected];

	  // Replace the randomly chosen card from array with the last one
	  cards[selected] = cards[amount - 1];
	}
}

void tutor_multiple_card_from_hand(int who_chooses, int t_player, int who_gets_the_card, int destination, int must_select, int ai_selection_mode, test_definition_t *this_test){
	int hand_copy[2][hand_count[t_player]];
	int hcc = 0;
	int count = 0;
	while( count < active_cards_count[t_player] ){
			if( in_hand(t_player, count) ){
				hand_copy[0][hcc] = get_original_internal_card_id(t_player, count);
				hand_copy[1][hcc] = count;
				hcc++;
			}
			count++;
	}
	this_test->zone = TARGET_ZONE_HAND;
	int amount = MIN(this_test->qty, check_battlefield_for_special_card(t_player, -1, t_player, CBFSC_GET_COUNT, this_test));
	if( amount ){
		int i;
		int tutored[amount];
		int tc = 0;
		while( amount ){
				int selected = select_card_from_zone(who_chooses, who_chooses, hand_copy[0], hcc, 0, ai_selection_mode, -1, this_test);
				if( selected != -1 ){
					tutored[tc] = hand_copy[0][selected];
					obliterate_card(t_player, hand_copy[1][selected]);
					tc++;
					for(i=selected; i<hcc; i++){
						hand_copy[0][i] = hand_copy[0][i+1];
						hand_copy[1][i] = hand_copy[1][i+1];
					}
					hcc--;
					amount--;
				}
				else{
					break;
				}
		}
		for(i=0; i<tc; i++){
			if( destination == TUTOR_PLAY || destination == TUTOR_PLAY_TAPPED ){
				int card_added = add_card_to_hand(who_gets_the_card, tutored[i]);
				if( destination == TUTOR_PLAY_TAPPED ){
					add_state(who_gets_the_card, card_added, STATE_TAPPED);
				}
				if( t_player != who_gets_the_card ){
					if( t_player == AI && who_gets_the_card == HUMAN){
						add_state(who_gets_the_card, card_added, STATE_OWNED_BY_OPPONENT);
					}
					if( t_player == HUMAN && who_gets_the_card == HUMAN){
						remove_state(who_gets_the_card, card_added, STATE_OWNED_BY_OPPONENT);
					}
				}
				put_into_play(who_gets_the_card, card_added);
			}
			if( destination == TUTOR_RFG ){
				rfg_card_in_hand(t_player, tutored[i]);
			}
		}
	}
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2650b50; original functions/tutors.c:313. */
static int effect_leonin_arbiter(int player, int card, event_t event)
{
  if (event != EVENT_CLEANUP)
  {
    return 0;
  }
  kill_card(player, card, KILL_EXILE);
  return 0;
}

/* ManalinkEh 2022: 0x2650b10; original functions/tutors.c:1154. */
static int qsort_compare_int_descending(const void *lhs, const void *rhs)
{
  return *(const int *)rhs - *(const int *)lhs;
}

/* ManalinkEh 2022: 0x2650b30; original functions/tutors.c:1162. */
static int qsort_compare_target_player_descending(const void *lhs, const void *rhs)
{
  return *(const int *)rhs - *(const int *)lhs;
}

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */
int select_multiple_cards_from_graveyard_to_array(int player, int targ_player, int must_select_all,
                                                  int ai_selection_mode,
                                                  test_definition_t *this_test, int max_targets,
                                                  target_t *ret_location);

/* Legacy 208-byte test compatibility for the to-array selector.
 * Native 2022 entry and private part are recovered below. */
int select_multiple_cards_from_graveyard_to_array(int player, int targ_player, int must_select_all,
                                                  int ai_selection_mode,
                                                  test_definition_t *this_test, int max_targets,
                                                  target_t *ret_location)
{
  if (max_targets == 0)
    return 0;

  target_t *ptVar1;
  target_t *ptVar2;
  int iVar3;
  int iVar4;
  bool bVar5;
  unsigned int uStack_ec;
  test_definition_t default_test;

  if (this_test == NULL)
  {
    this_test = &default_test;
    new_default_test_definition(this_test, 0, "");
    if (ai_is_speculating != 1)
    {
      if (max_targets == 1)
      {
        strncpy(default_test.message, "Select a card.", 0xf);
      }
      else if (must_select_all == 0)
      {
        sprintf(this_test->message, "Select up to %d cards.", max_targets);
      }
      else
      {
        sprintf(this_test->message, "Select %d cards.", max_targets);
      }
    }
    default_test.id = 0x388;
    default_test.id_flag = 1;
  }
  else if (this_test->id == 0)
  {
    this_test->id = 0x388;
    this_test->id_flag = 1;
  }
  else if ((this_test->id != 0x388) || (this_test->id_flag != 1))
  {
    backtrace_and_abort(
        "functions/tutors.c:1188\nthis_test->id == 904 && this_test->id_flag == DOESNT_MATCH");
  }
  if (0 < max_targets)
  {
    ptVar2 = ret_location;
    do
    {
      ptVar2->card = -1;
      ptVar2->player = -1;
      ptVar2 = ptVar2 + 1;
    } while (ptVar2 != ret_location + max_targets);
  }
  iVar3 = graveyard_has_shroud(targ_player);
  if ((iVar3 == 0) && (iVar3 = count_graveyard(targ_player), iVar3 != 0))
  {
    if ((max_targets <= iVar3) || (max_targets = iVar3, must_select_all == 0))
    {
      if (max_targets < 1)
      {
        qsort(ret_location, max_targets, 8, qsort_compare_target_player_descending);
        return max_targets;
      }
      uStack_ec = 0;
      do
      {
        iVar4 = new_select_a_card(player, targ_player, 2, (unsigned int)(0 < must_select_all),
                                  ai_selection_mode, -1, this_test);
        iVar3 = 0;
        if (iVar4 == -1)
        {
          bVar5 = (int)uStack_ec < max_targets && must_select_all != 0;
          if ((int)uStack_ec < 1)
            goto LAB_026517f8;
          goto LAB_026517d0;
        }
        ret_location[uStack_ec].player = iVar4;
        iVar4 = turn_card_in_grave_face_down(targ_player, iVar4);
        ret_location[uStack_ec].card = iVar4;
        uStack_ec = uStack_ec + 1;
      } while (uStack_ec != max_targets);
      bVar5 = false;
    LAB_026517d0:
      do
      {
        ptVar1 = ret_location + iVar3;
        ptVar2 = ret_location + iVar3;
        iVar3 = iVar3 + 1;
        turn_card_in_grave_face_up(targ_player, ptVar2->player, ptVar1->card);
      } while (iVar3 < (int)uStack_ec);
    LAB_026517f8:
      qsort(ret_location, uStack_ec, 8, qsort_compare_target_player_descending);
      if (!bVar5)
      {
        return uStack_ec;
      }
      spell_fizzled = 1;
      return uStack_ec;
    }
  }
  else if (must_select_all == 0)
  {
    return 0;
  }
  spell_fizzled = 1;
  return 0;
}

/* ManalinkEh 2022: 0x2652ab0; original functions/tutors.c:1158. */
void descending_qsort(int number, int *array)
{
  qsort(array, number, 4, qsort_compare_int_descending);
  return;
}

/* ManalinkEh 2022: 0x2652ae0; original functions/tutors.c:1166. */
void descending_qsort_on_player(int number, target_t *array)
{
  qsort(array, number, 8, qsort_compare_target_player_descending);
  return;
}


#ifndef SHANDALAR
/* Recovered October 2022 typed consumers and compiler parts. */
int32_t hack_show_deck_force_expansion;

/* ManalinkEh 2022: 0x2650b80; original functions/tutors.c:152. */
static void side_effects_to_tutor_deck_from_permanents_2022(int player, int targ_player)
{
  int amount;
  int card;
  int player_00;

  player_00 = 1 - player;
  amount = count_cards_by_id(player_00, 0x3741);
  if (amount == 0)
  {
    return;
  }
  lose_life(player, amount * 10);
  card = locate_id(player_00, 0x3741);
  impose_sacrifice(player_00, card, player, amount, 2, 0, 0, 0, 0, 0, 0, 0, -1, 0);
  return;
}


/* ManalinkEh 2022: 0x2650cf0; original functions/tutors.c:28. */
int get_average_value_from_zone_2022(int targ_player, int *s_location, int zone_count,
                                     test_definition_2022_t *this_test, int my_test, int mode)
{
  int *piVar1;
  int result;
  int iStack_1c;
  int iStack_18;

  if (0 < zone_count)
  {
    iStack_18 = 0;
    iStack_1c = 0;
    piVar1 = s_location + zone_count;
  LAB_02650d40:
    do
    {
      result = new_make_test_2022(targ_player, *s_location, my_test, this_test);
      if (result != 0)
      {
        iStack_18 = iStack_18 + 1;
        if (((mode & 0xfffffffbU) == 2) || (mode == 4))
        {
          result = get_cmc_by_internal_id(*s_location);
          iStack_1c = iStack_1c + result;
        }
        if ((mode & 0xfffffffdU) == 1)
        {
          result = *s_location;
          s_location = s_location + 1;
          result = get_base_value(-1, result);
          iStack_1c = iStack_1c + result;
          if (s_location == piVar1)
            break;
          goto LAB_02650d40;
        }
      }
      s_location = s_location + 1;
    } while (s_location != piVar1);
    if (iStack_18 != 0)
    {
      return iStack_1c / iStack_18;
    }
  }
  return 0;
}


/* ManalinkEh 2022: 0x2650de0; original functions/tutors.c:62. */
int select_card_from_zone_2022(int player, int targ_player, int *s_location, int zone_count,
                               int must_select, int ai_selection_mode, int test_type,
                               test_definition_2022_t *this_test)
{
  int iVar1;
  int iVar2;
  int iVar3;
  char cVar4;
  const char *argument6;
  int iVar5;
  bool bVar6;
  int iStack_34;
  int iStack_2c;
  int *piStack_28;
  unsigned int uStack_24;
  unsigned int uStack_20;
  int iStack_1c;
  int iStack_18;
  char cStack_11;

  if (player != 1)
  {
    while (true)
    {
      argument6 = hack_override_show_deck_done_button_label;
      if (hack_override_show_deck_done_button_label == NULL)
      {
        argument6 = (const char *)ENGINE_DONE_LABEL;
      }
      call_sub_437E20_unless_ai_is_speculating();
      hack_show_deck_force_expansion = 0;
      iVar2 = show_deck_exe(player, s_location, zone_count, this_test->message, 0, (int)argument6);
      if (must_select == 0)
        break;
      if (iVar2 != -1)
      {
        iVar5 = new_make_test_2022(targ_player, s_location[iVar2], test_type, this_test);
        if (iVar5 != 0)
        {
          return iVar2;
        }
      }
    }
    if (iVar2 == -1)
    {
      return -1;
    }
    iVar5 = new_make_test_2022(targ_player, s_location[iVar2], test_type, this_test);
    if (iVar5 != 0)
    {
      return iVar2;
    }
    return -1;
  }
  iStack_18 = get_average_value_from_zone_2022(targ_player, s_location, zone_count, this_test,
                                               test_type, ai_selection_mode);
  int ai_pick[zone_count];
  piStack_28 = ai_pick;
  if (zone_count < 1)
  {
    iVar5 = -1;
  }
  else
  {
    iStack_1c = 0;
    iVar5 = 0;
    uStack_20 = ai_selection_mode - 1;
    uStack_24 = ai_selection_mode - 3;
    do
    {
      while (true)
      {
        iVar1 = new_make_test_2022(targ_player, s_location[iVar5], test_type, this_test);
        if (iVar1 != 0)
          break;
      LAB_02650f20:
        iVar5 = iVar5 + 1;
        if (zone_count == iVar5)
          goto LAB_02650ff5;
      }
      if (ai_selection_mode == 5)
      {
      LAB_0265112d:
        bVar6 = iVar5 == -1;
        goto LAB_02650ffa;
      }
      if (ai_selection_mode == 7)
      {
        iVar1 = internal_rand(100);
        if (iVar1 < 0x32)
        {
          cStack_11 = '\0';
          iVar1 = -1;
          goto LAB_02650fa8;
        }
        goto LAB_0265112d;
      }
      if (ai_selection_mode == 6)
      {
        iVar1 = s_location[iVar5];
        iVar1 = good_to_put_in_grave(-1, iVar1);
        if (iVar1 != 0)
          goto LAB_0265112d;
        cStack_11 = '\x01';
        iVar1 = -1;
      }
      else
      {
        cStack_11 = '\0';
        cVar4 = (ai_selection_mode & 0xfffffffbU) == 2 || ai_selection_mode == 4;
        if ((ai_selection_mode & 0xfffffffdU) == 1)
        {
          if (this_test->has_mana_to_pay_cmc != '\0')
          {
            iVar1 = -1;
            if (this_test->has_mana_to_pay_cmc != '\x01')
              goto LAB_02650fa8;
            iStack_2c = cVar4;
            iStack_34 = -1;
            iVar1 = get_cmc_by_internal_id(s_location[iVar5]);
            iVar3 = has_mana(1, 0, iVar1);
            iVar1 = iStack_34;
            cVar4 = (char)iStack_2c;
            if (iVar3 == 0)
              goto LAB_026510e3;
          }
          iStack_2c = cVar4;
          iVar1 = get_base_value(-1, s_location[iVar5]);
          cVar4 = (char)iStack_2c;
        }
        else
        {
          iVar1 = -1;
        }
      LAB_026510e3:
        if (cVar4 == '\0')
          goto LAB_02650fa8;
      }
      if (this_test->has_mana_to_pay_cmc == '\0')
      {
      LAB_026510b0:
        iVar1 = get_cmc_by_internal_id(s_location[iVar5]);
      }
      else if (this_test->has_mana_to_pay_cmc == '\x01')
      {
        iStack_2c = iVar1;
        iVar1 = get_cmc_by_internal_id(s_location[iVar5]);
        iVar3 = has_mana(1, 0, iVar1);
        iVar1 = iStack_2c;
        if (iVar3 != 0)
          goto LAB_026510b0;
      }
    LAB_02650fa8:
      if ((cStack_11 != '\0' || uStack_20 < 2) && (iStack_18 <= iVar1))
      {
        piStack_28[iStack_1c] = iVar5;
        iStack_1c = iStack_1c + 1;
      }
      if ((1 < uStack_24) || (iStack_18 + 1 < iVar1))
        goto LAB_02650f20;
      piStack_28[iStack_1c] = iVar5;
      iVar5 = iVar5 + 1;
      iStack_1c = iStack_1c + 1;
    } while (zone_count != iVar5);
  LAB_02650ff5:
    bVar6 = true;
    iVar5 = -1;
  LAB_02650ffa:
    if ((0 < iStack_1c) && (bVar6))
    {
      iVar2 = internal_rand(iStack_1c);
      iVar5 = piStack_28[iVar2];
    }
  }
  return iVar5;
}


/* ManalinkEh 2022: 0x2651140; original functions/tutors.c:323. */
int get_tutoring_denial_2022(int player)
{
  card_instance_t *instance;
  int result;
  int card;
  int player_00;

  player_00 = 0;
  do
  {
    if (0 < active_cards_count[player_00])
    {
      card = 0;
      do
      {
        instance = in_play(player_00, card);
        if (instance != NULL)
        {
          result = is_what(player_00, card, 0x47);
          if (result != 0)
          {
            result = get_id(player_00, card);
            if (result == 0x271f)
            {
              return 1;
            }
            result = get_id(player_00, card);
            if (((result == 0x2ddf) || (result = get_id(player_00, card), result == 0x4d62)) &&
                (player != player_00))
            {
              return 1;
            }
            result = get_id(player_00, card);
            if ((result == 0x2c1e) &&
                (result = find_legacy_effect(player_00, card, effect_leonin_arbiter), result == -1))
            {
              result = has_mana(player, 0, 2);
              if (result == 0)
              {
                return 1;
              }
              result =
                  do_dialog(player, player_00, card, -1, -1, " Deactivate until EOT\n Pass", 0);
              if (result == 1)
              {
                return 1;
              }
              charge_mana(player, COLOR_COLORLESS, 2);
              if (spell_fizzled == 1)
              {
                return 1;
              }
              create_legacy_effect(player_00, card, effect_leonin_arbiter);
            }
          }
          result = is_what(player_00, card, 0x80);
          if ((result != 0) &&
              (instance = get_card_instance(player_00, card), instance->targets[2].card == 0x1f14))
          {
            return 1;
          }
        }
        card = card + 1;
      } while (card < active_cards_count[player_00]);
    }
    player_00 = player_00 + 1;
    if (player_00 == 2)
    {
      return 0;
    }
  } while (true);
}


/* ManalinkEh 2022: 0x2651300; original functions/tutors.c:178. */
int new_select_a_card_2022(int player, int targ_player, int search_location, int must_select,
                           int ai_selection_mode, int test_type, test_definition_2022_t *this_test)
{
  unsigned short uVar1;
  int *piVar2;
  card_instance_t *instance;
  int iVar4;
  int iVar5;
  unsigned int uVar6;
  int iStack_26c;
  int hand_index[150];

  if (search_location < 1)
  {
    return -1;
  }
  uVar1 = this_test->create_minideck;
  if (search_location == 1)
  {
    if (uVar1 == 0)
    {
      iVar5 = get_tutoring_denial_2022(targ_player);
      if (iVar5 != 0)
      {
        return -1;
      }
      uVar1 = this_test->create_minideck;
    }
    uVar6 = (unsigned int)uVar1;
    piVar2 = deck_ptr[targ_player];
    iVar5 = check_battlefield_for_id(1 - targ_player, 0x22b9);
    if (iVar5 != 0)
    {
      iVar5 = count_deck(targ_player);
      uVar6 = 4;
      if (iVar5 < 4)
      {
        uVar6 = count_deck(targ_player);
      }
    }
    goto LAB_026513fe;
  }
  uVar6 = (unsigned int)uVar1;
  if (search_location == 2)
  {
    iVar5 = graveyard_has_shroud(targ_player);
    if (iVar5 != 0)
    {
      return -1;
    }
  LAB_026514f4:
    piVar2 = graveyard_ptr_mutable[targ_player];
  }
  else
  {
    piVar2 = deck_ptr[targ_player];
    if (search_location == 5)
      goto LAB_026514f4;
  }
  if (search_location == 3)
  {
    piVar2 = rfg_ptr[targ_player];
  }
  else if (search_location == 4)
  {
    piVar2 = hand_index;
    do
    {
      *piVar2 = -1;
      piVar2 = piVar2 + 1;
    } while (piVar2 != hand_index + 150);
    if (0 < active_cards_count[targ_player])
    {
      iVar5 = 0;
      iStack_26c = 0;
      do
      {
        while (
            (instance = in_hand(targ_player, iVar5),
             instance != NULL && (iVar4 = check_state(targ_player, iVar5, 0x100000), iVar4 == 0)))
        {
          instance = get_card_instance(targ_player, iVar5);
          iVar5 = iVar5 + 1;
          uVar6 = uVar6 + 1;
          hand_index[iStack_26c] = instance->internal_card_id;
          iStack_26c = iStack_26c + 1;
          if (active_cards_count[targ_player] <= iVar5)
            goto LAB_026513f8;
        }
        iVar5 = iVar5 + 1;
      } while (iVar5 < active_cards_count[targ_player]);
    }
  LAB_026513f8:
    piVar2 = hand_index;
  }
LAB_026513fe:
  if (*piVar2 == -1)
  {
    return -1;
  }
  if ((int)uVar6 < 1)
  {
    for (; piVar2[uVar6] != -1; uVar6 = uVar6 + 1)
    {
    }
  }
  iVar5 = player;
  iVar4 = select_card_from_zone_2022(player, targ_player, piVar2, uVar6, must_select,
                                     ai_selection_mode, -1, this_test);
  if ((targ_player == player) && (search_location == 1))
  {
    side_effects_to_tutor_deck_from_permanents_2022(player, iVar5);
    return iVar4;
  }
  if (iVar4 == -1)
  {
    return -1;
  }
  if (search_location == 4)
  {
    uVar1 = cards_data[piVar2[iVar4]].id;
    if (active_cards_count[targ_player] < 1)
    {
      return iVar4;
    }
    iVar5 = 0;
    while ((instance = in_hand(targ_player, iVar5),
            instance == NULL || (uVar6 = get_id(targ_player, iVar5), uVar6 != uVar1)))
    {
      iVar5 = iVar5 + 1;
      if (active_cards_count[targ_player] <= iVar5)
      {
        return iVar4;
      }
    }
    return iVar5;
  }
  return iVar4;
}


/* ManalinkEh 2022: 0x26515c0; original functions/tutors.c:272. */
int select_a_card_2022(int player, int targ_player, int search_location, int must_select,
                       int ai_selection_mode, int test_type, int type, int flag1, int subtype,
                       int flag2, int color, int flag3, int id, int flag4, int cc, int flag5)
{
  int result;
  test_definition_2022_t this_test;
  default_test_definition_2022(&this_test, type);
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
  result = new_select_a_card_2022(player, targ_player, search_location, must_select,
                                  ai_selection_mode, test_type, &this_test);
  return result;
}


/* ManalinkEh 2022: 0x2651660; original functions/tutors.c:1180. */
static int select_multiple_cards_from_graveyard_to_array_part_2022(
    int player, int targ_player, int must_select_all, int ai_selection_mode,
    test_definition_2022_t *this_test, int max_targets, target_t *ret_location)
{
  target_t *ptVar1;
  target_t *ptVar2;
  int iVar3;
  int iVar4;
  bool bVar5;
  unsigned int uStack_ec;
  test_definition_2022_t default_test;

  if (this_test == NULL)
  {
    this_test = &default_test;
    new_default_test_definition_2022(this_test, 0, "");
    if (ai_is_speculating != 1)
    {
      if (max_targets == 1)
      {
        strncpy(default_test.message, "Select a card.", 0xf);
      }
      else if (must_select_all == 0)
      {
        sprintf(this_test->message, "Select up to %d cards.", max_targets);
      }
      else
      {
        sprintf(this_test->message, "Select %d cards.", max_targets);
      }
    }
    default_test.id = 0x388;
    default_test.id_flag = 1;
  }
  else if (this_test->id == 0)
  {
    this_test->id = 0x388;
    this_test->id_flag = 1;
  }
  else if ((this_test->id != 0x388) || (this_test->id_flag != 1))
  {
    backtrace_and_abort(
        "functions/tutors.c:1188\nthis_test->id == 904 && this_test->id_flag == DOESNT_MATCH");
  }
  if (0 < max_targets)
  {
    ptVar2 = ret_location;
    do
    {
      ptVar2->card = -1;
      ptVar2->player = -1;
      ptVar2 = ptVar2 + 1;
    } while (ptVar2 != ret_location + max_targets);
  }
  iVar3 = graveyard_has_shroud(targ_player);
  if ((iVar3 == 0) && (iVar3 = count_graveyard(targ_player), iVar3 != 0))
  {
    if ((max_targets <= iVar3) || (max_targets = iVar3, must_select_all == 0))
    {
      if (max_targets < 1)
      {
        qsort(ret_location, max_targets, 8, qsort_compare_target_player_descending);
        return max_targets;
      }
      uStack_ec = 0;
      do
      {
        iVar4 = new_select_a_card_2022(player, targ_player, 2, (unsigned int)(0 < must_select_all),
                                       ai_selection_mode, -1, this_test);
        iVar3 = 0;
        if (iVar4 == -1)
        {
          bVar5 = (int)uStack_ec < max_targets && must_select_all != 0;
          if ((int)uStack_ec < 1)
            goto LAB_026517f8;
          goto LAB_026517d0;
        }
        ret_location[uStack_ec].player = iVar4;
        iVar4 = turn_card_in_grave_face_down(targ_player, iVar4);
        ret_location[uStack_ec].card = iVar4;
        uStack_ec = uStack_ec + 1;
      } while (uStack_ec != max_targets);
      bVar5 = false;
    LAB_026517d0:
      do
      {
        ptVar1 = ret_location + iVar3;
        ptVar2 = ret_location + iVar3;
        iVar3 = iVar3 + 1;
        turn_card_in_grave_face_up(targ_player, ptVar2->player, ptVar1->card);
      } while (iVar3 < (int)uStack_ec);
    LAB_026517f8:
      qsort(ret_location, uStack_ec, 8, qsort_compare_target_player_descending);
      if (!bVar5)
      {
        return uStack_ec;
      }
      spell_fizzled = 1;
      return uStack_ec;
    }
  }
  else if (must_select_all == 0)
  {
    return 0;
  }
  spell_fizzled = 1;
  return 0;
}


/* ManalinkEh 2022: 0x2652b10; original functions/tutors.c:1170. */
int select_multiple_cards_from_graveyard_2022(int player, int targ_player, int must_select_all,
                                              int ai_selection_mode,
                                              test_definition_2022_t *this_test, int max_targets,
                                              target_t *ret_location)
{
  int result;
  if (max_targets == 0)
  {
    return 0;
  }
  if (max_targets - 1U < 0x13)
  {
    result = select_multiple_cards_from_graveyard_to_array_part_2022(
        player, targ_player, must_select_all, ai_selection_mode, this_test, max_targets,
        ret_location);
    return result;
  }
  backtrace_and_abort("functions/tutors.c:1175\nmax_targets > 0 && max_targets <= 19");
  __builtin_unreachable();
}


/* ManalinkEh 2022: 0x2652b90; original functions/tutors.c:1180. */
int select_multiple_cards_from_graveyard_to_array_2022(int player, int targ_player,
                                                       int must_select_all, int ai_selection_mode,
                                                       test_definition_2022_t *this_test,
                                                       int max_targets, target_t *ret_location)
{
  int result;

  if (max_targets != 0)
  {
    result = select_multiple_cards_from_graveyard_to_array_part_2022(
        player, targ_player, must_select_all, ai_selection_mode, this_test, max_targets,
        ret_location);
    return result;
  }
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2650c40; original functions/tutors.c:10. */
int show_deck_2022(int player, const int *deck, int num_cards, const char *prompt,
                   int suppress_done_label, int done_label)
{
  int result;
  call_sub_437E20_unless_ai_is_speculating();
  hack_show_deck_force_expansion = 0;
  result = show_deck_exe(player, deck, num_cards, prompt, suppress_done_label, (int)done_label);
  return result;
}

#endif


#ifndef SHANDALAR
/* Recovered October 2022 specialized activation costs. */

/* ManalinkEh 2022: 0x2652be0; original functions/tutors.c:1266. */
int select_multiple_cards_from_either_graveyard_2022(int player, int card, select_grave_t mode,
                                                     int ai_mode_own_graveyard,
                                                     int ai_mode_opponent_graveyard,
                                                     test_definition_2022_t *this_test,
                                                     int max_targets, target_t *ret_location)
{
  bool bVar1;
  int iVar2;
  int32_t iVar3;
  unsigned char bVar4;
  int legal_count;
  int *psVar5;
  int iVar6;
  target_t *ptVar7;
  unsigned int uVar8;
  select_grave_t sVar9;
  int iVar10;
  int iVar11;
  int iVar12;

  select_grave_t sStack_168;
  select_grave_t sStack_164;
  int iStack_160;
  unsigned int uStack_15c;
  select_grave_t sStack_158;
  int iStack_154;
  select_grave_t sStack_150;
  int minimum_or_other_count;
  int *psStack_14c;
  int iStack_148;
  target_t tgt_gy;
  target_definition_2022_t td;
  test_definition_2022_t default_test;

  if (max_targets == 0)
  {
    return 0;
  }
  iStack_148 = max_targets;
  if (0x12 < max_targets - 1U)
  {

    backtrace_and_abort("functions/tutors.c:1270\nmax_targets > 0 && max_targets <= 19");
  }
  sStack_158 = mode & SFG_CANNOT_CANCEL;
  if (this_test == NULL)
  {
    this_test = &default_test;

    new_default_test_definition_2022(this_test, 0, "");
    if (ai_is_speculating != 1)
    {
      if (max_targets == 1)
      {
        strncpy(default_test.message, "Select a card.", 0xf);
      }
      else if (sStack_158 == SFG_0)
      {

        sprintf(this_test->message, "Select up to %d cards.", max_targets);
      }
      else
      {

        sprintf(this_test->message, "Select %d cards.", max_targets);
      }
    }
    default_test.id = 0x388;
    default_test.id_flag = 1;
  }
  else if (this_test->id == 0)
  {
    this_test->id = 0x388;
    this_test->id_flag = 1;
  }
  else if ((this_test->id != 0x388) || (this_test->id_flag != 1))
  {

    backtrace_and_abort(
        "functions/tutors.c:1277\nthis_test->id == 904 && this_test->id_flag == DOESNT_MATCH");
  }

  legal_count = new_special_count_grave_2022(0, this_test);

  iVar6 = new_special_count_grave_2022(1, this_test);
  minimum_or_other_count = max_targets;
  if (sStack_158 == SFG_0)
  {
    minimum_or_other_count = 0x0;
  }
  if ((int)minimum_or_other_count < (int)legal_count)
  {

    sStack_150 = iVar6;
    iVar6 = graveyard_has_shroud(0);
    if (iVar6 == 0)
    {
      uStack_15c = 1;
      bVar1 = (int)sStack_150 <= (int)minimum_or_other_count;
      minimum_or_other_count = sStack_150;
      if (bVar1)
        goto LAB_02652dac;
      goto LAB_02652f78;
    }
    uStack_15c = (unsigned int)((mode & SFG_NOTARGET) != SFG_0);
    bVar1 = (int)minimum_or_other_count < (int)sStack_150;
    minimum_or_other_count = sStack_150;
    if (bVar1)
      goto LAB_02652f78;
    if ((mode & SFG_NOTARGET) == SFG_0)
      goto LAB_02652c7b;
  LAB_02652dac:
    uVar8 = 0;
  }
  else
  {
    if (iVar6 <= (int)minimum_or_other_count)
      goto LAB_02652c7b;
    uStack_15c = 0;
    minimum_or_other_count = iVar6;
  LAB_02652f78:

    iVar6 = graveyard_has_shroud(1);
    if (iVar6 == 0)
    {
      uVar8 = uStack_15c;
      if (uStack_15c == 0)
      {
        uVar8 = 1;
        legal_count = minimum_or_other_count;
      }
      else
      {
      LAB_02652fb8:
        legal_count = ((int)legal_count + (int)minimum_or_other_count);
      }
    }
    else
    {
      uVar8 = (unsigned int)((mode & SFG_NOTARGET) != SFG_0);
      if (uStack_15c == 0)
      {
        legal_count = minimum_or_other_count;
        if (uVar8 == 0)
          goto LAB_02652c7b;
      }
      else if ((mode & SFG_NOTARGET) != SFG_0)
        goto LAB_02652fb8;
    }
  }
  if (legal_count != 0x0)
  {
    if (((int)legal_count < max_targets) &&
        (max_targets = (int)legal_count, iStack_148 = (int)legal_count, sStack_158 != SFG_0))
    {
      spell_fizzled = 1;
      return 0;
    }
    iVar2 = iStack_148;
    int face_down_cards[iVar2];

    psStack_14c = face_down_cards;
    if (0 < max_targets)
    {
      ptVar7 = ret_location;
      do
      {
        ptVar7->card = -1;
        ptVar7->player = -1;
        ptVar7 = ptVar7 + 1;
      } while (ptVar7 != ret_location + max_targets);
    }
    bVar4 = uVar8 != 0 & (unsigned char)uStack_15c;
    iStack_148 = ((uint32_t)iStack_148 & 0xffffff00U) | (uint8_t)bVar4;
    sStack_150 = sStack_158;
    if (bVar4 != 0)
    {
      sStack_150 = SFG_0;
    }
    sStack_164 = mode & SFG_NOTARGET;
    iStack_160 = 3;
    iVar11 = 0;
    iStack_154 = (-(unsigned int)((mode & SFG_NOTARGET) == SFG_0) & 0xfffffffd) + 5;
    sStack_168 = (mode ^ SFG_CANNOT_CANCEL) & SFG_CANNOT_CANCEL;
    if (bVar4 != 0)
      goto LAB_02653086;
  LAB_02652e80:
    tgt_gy.player = uStack_15c ^ 1;
    iVar12 = iVar11;
    do
    {
      iVar11 = iVar12;
      if (max_targets <= iVar12)
      {
      LAB_02652fd0:
        iVar11 = iVar12;
        if (iVar12 < 1)
        {
          return iVar12;
        }
      LAB_02652fd8:
        psVar5 = psStack_14c;
        iStack_148 = iVar11;
        iVar11 = 0;
        do
        {
          uVar8 = ret_location[iVar11].player;

          iVar11 = iVar11 + 1;

          turn_card_in_grave_face_up((int)uVar8 >> 16, uVar8 & 0xffff, psVar5[iVar11 - 1]);
        } while (iVar11 < iStack_148);
      LAB_02653015:
        if (sStack_158 == SFG_0)
        {
          return iStack_148;
        }
        if (max_targets <= iStack_148)
        {
          return iStack_148;
        }
        spell_fizzled = 1;
        return iStack_148;
      }
      while (true)
      {
        iVar3 = tgt_gy.player;
        if ((char)iStack_148 != '\0')
        {
          hack_override_show_deck_done_button_label = "Rechoose GY";
        }
        sVar9 = ai_mode_opponent_graveyard;
        if (tgt_gy.player == player)
        {
          sVar9 = ai_mode_own_graveyard;
        }

        iVar12 = iStack_154;

        uVar8 = new_select_a_card_2022(player, tgt_gy.player, iStack_154, sStack_150, sVar9, -1,
                                       this_test);
        hack_override_show_deck_done_button_label = NULL;
        if (uVar8 == 0xffffffff)
          break;
        iVar12 = iVar11 + 1;
        ret_location[iVar11].player = tgt_gy.player << 0x10 | uVar8;
        ret_location[iVar11].card = graveyard_source[tgt_gy.player][uVar8];

        sVar9 = turn_card_in_grave_face_down(tgt_gy.player, uVar8);
        psStack_14c[iVar11] = sVar9;
        iVar11 = iVar12;
        if (max_targets == iVar12)
          goto LAB_02652fd0;
      }
      if (((((char)iStack_148 == '\0') || (ai_is_speculating == 1)) ||
           ((player == 1 && (((unsigned char)trace_mode & 2) == 0)))) ||
          (iStack_160 = iStack_160 + -1, iStack_160 == 0))
      {
        iStack_148 = iVar11;
        if (0 < iVar11)
          goto LAB_02652fd8;
        goto LAB_02653015;
      }
      spell_fizzled = 0;
      if ((char)iStack_148 == '\0')
        goto LAB_02652e80;
    LAB_02653086:

      base_target_definition_2022(player, card, &td, 0);
      td.zone = 0x1000;
      td.allow_cancel = sStack_168;
      if (sStack_164 != SFG_0)
      {
        td.illegal_abilities = 0;
      }

      load_text_2022(NULL, "TARGET_GRAVEYARD");

      iVar10 = select_target_2022(player, card - 1000, &td, text_lines[0], &tgt_gy);
      iVar12 = iVar11;
      if (iVar10 == 0)
      {
        spell_fizzled = 1;
        iVar12 = 0;
        if (0 < iVar11)
        {
          do
          {
            uVar8 = ret_location[iVar12].player;
            psVar5 = psStack_14c + iVar12;
            iVar12 = iVar12 + 1;

            turn_card_in_grave_face_up((int)uVar8 >> 16, uVar8 & 0xffff, *psVar5);
          } while (iVar11 != iVar12);
        }
        return -1;
      }
    } while (true);
  }
LAB_02652c7b:
  if (sStack_158 != SFG_0)
  {
    spell_fizzled = 1;
    return 0;
  }
  return 0;
}

#endif


#ifndef SHANDALAR
/* Recovered October 2022 global tutoring and control transfer. */

/* ManalinkEh 2022: 0x2651990; original functions/tutors.c:389. */
int new_global_tutor_2022(int player, int targ_player, int search_location, int destination,
                          int must_select, int ai_selection_mode, test_definition_2022_t *this_test)
{
  int avoid_shuffle = this_test->no_shuffle;
  int test_type = new_get_test_score_2022(this_test);
  int capacity = this_test->qty;
  int tutored[capacity];
  int selector = 1 - player;
  int opposition_agent = check_battlefield_for_id(selector, 0x56a2);
  int self_search = player == targ_player;
  int shuffle_after = avoid_shuffle == 0 && search_location == TUTOR_FROM2022_DECK;
  int shuffle_before;
  int zone;
  if (!opposition_agent || player != targ_player || search_location != TUTOR_FROM2022_DECK)
  {
    shuffle_before = destination == TUTOR2022_DECK && shuffle_after;
    shuffle_after = shuffle_after && destination != TUTOR2022_DECK;
    selector = player;
    zone =
        search_location == TUTOR_FROM2022_GRAVE_NOTARGET ? TUTOR_FROM2022_GRAVE : search_location;
  }
  else
  {
    shuffle_before = 0;
    self_search = selector == targ_player;
    destination = TUTOR2022_OPPOSITION_AGENT;
    zone = TUTOR_FROM2022_DECK;
  }

  int tc = 0;
  int cancelled = 0;
  int card_added = -1;
  int really_tutored = 0;
  if (this_test->qty != 0)
  {
    unsigned int q = 0;
    do
    {
      int selected = -1;
      if (zone == TUTOR_FROM2022_DECK_AUTO)
      {
        if (!get_tutoring_denial_2022(selector))
        {
          unsigned int starting_card = 0;
          if (check_battlefield_for_id(1 - targ_player, 0x22b9) && count_deck(targ_player) > 3)
            starting_card = count_deck(targ_player) - 4;
          while ((int)starting_card < count_deck(selector))
          {
            if (new_make_test_2022(targ_player, deck_ptr[targ_player][starting_card], test_type,
                                   this_test))
            {
              selected = starting_card;
              break;
            }
            ++starting_card;
          }
        }
        zone = TUTOR_FROM2022_DECK;
      }
      else
        selected = new_select_a_card_2022(selector, targ_player, search_location, must_select,
                                          ai_selection_mode, test_type, this_test);

      if (selected == -1)
      {
        if (this_test->qty > 1)
          break;
        cancelled = -2;
      }
      else
      {
        if (zone == TUTOR_FROM2022_GRAVE || zone == TUTOR_FROM2022_DECK)
        {
          int *s_location = zone == TUTOR_FROM2022_GRAVE ? graveyard_ptr_mutable[targ_player]
                                                         : deck_ptr[targ_player];
          if (zone == TUTOR_FROM2022_DECK && selector == targ_player)
            set_trap_condition(targ_player, 0x600, 1);
          int from = zone == TUTOR_FROM2022_GRAVE ? 2 : 1;
          if (!grafdiggers_cages(targ_player, (type_t)cards_data[s_location[selected]].type,
                                 from) ||
              ((unsigned int)(destination - TUTOR2022_PLAY) > 1 &&
               destination != TUTOR2022_PLAY_ATTACKING))
          {
            tutored[tc++] = s_location[selected];
            if (zone == TUTOR_FROM2022_GRAVE)
              remove_card_from_grave(targ_player, selected);
            else
              remove_card_from_deck(targ_player, selected);
          }
          else
            tutored[tc] = -1;
        }
        else if (zone == TUTOR_FROM2022_RFG)
        {
          int iid = rfg_ptr[targ_player][selected];
          tutored[tc++] = iid;
          remove_card_from_rfg(targ_player, cards_data[iid].id);
        }
        else if (zone == TUTOR_FROM2022_HAND)
        {
          if (destination == TUTOR2022_GET_ID)
            selected = get_id(targ_player, selected);
          tutored[tc++] = selected;
          get_card_instance(targ_player, selected)->state |= 0x20;
        }
        if (this_test->create_minideck > 1)
          --this_test->create_minideck;
      }
      ++q;
    } while (q < this_test->qty);
  }

  if (shuffle_before)
    shuffle(targ_player);
  else if (search_location == TUTOR_FROM2022_HAND)
    for (int q = 0; q < tc; ++q)
      get_card_instance(targ_player, tutored[q])->state &= ~0x20U;

  int storage_loc = 0;
  for (int q = 0; q < tc; ++q)
  {
    int iid = tutored[q];
    if (iid == -1)
      continue;
    ++really_tutored;
    switch (destination)
    {
    case TUTOR2022_HAND:
    case TUTOR2022_TPLAYER_HAND:
    case TUTOR2022_HAND_AND_MARK:
    {
      int dest_player = destination == TUTOR2022_TPLAYER_HAND ? targ_player : selector;
      card_added = add_card_to_hand(dest_player, iid);
      int reveal = selector == (destination == TUTOR2022_HAND) && avoid_shuffle != 2;
      if (reveal && ((unsigned int)(zone - TUTOR_FROM2022_GRAVE) < 2 ||
                     (test_type != 0 && (test_type != 1 || this_test->type != 0xff))))
        reveal_card(selector, card_added, -1, -1);
      if (destination == TUTOR2022_HAND_AND_MARK)
        add_state(dest_player, card_added, 0x200000);
      if (zone == TUTOR_FROM2022_GRAVE)
        from_grave_to_hand_triggers(dest_player, card_added);
      break;
    }
    case TUTOR2022_DECK:
      if (zone == TUTOR_FROM2022_HAND)
        card_added = iid;
      if ((test_type == 0 || (test_type == 1 && this_test->type == 0xff)) || selector != 1)
      {
        if (zone != TUTOR_FROM2022_HAND)
        {
          put_iid_under_the_first_x_cards_of_library(targ_player, iid, 0);
          break;
        }
      }
      else if (zone == TUTOR_FROM2022_HAND)
        reveal_card(1, card_added, -1, -1);
      else
      {
        card_added = add_card_to_hand_invisible(targ_player, iid);
        reveal_card(1, card_added, -1, -1);
      }
      put_on_top_of_deck(targ_player, card_added);
      break;
    case TUTOR2022_GRAVE:
      if (zone == TUTOR_FROM2022_HAND)
      {
        new_discard_card(targ_player, iid, selector, 0);
        card_added = iid;
      }
      else
      {
        card_added = add_card_to_hand(targ_player, iid);
        if (zone == TUTOR_FROM2022_DECK)
        {
          put_on_top_of_deck(targ_player, card_added);
          mill(targ_player, 1);
        }
        else
          kill_card(targ_player, card_added, KILL_DESTROY);
      }
      break;
    case TUTOR2022_PLAY:
    case TUTOR2022_PLAY_TAPPED:
    case TUTOR2022_PLAY_ATTACKING:
    {
      card_added = zone == TUTOR_FROM2022_HAND ? iid : add_card_to_hand(targ_player, iid);
      card_instance_t *instance = get_card_instance(targ_player, card_added);
      if (targ_player != selector)
      {
        if (selector == 0)
          add_state(targ_player, card_added, 0x1000);
        else
          remove_state(targ_player, card_added, 0x1000);
      }
      if (destination == TUTOR2022_PLAY_TAPPED || destination == TUTOR2022_PLAY_ATTACKING)
        instance->state |= 0x10;
      if (zone == TUTOR_FROM2022_GRAVE)
        set_special_flags3_2022(targ_player, card_added, 1);
      else if (zone == TUTOR_FROM2022_DECK)
        set_special_flags_2022(targ_player, card_added, 0x10000);
      put_into_play(targ_player, card_added);
      if (targ_player != selector)
      {
        card_added = gain_control_impl_2022(targ_player, card_added);
        instance = get_card_instance(selector, card_added);
      }
      if (destination == TUTOR2022_PLAY_ATTACKING)
      {
        instance->state |= 4;
        choose_who_attack(selector, card_added);
      }
      break;
    }
    case TUTOR2022_RFG:
      if (zone == TUTOR_FROM2022_HAND)
      {
        card_added = get_id(targ_player, iid);
        rfg_card_in_hand(targ_player, iid);
      }
      else
      {
        card_added = cards_data[iid].id;
        add_card_to_rfg(targ_player, iid);
      }
      play_sound_effect((wav_t)0x17);
      break;
    case TUTOR2022_BOTTOM_OF_DECK:
    case TUTOR2022_BOTTOM_OF_DECK_RANDOM:
      if (zone == TUTOR_FROM2022_HAND)
      {
        int position = count_deck(targ_player);
        deck_ptr[targ_player][position] = get_original_internal_card_id(targ_player, iid);
        obliterate_card(targ_player, iid);
      }
      else
      {
        card_added = cards_data[iid].id;
        deck_ptr[targ_player][count_deck(targ_player)] = iid;
      }
      break;
    case TUTOR2022_OPPOSITION_AGENT:
    {
      card_added = add_card_to_hand(selector, iid);
      card_instance_t *instance = get_card_instance(selector, card_added);
      instance->state |= 0x20;
      --hand_count[selector];
      reveal_card(selector, card_added, -1, -1);
      add_card_to_rfg(player, iid);
      create_may_play_card_from_exile_effect(selector, card_added, player, cards_data[iid].id, 1);
      obliterate_card(selector, card_added);
      break;
    }
    default:
      break;
    }
    if (this_test->storage >= 0)
      get_card_instance(selector, this_test->storage)->targets[storage_loc++].card = iid;
  }
  if (search_location == TUTOR_FROM2022_DECK_AUTO && self_search)
    side_effects_to_tutor_deck_from_permanents_2022(selector, targ_player);
  if (shuffle_after)
    shuffle(targ_player);
  if (cancelled == -2)
    return -2;
  return this_test->qty < 2 ? card_added : really_tutored;
}


/* ManalinkEh 2022: 0x26524d0; original functions/tutors.c:689. */
int global_tutor_2022(int player, int targ_player, int search_location, int destination,
                      int must_select, int ai_selection_mode, int type, int flag1, int subtype,
                      int flag2, int color, int flag3, int id, int flag4, int cc, int flag5)
{
  int result;
  test_definition_2022_t this_test;
  default_test_definition_2022(&this_test, type);
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
  result = new_global_tutor_2022(player, targ_player, search_location, destination, must_select,
                                 ai_selection_mode, &this_test);
  return result;
}

#endif


#ifndef SHANDALAR
/* Recovered October 2022 library reveal and selection logic. */
typedef union
{
  char display[100];
  char diagnostic[200];
} library_reveal_text_2022_t;

/* ManalinkEh 2022: 0x26526c0; original functions/tutors.c:757.
 * Native behavior: the 50-byte local is passed with a formatting bound of 100. */
int reveal_top_cards_of_library_2022(int player, int num_cards)
{
  int *argument2;
  int argument3;
  char buffer[50];

  argument3 = count_deck(player);
  if (num_cards < argument3)
  {
    argument3 = num_cards;
  }
  if (0 < argument3)
  {
    if (ai_is_speculating != 1)
    {
      if (player == 0)
      {
        scnprintf(buffer, 100, " You reveal");
      }
      else
      {
        scnprintf(buffer, 100, " %s reveals", opponent_name);
      }
      argument2 = deck_ptr[player];
      call_sub_437E20_unless_ai_is_speculating();
      hack_show_deck_force_expansion = 0;
      show_deck_exe(0, argument2, argument3, buffer, 0, 0x78f0a8);
    }
    return argument3;
  }
  return 0;
}


/* ManalinkEh 2022: 0x2654d60; original functions/tutors.c:796. */
int reveal_top_cards_of_library_and_choose_fn_who_chooses_2022(
    int src_player, int src_card, int t_player, int who_chooses, int num_cards, int must_select,
    tutor_destination_2022_t destination_chosen, int reveal_chosen,
    tutor_destination_2022_t destination_rest, int reveal_rest, test_definition_2022_t *test,
    library_reveal_callback_2022_t fn)
{
  library_reveal_text_2022_t text;
  if (reveal_rest)
    num_cards = reveal_top_cards_of_library_2022(t_player, num_cards);
  else
  {
    int count = count_deck(t_player);
    if (num_cards > count)
      num_cards = count;
  }
  if (num_cards <= 0)
    return -1;

  int num_to_pick = test->qty;
  if (num_to_pick > num_cards)
    num_to_pick = num_cards;
  if (num_to_pick <= 0)
  {
    scnprintf(text.display, 100,
              " %s reveals:", cards_ptr[get_id(src_player, src_card)]->name);
    call_sub_437E20_unless_ai_is_speculating();
    hack_show_deck_force_expansion = 0;
    show_deck_exe(who_chooses, deck_ptr[t_player], num_cards, text.display, 0, 0x7375b0);
    return -1;
  }

  test->qty = 1;
  test->create_minideck = (uint16_t)num_cards;
  int chosen_iids[num_to_pick];
  int num_chosen = 0;
  int selected = -1;
  int cage = 0;
  if (destination_chosen == TUTOR2022_PLAY || destination_chosen == TUTOR2022_PLAY_TAPPED ||
      destination_chosen == TUTOR2022_PLAY_ATTACKING)
    cage = grafdiggers_cages(t_player, TYPE_CREATURE, 1) != 0;

  for (int i = 0; i < num_to_pick; ++i)
  {
    selected = select_card_from_zone_2022(who_chooses, t_player, deck_ptr[t_player], num_cards,
                                          must_select, test->ai_selection_mode, -1, test);
    if (selected == -1)
      break;
    --num_cards;
    int iid = deck_ptr[t_player][selected];
    if (reveal_chosen)
      reveal_card_iid(src_player, src_card, iid);

    switch (destination_chosen)
    {
    case TUTOR2022_HAND:
    case TUTOR2022_HAND_AND_MARK:
      remove_card_from_deck(t_player, selected);
      selected = add_card_to_hand(src_player, iid);
      if (destination_chosen == TUTOR2022_HAND_AND_MARK && selected >= 0)
        add_state(src_player, selected, STATE_TARGETTED);
      break;
    case TUTOR2022_GRAVE:
      remove_card_from_deck(t_player, selected);
      chosen_iids[num_chosen++] = iid;
      break;
    case TUTOR2022_PLAY:
    case TUTOR2022_PLAY_TAPPED:
    case TUTOR2022_PLAY_ATTACKING:
      if (!(cage && is_what(-1, deck_ptr[t_player][selected], TYPE_CREATURE)))
      {
        remove_card_from_deck(t_player, selected);
        chosen_iids[num_chosen++] = iid;
        break;
      }
    case TUTOR2022_DECK:
      for (int n = selected; n < num_cards; ++n)
      {
        int temporary = deck_ptr[t_player][n];
        deck_ptr[t_player][n] = deck_ptr[t_player][n + 1];
        deck_ptr[t_player][n + 1] = temporary;
      }
      break;
    case TUTOR2022_EXILE:
      rfg_card_in_deck(t_player, selected);
      break;
    case TUTOR2022_GET_ID:
      selected = (uint16_t)cards_data[iid].id;
    case TUTOR2022_GET_POSITION:
      if (num_to_pick > 1)
      {
        sprintf(text.diagnostic, "%s:%d\012illegal destination_chosen: %d with qty=%d",
                "functions/tutors.c", 884, (int)destination_chosen, num_to_pick);
        backtrace_and_abort(text.diagnostic);
      }
      break;
    case TUTOR2022_BOTTOM_OF_DECK:
      put_card_in_deck_to_bottom(t_player, selected);
      break;
    case TUTOR2022_TPLAYER_HAND:
      remove_card_from_deck(t_player, selected);
      selected = add_card_to_hand(t_player, iid);
      break;
    case TUTOR2022_BOTTOM_OF_DECK_RANDOM:
      sprintf(text.diagnostic, "%s:%d\012illegal destination_chosen: %d", "functions/tutors.c", 902,
              (int)destination_chosen);
      backtrace_and_abort(text.diagnostic);
      break;
    }
  }

  if (num_cards > 0)
    switch (destination_rest)
    {
    case TUTOR2022_HAND:
    case TUTOR2022_HAND_AND_MARK:
      for (int i = 0; i < num_cards; ++i)
      {
        int card_added = add_card_to_hand(t_player, deck_ptr[t_player][i]);
        if (card_added >= 0 && destination_rest == TUTOR2022_HAND_AND_MARK)
          add_state(t_player, card_added, STATE_TARGETTED);
      }
      obliterate_top_n_cards_of_deck(t_player, num_cards);
      break;
    case TUTOR2022_DECK:
      rearrange_top_x(t_player, t_player, num_cards);
      break;
    case TUTOR2022_GRAVE:
      mill(t_player, num_cards);
      break;
    case TUTOR2022_EXILE:
      rfg_top_n_cards_of_deck(t_player, num_cards);
      break;
    case TUTOR2022_BOTTOM_OF_DECK:
      put_top_x_on_bottom(t_player, t_player, num_cards);
      break;
    case TUTOR2022_TPLAYER_HAND:
      for (int i = 0; i < num_cards; ++i)
        add_card_to_hand(1 - t_player, deck_ptr[t_player][i]);
      obliterate_top_n_cards_of_deck(t_player, num_cards);
      break;
    case TUTOR2022_BOTTOM_OF_DECK_RANDOM:
      put_top_x_on_bottom_in_random_order(t_player, num_cards);
      break;
    case TUTOR2022_PLAY:
    case TUTOR2022_PLAY_TAPPED:
    case TUTOR2022_GET_ID:
    case TUTOR2022_PLAY_ATTACKING:
    case TUTOR2022_GET_POSITION:
      sprintf(text.diagnostic, "%s:%d\012illegal destination_rest: %d", "functions/tutors.c", 945,
              (int)destination_rest);
      backtrace_and_abort(text.diagnostic);
      break;
    }

  if (num_chosen <= 0)
    return selected;
  if (destination_chosen == TUTOR2022_GRAVE)
  {
    for (int i = num_chosen - 1; i >= 0; --i)
      raw_put_iid_on_top_of_deck(t_player, chosen_iids[i]);
    mill(t_player, num_chosen);
    return num_chosen;
  }
  if (destination_chosen == TUTOR2022_PLAY || destination_chosen == TUTOR2022_PLAY_TAPPED ||
      destination_chosen == TUTOR2022_PLAY_ATTACKING)
  {
    int result = num_chosen;
    for (int i = 0; i < num_chosen; ++i)
    {
      int card_added = add_card_to_hand(src_player, chosen_iids[i]);
      set_special_flags_2022(src_player, card_added, SF_PLAYED_FROM_DECK);
      card_instance_t *instance = get_card_instance(src_player, card_added);
      if (t_player != src_player)
        instance->state ^= STATE_OWNED_BY_OPPONENT;
      if (fn)
        fn(src_player, card_added);
      if (destination_chosen == TUTOR2022_PLAY_ATTACKING)
      {
        instance->state |= STATE_TAPPED | STATE_ATTACKING;
        put_into_play(src_player, card_added);
        choose_who_attack(src_player, card_added);
      }
      else
      {
        if (destination_chosen == TUTOR2022_PLAY_TAPPED)
          instance->state |= STATE_TAPPED;
        put_into_play(src_player, card_added);
        if (num_chosen == 1)
          result = card_added;
      }
    }
    return result;
  }
  sprintf(text.diagnostic, "%s:%d\012illegal destination_chosen: %d with num_chosen=%d",
          "functions/tutors.c", 999, (int)destination_chosen, num_chosen);
  backtrace_and_abort(text.diagnostic);
  return num_chosen;
}


/* ManalinkEh 2022: 0x2655580; original functions/tutors.c:779. */
int reveal_top_cards_of_library_and_choose_2022(int src_player, int src_card, int t_player,
                                                int num_cards, int must_select,
                                                tutor_destination_2022_t destination_chosen,
                                                int reveal_chosen,
                                                tutor_destination_2022_t destination_rest,
                                                int reveal_rest, test_definition_2022_t *test)
{
  int result;
  result = reveal_top_cards_of_library_and_choose_fn_who_chooses_2022(
      src_player, src_card, t_player, src_player, num_cards, must_select, destination_chosen,
      reveal_chosen, destination_rest, reveal_rest, test, (library_reveal_callback_2022_t)0x0);
  return result;
}


/* ManalinkEh 2022: 0x26555e0; original functions/tutors.c:1009. */
int reveal_top_cards_of_library_and_choose_type_2022(int src_player, int src_card, int t_player,
                                                     int num_cards, int must_select,
                                                     tutor_destination_2022_t destination_chosen,
                                                     int reveal_chosen,
                                                     tutor_destination_2022_t destination_rest,
                                                     int reveal_rest, type_t legal_types)
{
  test_definition_2022_t test;
  if ((legal_types & ~TYPE_EFFECT) == TYPE_NONEFFECT || legal_types == TYPE_NONE)
    default_test_definition_2022(&test, TYPE_ANY);
  else
  {
    new_default_test_definition_2022(&test, legal_types, "Select a ");
    if (ai_is_speculating != 1)
    {
      int length = strlen(test.message);
      type_text(test.message + length, 100 - length, legal_types, TYPETEXT_0);
      length = strlen(test.message);
      scnprintf(test.message + length, 100 - length, " card.");
    }
  }
  return reveal_top_cards_of_library_and_choose_fn_who_chooses_2022(
      src_player, src_card, t_player, src_player, num_cards, must_select, destination_chosen,
      reveal_chosen, destination_rest, reveal_rest, &test, NULL);
}


/* ManalinkEh 2022: 0x2655740; original functions/tutors.c:784.
 * Native behavior: this frontend ignores fn and passes NULL to the worker. */
int reveal_top_cards_of_library_and_choose_fn_2022(int src_player, int src_card, int t_player,
                                                   int num_cards, int must_select,
                                                   tutor_destination_2022_t destination_chosen,
                                                   int reveal_chosen,
                                                   tutor_destination_2022_t destination_rest,
                                                   int reveal_rest, test_definition_2022_t *test,
                                                   library_reveal_callback_2022_t fn)
{
  int result;
  result = reveal_top_cards_of_library_and_choose_fn_who_chooses_2022(
      src_player, src_card, t_player, src_player, num_cards, must_select, destination_chosen,
      reveal_chosen, destination_rest, reveal_rest, test, (library_reveal_callback_2022_t)0x0);
  return result;
}


/* ManalinkEh 2022: 0x26557a0; original functions/tutors.c:790. */
int reveal_top_cards_of_library_and_choose_who_chooses_2022(
    int src_player, int src_card, int t_player, int who_chooses, int num_cards, int must_select,
    tutor_destination_2022_t destination_chosen, int reveal_chosen,
    tutor_destination_2022_t destination_rest, int reveal_rest, test_definition_2022_t *test)
{
  int result;
  result = reveal_top_cards_of_library_and_choose_fn_who_chooses_2022(
      src_player, src_card, t_player, who_chooses, num_cards, must_select, destination_chosen,
      reveal_chosen, destination_rest, reveal_rest, test, (library_reveal_callback_2022_t)0x0);
  return result;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2650ca0; original functions/tutors.c:18. */
int show_deck_force_expansion_2022(int player, const int *deck, int num_cards, const char *prompt,
                                   int suppress_done_label, const char *done_label, int expansion)
{
  call_sub_437E20_unless_ai_is_speculating();
  hack_show_deck_force_expansion = expansion;
  int result = show_deck_exe(player, deck, num_cards, prompt, suppress_done_label,
                             (int32_t)(intptr_t)done_label);
  hack_show_deck_force_expansion = 0;
  return result;
}

#endif
