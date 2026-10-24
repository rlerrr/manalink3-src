"""Typed projections of the native pack dialog and its two VLA card consumers."""

BODIES = {
'26ecc50':r'''
INT_PTR __stdcall dlgproc_choose_a_pack(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  static HGDIOBJ winbk_questn = NULL;
  static const char **expansion_names = NULL;
  static HBRUSH brush = NULL;
  static HPEN pen1 = NULL;
  static HPEN pen2 = NULL;
  static choose_a_pack_args_t *args = NULL;
  _Static_assert(sizeof(choose_a_pack_args_t) == 64 && offsetof(choose_a_pack_args_t, results) == 16,
                 "native six-pack argument record");
  _Static_assert(sizeof(DRAWITEMSTRUCT) == 48 && offsetof(DRAWITEMSTRUCT, hDC) == 24 &&
                 offsetof(DRAWITEMSTRUCT, rcItem) == 28 && offsetof(DRAWITEMSTRUCT, itemData) == 44,
                 "native owner-draw record");
  _Static_assert(sizeof(TEXTMETRICA) == 56 && sizeof(RECT) == 16, "native drawing scratch records");
  if (msg == WM_COMMAND) {
    unsigned command = (uint16_t)wparam;
    if (command != 1 && (command != 2 || args->allow_cancel == 0)) return 0;
    for (int i = 0; i < 6; ++i) {
      args->results[i].expansion = EXPANSION_0;
      args->results[i].booster = 0;
    }
    for (int i = 0; i < args->num_packs; ++i) {
      if (command == 1) {
        LRESULT selection = SendDlgItemMessageA(hwnd, i + 0x44d, 0x147, 0, 0);
        if (selection != -1) {
          LRESULT data = SendDlgItemMessageA(hwnd, i + 0x44d, 0x150, selection, 0);
          if (data != -1) {
            args->results[i].expansion = (uint32_t)data & 0xffff;
            args->results[i].booster = ((uint32_t)data & 0x10000) == 0;
          }
        }
      }
    }
    if (winbk_questn != NULL) { DeleteObject(winbk_questn); winbk_questn = NULL; }
    if (brush != NULL) { DeleteObject(brush); brush = NULL; }
    if (pen1 != NULL) { DeleteObject(pen1); pen1 = NULL; }
    if (pen2 != NULL) { DeleteObject(pen2); pen2 = NULL; }
    EndDialog(hwnd, command == 1);
    return 0;
  }
  if (msg == WM_INITDIALOG) {
    char path[279];
    sprintf(path, "%s\\WINBK_QuestN.pic", pack_path_duel_art);
    winbk_questn = pack_read_graphics_file(path);
    brush = CreateSolidBrush(0x100001c);
    pen1 = CreatePen(0, 0, 0x1000072);
    pen2 = CreatePen(0, 0, 0x10000ca);
    SetDlgItemTextA(hwnd, 1, pack_str_ok);
    SetDlgItemTextA(hwnd, 2, pack_str_cancel);
    args = (choose_a_pack_args_t *)lparam;
    if (args->num_packs > 6) args->num_packs = 6;
    else if (args->num_packs < 1) args->num_packs = 1;
    if (args->allow_cancel == 0) ShowWindow(GetDlgItem(hwnd, 2), 0);
    for (int i = args->num_packs; i < 6; ++i) ShowWindow(GetDlgItem(hwnd, i + 0x44d), 0);
    int num_textlines;
    expansion_names = load_text_nocopy_from_file("Menus.txt", "EXPANSIONNAMES", &num_textlines);
    for (int expansion = 1; expansion < 198 && num_textlines > expansion + 6; ++expansion) {
      char buf[300];
      if (args->allow_boosters != 0 && is_valid_pack(expansion, 1)) {
        int size = pack_size(expansion, 1);
        const char *prefix = "14-";
        if ((((uint32_t)expansion - 103) & 0xfffffffd) != 0 &&
            ((uint32_t)expansion & 0xfffffffd) != 145) prefix = "";
        sprintf(buf, "%s (%s%d cards)", expansion_names[expansion + 7], prefix, size);
        for (int i = 0; i < args->num_packs; ++i) {
          LRESULT result = SendDlgItemMessageA(hwnd, i + 0x44d, 0x143, 0, (LPARAM)buf);
          if ((uint32_t)result + 2 > 1)
            SendDlgItemMessageA(hwnd, i + 0x44d, 0x151, result, expansion);
        }
      }
      if (args->allow_starters != 0 && is_valid_pack(expansion, 0)) {
        int size = pack_size(expansion, 0);
        sprintf(buf, "%s %s deck (%d cards)", expansion_names[expansion + 7],
                expansion < 30 ? "starter" : "tournament", size);
        for (int i = 0; i < args->num_packs; ++i) {
          LRESULT result = SendDlgItemMessageA(hwnd, i + 0x44d, 0x143, 0, (LPARAM)buf);
          if ((uint32_t)result + 2 > 1)
            SendDlgItemMessageA(hwnd, i + 0x44d, 0x151, result, expansion | 0x10000);
        }
      }
    }
    for (int i = 0; i < args->num_packs; ++i)
      SendDlgItemMessageA(hwnd, i + 0x44d, 0x14e, 0, 0);
    SetFocus(GetDlgItem(hwnd, 0x44d));
    return 0;
  }
  if (msg == WM_DRAWITEM) {
    DRAWITEMSTRUCT *dis = (DRAWITEMSTRUCT *)lparam;
    if (wparam < 3) {
      if (wparam == 0) return 0;
      COLORREF color = ((uint32_t)(dis->hwndItem == GetFocus()) - 1U & 0xffffffdbU) + 0x10000bfU;
      pack_draw_button(dis, brush, pen1, pen2, color, 0);
      return 1;
    }
    if ((uint32_t)wparam - 0x44d > 5 || dis->itemID == (UINT)-1) return 0;
    unsigned expansion = (uint16_t)dis->itemData;
    COLORREF old_fg = SetTextColor(dis->hDC, GetSysColor(8));
    COLORREF old_bk = SetBkColor(dis->hDC, GetSysColor(5));
    TEXTMETRICA tm;
    GetTextMetricsA(dis->hDC, &tm);
    int bottom = dis->rcItem.bottom;
    int top = dis->rcItem.top;
    char buf[300];
    SendMessageA(dis->hwndItem, 0x148, dis->itemID, (LPARAM)buf);
    int width = (bottom - top) * 2;
    ExtTextOutA(dis->hDC, width + 4, (bottom + top - tm.tmHeight) / 2, 6,
                &dis->rcItem, buf, strlen(buf), NULL);
    SetTextColor(dis->hDC, old_fg);
    SetBkColor(dis->hDC, old_bk);
    RECT rect;
    rect.left = dis->rcItem.left;
    rect.top = dis->rcItem.top;
    rect.right = rect.left + width;
    rect.bottom = dis->rcItem.bottom;
    FillRect(dis->hDC, &rect, GetSysColorBrush(5));
    blt_expansion_symbol(dis->hDC, &rect, expansion, (dis->itemState & 0x11) == 0 ? 1 : 4, -1);
    if ((dis->itemState & 0x11) != 0) DrawFocusRect(dis->hDC, &dis->rcItem);
    return 1;
  }
  if (msg == WM_ERASEBKGND && winbk_questn != NULL) {
    RECT rect;
    pack_set_palette_and_stretch_mode((HDC)wparam);
    GetClientRect(hwnd, &rect);
    EnterCriticalSection(pack_critical_section_for_painting);
    int savedc = SaveDC(pack_dblbfr_hdc);
    if (winbk_questn == NULL) FillRect(pack_dblbfr_hdc, &rect, GetStockObject(2));
    else pack_blt_pic(pack_dblbfr_hdc, &rect, winbk_questn);
    RestoreDC(pack_dblbfr_hdc, savedc);
    GetClientRect(hwnd, &rect);
    BitBlt((HDC)wparam, 0, 0, rect.right, rect.bottom, pack_dblbfr_hdc, 0, 0, 0xcc0020);
    LeaveCriticalSection(pack_critical_section_for_painting);
    return 1;
  }
  if (msg == WM_LBUTTONDOWN) {
    SendMessageA(hwnd, 0x112, 0xf012, 0);
    return 0;
  }
  if ((uint32_t)msg - 0x30f < 3) return pack_handle_palette_msgs(hwnd, msg, wparam, lparam);
  return 0;
}
''',
'26ed780':r'''
LPCDLGTEMPLATE generate_choose_a_pack_dlg_template(void)
{
  return (LPCDLGTEMPLATE)choose_a_pack_template;
}
''',
'26ed790':r'''
int choose_packs(int num_packs, pack_flags_t flags, EXPANSION_t *expansions, int *is_booster)
{
  static int idx = -1;
  choose_a_pack_args_t args;
  if (num_packs > 6) num_packs = 6;
  else if (num_packs < 1) num_packs = 1;
  idx = (int32_t)((uint32_t)idx + 1U);
  args.num_packs = num_packs;
  args.allow_boosters = flags & PACK_ALLOW_BOOSTER;
  args.allow_starters = flags & PACK_ALLOW_STARTER;
  args.allow_cancel = flags & PACK_ALLOW_CANCEL;
  for (int i = 0; i < 6; ++i) {
    args.results[i].expansion = EXPANSION_0;
    args.results[i].booster = 0;
  }
  INT_PTR result = DialogBoxIndirectParamA(pack_main_hinstance, generate_choose_a_pack_dlg_template(),
                                           pack_hwnd_MainClass, dlgproc_choose_a_pack, (LPARAM)&args);
  if (result != 0) {
    for (int i = 0; i < num_packs; ++i) {
      expansions[i] = args.results[i].expansion;
      if (is_booster != NULL) is_booster[i] = args.results[i].booster;
    }
    return 1;
  }
  return 0;
}
''',
'2305250':r'''
int card_booster_tutor(int player, int card, event_t event)
{
  if (event == EVENT_RESOLVE_SPELL) {
    EXPANSION_t expansion = (EXPANSION_t)-1;
    if (ai_is_speculating == 1 || (player == 1 && (trace_mode & 2) == 0)) {
      expansion = EXPANSION_ARABIAN_NIGHTS;
      announce_expansion_choice(player, card, expansion);
    } else if ((trace_mode & 2) != 0 && player != 0) {
      EXE_FN(int, 0x4a70c0, int, int)(1, 26);
      expansion = dblword_628C10;
      announce_expansion_choice(player, card, expansion);
    } else {
      choose_packs(1, PACK_ALLOW_BOOSTER, &expansion, NULL);
      if ((trace_mode & 2) != 0) {
        dblword_628C10 = expansion;
        byte_628C0C = 26;
        EXE_FN(int, 0x4a6bc0, int, int)(0, 26);
      }
    }
    if (is_valid_pack(expansion, 1)) {
      int capacity = pack_size(expansion, 1);
      int csvids[capacity];
      int iids[capacity];
      int num_csvids = make_pack_2022(expansion, 1, csvids);
      for (int i = 0; i < num_csvids; ++i) iids[i] = get_internal_card_id_from_csv_id(csvids[i]);
      if (ai_is_speculating != 1 && player == 1)
        show_deck_2022(0, iids, capacity, "Booster contents", 0, ENGINE_DONE_LABEL);
      test_definition_2022_t test;
      _Static_assert(sizeof(test) == 216, "native Booster Tutor filter");
      default_test_definition_2022(&test, 0xff);
      int selected;
      do {
        selected = select_card_from_zone_2022(player, player, iids, capacity, 1, 1, -1, &test);
      } while (selected < 0 || selected >= capacity);
      int added = add_card_to_hand(player, iids[selected]);
      if (player == 1) reveal_card(1, card, 1, added);
    }
    kill_card(player, card, 2);
  }
  return basic_spell(player, card, event);
}
''',
'2411220':r'''
int card_stocking_tiger(int player, int card, event_t event)
{
  if (event == EVENT_RESOLVE_SPELL && !is_humiliated_for_ability(player, card, AL_STATIC)) {
    EXPANSION_t expansion = (EXPANSION_t)-1;
    if (ai_is_speculating == 1 || (player == 1 && (trace_mode & 2) == 0)) {
      expansion = EXPANSION_THE_DARK;
      announce_expansion_choice(player, card, expansion);
    } else if ((trace_mode & 2) != 0 && player != 0) {
      EXE_FN(int, 0x4a70c0, int, int)(1, 26);
      expansion = dblword_628C10;
      announce_expansion_choice(player, card, expansion);
    } else {
      choose_packs(1, PACK_ALLOW_BOOSTER, &expansion, NULL);
      if ((trace_mode & 2) != 0) {
        dblword_628C10 = expansion;
        byte_628C0C = 26;
        EXE_FN(int, 0x4a6bc0, int, int)(0, 26);
      }
    }
    if (is_valid_pack(expansion, 1))
      get_card_instance(player, card)->targets[3].card = ((uint32_t)expansion & 0xffff) | 0x3f7c0000;
  }
  if (has_combat_damage_been_inflicted_to_a_player(player, card, event)) {
    card_instance_t *instance = get_card_instance(player, card);
    uint32_t stored = instance->targets[3].card;
    if ((uint16_t)(stored >> 16) == 0x3f7c) {
      EXPANSION_t expansion = stored & 0xffff;
      if (is_valid_pack(expansion, 1)) {
        instance->targets[3].card = -1;
        int capacity = pack_size(expansion, 1);
        int csvids[capacity];
        int num_csvids = make_pack_2022(expansion, 1, csvids);
        for (int i = 0; i < num_csvids; ++i)
          add_card_to_hand(player, get_internal_card_id_from_csv_id(csvids[i]));
      }
    }
  }
  return 0;
}
'''
}
