"""Typed reconstructions of hacked colors/subtypes, sleight masks/text and its AI cache."""

BODIES = {
'2680be0': r'''
int get_hacked_subtype_2022(int player, int card, int orig_subtype){
	if (card < 0 || player < 0){
		return orig_subtype;
	}

	static int subtypes[][4] = {
		{ 0, 0, 0, 0 },
		{ SUBTYPE_SWAMP,	HARDCODED_SUBTYPE_SWAMP,	COLOR_BLACK,	CARD_ID_SWAMP },
		{ SUBTYPE_ISLAND,	HARDCODED_SUBTYPE_ISLAND,	COLOR_BLUE,		CARD_ID_ISLAND },
		{ SUBTYPE_FOREST,	HARDCODED_SUBTYPE_FOREST,	COLOR_GREEN,	CARD_ID_FOREST },
		{ SUBTYPE_MOUNTAIN,	HARDCODED_SUBTYPE_MOUNTAIN,	COLOR_RED,		CARD_ID_MOUNTAIN },
		{ SUBTYPE_PLAINS,	HARDCODED_SUBTYPE_PLAINS,	COLOR_WHITE,	CARD_ID_PLAINS }
	};

	int column, orig_color_t;

	switch (orig_subtype){
		case SUBTYPE_SWAMP:				column = 0;	orig_color_t = COLOR_BLACK;	break;
		case HARDCODED_SUBTYPE_SWAMP:	column = 1;	orig_color_t = COLOR_BLACK;	break;
		case COLOR_BLACK:				column = 2;	orig_color_t = COLOR_BLACK;	break;
		case CARD_ID_SWAMP:				column = 3;	orig_color_t = COLOR_BLACK;	break;

		case SUBTYPE_ISLAND:			column = 0;	orig_color_t = COLOR_BLUE;	break;
		case HARDCODED_SUBTYPE_ISLAND:	column = 1;	orig_color_t = COLOR_BLUE;	break;
		case COLOR_BLUE:				column = 2;	orig_color_t = COLOR_BLUE;	break;
		case CARD_ID_ISLAND:			column = 3;	orig_color_t = COLOR_BLUE;	break;

		case SUBTYPE_FOREST:			column = 0;	orig_color_t = COLOR_GREEN;	break;
		case HARDCODED_SUBTYPE_FOREST:	column = 1;	orig_color_t = COLOR_GREEN;	break;
		case COLOR_GREEN:				column = 2;	orig_color_t = COLOR_GREEN;	break;
		case CARD_ID_FOREST:			column = 3;	orig_color_t = COLOR_GREEN;	break;

		case SUBTYPE_MOUNTAIN:			column = 0;	orig_color_t = COLOR_RED;	break;
		case HARDCODED_SUBTYPE_MOUNTAIN:column = 1;	orig_color_t = COLOR_RED;	break;
		case COLOR_RED:					column = 2;	orig_color_t = COLOR_RED;	break;
		case CARD_ID_MOUNTAIN:			column = 3;	orig_color_t = COLOR_RED;	break;

		case SUBTYPE_PLAINS:			column = 0;	orig_color_t = COLOR_WHITE;	break;
		case HARDCODED_SUBTYPE_PLAINS:	column = 1;	orig_color_t = COLOR_WHITE;	break;
		case COLOR_WHITE:				column = 2;	orig_color_t = COLOR_WHITE;	break;
		case CARD_ID_PLAINS:			column = 3;	orig_color_t = COLOR_WHITE;	break;

		default:
			return orig_subtype;
	}

	return subtypes[get_hacked_color_2022(player, card, orig_color_t)][column];
}
''',

'26808c0': r'''
const char* get_hacked_land_text_2022(int player, int card, const char* fmt, ...){
	static char buf[500];
	if (ai_is_speculating == 1){
		*buf = 0;
		return buf;
	}

	static const char* words[][3] = {
		{ "land",		"Land",		"LAND" },
		{ "swamp",		"Swamp",	"SWAMP" },
		{ "island",		"Island",	"ISLAND" },
		{ "forest",		"Forest",	"FOREST" },
		{ "mountain",	"Mountain",	"MOUNTAIN" },
		{ "plains",		"Plains",	"PLAINS" },
		{ "land",		"Land",		"LAND" },
		{ "land",		"Land",		"LAND" },
	};

	va_list args;
	va_start(args, fmt);

	const char *p = fmt;
	char *q = buf;
	for (; (*q = *p); ++p, ++q){
		if (*p == '%' && (*(p + 1) == 's' || *(p + 1) == 'a' || *(p + 1) == 'l')){

			int hacked, orig_color_t, orig_subtype = va_arg(args, int);

			switch (orig_subtype){
				case HARDCODED_SUBTYPE_SWAMP:	case SUBTYPE_SWAMP:		case COLOR_BLACK:	orig_color_t = COLOR_BLACK;	break;
				case HARDCODED_SUBTYPE_ISLAND:	case SUBTYPE_ISLAND:	case COLOR_BLUE:	orig_color_t = COLOR_BLUE;	break;
				case HARDCODED_SUBTYPE_FOREST:	case SUBTYPE_FOREST:	case COLOR_GREEN:	orig_color_t = COLOR_GREEN;	break;
				case HARDCODED_SUBTYPE_MOUNTAIN:case SUBTYPE_MOUNTAIN:	case COLOR_RED:		orig_color_t = COLOR_RED;	break;
				case HARDCODED_SUBTYPE_PLAINS:	case SUBTYPE_PLAINS:	case COLOR_WHITE:	orig_color_t = COLOR_WHITE;	break;
				default:
					hacked = orig_color_t = 0;
					goto skip_hack;
			}
			hacked = (player >= 0 && card >= 0) ? get_hacked_color_2022(player, card, orig_color_t) : orig_color_t;

		skip_hack:
			++p;
			int article = *p == 'a';
			int lowercase = *p == 'l';

			if (article){
				*q++ = 'a';
				if (hacked == COLOR_BLUE){
					*q++ = 'n';
				}
				*q++ = ' ';
			}
			if (hacked != orig_color_t){
				q += sprintf(q, "%s", words[hacked][2]) - 1;
			} else if (lowercase){
				q += sprintf(q, "%s", words[hacked][0]) - 1;
			} else {
				q += sprintf(q, "%s", words[hacked][1]) - 1;
			}
		}
	}

	va_end(args);

	return buf;
}
''',

'26807a0': r'''
int get_sleighted_protection_2022(int player, int card, int orig_prot){
	switch (orig_prot){
		
		case KEYWORD_PROT_BLACK:	orig_prot = COLOR_BLACK;	break;
		case KEYWORD_PROT_BLUE:		orig_prot = COLOR_BLUE;		break;
		case KEYWORD_PROT_GREEN:	orig_prot = COLOR_GREEN;	break;
		case KEYWORD_PROT_RED:		orig_prot = COLOR_RED;		break;
		case KEYWORD_PROT_WHITE:	orig_prot = COLOR_WHITE;	break;
		default:{
			if (orig_prot & KEYWORD_PROT_COLORED){
				
				uint32_t col, new_prot = 0;
				for (col = COLOR_BLACK; col <= COLOR_WHITE; ++col){
					if (orig_prot & (KEYWORD_PROT_BLACK << (col - 1))){
						new_prot |= KEYWORD_PROT_BLACK << (get_sleighted_color_2022(player, card, col) - 1);
					}
				}
				return (orig_prot & ~KEYWORD_PROT_COLORED) | new_prot;
			}
			return orig_prot;
		}
	}
	return KEYWORD_PROT_BLACK << (get_sleighted_color_2022(player, card, orig_prot) - 1);
}
''',

'26802f0': r'''
const char* get_sleighted_color_text2_2022(int player, int card, const char* fmt, int orig_color_t_1, int orig_color_t_2){
	if (ai_is_speculating == 1){
		return "";
	}

	static char buf[500];

	static const char* words[][2] = {
		{ "colorless",	"COLORLESS" },
		{ "black",		"BLACK" },
		{ "blue",		"BLUE" },
		{ "green",		"GREEN" },
		{ "red",		"RED" },
		{ "white",		"WHITE" },
		{ "artifact",	"ARTIFACT" },
		{ "any",		"ANY" }
	};

	int sleighted1 = get_sleighted_color_2022(player, card, orig_color_t_1);
	int sleighted2 = get_sleighted_color_2022(player, card, orig_color_t_2);

	sprintf(buf, fmt, words[sleighted1][sleighted1 == orig_color_t_1 ? 0 : 1], words[sleighted2][sleighted2 == orig_color_t_2 ? 0 : 1]);

	return buf;
}
''',

'267f7a0': r'''
int get_colors_to_sleight_from_text_2022(int csvid)
{
	char *txt = cards_ptr[csvid]->rules_text;
	int colors_to_sleight = 0;
	if (txt){
		while ((txt = strstr(txt, "|S")))
		{
			txt += 2;
			int c;
			for (c = COLOR_BLACK; c <= COLOR_WHITE; ++c)
			{
				int i;
				for (i = 0; i < num_words_to_sleight; ++i){
					int l = strlen(strs_words_to_sleight[c][i]);
					if (!strncmp(txt, strs_words_to_sleight[c][i], l))
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

'267f8c0': r'''
int get_hacked_color_2022(int player, int card, int color)
{
	
  card_instance_t *v3; 
  int result; 

  v3 = get_card_instance(player, card);
  result = v3->hack_mode[color];
  if (!v3->hack_mode[color])
    result = color;
  return result;
}
''',

'267f900': r'''
int get_sleighted_color_2022(int player, int card, int color)
{
	
  card_instance_t *instance; 
  color_t result; 

  instance = get_card_instance(player, card);
  result = instance->color_id[color];
  if (!instance->color_id[color])
    result = color;
  return result;
}
''',

'2680270': r'''
const char* get_sleighted_color_text_2022(int player, int card, const char* fmt, int orig_color_t){
	if (ai_is_speculating == 1){
		return "";
	}

	static char buf[500];

	static const char* words[][2] = {
		{ "colorless",	"COLORLESS" },
		{ "black",		"BLACK" },
		{ "blue",		"BLUE" },
		{ "green",		"GREEN" },
		{ "red",		"RED" },
		{ "white",		"WHITE" },
		{ "artifact",	"ARTIFACT" },
		{ "any",		"ANY" }
	};

	int sleighted = get_sleighted_color_2022(player, card, orig_color_t);

	sprintf(buf, fmt, words[sleighted][sleighted == orig_color_t ? 0 : 1]);

	return buf;
}
''',

'26803c0': r'''
int get_sleighted_color_test_2022(int player, int card, int orig_color_test){
	if (card < 0 || player < 0){
		return orig_color_test;
	}
	switch (orig_color_test){
		
		case COLOR_TEST_BLACK:	orig_color_test = COLOR_BLACK;	break;
		case COLOR_TEST_BLUE:	orig_color_test = COLOR_BLUE;	break;
		case COLOR_TEST_GREEN:	orig_color_test = COLOR_GREEN;	break;
		case COLOR_TEST_RED:	orig_color_test = COLOR_RED;	break;
		case COLOR_TEST_WHITE:	orig_color_test = COLOR_WHITE;	break;
		default:{
			if (orig_color_test & COLOR_TEST_ANY_COLORED){
				
				uint32_t col, new_color_test = 0;
				for (col = COLOR_BLACK; col <= COLOR_WHITE; ++col){
					if (orig_color_test & (COLOR_TEST_BLACK << (col - 1))){
						new_color_test |= COLOR_TEST_BLACK << (get_sleighted_color_2022(player, card, col) - 1);
					}
				}
				return (orig_color_test & ~COLOR_TEST_ANY_COLORED) | new_color_test;
			}
			return orig_color_test;
		}
	}
	return COLOR_TEST_BLACK << (get_sleighted_color_2022(player, card, orig_color_test) - 1);
}
''',

'2680da0': r'''
int color_to_color_test_2022(int color)
{
  if (color == COLOR_ANY)
    return COLOR_TEST_ANY_COLORED;
  return (int)(1U << ((unsigned int)color & 31));
}
''',

'24cd640': r'''
int ai_can_sleight_card_2022(int player, int card){
	int colors_to_sleight = cards_ptr[get_id(player, card)]->sleight_color;
	if( ! check_special_flags3(player, card, SF3_HARDCODED_SLEIGHT_WORDS_REPLACED) ){
		set_special_flags3(player, card, SF3_HARDCODED_SLEIGHT_WORDS_REPLACED);
		cards_ptr[get_id(player, card)]->sleight_color = colors_to_sleight = get_colors_to_sleight_from_text_2022(get_id(player, card));
	}
	return colors_to_sleight;
}
''',

}
