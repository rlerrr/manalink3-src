"""Native dynamic-array and shared text/test reconstruction for Lich's Mastery."""
STORAGE='''typedef union
{
  char text[100];
  test_definition_2022_t test;
} lichs_mastery_storage_2022_t;'''

BODY='''void lichs_mastery_lifeloss_trigger(int player, int card, int amount)
{
  target_definition_t td;
  lichs_mastery_storage_2022_t storage;
  default_target_definition(player, card, &td, 0);
  td.zone = 0x300;
  td.allowed_controller = player;
  td.preferred_controller = player;
  td.illegal_abilities = 0;
  td.allow_cancel = 0;
  td.special = TARGET_SPECIAL_EXTRA_FUNCTION;
  td.extra = (int32_t)target_only_permanents_unless_in_hand_2022;
  int grave[amount];
  target_t tgts[amount + 1];
  card_instance_t *instance = get_card_instance(player, card);
  int num_tgts = 0;
  int i = 0;
  while (1)
  {
    int available = can_target(&td);
    if ((!available && get_grave(player)[0] == -1) || amount <= i) break;
    int grave_available = get_grave(player)[0] != -1;
    int hand_or_play_available = can_target(&td);
    int choice = dialog_impl(player, card, EVENT_ACTIVATE,
        &UNK_0273cdb8, &UNK_0273cd85, &UNK_0273ceea, &UNK_0273d151,
        &UNK_0273cd55, &UNK_0273d139, hand_or_play_available, 5,
        &UNK_0273d124, grave_available, 10, 0, 0, 0, 0);
    int next_count = num_tgts + 1;
    if (choice == 1)
    {
      scnprintf(storage.text, 100, NATIVE_TARGET_FORMAT, next_count, amount);
      if (select_target(player, card, &td, storage.text, &tgts[i]))
      {
        instance->number_of_targets = 0;
        state_untargettable(tgts[i].player, tgts[i].card, 1);
        num_tgts = next_count;
        grave[i] = 0;
      }
      ++i;
    }
    else
    {
      default_test_definition(&storage.test, 0xff);
      scnprintf(storage.text, 100, NATIVE_GRAVE_FORMAT, next_count, amount);
      new_global_tutor_2022(player, player, TUTOR_FROM2022_GRAVE,
                            TUTOR2022_RFG, 1, 3, &storage.test);
      num_tgts = next_count;
      grave[i] = 1;
      ++i;
    }
  }
  for (i = 0; i < num_tgts; ++i)
    if (!grave[i]) state_untargettable(tgts[i].player, tgts[i].card, 0);
  for (i = 0; i < num_tgts; ++i)
  {
    if (grave[i]) continue;
    if (in_hand(tgts[i].player, tgts[i].card))
    {
      put_on_top_of_deck(tgts[i].player, tgts[i].card);
      rfg_top_card_of_deck(tgts[i].player);
    }
    else kill_card(tgts[i].player, tgts[i].card, KILL_EXILE);
  }
}'''
