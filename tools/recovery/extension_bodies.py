"""Reviewed C reconstructions for the extension lifecycle and packed information API."""

BODIES = {
'memcpy_inst_and_ext': r'''void memcpy_inst_and_ext(card_instance_t *dest, const card_instance_t *src)
{
  card_ext_t *ext = dest->ext;
  memcpy(ext, src->ext, sizeof(*ext));
  memcpy(dest, src, sizeof(*dest));
  dest->ext = ext;
}
''',
'hook_helper_for_startduel': r'''void hook_helper_for_startduel(void)
{
  EXE_FN(void, 0x432f80, void)();
  for (int player = 0; player < 2; ++player)
    for (int card = 0; card < 150; ++card)
    {
      get_card_instance(player, card)->ext = &card_ext[player][card];
      get_displayed_card_instance(player, card)->ext = &disp_card_ext[player][card];
    }
}
''',
'copy_to_display_start': r'''void copy_to_display_start(void)
{
  for (int player = 0; player < 2; ++player)
    for (int card = 0; card < 150; ++card)
      get_displayed_card_instance(player, card)->ext = &card_ext[player][card];
}
''',
'copy_to_display_end': r'''int copy_to_display_end(int any_changes)
{
  if (memcmp(disp_card_ext, card_ext, sizeof(card_ext)) != 0)
  {
    memcpy(disp_card_ext, card_ext, sizeof(card_ext));
    any_changes = 1;
  }
  for (int player = 0; player < 2; ++player)
    for (int card = 0; card < 150; ++card)
      get_displayed_card_instance(player, card)->ext = &disp_card_ext[player][card];
  LeaveCriticalSection(critical_section_for_display);
  return any_changes;
}
''',
'set_inst_info': r'''int set_inst_info(card_instance_t *inst, int cid, uint32_t data, uint16_t data2)
{
  inst_info_t *entries = inst->ext->inst_info_data;
  int slot;
  for (slot = 0; slot < 6; ++slot)
    if ((uint32_t)entries[slot].id == (uint32_t)cid)
      break;
  if (slot == 6)
  {
    for (slot = 0; slot < 6; ++slot)
      if (entries[slot].id == 0)
        break;
    if (slot == 6)
      return 0;
  }
  entries[slot].id = (uint16_t)cid;
  entries[slot].data2 = data2;
  entries[slot].data = data;
  return 1;
}
''',
'inst_info': r'''inst_info_t inst_info(card_instance_t *inst, int cid)
{
  inst_info_t *entries = inst->ext->inst_info_data;
  int slot;
  for (slot = 0; slot < 6; ++slot)
  {
    if ((uint32_t)entries[slot].id == (uint32_t)cid)
      return entries[slot];
    if (entries[slot].id == 0)
      break;
  }
  if (slot == 6)
    slot = 5;
  entries[slot].id = (uint16_t)cid;
  entries[slot].data2 = 0;
  entries[slot].data = 0;
  return entries[slot];
}
''',
'increment_data2': r'''int increment_data2(card_instance_t *inst, int cid)
{
  inst_info_t initial = inst_info(inst, cid);
  inst_info_t second = inst_info(inst, cid);
  return set_inst_info(inst, cid, initial.data, (uint16_t)(second.data2 + 1));
}
''',
'clear_info': r'''void clear_info(card_instance_t *inst, int cid)
{
  inst_info_t *entries = inst->ext->inst_info_data;
  for (int slot = 0; slot < 6; ++slot)
  {
    if (entries[slot].id == 0)
      return;
    while ((uint32_t)entries[slot].id == (uint32_t)cid)
    {
      if (slot < 5)
        memmove(&entries[slot], &entries[slot + 1], (5 - slot) * sizeof(*entries));
      entries[5].id = 0;
    }
  }
}
''',
'store_iid': r'''int store_iid(card_instance_t *inst, int16_t generating_id, int16_t stored_iid)
{
  inst_stored_iid_t *entries = inst->ext->inst_stored_iids;
  for (int slot = 0; slot < 12; ++slot)
    if (entries[slot].generating_id == -1)
    {
      entries[slot].generating_id = generating_id;
      entries[slot].stored_iid = stored_iid;
      return slot;
    }
  return -1;
}
''',
'retrieve_stored_iids': r'''int retrieve_stored_iids(card_instance_t *inst, int16_t generating_id, int16_t *iids)
{
  int count = 0;
  inst_stored_iid_t *entries = inst->ext->inst_stored_iids;
  for (int slot = 0; slot < 12; ++slot)
    if (entries[slot].generating_id == generating_id)
      iids[count++] = entries[slot].stored_iid;
  return count;
}
''',
'clear_stored_iids': r'''void clear_stored_iids(card_instance_t *inst, int16_t generating_id)
{
  inst_stored_iid_t *entries = inst->ext->inst_stored_iids;
  for (int slot = 0; slot < 12; ++slot)
    if (entries[slot].generating_id == generating_id)
    {
      entries[slot].generating_id = -1;
      entries[slot].stored_iid = -1;
    }
}
'''
}
