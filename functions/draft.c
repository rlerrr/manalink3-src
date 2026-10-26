#include "manalink.h"

/* Recovered October 2022 implementations; see docs/recovery-progress.md. */

/* ManalinkEh 2022: 0x2662100; original functions/draft.c:32. */
static void pick_card(int *picks, int *pack, int selected)
{
  int result;
  int *piVar2;
  result = *picks;
  while (-1 < result)
  {
    picks = picks + 1;
    result = *picks;
  }
  *picks = pack[selected];
  result = pack[selected + 1];
  pack[selected] = result;
  piVar2 = pack + selected + 1;
  while (-1 < result)
  {
    result = piVar2[1];
    *piVar2 = result;
    piVar2 = piVar2 + 1;
  }
  return;
}


#ifndef SHANDALAR
#include <errno.h>
#define draft_get_path_base_2022 EXE_FN(void, 0x466020, char *)
#define DRAFT_IS_BASIC_2022(id) ((id) == CARD_ID_SWAMP || (id) == CARD_ID_ISLAND || (id) == CARD_ID_MOUNTAIN || (id) == CARD_ID_FOREST || (id) == CARD_ID_PLAINS)

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x2662180; original functions/draft.c:238. */
static void write_deck_file(char *deckname, int *iids, int num_packs, EXPANSION_t *exps, int *is_booster)
{
  char path[500];
  draft_get_path_base_2022(path);
  int l = strlen(path);
  snprintf(path + l, 500 - l, "\\PlayDeck\\%s.dck", deckname);
  FILE *f = fopen(path, "w");
  if (!f)
    fatal("Couldn't open \"%s\" for writing: %s", path, strerror(errno));
  const char **expnames = load_text_nocopy_from_file("Menus.txt", "EXPANSIONNAMES", NULL);
  fprintf(f, ";%s\n;Draft deck\n;\n;\n;%s\n;1\n;4th Edition\n;Drafted from %d pack%s", deckname,
          localtime_date(), num_packs, num_packs == 1 ? "" : "s");
  for (int packnum = 0; packnum < num_packs; ++packnum)
    fprintf(f, "%c %s %s", packnum == 0 ? ':' : ',', expnames[exps[packnum] + 7],
            is_booster[packnum] ? "booster" : "starter");
  fwrite("\n\n", 1, 2, f);
  for (int i = 0; iids[i] >= 0; ++i)
  {
    unsigned csvid = cards_data[iids[i]].id;
    fprintf(f, ".%d\t1\t%s\n", csvid, cards_ptr[csvid]->full_name);
  }
  fclose(f);
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26623d0; original functions/draft.c:271. */
int card_draft_2022(int player, int card, event_t event)
{
  if (ai_is_speculating == 1 || trigger_condition != TRIGGER_UPKEEP || card != affected_card ||
      player != affected_card_controller || reason_for_trigger_controller != player)
    return 0;
  if (event == EVENT_TRIGGER)
    event_result |= 2;
  else if (event == EVENT_RESOLVE_TRIGGER)
  {
    int num_opponents, num_packs;
    EXPANSION_t exps[6];
    int is_booster[6];
  choose_opponents:
    do
      num_opponents = choose_a_number(HUMAN, "How many opponents?", 8);
    while (num_opponents < 0);
  choose_number_of_packs:
    do
    {
      num_packs = choose_a_number(HUMAN, "How many packs?", 6);
      if (num_packs < 0)
        goto choose_opponents;
    } while ((unsigned)(num_packs - 1) > 5);
    if (!choose_packs(num_packs, PACK_ALLOW_CANCEL | PACK_ALLOW_STARTER | PACK_ALLOW_BOOSTER, exps,
                      is_booster))
      goto choose_number_of_packs;
    int total_cards = 0;
    for (int packnum = 0; packnum < num_packs; ++packnum)
      total_cards += pack_size(exps[packnum], is_booster[packnum]);
    int *picks[num_opponents + 1];
    int iids_size = total_cards + 41;
    for (int plr = 0; plr <= num_opponents; ++plr)
    {
      picks[plr] = malloc(iids_size * sizeof(int));
      if (!picks[plr])
        fatal("could not malloc %d bytes for picks[%d]", (int)(iids_size * sizeof(int)), plr);
      memset(picks[plr], 255, iids_size * sizeof(int));
    }
    color_test_t colors[num_opponents + 1];
    memset(colors, 0, sizeof(colors));
    for (int packnum = 0; packnum < num_packs; ++packnum)
    {
      int *packs[num_opponents + 1];
      int max_in_pack = pack_size(exps[packnum], is_booster[packnum]);
      for (int plr = 0; plr <= num_opponents; ++plr)
      {
        int csvids[max_in_pack];
        int num = make_pack_2022(exps[packnum], is_booster[packnum], csvids);
        if (num <= 0)
        {
          const char **names = load_text_nocopy_from_file("Menus.txt", "EXPANSIONNAMES", NULL);
          const char *article =
              names[exps[packnum]][0] && strchr("AEIOUaeiou", names[exps[packnum]][0]) ? "an" : "a";
          char buf[600];
          snprintf(buf, 600, "Could not create %s %s %s", article, names[exps[packnum] + 7],
                   is_booster[packnum] ? "booster pack" : "starter deck");
          buf[599] = 0;
          do_dialog(HUMAN, player, card, -1, -1, buf, 0);
          lose_the_game(HUMAN);
        }
        packs[plr] = malloc((num + 1) * sizeof(int));
        if (!packs[plr])
          fatal("could not malloc %d bytes for packs[%d]", (int)((num + 1) * sizeof(int)), plr);
        for (int crd = 0; crd < num; ++crd)
          packs[plr][crd] = get_internal_card_id_from_csv_id(csvids[crd]);
        packs[plr][num] = -1;
      }
      for (;;)
      {
        int any_left = 0;
        for (int i = 0; i <= num_opponents; ++i)
          if (packs[i][0] >= 0)
          {
            any_left = 1;
            break;
          }
        if (!any_left)
          break;
        if (packs[0][0] >= 0)
        {
          int selected;
          for (;;)
          {
            if (picks[0][0] >= 0)
            {
              selected = show_deck_force_expansion_2022(HUMAN, packs[0], 500, "Draft a card", 0,
                                                        "View prev", exps[packnum]);
              if (selected >= 0)
                break;
              show_deck(HUMAN, picks[0], 500, "Drafted so far", 0,
                             (int)(uintptr_t)EXE_STR(0x78f0a8));
            }
            else
            {
              selected = show_deck_force_expansion_2022(HUMAN, packs[0], 500, "Draft a card", 1, "",
                                                        exps[packnum]);
              if (selected >= 0)
                break;
            }
          }
          pick_card(picks[0], packs[0], selected);
        }
        for (int oppnum = 1; oppnum <= num_opponents; ++oppnum)
        {
          int *pack = packs[oppnum];
          if (*pack < 0)
            continue;
          int num_drafted_colors = num_bits_set(colors[oppnum]);
          int max_score = -1, best_card = -1, max_score_backup = -1, best_card_backup = 0;
          for (int i = 0; pack[i] >= 0; ++i)
          {
            int iid = pack[i];
            card_data_t *cd = &cards_data[iid];
            int score = cards_ptr[cd->id]->ai_base_value + internal_rand(10);
            if (score > max_score_backup)
            {
              max_score_backup = score;
              best_card_backup = i;
            }
            if (score <= max_score)
              continue;
            color_test_t color = cd->color & COLOR_TEST_ANY_COLORED;
            int num_card_colors = num_bits_set(color);
            if (num_card_colors > 1 && num_drafted_colors < 2)
              continue;
            int candidate = i, reject = 0, restrict_color = 1;
            if (cd->type & (TYPE_LAND | TYPE_ARTIFACT))
              if (num_card_colors == 0 || num_card_colors == 5 || !(color & ~colors[oppnum]))
                restrict_color = DRAFT_IS_BASIC_2022(cd->id);
            if (restrict_color)
            {
              if (num_drafted_colors >= 2)
                reject = (color & ~colors[oppnum]) != 0;
              else if (num_drafted_colors == 1)
                reject = num_bits_set(color | colors[oppnum]) > 2;
            }
            if (reject)
            {
              candidate = best_card;
              if (i != best_card)
                continue;
            }
            best_card = candidate;
            max_score = score;
          }
          if (best_card == -1)
            best_card = best_card_backup;
          card_data_t *cd = &cards_data[pack[best_card]];
          color_test_t color = cd->color & COLOR_TEST_ANY_COLORED;
          if (num_bits_set(color | colors[oppnum]) < 3 && !DRAFT_IS_BASIC_2022(cd->id))
            colors[oppnum] |= color;
          pick_card(picks[oppnum], pack, best_card);
        }
        if (num_opponents > 0)
        {
          int *pack0 = packs[0], plr = 0;
          do
          {
            packs[plr] = packs[plr + 1];
            ++plr;
          } while (plr != num_opponents);
          packs[num_opponents] = pack0;
        }
      }
      if (packnum < num_packs - 1)
        show_deck(HUMAN, picks[0], 500, "Drafted so far", 0,
                       (int)(uintptr_t)EXE_STR(0x78f0a8));
      for (int plr = 0; plr <= num_opponents; ++plr)
        free(packs[plr]);
    }
    write_deck_file("Draft - Human", picks[0], num_packs, exps, is_booster);
    for (int oppnum = 1; oppnum <= num_opponents; ++oppnum)
    {
      int *iids = picks[oppnum];
      int num_in_iids;
      {
        target_t deck[iids_size];
        memset(deck, 255, sizeof(deck));
        int num_in_deck = 0;
        for (int i = 0; iids[i] >= 0; ++i)
        {
          card_data_t *cd = &cards_data[iids[i]];
          int score = cards_ptr[cd->id]->ai_base_value;
          if (DRAFT_IS_BASIC_2022(cd->id))
            score -= 100;
          color_test_t color = cd->color & COLOR_TEST_ANY_COLORED;
          if ((cd->type & (TYPE_LAND | TYPE_ARTIFACT)) && (cd->extra_ability & EA_MANA_SOURCE))
          {
            int matching_colors = num_bits_set(cd->color & colors[oppnum]);
            score += (matching_colors * 3 + 2) * (matching_colors * 3 + 2);
          }
          else if (!(cd->type & TYPE_LAND) || (cd->type & TYPE_ARTIFACT))
          {
            if (color & ~colors[oppnum])
              score -= 50;
            else if ((color & colors[oppnum]) == colors[oppnum])
              score += 10;
          }
          deck[num_in_deck].player = score;
          deck[num_in_deck++].card = iids[i];
        }
        descending_qsort_on_player(num_in_deck, deck);
        memset(iids, 255, iids_size * sizeof(int));
        num_in_iids = 0;
        int num_nonland_in_iids = 0;
        for (int i = 0; i < num_in_deck && num_nonland_in_iids < 23; ++i)
        {
          if (num_in_iids >= iids_size - 1)
            backtrace_and_abort("functions/draft.c:207\nnum_in_iids < iids_size - 1");
          iids[num_in_iids++] = deck[i].card;
          num_nonland_in_iids += (cards_data[deck[i].card].type & TYPE_LAND) == 0;
        }
      }
      {
        int basics[5], num_basics = 0;
        if (colors[oppnum] & COLOR_TEST_BLACK)
          basics[num_basics++] = get_internal_card_id_from_csv_id(CARD_ID_SWAMP);
        if (colors[oppnum] & COLOR_TEST_BLUE)
          basics[num_basics++] = get_internal_card_id_from_csv_id(CARD_ID_ISLAND);
        if (colors[oppnum] & COLOR_TEST_RED)
          basics[num_basics++] = get_internal_card_id_from_csv_id(CARD_ID_MOUNTAIN);
        if (colors[oppnum] & COLOR_TEST_GREEN)
          basics[num_basics++] = get_internal_card_id_from_csv_id(CARD_ID_FOREST);
        if ((colors[oppnum] & COLOR_TEST_WHITE) || !num_basics)
          basics[num_basics++] = get_internal_card_id_from_csv_id(CARD_ID_PLAINS);
        int basicidx = 0;
        while (num_in_iids < 40)
        {
          iids[num_in_iids++] = basics[basicidx];
          basicidx = (basicidx + 1) % num_basics;
        }
      }
      if (num_in_iids >= iids_size - 1)
        backtrace_and_abort("functions/draft.c:233\nnum_in_iids < iids_size - 1");
      iids[num_in_iids] = -1;
      {
        char buf[50];
        sprintf(buf, "Draft Opponent %d", oppnum);
        write_deck_file(buf, iids, num_packs, exps, is_booster);
      }
    }
    for (int plr = 0; plr <= num_opponents; ++plr)
      free(picks[plr]);
    lose_the_game(AI);
  }
  return 0;
}

#endif
