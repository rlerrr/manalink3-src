"""Typed reconstruction of the October 2022 card-display helper group."""

BODIES = {
'2657690': r'''
int memcpy_displayed_inst_and_ext(inst_and_ext_t *dest, int player, int card)
{
  if (!dest || (unsigned)player >= 2 || (unsigned)card >= 150)
    return 0;
  card_instance_t *src = get_displayed_card_instance(player, card);
  EnterCriticalSection(critical_section_for_display);
  memcpy(&dest->ext, src->ext, sizeof(dest->ext));
  memcpy(&dest->inst, src, sizeof(dest->inst));
  LeaveCriticalSection(critical_section_for_display);
  dest->inst.ext = &dest->ext;
  return 1;
}
''',
'26590a0': r'''
void legacy_name_or_text(char *str, int player, int card, event_t event)
{
  if (event != EVENT_SET_LEGACY_EFFECT_NAME && event != EVENT_SET_LEGACY_EFFECT_TEXT)
    backtrace_and_abort("functions/windows.c:2241\nevent == EVENT_SET_LEGACY_EFFECT_NAME || event == EVENT_SET_LEGACY_EFFECT_TEXT");
  card_instance_t *instance = get_displayed_card_instance(player, card);
  int alternative = instance->eot_toughness & 255;
  if ((instance->eot_toughness & event) &&
      (instance->internal_card_id == LEGACY_EFFECT_CUSTOM || instance->internal_card_id == LEGACY_EFFECT_ACTIVATED))
  {
    set_legacy_effect_name_addr = str;
    instance->info_slot_fn(player, card, event);
    set_legacy_effect_name_addr[event == EVENT_SET_LEGACY_EFFECT_NAME ? 51 : 999] = 0;
  }
  if ((unsigned)(alternative - 1) < 9)
    for (char *p = str; *p; )
    {
      if (p[0] == '|' && p[1] == '#')
      {
        if (atoi(p + 2) == alternative)
        {
          char tmp[2000], *q = tmp;
          p += 3;
          while (*p && !(p[0] == '|' && p[1] == '#'))
            *q++ = *p++;
          *q = 0;
          strcpy(str, tmp);
          return;
        }
        p += 2;
      }
      else
        ++p;
    }
}
''',
'265b7c0': r'''
void keywords_to_str(char *dest, keyword_t keywords, sp_keyword_t spkeywords)
{
  if (!kws_strs[0])
  {
    kws_strs[0] = ability_tooltip_names[10];
    kws_strs[1] = ability_tooltip_names[11];
    kws_strs[2] = ability_tooltip_names[12];
    kws_strs[3] = ability_tooltip_names[13];
    kws_strs[4] = ability_tooltip_names[14];
    kws_strs[5] = ability_tooltip_names[0];
    kws_strs[6] = ability_tooltip_names[2];
    kws_strs[7] = ability_tooltip_names[3];
    kws_strs[8] = ability_tooltip_names[5];
    kws_strs[9] = ability_tooltip_names[7];
    kws_strs[10] = ability_tooltip_names[1];
    kws_strs[11] = ability_tooltip_names[15];
    kws_strs[12] = ability_tooltip_names[16];
    kws_strs[13] = ability_tooltip_names[17];
    kws_strs[14] = ability_tooltip_names[18];
    kws_strs[15] = ability_tooltip_names[19];
    kws_strs[16] = ability_tooltip_names[20];
    kws_strs[17] = ability_tooltip_names[39];
    kws_strs[21] = ability_tooltip_names[21];
    kws_strs[23] = ability_tooltip_names[22];
    kws_strs[29] = ability_tooltip_names[40];
    kws_strs[30] = ability_tooltip_names[6];
    kws_strs[18] = kws_strs[19] = kws_strs[20] = kws_strs[22] = "";
    kws_strs[24] = kws_strs[25] = kws_strs[26] = kws_strs[27] = kws_strs[28] = kws_strs[31] = "";
    spkws_strs[0] = ability_tooltip_names[27];
    spkws_strs[1] = ability_tooltip_names[30];
    spkws_strs[2] = ability_tooltip_names[37];
    spkws_strs[3] = ability_tooltip_names[4];
    spkws_strs[4] = ability_tooltip_names[24];
    spkws_strs[5] = ability_tooltip_names[23];
    spkws_strs[6] = ability_tooltip_names[32];
    spkws_strs[8] = ability_tooltip_names[25];
    spkws_strs[9] = ability_tooltip_names[29];
    spkws_strs[11] = ability_tooltip_names[36];
    spkws_strs[13] = ability_tooltip_names[31];
    spkws_strs[14] = ability_tooltip_names[34];
    spkws_strs[15] = ability_tooltip_names[33];
    spkws_strs[16] = ability_tooltip_names[49];
    spkws_strs[17] = ability_tooltip_names[48];
    spkws_strs[23] = ability_tooltip_names[38];
    spkws_strs[25] = ability_tooltip_names[28];
    spkws_strs[29] = ability_tooltip_names[66];
    spkws_strs[30] = ability_tooltip_names[26];
    spkws_strs[7] = spkws_strs[10] = spkws_strs[12] = "";
    spkws_strs[18] = spkws_strs[19] = spkws_strs[20] = spkws_strs[21] = spkws_strs[22] = "";
    spkws_strs[24] = spkws_strs[26] = spkws_strs[27] = spkws_strs[28] = spkws_strs[31] = "";
  }
  const char *strs[65];
  int num_strs = 0;
  unsigned kw = keywords, kw2 = spkeywords;
  while (kw)
  {
    int bit = __builtin_ffs(kw) - 1;
    kw &= ~(1u << bit);
    if (*kws_strs[bit])
      strs[num_strs++] = kws_strs[bit];
  }
  while (kw2)
  {
    int bit = __builtin_ffs(kw2) - 1;
    kw2 &= ~(1u << bit);
    if (*spkws_strs[bit])
      strs[num_strs++] = spkws_strs[bit];
  }
  if (!num_strs)
    *dest = 0;
  else if (num_strs == 1)
  {
    strcpy(dest, strs[0]);
    *dest = tolower((signed char)*dest);
  }
  else
  {
    sort_ArrayConstCharPtr(strs, num_strs);
    if (num_strs == 2)
      sprintf(dest, "%c%s and %c%s", tolower((signed char)*strs[0]), strs[0] + 1,
              tolower((signed char)*strs[1]), strs[1] + 1);
    else
    {
      for (int i = 0; i < num_strs - 1; ++i)
        dest += sprintf(dest, "%c%s, ", tolower((signed char)*strs[i]), strs[i] + 1);
      sprintf(dest, "and %c%s", tolower((signed char)*strs[num_strs - 1]), strs[num_strs - 1] + 1);
    }
  }
}
''',
'265bca0': r'''
void draw_fullcard_special_effect_card_impl_2022(HDC hdc, RECT *rect, int csvid, int player, int card)
{
  card_ptr_t cp;
  memset(&cp, 0, sizeof(cp));
  card_instance_t *dispinst = get_displayed_card_instance(player, card), inst;
  EnterCriticalSection(critical_section_for_display);
  memcpy(&inst, dispinst, sizeof(inst));
  LeaveCriticalSection(critical_section_for_display);
  int src_csvid = inst.display_pic_csv_id, version = inst.display_pic_num;
  char card_name[100], rules_text[2000];
  const char *type_text;
  switch (csvid)
  {
    case 901:
    {
      int l = sprintf(card_name, "%s: %d", str_Damage, inst.info_slot);
      color_test_t cols = inst.color & COLOR_TEST_ANY_COLORED;
      if (num_bits_set(cols) > 1)
        strcpy(card_name + l, " (Multi)");
      else if (cols)
        sprintf(card_name + l, " (%s)", strs_Colorless_Black_Blue_Green_Red_White[single_color_test_bit_to_color(cols)]);
      strcpy(rules_text, cards_txt[src_csvid].damage_text);
      type_text = "Damage";
      unsigned token_status = inst.token_status, spkw = inst.ext->sp_keywords;
      if ((token_status & 0x1c0000) || (inst.regen_status & 0x20000000) || (spkw & 0x4202))
      {
        char *end = rules_text + strlen(rules_text);
        if (end != rules_text && end[-1] != '\n')
          *end++ = '\n', *end = 0;
        if (token_status & 0x140000)
          strcpy(end, "Combat damage\n"), end += 14;
        if (spkw & 0x200)
          end += sprintf(end, "%s\n", ability_tooltip_names[29]);
        if (token_status & 0x100000)
          end += sprintf(end, "%s\n", ability_tooltip_names[5]);
        if (inst.regen_status & 0x20000000)
          end += sprintf(end, "%s\n", ability_tooltip_names[40]);
        if (spkw & 2)
          end += sprintf(end, "%s\n", ability_tooltip_names[30]);
        if (token_status & 0x80000)
          end += sprintf(end, "%s\n", ability_tooltip_names[3]);
        if (spkw & 0x4000)
          sprintf(end, "%s\n", ability_tooltip_names[34]);
      }
      break;
    }
    case 902:
      strcpy(card_name, cards_txt[src_csvid].legacy_title_text);
      strcpy(rules_text, cards_txt[src_csvid].legacy_text);
      type_text = "Effect";
      if (inst.eot_toughness)
        legacy_name_or_text(rules_text, player, card, EVENT_SET_LEGACY_EFFECT_TEXT);
      break;
    case 903:
    {
      const card_txt_t *text = cards_txt + src_csvid;
      strcpy(card_name, text->effect_title_text ? text->effect_title_text : "");
      if (inst.eot_toughness && (unsigned)(src_csvid - 864) > 1)
      {
        legacy_name_or_text(card_name, player, card, EVENT_SET_LEGACY_EFFECT_NAME);
        text = cards_txt + src_csvid;
      }
      if (!*card_name)
        strcpy(card_name, cards_ptr[src_csvid]->name);
      strcpy(rules_text, text->effect_text ? text->effect_text : "");
      type_text = "Effect";
      if (inst.eot_toughness)
        legacy_name_or_text(rules_text, player, card, EVENT_SET_LEGACY_EFFECT_TEXT);
      break;
    }
    case 905:
      strcpy(card_name, get_card_or_subtype_name(inst.info_slot));
      strcpy(rules_text, cards_txt[src_csvid].effect_text);
      type_text = "Effect";
      break;
    case 907:
      strcpy(card_name, str_MultiBlock_Creature);
      strcpy(rules_text, EXE_STR(0x78fcd8));
      type_text = "Blocker";
      break;
    default:
      *card_name = *rules_text = 0;
      type_text = NULL;
      break;
  }
  char *first_pipe = strchr(rules_text, '|');
  if (first_pipe)
  {
    if (inst.token_status & 0x2000000)
      player = inst.damage_source_player, card = inst.damage_source_card;
    char *pipe_s = strstr(rules_text, "|S");
    if (pipe_s)
    {
      display_sleight_text_2022(player, card, pipe_s);
      pipe_s = strstr(first_pipe, "|S");
      if (pipe_s)
      {
        display_replace_text_2022(pipe_s, "|S1", 1, "");
        display_replace_text_2022(pipe_s, "|S2", 1, "");
        display_replace_text_2022(pipe_s, "|S3", 1, "");
        display_replace_text_2022(pipe_s, "|S4", 1, "");
        display_replace_text_2022(pipe_s, "|S", 1, "");
      }
    }
    char *pipe_h = strstr(rules_text, "|H");
    if (pipe_h)
    {
      display_hack_text_2022(player, card, pipe_h);
      pipe_h = strstr(pipe_h, "|H");
      if (pipe_h)
      {
        display_replace_text_2022(pipe_h, "|H1", 1, "");
        display_replace_text_2022(pipe_h, "|H2", 1, "");
        display_replace_text_2022(pipe_h, "|H3", 1, "");
        display_replace_text_2022(pipe_h, "|H4", 1, "");
        display_replace_text_2022(pipe_h, "|H", 1, "");
      }
    }
    char *pipe_a = strstr(first_pipe, "|a");
    if (pipe_a)
    {
      if (inst.info_slot_fn == undying_legacy)
        display_replace_text_2022(pipe_a, "|a", 1, "undying");
      else
      {
        char kwstr[1000];
        keyword_t kw = inst.targets[1].player == -1 ? 0 : inst.targets[1].player;
        sp_keyword_t spkw = inst.targets[1].card == -1 ? 0 : inst.targets[1].card;
        keywords_to_str(kwstr, kw, spkw);
        display_replace_text_2022(pipe_a, "|a", 1, kwstr);
      }
    }
    char *pipe_e = strstr(first_pipe, "|e");
    if (pipe_e)
    {
      char et[15];
      char *pipe_e0 = strstr(pipe_e, "|e0");
      if (pipe_e0)
      {
        sprintf(et, "%d", inst.eot_toughness & 255);
        display_replace_text_2022(pipe_e0, "|N", 1, et);
      }
      char *pipe_e1 = strstr(pipe_e, "|e1");
      if (pipe_e1)
      {
        sprintf(et, "%d", (inst.eot_toughness >> 8) & 255);
        display_replace_text_2022(pipe_e1, "|N", 1, et);
      }
      char *pipe_e2 = strstr(pipe_e, "|e2");
      if (pipe_e2)
      {
        unsigned saved_eot = inst.eot_toughness;
        sprintf(et, "%d", (saved_eot >> 16) & 255);
        display_replace_text_2022(pipe_e2, "|N", 1, et);
        char *pipe_e3 = strstr(pipe_e, "|e3");
        sprintf(et, "%d", saved_eot >> 24);
        display_replace_text_2022(pipe_e3, "|N", 1, et);
      }
      pipe_e = strstr(pipe_e, "|e");
      if (pipe_e)
      {
        sprintf(et, "%d", (int)inst.eot_toughness);
        display_replace_text_2022(pipe_e, "|N", 1, et);
      }
    }
    char *pipe_N = strstr(first_pipe, "|N");
    if (pipe_N)
    {
      char pt[15];
      sprintf(pt, "%d", inst.counter_power);
      display_replace_text_2022(pipe_N, "|N", 1, pt);
    }
    char *pipe_n = strstr(first_pipe, "|n");
    if (pipe_n)
      effect_card_text_replace_pipe_n(rules_text < pipe_n - 4 ? pipe_n - 4 : rules_text, inst.counter_power, inst.counter_toughness);
  }
  cp.expansion = -1;
  cp.id = src_csvid;
  cp.name = cp.full_name = card_name;
  cp.color = -1;
  cp.type_text = type_text;
  cp.card_type = 0;
  cp.db_card_type_2 = -1;
  char emptystr = 0;
  cp.flavor_text = &emptystr;
  cp.rules_text = rules_text;
  cp.subtype1 = cp.subtype2 = -1;
  DrawFullCard(hdc, rect, &cp, version, 2, EXE_DWORD(0x787248), EXE_STR(0x6202a8));
}
''',
'265c8b0': r'''
void draw_fullcard_special_effect_card(HDC hdc, RECT *rect, int csvid, int player, int card)
{
  if (hdc && rect)
    draw_fullcard_special_effect_card_impl_2022(hdc, rect, csvid, player, card);
}
''',
'26577b0': r'''
LRESULT CALLBACK wndproc_ShowListCard_hook(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  if (msg == WM_PAINT)
  {
    int csvid = GetWindowLongA(hwnd, EXE_DWORD(0x4d74b4));
    int show_numeral = GetWindowLongA(hwnd, EXE_DWORD(0x4d74bc));
    int numeral = GetWindowLongA(hwnd, EXE_DWORD(0x4d74b8));
    EnterCriticalSection(display_critical_section_for_painting_2022);
    RECT rect;
    GetClientRect(hwnd, &rect);
    FillRect(display_dblbfr_hdc_2022, &rect, GetStockObject(4));
    if (csvid == 904)
      DrawCardBack(display_dblbfr_hdc_2022, &rect, 0);
    else
      DrawSmallCardEx(display_dblbfr_hdc_2022, &rect, cards_ptr[csvid], 0, 0, -1, -1, hack_show_deck_force_expansion);
    if (show_numeral)
      display_draw_numeral_2022(display_dblbfr_hdc_2022, &rect, numeral);
    PAINTSTRUCT paint;
    HDC hdc = BeginPaint(hwnd, &paint);
    if (hdc)
    {
      set_palette_and_stretch_mode(hdc);
      BitBlt(hdc, 0, 0, rect.right, rect.bottom, display_dblbfr_hdc_2022, 0, 0, SRCCOPY);
      EndPaint(hwnd, &paint);
    }
    LeaveCriticalSection(display_critical_section_for_painting_2022);
    return 0;
  }
  if (msg == 0x437)
  {
    if (option_Layout != 2)
    {
      int expansion = hack_show_deck_force_expansion;
      int forced = (unsigned)(expansion - 1) < 197;
      int csvid = GetWindowLongA(hwnd, EXE_DWORD(0x4d74b4));
      SendMessageA(hwnd_FullCardClass, 0x401, forced ? csvid | 0x40000000 : csvid, forced ? expansion : 0);
    }
    return 0;
  }
  if (msg == WM_MOUSEMOVE || msg == WM_RBUTTONDOWN)
  {
    if ((option_Layout == 2) != (msg == WM_MOUSEMOVE) && display_last_hover_hwnd_2022 != hwnd)
    {
      int expansion = hack_show_deck_force_expansion;
      int forced = (unsigned)(expansion - 1) < 197;
      int csvid = GetWindowLongA(hwnd, EXE_DWORD(0x4d74b4));
      SendMessageA(hwnd_FullCardClass, 0x401, forced ? csvid | 0x40000000 : csvid, forced ? expansion : 0);
      display_last_hover_hwnd_2022 = hwnd;
      if (msg == WM_RBUTTONDOWN)
        SetActiveWindow(GetParent(hwnd));
    }
    return 0;
  }
  return ((WNDPROC)0x46d9a0)(hwnd, msg, wparam, lparam);
}
''',
'265c8f0': r'''
LRESULT CALLBACK wndproc_FullCardClass_hook(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  static EXPANSION_t hack_force_expansion = 0;
  if (msg != WM_PAINT)
  {
    if (msg == 0x401 && (wparam & 0x40000000))
    {
      hack_force_expansion = lparam;
      wparam &= ~0x40000000u;
      lparam = 0;
    }
    else if (msg == 0x401 || msg == WM_CREATE)
      hack_force_expansion = 0;
    return ((WNDPROC)0x4cff20)(hwnd, msg, wparam, lparam);
  }
  int csvid = GetWindowLongA(hwnd, EXE_DWORD(0x4d75bc));
  int player = GetWindowLongA(hwnd, EXE_DWORD(0x4d75c0));
  int card = GetWindowLongA(hwnd, EXE_DWORD(0x4d75c4));
  int infoslot_pt = GetWindowLongA(hwnd, EXE_DWORD(0x4d75c8));
  int col = GetWindowLongA(hwnd, EXE_DWORD(0x4d75cc));
  if ((csvid == 904 || csvid < 0 || csvid >= available_slots) && option_Layout == 2 && hwnd_FullCardClass == hwnd)
  {
    SendMessageA(hwnd, WM_TIMER, 1, 0);
    return DefWindowProcA(hwnd, msg, wparam, lparam);
  }
  int new_infoslot_pt = 0;
  if (csvid == 901)
    new_infoslot_pt = display_get_infoslot_2022(player, card);
  else if (csvid == 903)
  {
    int counter_power, counter_toughness;
    display_get_counters_2022(player, card, &counter_power, &counter_toughness);
    new_infoslot_pt = (counter_power & 65535) | ((unsigned)counter_toughness << 16);
  }
  int color_changed = 0;
  if (player != -1 && card != -1)
  {
    int newcol = single_color_test_bit_to_color(display_get_color_2022(player, card));
    color_changed = newcol != col;
    col = newcol;
  }
  if (new_infoslot_pt != infoslot_pt || color_changed)
    InvalidateRect(hwnd, NULL, FALSE);
  EnterCriticalSection(display_critical_section_for_painting_2022);
  unsigned tick_count = GetTickCount();
  RECT rect;
  PAINTSTRUCT paint;
  GetClientRect(hwnd, &rect);
  HDC hdc = BeginPaint(hwnd, &paint);
  if (hdc)
  {
    set_palette_and_stretch_mode(hdc);
    FillRect(display_dblbfr_hdc_2022, &rect, GetStockObject(4));
    if (csvid == 904 || csvid < 0 || csvid >= available_slots)
      DrawCardBack(display_dblbfr_hdc_2022, &rect, 0);
    else if ((unsigned)(csvid - 901) < 7)
    {
      if (get_displayed_csvid(player, card) < 0)
        DrawCardBack(display_dblbfr_hdc_2022, &rect, 0);
      else if (csvid == 906)
        display_draw_full_activation_2022(display_dblbfr_hdc_2022, &rect, player, card);
      else if (display_dblbfr_hdc_2022)
        draw_fullcard_special_effect_card_impl_2022(display_dblbfr_hdc_2022, &rect, csvid, player, card);
    }
    else if (player == -1 || card == -1)
    {
      int rval = DrawFullCardEx(display_dblbfr_hdc_2022, &rect, cards_ptr[csvid], 0, 0,
                                EXE_DWORD(0x787248), EXE_STR(0x6202a8), hack_force_expansion);
      if (!rval)
        DrawFullCardEx(display_dblbfr_hdc_2022, &rect, cards_ptr[csvid], 0, 2,
                       EXE_DWORD(0x787248), EXE_STR(0x6202a8), hack_force_expansion);
    }
    else
    {
      int rval;
      if (get_displayed_csvid(player, card) < 0)
        rval = DrawFullCardEx(display_dblbfr_hdc_2022, &rect, cards_ptr[csvid], 0, 0,
                              EXE_DWORD(0x787248), EXE_STR(0x6202a8), hack_force_expansion);
      else
        rval = display_draw_full_instance_2022(display_dblbfr_hdc_2022, &rect, cards_ptr[csvid], player, card, 0, EXE_DWORD(0x787248));
      if (!rval)
      {
        if (get_displayed_csvid(player, card) < 0)
          DrawFullCardEx(display_dblbfr_hdc_2022, &rect, cards_ptr[csvid], 0, 2,
                         EXE_DWORD(0x787248), EXE_STR(0x6202a8), hack_force_expansion);
        else
          display_draw_full_instance_2022(display_dblbfr_hdc_2022, &rect, cards_ptr[csvid], player, card, 2, EXE_DWORD(0x787248));
      }
    }
    BitBlt(hdc, 0, 0, rect.right, rect.bottom, display_dblbfr_hdc_2022, 0, 0, SRCCOPY);
    EndPaint(hwnd, &paint);
    SetWindowLongA(hwnd, EXE_DWORD(0x4d75c8), new_infoslot_pt);
    SetWindowLongA(hwnd, EXE_DWORD(0x4d75cc), col);
  }
  display_tick_adjustment_2022 += GetTickCount() - tick_count;
  LeaveCriticalSection(display_critical_section_for_painting_2022);
  return 0;
}
'''
}
