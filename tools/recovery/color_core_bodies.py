"""Typed color query/change, hack-word scanner, counting and chooser reconstructions."""

BODIES = {
'267c3a0': r'''
int get_global_color_hack_2022(int player)
{
  unsigned int timestamp = 0;
  int result = 0;
  for (int p = 0; p < 2; ++p)
    for (int c = 0; c < active_cards_count[p]; ++c)
      if (in_play(p, c))
      {
        if (get_id(p, c) == CARD_ID_PAINTERS_SERVANT)
        {
          card_instance_t *instance = get_card_instance(p, c);
          if (instance->targets[1].card >= 0 && instance->timestamp > timestamp)
          {
            result |= instance->targets[1].card;
            timestamp = instance->timestamp;
          }
        }
        if (get_id(p, c) == CARD_ID_MYCOSYNTH_LATTICE)
        {
          card_instance_t *instance = get_card_instance(p, c);
          if (instance->timestamp > timestamp)
          {
            result = COLOR_TEST_COLORLESS;
            timestamp = instance->timestamp;
          }
        }
      }
  return result;
}
''',

'267ea70': r'''
int get_color_2022(int player, int card)
{
  int result = get_abilities(player, card, EVENT_SET_COLOR, -1);
  int half_played = get_card_instance(player, card)->info_slot;
  int csvid = get_id(player, card);
  int split_csvid = split_cards_id_table(csvid, half_played);
  int iid = get_internal_card_id_from_csv_id(split_csvid);
  if (cards_data[iid].color)
    result = cards_data[iid].color;
  return result;
}
''',

'267eaf0': r'''
int get_color_by_internal_id_no_hack_2022(int internal_id)
{
  card_data_t *data = &cards_data[internal_id];
  int color = data->color;
  if (data->type == TYPE_LAND)
  {
    if (data->id == CARD_ID_DRYAD_ARBOR)
      color = COLOR_TEST_GREEN;
    else if (data->id == 0x5186) /* Jasconian Isle, csvid 20870 in the release database. */
      color = COLOR_TEST_BLUE;
    else
      color = 0;
  }
  else if ((data->type & TYPE_ARTIFACT) && data->cc[0] == 0)
    color = 0;
  if ((data->extra_ability & EA_MANA_SOURCE) && data->color != COLOR_TEST_COLORLESS)
    color &= COLOR_TEST_ANY_COLORED;
  return color;
}
''',

'26804d0': r'''
int get_color_by_internal_id_2022(int player, int internal_id)
{
  unsigned int timestamp = 0;
  int changer = -1;
  int hack = 0;
  for (int p = 0; p < 2; ++p)
    for (int c = 0; c < active_cards_count[p]; ++c)
      if (in_play(p, c) && !is_humiliated_for_ability(p, c, AL_STATIC))
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
            hack = get_sleighted_color_test_2022(player, c, COLOR_TEST_WHITE);
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
  int color = get_color_by_internal_id_no_hack_2022(internal_id);
  if (changer == CARD_ID_MYCOSYNTH_LATTICE)
    return hack;
  if (changer == CARD_ID_PAINTERS_SERVANT)
    return color | hack;
  if (changer == CARD_ID_CELESTIAL_DAWN && !(cards_data[internal_id].type & TYPE_LAND))
    return hack;
  return color;
}
''',

'26806c0': r'''
int get_color_real_or_iid_2022(int player_or_owner, int player_or_negone, int card_or_iid)
{
  
  return (player_or_negone >= 0
		  ? get_color_2022(player_or_negone, card_or_iid)					
		  : get_color_by_internal_id_2022(player_or_owner, card_or_iid));	
}
''',

'26806f0': r'''
void change_color_2022(int player, int card, int t_player, int t_card, color_test_t new_color, change_color_t mode){
	
	
	
	
	int legacy = create_targetted_legacy_effect_no_repeat(player, card, &legacy_change_color_2022, t_player, t_card);
	card_instance_t *instance = get_card_instance(player, legacy);
	instance->targets[1].player = mode;
	if (!(mode & CHANGE_COLOR_NO_SLEIGHT)){
		new_color = get_sleighted_color_test_2022(player, card, new_color);
	}
	instance->targets[1].card = new_color;
	if (!(mode & CHANGE_COLOR_NO_SOUND)){
		play_sound_effect(WAV_CHANGEC);
	}
}
''',

'2682fe0': r'''
int legacy_change_color_2022(int player, int card, event_t event){
	card_instance_t *instance = get_card_instance(player, card);
	int p = instance->targets[0].player;
	int c = instance->targets[0].card;

	if( instance->targets[1].player > 0 ){
		if( event == EVENT_SET_COLOR && affect_me(p, c) ){
			if( instance->targets[1].player & CHANGE_COLOR_ADD ){
				event_result |= instance->targets[1].card;
			}
			if( instance->targets[1].player & CHANGE_COLOR_SET ){
				event_result = instance->targets[1].card;
			}
		}
		if( (instance->targets[1].player & CHANGE_COLOR_END_AT_EOT) && eot_trigger(player, card, event) ){
			kill_card(player, card, KILL_EXILE);
		}
	}

	return 0;
}
''',

'2680af0': r'''
int get_hacked_walk_2022(int player, int card, int orig_walk){
	switch (orig_walk){
		
		case KEYWORD_SWAMPWALK:		orig_walk = COLOR_BLACK;	break;
		case KEYWORD_ISLANDWALK:	orig_walk = COLOR_BLUE;		break;
		case KEYWORD_FORESTWALK:	orig_walk = COLOR_GREEN;	break;
		case KEYWORD_MOUNTAINWALK:	orig_walk = COLOR_RED;		break;
		case KEYWORD_PLAINSWALK:	orig_walk = COLOR_WHITE;	break;
		default:{
			if (orig_walk & KEYWORD_BASIC_LANDWALK){
				
				uint32_t col, new_walk = 0;
				for (col = COLOR_BLACK; col <= COLOR_WHITE; ++col){
					if (orig_walk & (KEYWORD_SWAMPWALK << (col - 1))){
						new_walk |= KEYWORD_SWAMPWALK << (get_hacked_color_2022(player, card, col) - 1);
					}
				}
				return (orig_walk & ~KEYWORD_BASIC_LANDWALK) | new_walk;
			}
			return orig_walk;
		}
	}
	return KEYWORD_SWAMPWALK << (get_hacked_color_2022(player, card, orig_walk) - 1);
}
''',

'2680dc0': r'''
int keyword_to_color_2022(int keyword){

	int i;
	for(i=0; i<5; i++){
		if( (1 << (11+i)) == keyword ){ 
			return i+1;
		}
	}
	return 0;
}
''',

'2678330': r'''
int get_colors_to_hack_from_type_text_2022(int csvid)
{
	const char *txt = cards_ptr[csvid]->type_text;
	int colors_to_sleight = 0;
	if (txt){
		while ((txt = strstr(txt, "|H")))
		{
			txt += 2;
			int c;
			for (c = COLOR_BLACK; c <= COLOR_WHITE; ++c)
			{
				int i;
				for (i = 0; i < num_words_to_hack; ++i){
					int l = strlen(strs_words_to_hack[c][i]);
					if (!strncmp(txt, strs_words_to_hack[c][i], l))
					{
						txt += l;
						colors_to_sleight |= 1 << c;
						goto outer_continue;
					}
				}
			}
			outer_continue:;
		}
	}
	return colors_to_sleight;
}
''',

'2678430': r'''
int get_colors_to_hack_from_rules_text_2022(int csvid)
{
	char *txt = cards_ptr[csvid]->rules_text;
	int colors_to_sleight = 0;
	if (txt){
		while ((txt = strstr(txt, "|H")))
		{
			txt += 2;
			int c;
			for (c = COLOR_BLACK; c <= COLOR_WHITE; ++c)
			{
				int i;
				for (i = 0; i < num_words_to_hack; ++i){
					int l = strlen(strs_words_to_hack[c][i]);
					if (!strncmp(txt, strs_words_to_hack[c][i], l))
					{
						txt += l;
						colors_to_sleight |= 1 << c;
						goto outer_continue;
					}
				}
			}
			outer_continue:;
		}
	}
	return colors_to_sleight;
}
''',

'267f8a0': r'''
int get_colors_to_hack_from_csvid_2022(int csvid){
	int result = get_colors_to_hack_from_type_text_2022(csvid);
	result |= get_colors_to_hack_from_rules_text_2022(csvid);
	return result;
}
''',

'267f6a0': r'''
int is_colorless_2022(int player, int card){
	if( get_color_2022(player, card) < 2 ){
		return 1;
	}
	return 0;
}
''',

'2688770': r'''
int colors_shared_2022(int clr1, int clr2){
	return clr1 & clr2 & COLOR_TEST_ANY_COLORED;
}
''',

'2688780': r'''
int has_my_colors_2022(int player, int card, int t_player, int t_card ){

	int clrs[2];
	if( player != -1 ){
		clrs[0] = get_color_2022(player, card);
	}
	else{
		clrs[0] = cards_data[card].color;
	}
	if( t_player != -1 ){
		clrs[1] = get_color_2022(t_player, t_card);
	}
	else{
		clrs[1] = cards_data[t_card].color;
	}

	return colors_shared_2022(clrs[0], clrs[1]);
}
''',

'268c860': r'''
int real_count_colors_2022(int clr)
{
  return num_bits_set((unsigned int)clr & COLOR_TEST_ANY_COLORED);
}
''',

'268c8b0': r'''
int count_colors_2022(int player, int card){

	int clr;
	if( player != -1 ){
		clr = get_color_2022(player, card);
	}
	else{
		clr = cards_data[card].color;
		if (cards_data[card].type == TYPE_LAND){
			return cards_data[card].id == CARD_ID_DRYAD_ARBOR ? 1 : 0;
		}
	}
	return real_count_colors_2022(clr);
}
''',

'268c950': r'''
int evaluate_colors_2022(int color1, int color2){
	if( color2 != COLOR_TEST_COLORLESS ){
		int i;
		for(i=1; i<6; i++){
			if( !(color1 & (1<<i)) && (color2 & (1<<i)) ){
				return 0;
			}
		}
	}
	return 1;
}
''',

'268b5f0': r'''
int is_monocolor_hybrid_2022(card_ptr_t *cp)
{
  const char *c = cp->mana_cost_text;
  return c && c[0] == '|' && c[1] == '2' &&
         (c[2] == 'B' || c[2] == 'U' || c[2] == 'G' || c[2] == 'R' || c[2] == 'W');
}
''',

'26845a0': r'''
int get_deck_color_2022(int player, int whose_deck){
	if (player >= 0 && !IS_AI(player) && player != whose_deck){
		return COLOR_BLACK;	
	}
	int *deck = deck_ptr[whose_deck];
	int count = count_deck(whose_deck)-1;
	int clrs[5] = {0, 0, 0, 0, 0};
	while( count > -1 ){
			int color = cards_data[ deck[count] ].color;
			if( (color & COLOR_TEST_ANY_COLORED) && ! is_what(-1, deck[count], TYPE_LAND) ){
				int i;
				for (i = COLOR_BLACK; i <= COLOR_WHITE; i++){
					if( color & (1 << i) ){
						clrs[i-1]++;
					}
				}
			}
			count--;
	}
	if( player != -1 ){
		int ai_choice = 1;
		int i;
		int par = -1;
		for (i = COLOR_BLACK; i <= COLOR_WHITE; i++){
			if( clrs[i - 1] > par ){
				par = clrs[i - 1];
				ai_choice = i;
			}
		}
		return ai_choice;
	}
	else{
		int clrs_res[2] = {0, 0};
		int par = -1;
		int i;
		for (i = COLOR_BLACK; i <= COLOR_WHITE; i++){
			if( clrs[i - 1] > par ){
				par = clrs[i - 1];
				clrs_res[0] = i;
			}
		}
		par = -1;
		for (i = COLOR_BLACK; i <= COLOR_WHITE; i++){
			if( clrs[i - 1] > par && i != clrs_res[0] ){
				par = clrs[i - 1];
				clrs_res[1] = i;
			}
		}
		return (1 << clrs_res[0]) | (1 << clrs_res[1]);
	}
}
''',

'2684730': r'''
int select_a_color_2022(int player){

  int ai_choice = 0;

  if( player == AI ){
	 ai_choice = get_deck_color_2022(player, 1-player);
  }

  int color = choose_a_color_2022(player, ai_choice);
  int keyword = color_to_color_test_2022(color);

  return keyword;
}
''',

'267c1d0': r'''
int choose_a_color_from_2022(int player, int ai_choice, color_test_t available_colors){
	int result = -1;
	while(result == -1){
		result = choose_a_color_exe(player, "Choose a color", 1, ai_choice, available_colors);
	}
	return result;
}
''',

'267c190': r'''
int choose_a_color_2022(int player, int ai_choice){
	int result = -1;
	while(result == -1){
		result = choose_a_color_exe(player, "Choose a color", 1, ai_choice, COLOR_TEST_ANY_COLORED);
	}
	return result;
}
''',

}
