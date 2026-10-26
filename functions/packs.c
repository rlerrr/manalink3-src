#include "manalink.h"

#ifndef SHANDALAR
typedef enum {
  XNONE = 0,
  XCOMMON = 1,
  XUNCOMMON = 2,
  XRARE = 3,
  XMYTHIC = 4,
  XLAND = 5,
  XSPECIAL = 6,
  TIMESHIFT = 6,
  COMMON_ARABIAN_NIGHTS = 7,
  UNCOMMON_ARABIAN_NIGHTS = 8,
  COMMON_ANTIQUITIES = 9,
  UNCOMMON_ANTIQUITIES = 10,
  COMMON_LEGENDS = 11,
  UNCOMMON_LEGENDS = 12,
  COMMON_THE_DARK = 13,
  UNCOMMON_THE_DARK = 14,
  COMMON_FALLEN_EMPIRES = 15,
  UNCOMMON_FALLEN_EMPIRES = 16,
  COMMON_CHRONICLES = 17,
  UNCOMMON_CHRONICLES = 18,
  COMMON_HOMELANDS = 19,
  UNCOMMON_HOMELANDS = 20,
  COMMON_ALLIANCES = 21,
  UNCOMMON_ALLIANCES = 22,
  RARE_ALLIANCES = 23,
  PLANESHIFT_COMMON = 24,
  PLANESHIFT_UNCOMMON = 25,
  PLANESHIFT_RARE = 26,
  DFC_ANY_RARITY = 27,
  DFC_C_OR_U = 28,
  DFC_R_OR_M = 29,
  DFC_COMMON = 30,
  CONTRAPTION = 31,
  ISLAND = 32,
  LAND_BASIC_OR_SNOW = 33,
  ANY_RARITY = 34,
  GUILDGATE_OR_SHOCKLAND = 35,
  GUILDGATE = 36,
  LEGENDARY_CREATURE = 37,
  NONLEGENDARY_RARE = 38,
  NONLEGENDARY_MYTHIC = 39,
  LESSON = 40,
  MYSTICAL_ARCHIVE = 41,
  NEO_LAND = 42,
  VAL_IFELSE = 43,
  MAX_RARITY_VAL_USED = 43,
} draftrarity_t;

typedef struct rarity_num_t { draftrarity_t rarity; int num; } rarity_num_t;
typedef struct booster_t { EXPANSION_t expansion; const rarity_num_t *booster; const rarity_num_t *starter; } booster_t;
typedef struct csvid_choices_t { int num; int *csvids; } csvid_choices_t;
_Static_assert(sizeof(rarity_num_t)==8 && sizeof(booster_t)==12 && sizeof(csvid_choices_t)==8, "native pack description layouts");

static const rarity_num_t no_boosters[1] = {
  {0, 0},
};

static const rarity_num_t no_starters[1] = {
  {0, 0},
};

static const rarity_num_t booster_15_cur[4] = {
  {1, 11},
  {2, 3},
  {3, 1},
  {0, 0},
};

static const rarity_num_t booster_15_lcur[5] = {
  {5, 1},
  {1, 10},
  {2, 3},
  {3, 1},
  {0, 0},
};

static const rarity_num_t booster_15_l2cur[5] = {
  {5, 2},
  {1, 9},
  {2, 3},
  {3, 1},
  {0, 0},
};

static const rarity_num_t booster_15_lcurm71[7] = {
  {5, 1},
  {1, 10},
  {2, 3},
  {43, 7929962},
  {3, 1},
  {4, 1},
  {0, 0},
};

static const rarity_num_t booster_15_lcurm74[7] = {
  {5, 1},
  {1, 10},
  {2, 3},
  {43, 4849728},
  {3, 1},
  {4, 1},
  {0, 0},
};

static const rarity_num_t booster_15_curm71a[7] = {
  {1, 10},
  {2, 3},
  {43, 7929962},
  {3, 1},
  {4, 1},
  {34, 1},
  {0, 0},
};

static const rarity_num_t starter_l22_cur[5] = {
  {5, 22},
  {1, 26},
  {2, 9},
  {3, 3},
  {0, 0},
};

static const rarity_num_t starter_l30_cur[5] = {
  {5, 30},
  {1, 32},
  {2, 10},
  {3, 3},
  {0, 0},
};

static const rarity_num_t starter_l30_curm[7] = {
  {5, 30},
  {1, 32},
  {2, 10},
  {43, 7929962},
  {3, 3},
  {4, 3},
  {0, 0},
};

static const rarity_num_t booster_alpha[10] = {
  {43, 7929903},
  {5, 11},
  {1, 11},
  {43, 7929882},
  {5, 3},
  {2, 3},
  {43, 7929861},
  {32, 1},
  {3, 1},
  {0, 0},
};

static const rarity_num_t starter_alpha[10] = {
  {43, 7929903},
  {5, 45},
  {1, 45},
  {43, 7929882},
  {5, 13},
  {2, 13},
  {43, 7929861},
  {32, 2},
  {3, 2},
  {0, 0},
};

static const rarity_num_t booster_beta[10] = {
  {43, 7929902},
  {5, 11},
  {1, 11},
  {43, 7929882},
  {5, 3},
  {2, 3},
  {43, 7929860},
  {32, 1},
  {3, 1},
  {0, 0},
};

static const rarity_num_t starter_beta[10] = {
  {43, 7929902},
  {5, 45},
  {1, 45},
  {43, 7929882},
  {5, 13},
  {2, 13},
  {43, 7929860},
  {32, 2},
  {3, 2},
  {0, 0},
};

static const rarity_num_t booster_arabian_nights[3] = {
  {7, 6},
  {8, 2},
  {0, 0},
};

static const rarity_num_t booster_antiquities[3] = {
  {9, 6},
  {10, 2},
  {0, 0},
};

static const rarity_num_t booster_revised[8] = {
  {43, 7929902},
  {5, 11},
  {1, 11},
  {43, 7929882},
  {5, 3},
  {2, 3},
  {3, 1},
  {0, 0},
};

static const rarity_num_t starter_revised[8] = {
  {43, 7929902},
  {5, 45},
  {1, 45},
  {43, 7929882},
  {5, 13},
  {2, 13},
  {3, 2},
  {0, 0},
};

static const rarity_num_t booster_legends[4] = {
  {11, 11},
  {12, 3},
  {3, 1},
  {0, 0},
};

static const rarity_num_t booster_the_dark[3] = {
  {13, 6},
  {14, 2},
  {0, 0},
};

static const rarity_num_t booster_fallen_empires[3] = {
  {15, 6},
  {16, 2},
  {0, 0},
};

static const rarity_num_t booster_chronicles[3] = {
  {17, 9},
  {18, 3},
  {0, 0},
};

static const rarity_num_t booster_homelands[3] = {
  {19, 6},
  {20, 2},
  {0, 0},
};

static const rarity_num_t booster_alliances[4] = {
  {21, 8},
  {22, 3},
  {23, 1},
  {0, 0},
};

static const rarity_num_t starter_ice_age[5] = {
  {33, 22},
  {1, 26},
  {2, 9},
  {3, 3},
  {0, 0},
};

static const rarity_num_t booster_unglued[5] = {
  {5, 1},
  {1, 6},
  {2, 2},
  {3, 1},
  {0, 0},
};

static const rarity_num_t booster_portal3k[5] = {
  {5, 2},
  {1, 5},
  {2, 2},
  {3, 1},
  {0, 0},
};

static const rarity_num_t booster_time_spiral[5] = {
  {1, 10},
  {2, 3},
  {3, 1},
  {6, 1},
  {0, 0},
};

static const rarity_num_t starter_time_spiral[6] = {
  {5, 30},
  {1, 29},
  {2, 10},
  {3, 3},
  {6, 3},
  {0, 0},
};

static const rarity_num_t booster_planar_chaos[8] = {
  {1, 8},
  {24, 3},
  {2, 2},
  {43, 262147},
  {25, 1},
  {26, 1},
  {3, 1},
  {0, 0},
};

static const rarity_num_t booster_innistrad[10] = {
  {43, 262145},
  {5, 1},
  {5, 0},
  {1, 9},
  {2, 3},
  {43, 7929962},
  {3, 1},
  {4, 1},
  {27, 1},
  {0, 0},
};

static const rarity_num_t booster_dragons_maze[7] = {
  {35, 1},
  {1, 10},
  {2, 3},
  {43, 4849728},
  {3, 1},
  {4, 1},
  {0, 0},
};

static const rarity_num_t booster_conspiracy[6] = {
  {1, 10},
  {2, 3},
  {43, 7929962},
  {3, 1},
  {4, 1},
  {0, 0},
};

static const rarity_num_t booster_shadows_over_innistrad[13] = {
  {43, 131073},
  {5, 1},
  {5, 0},
  {1, 8},
  {43, 524295},
  {1, 1},
  {29, 1},
  {28, 1},
  {2, 3},
  {43, 7929962},
  {3, 1},
  {4, 1},
  {0, 0},
};

static const rarity_num_t booster_ixalan[7] = {
  {5, 1},
  {1, 10},
  {2, 3},
  {43, 8257647},
  {3, 1},
  {4, 1},
  {0, 0},
};

static const rarity_num_t booster_unstable[8] = {
  {5, 1},
  {31, 2},
  {1, 8},
  {2, 3},
  {43, 7929962},
  {3, 1},
  {4, 1},
  {0, 0},
};

static const rarity_num_t booster_guilds_of_ravnica[7] = {
  {36, 1},
  {1, 10},
  {2, 3},
  {43, 7929962},
  {3, 1},
  {4, 1},
  {0, 0},
};

static const rarity_num_t booster_jumpstart[10] = {
  {5, 7},
  {43, 196610},
  {5, 1},
  {3, 1},
  {1, 8},
  {2, 3},
  {43, 4849728},
  {3, 1},
  {4, 1},
  {0, 0},
};

static const rarity_num_t booster_commander_legends[8] = {
  {1, 13},
  {2, 3},
  {43, 4849728},
  {38, 1},
  {39, 1},
  {37, 2},
  {34, 1},
  {0, 0},
};

static const rarity_num_t booster_strixhaven[8] = {
  {1, 9},
  {2, 3},
  {43, 4849728},
  {3, 1},
  {4, 1},
  {40, 1},
  {41, 1},
  {0, 0},
};

static const rarity_num_t booster_mid_vow[8] = {
  {5, 1},
  {1, 9},
  {30, 1},
  {2, 3},
  {43, 4849728},
  {3, 1},
  {4, 1},
  {0, 0},
};

static const rarity_num_t booster_kamigawa_neo[8] = {
  {42, 1},
  {1, 9},
  {2, 3},
  {28, 1},
  {43, 4849728},
  {3, 1},
  {4, 1},
  {0, 0},
};

static booster_t pack_contents[198] = {
  {0, no_boosters, no_starters},
  {1, no_boosters, no_starters},
  {2, booster_alpha, starter_alpha},
  {3, booster_beta, starter_beta},
  {4, booster_beta, starter_beta},
  {5, booster_arabian_nights, no_starters},
  {6, booster_antiquities, no_starters},
  {7, booster_revised, starter_revised},
  {8, booster_legends, no_starters},
  {9, booster_the_dark, no_starters},
  {10, booster_fallen_empires, no_starters},
  {11, booster_15_cur, starter_l22_cur},
  {12, booster_15_cur, starter_ice_age},
  {13, booster_chronicles, no_starters},
  {14, booster_homelands, no_starters},
  {15, booster_alliances, no_starters},
  {16, booster_15_cur, starter_l22_cur},
  {17, booster_15_cur, no_starters},
  {18, booster_15_cur, starter_l22_cur},
  {19, no_boosters, no_starters},
  {20, booster_15_l2cur, no_starters},
  {21, booster_15_cur, no_starters},
  {22, booster_15_cur, starter_l22_cur},
  {23, booster_15_cur, no_starters},
  {24, booster_15_cur, no_starters},
  {25, booster_15_l2cur, no_starters},
  {26, booster_unglued, no_starters},
  {27, booster_15_cur, starter_l30_cur},
  {28, booster_15_cur, no_starters},
  {29, booster_15_cur, starter_l30_cur},
  {30, booster_15_cur, no_starters},
  {31, booster_portal3k, no_starters},
  {32, booster_15_l2cur, no_starters},
  {33, booster_15_cur, starter_l30_cur},
  {34, no_boosters, no_starters},
  {35, booster_15_cur, no_starters},
  {36, no_boosters, no_starters},
  {37, booster_15_cur, no_starters},
  {38, booster_15_cur, starter_l30_cur},
  {39, no_boosters, no_starters},
  {40, booster_15_cur, no_starters},
  {41, booster_15_lcur, no_starters},
  {42, booster_15_cur, no_starters},
  {43, booster_15_cur, starter_l30_cur},
  {44, booster_15_cur, no_starters},
  {45, booster_15_cur, no_starters},
  {46, booster_15_cur, starter_l30_cur},
  {47, booster_15_cur, no_starters},
  {48, booster_15_cur, no_starters},
  {49, booster_15_lcur, no_starters},
  {50, booster_15_cur, starter_l30_cur},
  {51, booster_15_cur, no_starters},
  {52, booster_15_cur, no_starters},
  {53, booster_15_cur, starter_l30_cur},
  {54, booster_15_lcur, no_starters},
  {55, booster_15_cur, no_starters},
  {56, booster_15_cur, no_starters},
  {57, booster_15_lcur, no_starters},
  {58, booster_15_cur, starter_l30_cur},
  {59, booster_15_cur, no_starters},
  {60, booster_15_cur, no_starters},
  {61, booster_15_cur, no_starters},
  {62, booster_time_spiral, starter_time_spiral},
  {63, booster_planar_chaos, no_starters},
  {64, booster_15_cur, no_starters},
  {65, booster_15_lcur, no_starters},
  {66, booster_15_lcur, no_starters},
  {67, booster_15_cur, starter_l30_cur},
  {68, no_boosters, no_starters},
  {69, booster_15_cur, no_starters},
  {70, booster_15_cur, starter_l30_cur},
  {71, booster_15_cur, no_starters},
  {72, no_boosters, no_starters},
  {73, booster_15_lcur, no_starters},
  {74, booster_15_lcurm71, starter_l30_curm},
  {75, no_boosters, no_starters},
  {76, booster_15_lcurm71, no_starters},
  {77, no_boosters, no_starters},
  {78, booster_15_lcurm71, no_starters},
  {79, booster_15_lcurm71, no_starters},
  {80, no_boosters, no_starters},
  {81, no_boosters, no_starters},
  {82, booster_15_cur, no_starters},
  {83, booster_15_lcurm71, no_starters},
  {84, no_boosters, no_starters},
  {85, no_boosters, no_starters},
  {86, booster_15_lcurm71, no_starters},
  {87, no_boosters, no_starters},
  {88, booster_15_lcurm71, no_starters},
  {89, no_boosters, no_starters},
  {90, booster_15_lcurm71, no_starters},
  {91, no_boosters, no_starters},
  {92, no_boosters, no_starters},
  {93, booster_15_lcurm71, no_starters},
  {94, no_boosters, no_starters},
  {95, booster_15_lcur, no_starters},
  {96, booster_15_lcurm71, no_starters},
  {97, no_boosters, no_starters},
  {98, booster_15_lcurm71, no_starters},
  {99, no_boosters, no_starters},
  {100, booster_15_lcurm71, no_starters},
  {101, no_boosters, no_starters},
  {102, no_boosters, no_starters},
  {103, booster_innistrad, no_starters},
  {104, no_boosters, no_starters},
  {105, booster_innistrad, no_starters},
  {106, no_boosters, no_starters},
  {107, booster_15_lcurm71, no_starters},
  {108, no_boosters, no_starters},
  {109, booster_15_lcurm71, no_starters},
  {110, no_boosters, no_starters},
  {111, no_boosters, no_starters},
  {112, booster_15_lcurm71, no_starters},
  {113, no_boosters, no_starters},
  {114, booster_15_lcurm71, no_starters},
  {115, no_boosters, no_starters},
  {116, booster_dragons_maze, no_starters},
  {117, booster_15_curm71a, no_starters},
  {118, booster_15_lcurm71, no_starters},
  {119, no_boosters, no_starters},
  {120, no_boosters, no_starters},
  {121, booster_15_lcurm71, no_starters},
  {122, no_boosters, no_starters},
  {123, booster_15_lcurm71, no_starters},
  {124, no_boosters, no_starters},
  {125, booster_15_lcurm71, no_starters},
  {126, no_boosters, no_starters},
  {127, booster_conspiracy, no_starters},
  {128, booster_15_curm71a, no_starters},
  {129, booster_15_lcurm71, no_starters},
  {130, no_boosters, no_starters},
  {131, no_boosters, no_starters},
  {132, booster_15_lcurm71, no_starters},
  {133, no_boosters, no_starters},
  {134, booster_15_lcurm71, no_starters},
  {135, no_boosters, no_starters},
  {136, booster_15_lcurm71, no_starters},
  {137, booster_15_curm71a, no_starters},
  {138, booster_15_lcurm71, no_starters},
  {139, no_boosters, no_starters},
  {140, no_boosters, no_starters},
  {141, booster_15_lcurm71, no_starters},
  {142, no_boosters, no_starters},
  {143, booster_15_lcurm71, no_starters},
  {144, no_boosters, no_starters},
  {145, booster_shadows_over_innistrad, no_starters},
  {146, booster_15_curm71a, no_starters},
  {147, booster_shadows_over_innistrad, no_starters},
  {148, no_boosters, no_starters},
  {149, booster_conspiracy, no_starters},
  {150, no_boosters, no_starters},
  {151, booster_15_lcurm71, no_starters},
  {152, no_boosters, no_starters},
  {153, booster_15_lcurm71, no_starters},
  {154, booster_15_curm71a, no_starters},
  {155, no_boosters, no_starters},
  {156, booster_15_lcurm71, no_starters},
  {157, no_boosters, no_starters},
  {158, booster_15_lcurm71, no_starters},
  {159, no_boosters, no_starters},
  {160, booster_ixalan, no_starters},
  {161, booster_unstable, no_starters},
  {162, booster_15_lcurm71, no_starters},
  {163, booster_15_lcurm71, no_starters},
  {164, booster_15_lcurm71, no_starters},
  {165, no_boosters, no_starters},
  {166, booster_15_lcurm71, no_starters},
  {167, no_boosters, no_starters},
  {168, booster_guilds_of_ravnica, no_starters},
  {169, booster_guilds_of_ravnica, no_starters},
  {170, booster_15_lcurm71, no_starters},
  {171, booster_15_curm71a, no_starters},
  {172, booster_15_curm71a, no_starters},
  {173, no_boosters, no_starters},
  {174, booster_15_lcurm71, no_starters},
  {175, no_boosters, no_starters},
  {176, booster_15_lcurm71, no_starters},
  {177, booster_15_lcurm71, no_starters},
  {178, no_boosters, no_starters},
  {179, booster_15_lcurm71, no_starters},
  {180, booster_jumpstart, no_starters},
  {181, booster_15_lcurm74, no_starters},
  {182, booster_commander_legends, no_starters},
  {183, booster_15_lcurm74, no_starters},
  {184, no_boosters, no_starters},
  {185, booster_strixhaven, no_starters},
  {186, no_boosters, no_starters},
  {187, booster_15_lcurm74, no_starters},
  {188, booster_15_lcurm74, no_starters},
  {189, no_boosters, no_starters},
  {190, booster_mid_vow, no_starters},
  {191, no_boosters, no_starters},
  {192, booster_mid_vow, no_starters},
  {193, no_boosters, no_starters},
  {194, booster_kamigawa_neo, no_starters},
  {195, no_boosters, no_starters},
  {196, booster_15_lcurm74, no_starters},
  {197, no_boosters, no_starters},
};

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26eb550; original functions/packs.c:650. */
static void add_to_csvid_choices(int *choice_num, int **choice_csvids, int csvid)
{
  int result;
  int *piVar2;
  result = *choice_num;
  if (result < 0)
  {
    backtrace_and_abort("functions/packs.c:652\nchoices->num >= 0");
  }
  if (*choice_csvids == NULL)
  {
    if (result != 0)
    {
      backtrace_and_abort("functions/packs.c:656\nchoices->num == 0");
    }
    piVar2 = malloc(800);
    *choice_csvids = piVar2;
    if (piVar2 == NULL)
    {
      fatal("could not malloc %d bytes for choices->csvids", 800);
    }
    result = *choice_num;
  }
  if (((result == 200 || result == 400) || result == 800) || (result == 0x640))
  {
    piVar2 = realloc(*choice_csvids, result << 3);
    *choice_csvids = piVar2;
    result = *choice_num;
    if (piVar2 == NULL)
    {
      fatal("could not realloc %d bytes for choices->csvids", result << 3);
    }
  }
  if (result != 0xc80)
  {
    piVar2 = *choice_csvids;
    *choice_num = result + 1;
    piVar2[result] = csvid;
    return;
  }
  fatal("Tried to insert 3200th card in csvid_t_choices_t");
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26eb660; original functions/packs.c:3042. */
static int add_card_to_pack(int *out_csvids, int pos, int *choice_num, int **choice_csvids, int num,
                          int rarity, int expansion, int booster, int testing_mode)
{
  unsigned int *puVar1;
  unsigned int uVar2;
  bool bVar3;
  void *argument1;
  int iVar4;
  FILE *pFVar5;
  int iVar6;
  int *piVar7;
  int *piVar8;
  bool bVar9;
  bool bVar10;
  int iStack_20;

  if (num < 1)
  {
    return 0;
  }
  iVar6 = *choice_num;
  if (iVar6 < 1)
  {
    if (testing_mode == 0)
    {
      return 0;
    }
    pFVar5 = stderr;
    fprintf(pFVar5, "Expansion #%d, booster=%d: no available cards of rarity %d\012", expansion,
            booster, rarity);
    return 0;
  }
  uVar2 = iVar6 * 4;
  argument1 = malloc(uVar2);
  if (argument1 == NULL)
  {
    fatal("could not malloc %d bytes for local_choices.csvids", uVar2);
  }
  memcpy(argument1, *choice_csvids, uVar2);
  iStack_20 = 0;
  do
  {
    iVar4 = internal_rand(iVar6);
    puVar1 = (unsigned int *)((int)argument1 + iVar4 * 4);
    uVar2 = *puVar1;
    if (uVar2 == 0xef)
    {
    LAB_026eb7b2:
      out_csvids[pos] = uVar2;
      iStack_20 = iStack_20 + 1;
      pos = pos + 1;
    }
    else
    {
      if (uVar2 < 0xf0)
      {
        if (uVar2 != 0xa4)
        {
          if (uVar2 < 0xa5)
          {
            if ((uVar2 != 0x5b) && (uVar2 != 0x7e))
              goto LAB_026eb757;
          }
          else if (uVar2 != 0xbc)
          {
          LAB_026eb757:
            bVar10 = true;
            bVar9 = false;
            goto LAB_026eb762;
          }
        }
        goto LAB_026eb7b2;
      }
      if (4 < uVar2 - 0x834)
        goto LAB_026eb757;
      bVar9 = expansion == 0xc;
      bVar10 = expansion != 0xc;
      if (bVar10)
      {
      LAB_026eb762:
        iVar4 = 0;
        if (0 < pos)
        {
          do
          {
            if (uVar2 == out_csvids[iVar4])
            {
              bVar3 = true;
              goto LAB_026eb6f4;
            }
            iVar4 = iVar4 + 1;
          } while (iVar4 != pos);
        }
      }
      bVar3 = false;
    LAB_026eb6f4:
      if ((iVar6 == 1) && (bVar10))
      {
        if (!bVar3)
        {
          out_csvids[pos] = uVar2;
          iStack_20 = iStack_20 + 1;
          pos = pos + 1;
        }
        if (num <= iStack_20)
          goto LAB_026eb7c9;
        piVar7 = out_csvids + pos;
        do
        {
          piVar8 = piVar7 + 1;
          iVar6 = internal_rand(1);
          *piVar7 = *(int *)((int)argument1 + iVar6 * 4);
          piVar7 = piVar8;
        } while (piVar8 != out_csvids + ((num + pos) - iStack_20));
        if (testing_mode == 0)
        {
        LAB_026eb7c9:
          free(argument1);
        }
        else
        {
          pFVar5 = stderr;
          fprintf(pFVar5, "Expansion #%d, booster=%d: added duplicate cards of rarity %d\012",
                  expansion, booster, rarity);
          free(argument1);
        }
        return num;
      }
      if (!bVar9)
      {
        iVar4 = iVar6 * 4;
        iVar6 = iVar6 + -1;
        *puVar1 = *(unsigned int *)((int)argument1 + iVar4 + -4);
      }
      if (!bVar3)
        goto LAB_026eb7b2;
    }
    if (num <= iStack_20)
      goto LAB_026eb7c9;
  } while (true);
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26ec840; original functions/packs.c:3397. */
int is_valid_pack(EXPANSION_t expansion, int booster)
{
  const rarity_num_t *prVar1;

  if (((uint32_t)expansion < 198U) && (expansion != EXPANSION_UNHINGED))
  {
    switch (expansion)
    {
    case EXPANSION_UNSTABLE:
    case EXPANSION_CORE_SET_2021:
    case EXPANSION_ZENDIKAR_RISING:
    case EXPANSION_COMMANDER_LEGENDS:
    case EXPANSION_KALDHEIM:
    case EXPANSION_STRIXHAVEN_SCHOOL_OF_MAGES:
    case EXPANSION_MODERN_HORIZONS_2:
    case EXPANSION_ADVENTURES_IN_THE_FORGOTTEN_REALMS:
    case EXPANSION_INNISTRAD_MIDNIGHT_HUNT:
    case EXPANSION_INNISTRAD_CRIMSON_VOW:
    case EXPANSION_KAMIGAWA_NEON_DYNASTY:
    case EXPANSION_STREETS_OF_NEW_CAPENNA:
      break;
    default:
      if (booster == 0)
      {
        prVar1 = pack_contents[expansion].starter;
      }
      else
      {
        prVar1 = pack_contents[expansion].booster;
      }
      return (unsigned int)(prVar1->rarity != XNONE);
    }
  }
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26ec8a0; original functions/packs.c:2989. */
int pack_size(EXPANSION_t expansion, int booster)
{
  int iVar1;
  int iVar2;
  int iVar3;
  const rarity_num_t *prVar4;
  draftrarity_t dVar5;
  int iVar6;
  const rarity_num_t *prVar7;

  iVar2 = is_valid_pack(expansion, booster);
  iVar3 = 0;
  if (iVar2 != 0)
  {
    if (booster == 0)
    {
      prVar7 = pack_contents[expansion].starter;
      if (prVar7 == no_starters)
      {
        return 0;
      }
    }
    else
    {
      prVar7 = pack_contents[expansion].booster;
    }
    dVar5 = prVar7->rarity;
    if (dVar5 == XNONE)
    {
      return 0;
    }
    iVar3 = 0;
    iVar2 = 0;
    prVar4 = prVar7;
    do
    {
      while (dVar5 == MAX_RARITY_VAL_USED)
      {
        if (prVar7[iVar2 + 1].rarity == XNONE)
        {
          backtrace_and_abort("functions/packs.c:3007\nrn[i + 1].rarity != NONE");
        }
        if (prVar7[iVar2 + 2].rarity == XNONE)
        {
          backtrace_and_abort("functions/packs.c:3008\nrn[i + 2].rarity != NONE");
        }
        iVar1 = prVar7[iVar2 + 2].num;
        iVar6 = prVar7[iVar2 + 1].num;
        if (iVar6 < iVar1)
        {
          iVar6 = iVar1;
        }
        iVar3 = iVar3 + iVar6;
        iVar2 = iVar2 + 3;
        prVar4 = prVar7 + iVar2;
        dVar5 = prVar4->rarity;
        if (dVar5 == XNONE)
        {
          return iVar3;
        }
      }
      iVar2 = iVar2 + 1;
      iVar3 = iVar3 + prVar4->num;
      prVar4 = prVar7 + iVar2;
      dVar5 = prVar4->rarity;
    } while (dVar5 != XNONE);
  }
  return iVar3;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26ed870; original functions/packs.c:3770. */
void announce_expansion_choice(int player, int card, EXPANSION_t expansion)
{
  const char **ppcVar1;
  int num_textlines;
  if (ai_is_speculating == 1)
  {
    return;
  }
  if (player != 0)
  {
    ppcVar1 = load_text_nocopy_from_file("Menus.txt", "EXPANSIONNAMES", &num_textlines);
    if ((int)(expansion + EXPANSION_ANTIQUITIES) < num_textlines)
    {
      dialog_impl(player, card, EVENT_ACTIVATE, "\001\011",
                  ppcVar1[expansion + EXPANSION_REVISED_EDITION], "\001\014", "\001\003", 0, 0, 0,
                  0);
      return;
    }
  }
  return;
}

#endif


#ifndef SHANDALAR
typedef struct count_and_card_t { int count; int csvid; } count_and_card_t;
typedef struct varying_copies_entry_t { draftrarity_t rarity; const count_and_card_t *counts_and_cards; } varying_copies_entry_t;
_Static_assert(sizeof(count_and_card_t)==8 && sizeof(varying_copies_entry_t)==8, "native collation record widths");

static const count_and_card_t copies_common_arabian_nights[28] = {
  {4, 397},
  {4, 399},
  {5, 401},
  {4, 404},
  {4, 406},
  {11, 407},
  {4, 408},
  {5, 415},
  {4, 418},
  {5, 420},
  {4, 421},
  {4, 422},
  {5, 424},
  {4, 425},
  {5, 437},
  {4, 441},
  {5, 443},
  {1, 164},
  {5, 444},
  {4, 447},
  {4, 448},
  {4, 452},
  {4, 454},
  {4, 461},
  {5, 462},
  {4, 463},
  {5, 464},
  {-1, 0},
};

static const count_and_card_t copies_uncommon_arabian_nights[52] = {
  {3, 391},
  {2, 392},
  {2, 393},
  {2, 396},
  {3, 394},
  {2, 395},
  {3, 398},
  {2, 400},
  {3, 930},
  {2, 402},
  {3, 403},
  {3, 405},
  {2, 931},
  {3, 409},
  {2, 410},
  {2, 411},
  {2, 412},
  {2, 414},
  {2, 413},
  {2, 416},
  {3, 417},
  {3, 419},
  {2, 423},
  {2, 426},
  {2, 427},
  {2, 428},
  {2, 429},
  {2, 430},
  {3, 431},
  {2, 432},
  {2, 433},
  {2, 434},
  {3, 435},
  {2, 436},
  {3, 438},
  {3, 439},
  {3, 440},
  {2, 442},
  {4, 445},
  {2, 446},
  {2, 449},
  {2, 450},
  {2, 451},
  {3, 453},
  {2, 455},
  {2, 456},
  {2, 457},
  {3, 458},
  {2, 459},
  {3, 460},
  {2, 465},
  {-1, 0},
};

static const count_and_card_t copies_common_antiquities[33] = {
  {4, 466},
  {4, 468},
  {4, 469},
  {4, 470},
  {4, 472},
  {4, 516},
  {4, 473},
  {4, 477},
  {4, 478},
  {4, 483},
  {4, 487},
  {1, 488},
  {4, 491},
  {4, 492},
  {1, 495},
  {4, 500},
  {1, 508},
  {4, 513},
  {4, 514},
  {4, 515},
  {4, 549},
  {4, 521},
  {4, 522},
  {4, 524},
  {4, 527},
  {1, 528},
  {4, 530},
  {4, 540},
  {6, 541},
  {6, 543},
  {6, 544},
  {4, 550},
  {-1, 0},
};

static const count_and_card_t copies_uncommon_antiquities[56] = {
  {1, 467},
  {2, 471},
  {2, 474},
  {2, 475},
  {3, 476},
  {1, 479},
  {1, 480},
  {3, 481},
  {3, 482},
  {1, 484},
  {1, 485},
  {1, 486},
  {3, 489},
  {3, 490},
  {3, 493},
  {3, 494},
  {1, 496},
  {3, 497},
  {3, 498},
  {1, 499},
  {3, 501},
  {1, 502},
  {3, 503},
  {2, 504},
  {3, 505},
  {3, 506},
  {3, 507},
  {3, 508},
  {1, 509},
  {1, 510},
  {1, 511},
  {3, 512},
  {3, 518},
  {3, 517},
  {3, 519},
  {3, 520},
  {3, 523},
  {1, 525},
  {1, 526},
  {3, 528},
  {3, 529},
  {1, 531},
  {3, 532},
  {3, 533},
  {1, 534},
  {3, 535},
  {3, 536},
  {3, 537},
  {1, 538},
  {1, 539},
  {1, 542},
  {3, 545},
  {3, 546},
  {3, 547},
  {1, 548},
  {-1, 0},
};

static const count_and_card_t copies_common_the_dark[42] = {
  {3, 294},
  {3, 336},
  {3, 350},
  {3, 303},
  {3, 305},
  {3, 298},
  {3, 310},
  {3, 370},
  {3, 311},
  {3, 314},
  {3, 934},
  {3, 321},
  {3, 334},
  {3, 326},
  {3, 327},
  {3, 332},
  {3, 329},
  {3, 936},
  {3, 330},
  {3, 937},
  {3, 938},
  {3, 375},
  {3, 374},
  {3, 339},
  {3, 365},
  {3, 366},
  {3, 939},
  {1, 377},
  {3, 941},
  {3, 347},
  {3, 371},
  {3, 942},
  {3, 355},
  {3, 944},
  {3, 946},
  {3, 947},
  {3, 948},
  {3, 364},
  {3, 383},
  {3, 951},
  {3, 388},
  {-1, 0},
};

static const count_and_card_t copies_uncommon_the_dark[79] = {
  {2, 292},
  {2, 293},
  {1, 300},
  {1, 295},
  {2, 296},
  {1, 297},
  {1, 299},
  {2, 301},
  {2, 302},
  {2, 353},
  {2, 304},
  {2, 306},
  {1, 307},
  {1, 367},
  {2, 308},
  {2, 932},
  {1, 309},
  {2, 369},
  {2, 376},
  {2, 312},
  {2, 933},
  {2, 313},
  {1, 315},
  {1, 316},
  {2, 317},
  {2, 318},
  {2, 319},
  {2, 320},
  {2, 935},
  {2, 324},
  {1, 325},
  {1, 331},
  {1, 333},
  {1, 335},
  {1, 337},
  {1, 338},
  {1, 340},
  {2, 341},
  {1, 342},
  {1, 343},
  {1, 344},
  {1, 345},
  {2, 940},
  {1, 346},
  {1, 378},
  {2, 348},
  {1, 373},
  {2, 349},
  {2, 323},
  {1, 351},
  {1, 352},
  {1, 943},
  {2, 354},
  {2, 356},
  {1, 357},
  {2, 358},
  {1, 945},
  {2, 322},
  {1, 359},
  {2, 360},
  {2, 361},
  {1, 381},
  {2, 328},
  {2, 362},
  {1, 363},
  {2, 949},
  {2, 372},
  {2, 950},
  {2, 368},
  {2, 379},
  {1, 380},
  {2, 382},
  {2, 384},
  {2, 385},
  {2, 386},
  {1, 387},
  {1, 390},
  {1, 389},
  {-1, 0},
};

static const count_and_card_t copies_common_fallen_empires[37] = {
  {4, 1750},
  {4, 1752},
  {4, 1754},
  {4, 1756},
  {1, 1759},
  {3, 1768},
  {4, 1771},
  {3, 1774},
  {3, 1775},
  {3, 1777},
  {3, 1781},
  {3, 1783},
  {4, 1785},
  {3, 1790},
  {4, 1792},
  {3, 1795},
  {4, 1456},
  {4, 1796},
  {3, 1797},
  {3, 1799},
  {4, 1802},
  {3, 1806},
  {4, 1807},
  {3, 1808},
  {3, 1809},
  {3, 1810},
  {3, 1104},
  {4, 1812},
  {3, 1555},
  {3, 1556},
  {3, 1823},
  {4, 1827},
  {4, 1833},
  {3, 1837},
  {3, 1841},
  {4, 1842},
  {-1, 0},
};

static const count_and_card_t copies_uncommon_fallen_empires[67] = {
  {1, 1749},
  {1, 1751},
  {1, 1753},
  {3, 1755},
  {1, 1757},
  {3, 1758},
  {1, 1760},
  {1, 1761},
  {1, 1762},
  {1, 1763},
  {3, 1764},
  {1, 1765},
  {3, 1766},
  {2, 1767},
  {1, 1769},
  {2, 1770},
  {1, 1772},
  {1, 1773},
  {3, 1778},
  {3, 1776},
  {3, 1779},
  {1, 1780},
  {1, 1782},
  {3, 1784},
  {1, 1786},
  {1, 1787},
  {2, 1788},
  {3, 1789},
  {1, 1791},
  {1, 1793},
  {3, 1794},
  {1, 1798},
  {3, 1800},
  {3, 1801},
  {1, 1803},
  {1, 1804},
  {1, 1447},
  {1, 1805},
  {3, 1811},
  {1, 1813},
  {3, 1814},
  {1, 1815},
  {1, 1816},
  {1, 1817},
  {2, 1818},
  {1, 1819},
  {3, 1820},
  {3, 1821},
  {1, 1822},
  {3, 1824},
  {3, 1825},
  {2, 1826},
  {3, 1828},
  {3, 1831},
  {1, 1832},
  {3, 1829},
  {1, 1830},
  {1, 1834},
  {3, 1835},
  {3, 1836},
  {3, 1838},
  {3, 1839},
  {1, 1840},
  {1, 1573},
  {1, 1843},
  {1, 1844},
  {-1, 0},
};

static const count_and_card_t copies_common_chronicles[46] = {
  {3, 553},
  {3, 469},
  {2, 474},
  {2, 476},
  {3, 350},
  {3, 585},
  {3, 590},
  {3, 404},
  {3, 406},
  {3, 607},
  {3, 615},
  {3, 623},
  {2, 495},
  {3, 418},
  {3, 638},
  {2, 324},
  {3, 421},
  {3, 648},
  {3, 332},
  {3, 936},
  {3, 938},
  {3, 424},
  {3, 682},
  {3, 696},
  {1, 697},
  {2, 341},
  {1, 723},
  {3, 441},
  {3, 730},
  {3, 770},
  {3, 450},
  {2, 356},
  {3, 947},
  {1, 790},
  {1, 818},
  {2, 368},
  {1, 820},
  {3, 823},
  {4, 541},
  {4, 543},
  {4, 544},
  {3, 843},
  {3, 847},
  {3, 849},
  {3, 463},
  {-1, 0},
};

static const count_and_card_t copies_uncommon_chronicles[72] = {
  {3, 391},
  {1, 558},
  {1, 392},
  {1, 564},
  {1, 568},
  {1, 569},
  {1, 571},
  {1, 572},
  {3, 573},
  {3, 296},
  {1, 297},
  {3, 579},
  {1, 299},
  {3, 301},
  {1, 353},
  {1, 588},
  {1, 595},
  {1, 403},
  {3, 598},
  {1, 599},
  {3, 601},
  {1, 405},
  {1, 608},
  {1, 309},
  {3, 625},
  {3, 416},
  {3, 631},
  {3, 320},
  {1, 644},
  {1, 646},
  {3, 498},
  {1, 670},
  {1, 673},
  {3, 686},
  {1, 504},
  {1, 431},
  {1, 691},
  {1, 693},
  {1, 711},
  {1, 731},
  {1, 733},
  {1, 511},
  {1, 737},
  {1, 741},
  {3, 747},
  {3, 751},
  {3, 755},
  {1, 520},
  {3, 764},
  {1, 772},
  {1, 952},
  {1, 357},
  {1, 783},
  {1, 784},
  {3, 786},
  {3, 787},
  {1, 791},
  {1, 797},
  {3, 798},
  {3, 807},
  {1, 809},
  {3, 372},
  {1, 815},
  {1, 824},
  {1, 834},
  {1, 838},
  {3, 845},
  {3, 850},
  {3, 387},
  {1, 858},
  {1, 548},
  {-1, 0},
};

static const count_and_card_t copies_common_homelands[42] = {
  {4, 2168},
  {4, 2169},
  {1, 2170},
  {4, 2171},
  {4, 2177},
  {4, 973},
  {4, 2181},
  {4, 2187},
  {4, 2189},
  {1, 2191},
  {1, 2192},
  {1, 2193},
  {1, 2194},
  {4, 2195},
  {4, 2197},
  {4, 2199},
  {1, 2200},
  {4, 2202},
  {4, 2204},
  {1, 2206},
  {4, 2208},
  {1, 2212},
  {4, 2213},
  {1, 2215},
  {1, 2216},
  {4, 2220},
  {1, 2221},
  {4, 1455},
  {1, 1084},
  {4, 2225},
  {4, 1646},
  {1, 2230},
  {1, 2235},
  {4, 2237},
  {4, 2240},
  {1, 1648},
  {1, 2244},
  {4, 2245},
  {4, 1651},
  {4, 2247},
  {4, 2250},
  {-1, 0},
};

static const count_and_card_t copies_uncommon_homelands[75] = {
  {3, 1628},
  {3, 2179},
  {1, 2176},
  {1, 1629},
  {1, 2172},
  {3, 2173},
  {3, 2174},
  {1, 2175},
  {1, 2178},
  {1, 1630},
  {3, 2180},
  {1, 1631},
  {1, 2182},
  {1, 2183},
  {1, 1632},
  {1, 2184},
  {1, 2185},
  {1, 2186},
  {3, 2188},
  {1, 2190},
  {1, 1633},
  {1, 2196},
  {3, 1634},
  {1, 1635},
  {3, 1636},
  {1, 1637},
  {1, 2198},
  {3, 1638},
  {3, 2201},
  {1, 1639},
  {1, 2203},
  {1, 2205},
  {3, 2207},
  {3, 2209},
  {1, 1640},
  {1, 2210},
  {1, 2211},
  {1, 1641},
  {3, 1374},
  {3, 1642},
  {1, 2214},
  {1, 1643},
  {3, 2217},
  {1, 2218},
  {3, 2219},
  {1, 2222},
  {1, 2223},
  {1, 2224},
  {1, 2226},
  {1, 1644},
  {3, 2227},
  {1, 1645},
  {1, 2228},
  {3, 2229},
  {3, 2231},
  {1, 2232},
  {3, 2234},
  {3, 2233},
  {1, 2236},
  {3, 2238},
  {3, 2239},
  {1, 1647},
  {1, 2241},
  {1, 2242},
  {3, 2243},
  {1, 1649},
  {3, 1650},
  {1, 2246},
  {1, 2248},
  {1, 2249},
  {1, 1652},
  {1, 1654},
  {1, 2251},
  {3, 2252},
  {-1, 0},
};

static const count_and_card_t copies_common_alliances[51] = {
  {2, 2253},
  {2, 2254},
  {2, 2255},
  {2, 2257},
  {2, 2258},
  {2, 1685},
  {2, 2261},
  {3, 2262},
  {2, 2266},
  {2, 2267},
  {2, 1400},
  {2, 2277},
  {2, 2278},
  {3, 2283},
  {2, 2286},
  {2, 2287},
  {2, 2289},
  {3, 2290},
  {2, 2291},
  {2, 2293},
  {3, 1052},
  {2, 2299},
  {2, 2303},
  {2, 2306},
  {2, 2310},
  {2, 1407},
  {2, 2316},
  {2, 2324},
  {2, 2326},
  {2, 2329},
  {2, 2332},
  {2, 2335},
  {2, 2340},
  {2, 2348},
  {2, 2351},
  {3, 2352},
  {2, 2353},
  {2, 2354},
  {2, 2358},
  {3, 1162},
  {2, 1679},
  {3, 2365},
  {3, 2367},
  {2, 2372},
  {2, 2375},
  {2, 2377},
  {2, 2378},
  {2, 2381},
  {3, 1667},
  {2, 2384},
  {-1, 0},
};

static const count_and_card_t copies_uncommon_alliances[46] = {
  {2, 2259},
  {2, 2263},
  {2, 2264},
  {2, 2265},
  {2, 2269},
  {6, 1684},
  {2, 2270},
  {2, 2272},
  {2, 2274},
  {2, 2275},
  {2, 2276},
  {6, 2280},
  {6, 2282},
  {2, 2285},
  {5, 2292},
  {2, 2295},
  {2, 2298},
  {2, 2301},
  {2, 2304},
  {2, 2308},
  {2, 2312},
  {2, 2313},
  {2, 2318},
  {2, 2319},
  {2, 2320},
  {2, 2321},
  {2, 2322},
  {2, 1406},
  {2, 2333},
  {2, 2334},
  {6, 2336},
  {2, 2341},
  {2, 2342},
  {2, 2343},
  {2, 2344},
  {2, 2346},
  {2, 2355},
  {2, 2356},
  {2, 2360},
  {2, 2361},
  {2, 2362},
  {2, 2363},
  {2, 2373},
  {2, 2379},
  {2, 2385},
  {-1, 0},
};

static const count_and_card_t copies_rare_alliances[50] = {
  {2, 2256},
  {2, 1686},
  {2, 2260},
  {2, 2268},
  {2, 2271},
  {2, 2273},
  {2, 2279},
  {2, 2281},
  {2, 2284},
  {2, 2288},
  {2, 2294},
  {2, 2296},
  {2, 2297},
  {2, 2300},
  {2, 1481},
  {2, 2302},
  {2, 2305},
  {2, 2307},
  {2, 2309},
  {2, 2311},
  {2, 2314},
  {2, 2315},
  {2, 2317},
  {2, 2323},
  {2, 2325},
  {2, 2327},
  {2, 2328},
  {2, 2330},
  {2, 2331},
  {2, 2337},
  {2, 2338},
  {2, 2339},
  {2, 2345},
  {2, 2349},
  {2, 2350},
  {6, 2347},
  {2, 2357},
  {2, 2359},
  {2, 2364},
  {2, 2366},
  {2, 2368},
  {2, 2369},
  {2, 2370},
  {2, 2371},
  {6, 2374},
  {2, 2376},
  {2, 2380},
  {6, 2382},
  {2, 2383},
  {-1, 0},
};

static const count_and_card_t copies_guildgate_or_shockland[22] = {
  {65, 12662},
  {65, 12900},
  {65, 12926},
  {65, 12733},
  {65, 12964},
  {65, 12754},
  {65, 13025},
  {65, 12794},
  {65, 12815},
  {65, 13058},
  {7, 1714},
  {7, 1715},
  {7, 1711},
  {7, 1713},
  {7, 1707},
  {7, 1708},
  {7, 1712},
  {7, 1710},
  {7, 1498},
  {7, 1709},
  {6, 13177},
  {0, 0},
};

static const count_and_card_t copies_mystical_archive[64] = {
  {3, 16838},
  {3, 11422},
  {3, 30},
  {3, 11739},
  {3, 14532},
  {3, 10527},
  {3, 62},
  {3, 12220},
  {3, 6552},
  {3, 15531},
  {3, 2748},
  {3, 9237},
  {3, 5725},
  {3, 17331},
  {3, 1281},
  {6, 22951},
  {6, 1547},
  {6, 1549},
  {6, 48},
  {6, 55},
  {6, 19850},
  {6, 10408},
  {6, 1552},
  {6, 20094},
  {6, 12174},
  {6, 2905},
  {6, 13450},
  {6, 8566},
  {6, 19642},
  {6, 1243},
  {6, 10958},
  {6, 8592},
  {6, 145},
  {6, 1328},
  {6, 8803},
  {6, 1455},
  {6, 1506},
  {6, 202},
  {6, 10465},
  {6, 236},
  {6, 240},
  {6, 6589},
  {6, 11696},
  {6, 5173},
  {6, 20259},
  {25, 18341},
  {25, 20947},
  {25, 20551},
  {25, 11110},
  {25, 14251},
  {25, 22312},
  {25, 3755},
  {25, 21569},
  {25, 20355},
  {25, 9395},
  {25, 5051},
  {25, 18988},
  {25, 1147},
  {25, 22497},
  {25, 4255},
  {25, 20778},
  {25, 21701},
  {25, 21188},
  {-1, 0},
};

static const count_and_card_t copies_neo_land[16] = {
  {4, 188},
  {4, 126},
  {4, 239},
  {4, 164},
  {4, 91},
  {1, 14230},
  {1, 14234},
  {1, 14255},
  {1, 14301},
  {1, 14355},
  {1, 14371},
  {1, 14399},
  {1, 14406},
  {1, 14413},
  {1, 14432},
  {-1, 0},
};

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26eb8e0; original functions/packs.c:3158. */
static int make_pack_normal(EXPANSION_t expansion, int booster, int *out_csvids, int testing_mode)
{
  const count_and_card_t *pcVar1;
  varying_copies_entry_t *paVar2;
  static const int contraption_copies[3] = {16, 7, 2};
  uint8_t uVar3;
  short sVar4;
  bool bVar5;
  unsigned char bVar6;
  csvid_choices_t *pcVar7;
  int16_t *piVar8;
  draftrarity_t dVar9;
  int iVar10;
  int iVar11;
  const Rarities *pRVar12;
  FILE *pFVar13;
  int iVar14;
  int iVar15;
  const rarity_num_t *prVar16;
  int *pcVar17;
  unsigned int uVar18;
  unsigned int uVar19;
  const rarity_num_t *prVar21;
  int **ppcVar22;
  unsigned int uVar23;
  varying_copies_entry_t *paVar24;
  const count_and_card_t *pcVar25;
  int *choice_num;
  unsigned char bVar26;
  _Static_assert(sizeof(Rarities) == 5 && offsetof(Rarities, rarity_in_expansion) == 5,
                 "native rarity byte tail");
  _Static_assert(offsetof(card_ptr_t, types) == 132 &&
                     offsetof(card_ptr_t, mana_source_colors) == 146,
                 "native seven subtypes");
  int iStack_22c;
  unsigned int uStack_220;
  unsigned int uStack_21c;
  const rarity_num_t *prStack_20c;
  varying_copies_entry_t varying_copies[19] = {
      {7, copies_common_arabian_nights},
      {8, copies_uncommon_arabian_nights},
      {9, copies_common_antiquities},
      {10, copies_uncommon_antiquities},
      {13, copies_common_the_dark},
      {14, copies_uncommon_the_dark},
      {15, copies_common_fallen_empires},
      {16, copies_uncommon_fallen_empires},
      {17, copies_common_chronicles},
      {18, copies_uncommon_chronicles},
      {19, copies_common_homelands},
      {20, copies_uncommon_homelands},
      {21, copies_common_alliances},
      {22, copies_uncommon_alliances},
      {23, copies_rare_alliances},
      {35, copies_guildgate_or_shockland},
      {41, copies_mystical_archive},
      {42, copies_neo_land},
      {0, NULL},
  };
  csvid_choices_t choices[44];

  bVar26 = 0;
  if (booster == 0)
  {
    prStack_20c = pack_contents[expansion].starter;
  }
  else
  {
    prStack_20c = pack_contents[expansion].booster;
  }
  pcVar7 = choices;
  do
  {
    pcVar7->num = -1;
    pcVar7->csvids = NULL;
    pcVar7 = pcVar7 + 1;
  } while (choices + 44 != pcVar7);
  dVar9 = prStack_20c->rarity;
  prVar21 = prStack_20c;
  while (dVar9 != XNONE)
  {
    prVar16 = prVar21 + 1;
    if (dVar9 == MAX_RARITY_VAL_USED)
    {
      if ((prVar16->rarity == XNONE) || (prVar16->rarity == MAX_RARITY_VAL_USED))
      {
        backtrace_and_abort(
            "functions/packs.c:3175\nrn[i + 1].rarity != NONE && rn[i + 1].rarity != VAL_IFELSE");
      }
      if ((prVar21[2].rarity == XNONE) || (prVar21[2].rarity == MAX_RARITY_VAL_USED))
      {
        backtrace_and_abort(
            "functions/packs.c:3176\nrn[i + 2].rarity != NONE && rn[i + 2].rarity != VAL_IFELSE");
      }
    }
    else
    {
      if (MAX_RARITY_VAL_USED < dVar9)
      {
        backtrace_and_abort("functions/packs.c:3180\nrn[i].rarity <= MAX_RARITY_VAL_USED");
      }
      choices[prVar21->rarity].num = 0;
    }
    prVar21 = prVar16;
    dVar9 = prVar16->rarity;
  }
  if (((((expansion & ~EXPANSION_FUTURE_SIGHT) == EXPANSION_VINTAGE_MASTERS) ||
        (expansion == EXPANSION_LEGENDS)) ||
       (expansion == EXPANSION_PLANAR_CHAOS)) ||
      (expansion == EXPANSION_UNSTABLE))
  {
    uStack_21c = 1;
  }
  else
  {
    uStack_21c = 1;
    if (((expansion - EXPANSION_INNISTRAD & 0xfffffffd) != 0) &&
        ((expansion & ~EXPANSION_LIMITED_EDITION_ALPHA) != EXPANSION_SHADOWS_OVER_INNISTRAD))
    {
      uStack_21c =
          (unsigned int)((expansion - EXPANSION_INNISTRAD_MIDNIGHT_HUNT & 0xfffffffb) == 0);
    }
  }

  uVar19 = 0;
  if (1 < available_slots)
  {
    do
    {
      while (true)
      {
        pRVar12 = get_rarity(uVar19);
        uStack_220 = (unsigned int)pRVar12->rarity_in_expansion[expansion];
        if (((uStack_220 != 0) && (iVar14 = is_valid_card(uVar19), iVar14 != 0)) &&
            ((bVar6 = pRVar12->dfc_symbol, bVar6 == 0xff || ((bVar6 & 1) == 0))))
          break;
      LAB_026eba48:
        uVar19 = uVar19 + 1;
        if (available_slots + -1 <= (int)uVar19)
          goto LAB_026ec0a8;
      }
      if (uStack_21c == 0)
        goto LAB_026ebaf0;
      if (expansion == EXPANSION_SHADOWS_OVER_INNISTRAD)
      {
      LAB_026ec676:
        if (bVar6 != 0xff)
        {
          if (uStack_220 - 1 < 2)
          {
          LAB_026ebbb8:
            uStack_220 = 0x1c;
          }
          else if (uStack_220 - 3 < 2)
          {
            uStack_220 = 0x1d;
          }
        }
        goto LAB_026ebaf0;
      }
      if (EXPANSION_SHADOWS_OVER_INNISTRAD < expansion)
      {
        if (expansion == EXPANSION_INNISTRAD_MIDNIGHT_HUNT)
        {
        LAB_026ec6c2:
          if ((uStack_220 == 1) && (bVar6 != 0xff))
          {
            uStack_220 = 0x1e;
          }
        }
        else if (expansion < EXPANSION_INNISTRAD_MIDNIGHT_HUNT_COMMANDER)
        {
          if (expansion == EXPANSION_ELDRITCH_MOON)
            goto LAB_026ec676;
          if (expansion == EXPANSION_UNSTABLE)
          {
            piVar8 = cards_ptr[uVar19]->types;
            do
            {
              if (*piVar8 == 0x3000)
              {
                uStack_220 = 0x1f;
                break;
              }
              piVar8 = piVar8 + 1;
            } while (cards_ptr[uVar19]->types + 7 != piVar8);
          }
        }
        else
        {
          if (expansion == EXPANSION_INNISTRAD_CRIMSON_VOW)
            goto LAB_026ec6c2;
          if (((expansion == EXPANSION_KAMIGAWA_NEON_DYNASTY) && (bVar6 != 0xff)) &&
              (uStack_220 - 1 < 2))
            goto LAB_026ebbb8;
        }
        goto LAB_026ebaf0;
      }
      if (expansion != EXPANSION_INNISTRAD)
      {
        if (EXPANSION_INNISTRAD < expansion)
        {
          if (expansion == EXPANSION_DARK_ASCENSION)
            goto LAB_026ec6ab;
          if ((expansion == EXPANSION_VINTAGE_MASTERS) && (uStack_220 == 4))
          {
            if ((int)uVar19 < 0xaa)
            {
              if ((0xa4 < (int)uVar19) || ((uVar19 & 0xffffffef) == 1))
              {
              LAB_026ec7c9:
                uStack_220 = 6;
              }
            }
            else if (uVar19 - 0xf9 < 2)
              goto LAB_026ec7c9;
          }
          goto LAB_026ebaf0;
        }
        if (expansion != EXPANSION_LEGENDS)
        {
          if (((expansion == EXPANSION_PLANAR_CHAOS) && (uStack_220 - 1 < 3)) &&
              ((pRVar12->flags & 1) != 0))
          {
            uStack_220 = uStack_220 + 0x17;
          }
          goto LAB_026ebaf0;
        }
        if (1 < uStack_220 - 1)
          goto LAB_026ebaf0;
        uVar18 = uStack_21c;
        switch (uVar19)
        {
        case 0x22d:
        case 0x236:
        case 0x23a:
        case 0x240:
        case 0x261:
        case 0x270:
        case 0x271:
        case 0x273:
        case 0x279:
        case 0x285:
        case 0x28b:
        case 0x28f:
        case 0x29b:
        case 0x29d:
        case 0x2a0:
        case 0x2a7:
        case 0x2e0:
        case 0x2ee:
        case 0x2f5:
        case 0x301:
        case 0x311:
        case 0x31b:
        case 800:
        case 0x32d:
        case 0x337:
        case 0x344:
        case 0x347:
        case 0x348:
        case 0x34b:
          uVar18 = 0;
          break;
        default:
          if (uStack_220 == 1)
          {
            iVar14 = 0x58;
          }
          else
          {
            iVar14 = 0x60;
            uVar18 = 0;
          }
          goto LAB_026ec74e;
        case 0x299:
        case 0x2b6:
        case 0x2c9:
        case 0x2e4:
        case 0x2ff:
        case 0x30c:
        case 0x333:
        case 0x33e:
        case 0x341:;
        }
        iVar14 = (unsigned int)(uStack_220 != 1) * 8 + 0x58;
      LAB_026ec74e:
        choice_num = (int *)((int)&choices[0].num + iVar14);
        if ((*choice_num < 0) || (-1 < choices[uStack_220].num))
        {
          backtrace_and_abort(
              "functions/packs.c:2636\nchoices[munged].num >= 0 && choices[rar].num < 0");
        }
        do
        {
          uVar18 = uVar18 - 1;
          add_to_csvid_choices(choice_num, (int **)((int)&choices[0].csvids + iVar14), uVar19);
        } while (uVar18 != 0xffffffff);
        goto LAB_026eba48;
      }
    LAB_026ec6ab:
      if (bVar6 != 0xff)
      {
        uStack_220 = 0x1b;
      }
    LAB_026ebaf0:
      uVar18 = 1;
    LAB_026ebb08:
      do
      {
        uVar23 = uVar18;
        pcVar7 = choices + uVar23;
        uVar18 = uVar23 + 1;
        if (choices[uVar23].num < 0)
          goto LAB_026ebc75;
        switch (uVar23)
        {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 0x18:
        case 0x19:
        case 0x1a:
        case 0x1e:
          if (uStack_220 == uVar23)
          {
          code_r0x026ebc5e:
            add_to_csvid_choices(&pcVar7->num, (int **)&choices[uVar23].csvids, uVar19);
            break;
          }
          goto joined_r0x026ebf77;
        case 0x1b:
          if (uStack_220 == 0x1b)
          {
            uVar23 = (unsigned int)pRVar12->rarity_in_expansion[expansion];
            if (expansion == EXPANSION_INNISTRAD)
            {
              iVar14 = 0;
              if (1 < uVar23 - 3)
              {
                iVar14 = (unsigned int)(uVar23 != 2) * 9 + 2;
              }
            }
            else if (uVar23 == 4)
            {
              iVar14 = 2;
            }
            else if (uVar23 == 3)
            {
              iVar14 = 0xd;
            }
            else
            {
              iVar14 = ((uVar23 != 2) - 1 & 0xffffffac) + 0x77;
            }
            do
            {
              iVar14 = iVar14 + -1;
              add_to_csvid_choices(&pcVar7->num, (int **)&choices[0x1b].csvids, uVar19);
            } while (iVar14 != -1);
          }
          goto LAB_026ebb08;
        case 0x1c:
          if (uStack_220 == 0x1c)
          {
            if (expansion == EXPANSION_SHADOWS_OVER_INNISTRAD)
            {
              uVar23 = (pRVar12->rarity_in_expansion[expansion] != '\x01') - 1 & 0xe;
            }
            else
            {
              uVar23 = (unsigned int)(pRVar12->rarity_in_expansion[expansion] == '\x01');
              if (expansion == EXPANSION_ELDRITCH_MOON)
              {
                uVar23 = (uVar23 - 1 & 0xfffffff3) + 0xe;
              }
              else
              {
                uVar23 = uVar23 * 3;
              }
            }
            do
            {
              uVar23 = uVar23 - 1;
              add_to_csvid_choices(&pcVar7->num, (int **)&choices[0x1c].csvids, uVar19);
            } while (uVar23 != 0xffffffff);
          }
          goto LAB_026ebb08;
        case 0x1d:
          if (uStack_220 == 0x1d)
          {
            if (expansion == EXPANSION_SHADOWS_OVER_INNISTRAD)
            {
              iVar14 = (unsigned int)(pRVar12->rarity_in_expansion[expansion] == '\x03') * 5 + 1;
            }
            else
            {
              iVar14 = (unsigned int)(pRVar12->rarity_in_expansion[expansion] == '\x01') * 9 + 4;
            }
            do
            {
              iVar14 = iVar14 + -1;
              add_to_csvid_choices(&pcVar7->num, (int **)&choices[0x1d].csvids, uVar19);
            } while (iVar14 != -1);
          }
          goto LAB_026ebb08;
        case 0x1f:
          if (uStack_220 == 0x1f)
          {
            bVar6 = pRVar12->rarity_in_expansion[expansion] - 2;
            if (bVar6 < 3)
            {
              if (contraption_copies[(unsigned int)bVar6] < 1)
                goto LAB_026ebb08;
              iVar14 = contraption_copies[(unsigned int)bVar6] + -1;
            }
            else
            {
              iVar14 = 0x2a;
            }
            do
            {
              add_to_csvid_choices(&pcVar7->num, (int **)&choices[0x1f].csvids, uVar19);
              bVar5 = 0 < iVar14;
              iVar14 = iVar14 + -1;
            } while (bVar5);
          }
          goto LAB_026ebb08;
        case 0x20:
          if (uVar19 == 0x7e)
          {
            add_to_csvid_choices(&pcVar7->num, (int **)&choices[0x20].csvids, 0x7e);
          }
          goto LAB_026ebb08;
        case 0x21:
          if (uStack_220 == 5)
          {
            ppcVar22 = &choices[0x21].csvids;
            add_to_csvid_choices(&pcVar7->num, (int **)ppcVar22, uVar19);
            piVar8 = cards_ptr[uVar19]->types;
            do
            {
              if (*piVar8 == 0x2003)
                goto LAB_026ebb08;
              piVar8 = piVar8 + 1;
            } while (cards_ptr[uVar19]->types + 7 != piVar8);
            add_to_csvid_choices(&pcVar7->num, (int **)ppcVar22, uVar19);
            add_to_csvid_choices(&pcVar7->num, (int **)ppcVar22, uVar19);
          }
          goto LAB_026ebb08;
        case 0x22:
          add_to_csvid_choices(&pcVar7->num, (int **)&choices[0x22].csvids, uVar19);
          goto LAB_026ebb08;
        case 0x24:
          piVar8 = cards_ptr[uVar19]->types;
          sVar4 = *piVar8;
          while (sVar4 != 0x500c)
          {
            piVar8 = piVar8 + 1;
            if (cards_ptr[uVar19]->types + 7 == piVar8)
              goto LAB_026ebb08;
            sVar4 = *piVar8;
          }
          if (uVar19 != 0x4bac)
          {
            add_to_csvid_choices(&pcVar7->num, (int **)&choices[0x24].csvids, uVar19);
          }
          goto LAB_026ebb08;
        case 0x25:
          iVar14 = get_internal_card_id_from_csv_id(uVar19);
          if ((cards_data[iVar14].type & 6) != 0)
          {
            piVar8 = cards_ptr[uVar19]->types;
            sVar4 = *piVar8;
            while (sVar4 != 0x2001)
            {
              piVar8 = piVar8 + 1;
              if (cards_ptr[uVar19]->types + 7 == piVar8)
                goto LAB_026ebb08;
              sVar4 = *piVar8;
            }
            uVar3 = pRVar12->rarity_in_expansion[expansion];
            iVar14 = 0;
            if (uVar3 != '\x04')
            {
              if (uVar3 == '\x03')
              {
                iVar14 = 7;
              }
              else
              {
                if (uVar3 == '\x06')
                  goto LAB_026ebb08;
                iVar14 = 0x13;
              }
            }
            do
            {
              iVar14 = iVar14 + -1;
              add_to_csvid_choices(&pcVar7->num, (int **)&choices[0x25].csvids, uVar19);
            } while (iVar14 != -1);
          }
          goto LAB_026ebb08;
        case 0x26:
        case 0x27:
          if (uVar23 == 0x26)
          {
            if (uStack_220 != 3)
              goto LAB_026ebb08;
          }
          else if (uStack_220 != 4)
            goto joined_r0x026ebf77;
          piVar8 = cards_ptr[uVar19]->types;
          do
          {
            if (*piVar8 == 0x2001)
              goto LAB_026ebc75;
            piVar8 = piVar8 + 1;
          } while (cards_ptr[uVar19]->types + 7 != piVar8);
          iVar14 = get_internal_card_id_from_csv_id(uVar19);
          if ((cards_data[iVar14].type & 6) == 0)
            goto code_r0x026ebc5e;
          break;
        case 0x28:
          piVar8 = cards_ptr[uVar19]->types;
          sVar4 = *piVar8;
          while (sVar4 != 0x6003)
          {
            piVar8 = piVar8 + 1;
            if (cards_ptr[uVar19]->types + 7 == piVar8)
              goto LAB_026ebb08;
            sVar4 = *piVar8;
          }
          add_to_csvid_choices(&pcVar7->num, (int **)&choices[0x28].csvids, uVar19);
          goto LAB_026ebb08;
        }
      LAB_026ebc75:
      joined_r0x026ebf77:;
      } while (uVar18 < 0x2b);
      uVar19 = uVar19 + 1;
    } while ((int)uVar19 < available_slots + -1);
  }
LAB_026ec0a8:
  paVar2 = varying_copies;
  pcVar25 = paVar2->counts_and_cards;
  while (pcVar25 != NULL)
  {
    dVar9 = paVar2->rarity;
    if ((-1 < choices[dVar9].num) && (0 < pcVar25->count))
    {
      do
      {
        iVar14 = is_valid_card(pcVar25->csvid);
        if ((iVar14 != 0) && (0 < pcVar25->count))
        {
          iVar14 = 0;
          do
          {
            iVar14 = iVar14 + 1;
            add_to_csvid_choices(&choices[dVar9].num, (int **)&choices[dVar9].csvids,
                                      pcVar25->csvid);
          } while (iVar14 < pcVar25->count);
        }
        pcVar1 = pcVar25 + 1;
        pcVar25 = pcVar25 + 1;
      } while (0 < pcVar1->count);
    }
    paVar24 = paVar2 + 1;
    paVar2 = paVar2 + 1;
    pcVar25 = paVar24->counts_and_cards;
  }
  if ((choices[5].num == 0) &&
      ((((((expansion & ~EXPANSION_LIMITED_EDITION_ALPHA) == EXPANSION_CONFLUX ||
           (expansion == EXPANSION_WORLDWAKE)) ||
          (expansion == EXPANSION_DARK_ASCENSION)) ||
         (((expansion - EXPANSION_BORN_OF_THE_GODS & 0xfffffffd) == 0 ||
           (expansion == EXPANSION_GATECRASH)))) ||
        (((expansion - EXPANSION_OATH_OF_THE_GATEWATCH & 0xfffffffb) == 0 ||
          (expansion == EXPANSION_AETHER_REVOLT))))))
  {
    add_to_csvid_choices(&choices[5].num, (int **)&choices[5].csvids, 0xbc);
    add_to_csvid_choices(&choices[5].num, (int **)&choices[5].csvids, 0x7e);
    add_to_csvid_choices(&choices[5].num, (int **)&choices[5].csvids, 0xef);
    add_to_csvid_choices(&choices[5].num, (int **)&choices[5].csvids, 0xa4);
    add_to_csvid_choices(&choices[5].num, (int **)&choices[5].csvids, 0x5b);
  }
  bVar5 = true;
  iStack_22c = (-(unsigned int)(testing_mode == 0) & 0x1863c) + 100;
  uVar19 = expansion - EXPANSION_INNISTRAD_MIDNIGHT_HUNT & 0xfffffffd;
  do
  {
    dVar9 = prStack_20c->rarity;
    if (dVar9 == XNONE)
    {
      if (uVar19 != 0)
      {
        uStack_220 = 0;
        goto LAB_026ec3e0;
      }
    }
    else
    {
      iVar14 = 0;
      uStack_220 = 0;
      prVar21 = prStack_20c;
      do
      {
        uVar18 = prVar21->num;
        if (dVar9 == MAX_RARITY_VAL_USED)
        {
          if (prStack_20c[iVar14 + 1].num == prStack_20c[iVar14 + 2].num)
          {
            if (0 < prStack_20c[iVar14 + 1].num)
            {
              iVar10 = 0;
              do
              {
                iVar11 = internal_rand(uVar18 >> 0x10);
                dVar9 = prStack_20c[((int)(uVar18 & 0xffff) <= iVar11) + 1 + iVar14].rarity;
                iVar11 = add_card_to_pack(out_csvids, uStack_220, &choices[dVar9].num,
                                               (int **)&choices[dVar9].csvids, 1, dVar9, expansion,
                                               booster, testing_mode);
                uStack_220 = (uStack_220 + 1) - (unsigned int)(iVar11 == 0);
                iVar10 = iVar10 + 1;
              } while (iVar10 < prStack_20c[iVar14 + 1].num);
            }
          }
          else
          {
            iVar10 = internal_rand(uVar18 >> 0x10);
            prVar21 = prStack_20c + ((int)(uVar18 & 0xffff) <= iVar10) + 1 + iVar14;
            dVar9 = prVar21->rarity;
            iVar10 = add_card_to_pack(out_csvids, uStack_220, &choices[dVar9].num,
                                           (int **)&choices[dVar9].csvids, prVar21->num, dVar9,
                                           expansion, booster, testing_mode);
            if (iVar10 != 0)
            {
              uStack_220 = uStack_220 + prVar21->num;
            }
          }
          iVar14 = iVar14 + 2;
        }
        else
        {
          iVar10 = add_card_to_pack(out_csvids, uStack_220, &choices[dVar9].num,
                                         (int **)&choices[dVar9].csvids, uVar18, dVar9, expansion,
                                         booster, testing_mode);
          if (iVar10 != 0)
          {
            uStack_220 = uStack_220 + prVar21->num;
          }
        }
        iVar14 = iVar14 + 1;
        prVar21 = prStack_20c + iVar14;
        dVar9 = prVar21->rarity;
      } while (dVar9 != XNONE);
      iVar14 = uStack_220 + -1;
      if (0 < iVar14)
      {
        iVar10 = 0;
      LAB_026ec3b1:
        do
        {
          iVar11 = out_csvids[iVar10];
          iVar10 = iVar10 + 1;
          if (iVar11 != 0xef)
          {
            if (0xef < iVar11)
            {
              if ((4 < iVar11 - 0x834U) || (expansion != EXPANSION_ICE_AGE))
                goto LAB_026ec438;
              if (iVar14 <= iVar10)
                goto LAB_026ec3e0;
              goto LAB_026ec3b1;
            }
            if (iVar11 != 0xa4)
            {
              if (iVar11 < 0xa5)
              {
                if ((iVar11 != 0x5b) && (iVar11 != 0x7e))
                  goto LAB_026ec438;
              }
              else if (iVar11 != 0xbc)
              {
              LAB_026ec438:
                iVar15 = iVar10;
                if (iVar10 < (int)uStack_220)
                {
                  do
                  {
                    if (iVar11 == out_csvids[iVar15])
                    {
                      if (testing_mode != 0)
                      {
                        pFVar13 = stderr;
                        fprintf(pFVar13,
                                "Expansion #%d, booster=%d: found a duplicate card late in "
                                "make_pack_normal()\012",
                                expansion, booster);
                        bVar5 = false;
                      }
                      goto LAB_026ec455;
                    }
                    iVar15 = iVar15 + 1;
                  } while (uStack_220 != iVar15);
                }
              }
            }
          }
        } while (iVar10 < iVar14);
      }
      if (uVar19 != 0)
        goto LAB_026ec3e0;
      if (0 < (int)uStack_220)
      {
        iVar14 = 0;
        pcVar17 = out_csvids;
        do
        {
          pRVar12 = get_rarity(*pcVar17);
          if (pRVar12->dfc_symbol != 0xff)
          {
            iVar14 =
                iVar14 +
                (unsigned int)((unsigned char)(pRVar12->rarity_in_expansion[expansion] - 2) < 3);
          }
          pcVar17 = pcVar17 + 1;
        } while (out_csvids + uStack_220 != pcVar17);
        if (iVar14 == 1)
          goto LAB_026ec3e0;
      }
    }
  LAB_026ec455:
    iStack_22c = iStack_22c + -1;
  } while (iStack_22c != 0);
  uStack_220 = 0;
LAB_026ec3e0:
  for (iVar14 = 0; iVar14 < 44; ++iVar14)
  {
    if (choices[iVar14].csvids != NULL)
      free(choices[iVar14].csvids);
  }
  if ((testing_mode != 0) && (!bVar5))
  {
    uStack_220 = 0;
  }
  return uStack_220;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26ec9b0; original functions/packs.c:3292. */
int make_pack(EXPANSION_t expansion, int booster, int *out_csvids)
{
  int iVar1;
  int iVar2;
  int *out_csvids_00;
  FILE *pFVar3;

  iVar1 = is_valid_pack(expansion, booster);
  iVar2 = 0;
  if (iVar1 != 0)
  {
    if (out_csvids != NULL)
    {
      iVar2 = make_pack_normal(expansion, booster, out_csvids, 0);
      return iVar2;
    }
    iVar2 = pack_size(expansion, booster);
    out_csvids_00 = malloc(iVar2 * 4);
    if (out_csvids_00 == NULL)
    {
      fatal("could not malloc %d bytes to test out_csvids", iVar2 * 4);
    }
    iVar2 = make_pack_normal(expansion, booster, out_csvids_00, 1);
    free(out_csvids_00);
    if (iVar2 == 0)
    {
      pFVar3 = stderr;
      fprintf(pFVar3, "Expansion #%d, booster=%d: could not create valid pack.\012", expansion,
              booster);
    }
  }
  return iVar2;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26eca90; original functions/packs.c:3339. */
int test_packs(int run_time_test)
{
  static int once = 0;
  EXPANSION_t eVar1;
  float fVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  FILE *pFVar6;
  int iVar7;
  EXPANSION_t expansion;
  int iStack_20;
  int iStack_18;
  int iStack_14;

  if (once == 0)
  {
    once = 1;
    iStack_18 = 0;
    if (0 < run_time_test)
    {
      iStack_18 = time_in_ms();
    }
    iVar5 = 0;
    expansion = EXPANSION_PROMO;
    iStack_14 = 1;
    do
    {
      eVar1 = pack_contents[expansion].expansion;
      if (eVar1 == expansion)
      {
        iVar7 = 0;
        iVar3 = pack_size(expansion, 0);
        while (true)
        {
          if (iVar3 != 0)
          {
            iStack_20 = run_time_test;
            iVar3 = run_time_test;
            if (run_time_test < 1)
            {
              iStack_20 = 1;
              iVar3 = iStack_20;
            }
            do
            {
              iVar4 = make_pack(expansion, iVar7, NULL);
              if (iVar4 == 0)
              {
                iStack_14 = 0;
              }
              iStack_20 = iStack_20 + -1;
            } while (iStack_20 != 0);
            iVar5 = iVar5 + iVar3;
          }
          iVar7 = iVar7 + 1;
          if (iVar7 == 2)
            break;
          iVar3 = pack_size(expansion, iVar7);
        }
      }
      else
      {
        pFVar6 = stderr;
        fprintf(pFVar6, "pack_contents[%d].expansion == %d\012", expansion, eVar1);
      }
      expansion = expansion + EXPANSION_PROMO;
    } while (expansion != (EXPANSION_STREETS_OF_NEW_CAPENNA | EXPANSION_LIMITED_EDITION_ALPHA));
    if (0 < run_time_test)
    {
      iVar7 = time_in_ms();
      fVar2 = (float)(int32_t)((uint32_t)iVar7 - (uint32_t)iStack_18) / 1000.0;
      pFVar6 = stderr;
      fprintf(pFVar6, "Made %d packs in %f seconds (%f seconds each).\012", iVar5, (double)fVar2,
              (double)(fVar2 / (float)iVar5));
    }
    pFVar6 = stderr;
    fflush(pFVar6);
    return iStack_14;
  }
  return 1;
}

#endif


#ifndef SHANDALAR
#include <windows.h>
typedef struct pack_selection_t { EXPANSION_t expansion; int booster; } pack_selection_t;
typedef struct choose_a_pack_args_t {
  int num_packs, allow_boosters, allow_starters, allow_cancel;
  pack_selection_t results[6];
} choose_a_pack_args_t;
#define pack_path_duel_art EXE_STR(0x739e30)
#define pack_str_ok EXE_STR(0x78f0a8)
#define pack_str_cancel EXE_STR(0x7281a4)
#define pack_main_hinstance EXE_TYP(HINSTANCE, 0x78f0c8)
#define pack_hwnd_MainClass EXE_TYP(HWND, 0x739d54)
#define pack_dblbfr_hdc EXE_TYP(HDC, 0x62047c)
#define pack_critical_section_for_painting EXE_TYP_PTR(CRITICAL_SECTION, 0x790500)
#define pack_read_graphics_file EXE_FN(HGDIOBJ, 0x459fa0, const char *)
#define pack_set_palette_and_stretch_mode EXE_FN(void, 0x466050, HDC)
#define pack_blt_pic EXE_FN(void, 0x4662f0, HDC, RECT *, HGDIOBJ)
#define pack_handle_palette_msgs EXE_FN(INT_PTR, 0x467fa0, HWND, UINT, WPARAM, LPARAM)
#define pack_draw_button EXE_FN(void, 0x4673c0, DRAWITEMSTRUCT *, HBRUSH, HPEN, HPEN, COLORREF, int)
__attribute__((dllimport)) void blt_expansion_symbol(HDC hdc, const RECT *rect, int expansion, int rarity, int force_color);

static unsigned char choose_a_pack_template[294] __attribute__((aligned(4))) = {
  0x40, 0x08, 0x80, 0x90, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0xba, 0x00,
  0x74, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x4d, 0x00, 0x53, 0x00, 0x20, 0x00,
  0x53, 0x00, 0x61, 0x00, 0x6e, 0x00, 0x73, 0x00, 0x20, 0x00, 0x53, 0x00, 0x65, 0x00, 0x72, 0x00,
  0x69, 0x00, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x13, 0x02, 0x23, 0x50, 0x00, 0x00, 0x00, 0x00,
  0x07, 0x00, 0x05, 0x00, 0xac, 0x00, 0x48, 0x00, 0x4d, 0x04, 0xff, 0xff, 0x85, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x13, 0x02, 0x23, 0x50, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x14, 0x00,
  0xac, 0x00, 0x48, 0x00, 0x4e, 0x04, 0xff, 0xff, 0x85, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x13, 0x02, 0x23, 0x50, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x23, 0x00, 0xac, 0x00, 0x48, 0x00,
  0x4f, 0x04, 0xff, 0xff, 0x85, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x13, 0x02, 0x23, 0x50,
  0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x32, 0x00, 0xac, 0x00, 0x48, 0x00, 0x50, 0x04, 0xff, 0xff,
  0x85, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x13, 0x02, 0x23, 0x50, 0x00, 0x00, 0x00, 0x00,
  0x07, 0x00, 0x41, 0x00, 0xac, 0x00, 0x48, 0x00, 0x51, 0x04, 0xff, 0xff, 0x85, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x13, 0x02, 0x23, 0x50, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x50, 0x00,
  0xac, 0x00, 0x48, 0x00, 0x52, 0x04, 0xff, 0xff, 0x85, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x0b, 0x00, 0x01, 0x50, 0x00, 0x00, 0x00, 0x00, 0x26, 0x00, 0x5f, 0x00, 0x32, 0x00, 0x0e, 0x00,
  0x01, 0x00, 0xff, 0xff, 0x80, 0x00, 0x4f, 0x00, 0x4b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x0b, 0x00, 0x01, 0x50, 0x00, 0x00, 0x00, 0x00, 0x63, 0x00, 0x5f, 0x00, 0x32, 0x00, 0x0e, 0x00,
  0x02, 0x00, 0xff, 0xff, 0x80, 0x00, 0x43, 0x00, 0x61, 0x00, 0x6e, 0x00, 0x63, 0x00, 0x65, 0x00,
  0x6c, 0x00, 0x00, 0x00, 0x00, 0x00,
};
#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26ecc50; original functions/packs.c:3506. */
static INT_PTR __stdcall dlgproc_choose_a_pack(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  static HGDIOBJ winbk_questn = NULL;
  static const char **expansion_names = NULL;
  static HBRUSH brush = NULL;
  static HPEN pen1 = NULL;
  static HPEN pen2 = NULL;
  static choose_a_pack_args_t *args = NULL;
  _Static_assert(sizeof(choose_a_pack_args_t) == 64 &&
                     offsetof(choose_a_pack_args_t, results) == 16,
                 "native six-pack argument record");
  _Static_assert(sizeof(DRAWITEMSTRUCT) == 48 && offsetof(DRAWITEMSTRUCT, hDC) == 24 &&
                     offsetof(DRAWITEMSTRUCT, rcItem) == 28 &&
                     offsetof(DRAWITEMSTRUCT, itemData) == 44,
                 "native owner-draw record");
  _Static_assert(sizeof(TEXTMETRICA) == 56 && sizeof(RECT) == 16, "native drawing scratch records");
  if (msg == WM_COMMAND)
  {
    unsigned command = (uint16_t)wparam;
    if (command != 1 && (command != 2 || args->allow_cancel == 0))
      return 0;
    for (int i = 0; i < 6; ++i)
    {
      args->results[i].expansion = EXPANSION_0;
      args->results[i].booster = 0;
    }
    for (int i = 0; i < args->num_packs; ++i)
    {
      if (command == 1)
      {
        LRESULT selection = SendDlgItemMessageA(hwnd, i + 0x44d, 0x147, 0, 0);
        if (selection != -1)
        {
          LRESULT data = SendDlgItemMessageA(hwnd, i + 0x44d, 0x150, selection, 0);
          if (data != -1)
          {
            args->results[i].expansion = (uint32_t)data & 0xffff;
            args->results[i].booster = ((uint32_t)data & 0x10000) == 0;
          }
        }
      }
    }
    if (winbk_questn != NULL)
    {
      DeleteObject(winbk_questn);
      winbk_questn = NULL;
    }
    if (brush != NULL)
    {
      DeleteObject(brush);
      brush = NULL;
    }
    if (pen1 != NULL)
    {
      DeleteObject(pen1);
      pen1 = NULL;
    }
    if (pen2 != NULL)
    {
      DeleteObject(pen2);
      pen2 = NULL;
    }
    EndDialog(hwnd, command == 1);
    return 0;
  }
  if (msg == WM_INITDIALOG)
  {
    char path[279];
    sprintf(path, "%s\\WINBK_QuestN.pic", pack_path_duel_art);
    winbk_questn = pack_read_graphics_file(path);
    brush = CreateSolidBrush(0x100001c);
    pen1 = CreatePen(0, 0, 0x1000072);
    pen2 = CreatePen(0, 0, 0x10000ca);
    SetDlgItemTextA(hwnd, 1, pack_str_ok);
    SetDlgItemTextA(hwnd, 2, pack_str_cancel);
    args = (choose_a_pack_args_t *)lparam;
    if (args->num_packs > 6)
      args->num_packs = 6;
    else if (args->num_packs < 1)
      args->num_packs = 1;
    if (args->allow_cancel == 0)
      ShowWindow(GetDlgItem(hwnd, 2), 0);
    for (int i = args->num_packs; i < 6; ++i)
      ShowWindow(GetDlgItem(hwnd, i + 0x44d), 0);
    int num_textlines;
    expansion_names = load_text_nocopy_from_file("Menus.txt", "EXPANSIONNAMES", &num_textlines);
    for (int expansion = 1; expansion < 198 && num_textlines > expansion + 6; ++expansion)
    {
      char buf[300];
      if (args->allow_boosters != 0 && is_valid_pack(expansion, 1))
      {
        int size = pack_size(expansion, 1);
        const char *prefix = "14-";
        if ((((uint32_t)expansion - 103) & 0xfffffffd) != 0 &&
            ((uint32_t)expansion & 0xfffffffd) != 145)
          prefix = "";
        sprintf(buf, "%s (%s%d cards)", expansion_names[expansion + 7], prefix, size);
        for (int i = 0; i < args->num_packs; ++i)
        {
          LRESULT result = SendDlgItemMessageA(hwnd, i + 0x44d, 0x143, 0, (LPARAM)buf);
          if ((uint32_t)result + 2 > 1)
            SendDlgItemMessageA(hwnd, i + 0x44d, 0x151, result, expansion);
        }
      }
      if (args->allow_starters != 0 && is_valid_pack(expansion, 0))
      {
        int size = pack_size(expansion, 0);
        sprintf(buf, "%s %s deck (%d cards)", expansion_names[expansion + 7],
                expansion < 30 ? "starter" : "tournament", size);
        for (int i = 0; i < args->num_packs; ++i)
        {
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
  if (msg == WM_DRAWITEM)
  {
    DRAWITEMSTRUCT *dis = (DRAWITEMSTRUCT *)lparam;
    if (wparam < 3)
    {
      if (wparam == 0)
        return 0;
      COLORREF color = ((uint32_t)(dis->hwndItem == GetFocus()) - 1U & 0xffffffdbU) + 0x10000bfU;
      pack_draw_button(dis, brush, pen1, pen2, color, 0);
      return 1;
    }
    if ((uint32_t)wparam - 0x44d > 5 || dis->itemID == (UINT)-1)
      return 0;
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
    ExtTextOutA(dis->hDC, width + 4, (bottom + top - tm.tmHeight) / 2, 6, &dis->rcItem, buf,
                strlen(buf), NULL);
    SetTextColor(dis->hDC, old_fg);
    SetBkColor(dis->hDC, old_bk);
    RECT rect;
    rect.left = dis->rcItem.left;
    rect.top = dis->rcItem.top;
    rect.right = rect.left + width;
    rect.bottom = dis->rcItem.bottom;
    FillRect(dis->hDC, &rect, GetSysColorBrush(5));
    blt_expansion_symbol(dis->hDC, &rect, expansion, (dis->itemState & 0x11) == 0 ? 1 : 4, -1);
    if ((dis->itemState & 0x11) != 0)
      DrawFocusRect(dis->hDC, &dis->rcItem);
    return 1;
  }
  if (msg == WM_ERASEBKGND && winbk_questn != NULL)
  {
    RECT rect;
    pack_set_palette_and_stretch_mode((HDC)wparam);
    GetClientRect(hwnd, &rect);
    EnterCriticalSection(pack_critical_section_for_painting);
    int savedc = SaveDC(pack_dblbfr_hdc);
    if (winbk_questn == NULL)
      FillRect(pack_dblbfr_hdc, &rect, GetStockObject(2));
    else
      pack_blt_pic(pack_dblbfr_hdc, &rect, winbk_questn);
    RestoreDC(pack_dblbfr_hdc, savedc);
    GetClientRect(hwnd, &rect);
    BitBlt((HDC)wparam, 0, 0, rect.right, rect.bottom, pack_dblbfr_hdc, 0, 0, 0xcc0020);
    LeaveCriticalSection(pack_critical_section_for_painting);
    return 1;
  }
  if (msg == WM_LBUTTONDOWN)
  {
    SendMessageA(hwnd, 0x112, 0xf012, 0);
    return 0;
  }
  if ((uint32_t)msg - 0x30f < 3)
    return pack_handle_palette_msgs(hwnd, msg, wparam, lparam);
  return 0;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26ed780; original functions/packs.c:3449. */
LPCDLGTEMPLATE generate_choose_a_pack_dlg_template(void)
{
  return (LPCDLGTEMPLATE)choose_a_pack_template;
}

#endif


#ifndef SHANDALAR
/* ManalinkEh 2022: 0x26ed790; original functions/packs.c:3737. */
int choose_packs(int num_packs, pack_flags_t flags, EXPANSION_t *expansions, int *is_booster)
{
  static int idx = -1;
  choose_a_pack_args_t args;
  if (num_packs > 6)
    num_packs = 6;
  else if (num_packs < 1)
    num_packs = 1;
  idx = (int32_t)((uint32_t)idx + 1U);
  args.num_packs = num_packs;
  args.allow_boosters = flags & PACK_ALLOW_BOOSTER;
  args.allow_starters = flags & PACK_ALLOW_STARTER;
  args.allow_cancel = flags & PACK_ALLOW_CANCEL;
  for (int i = 0; i < 6; ++i)
  {
    args.results[i].expansion = EXPANSION_0;
    args.results[i].booster = 0;
  }
  INT_PTR result =
      DialogBoxIndirectParamA(pack_main_hinstance, generate_choose_a_pack_dlg_template(),
                              pack_hwnd_MainClass, dlgproc_choose_a_pack, (LPARAM)&args);
  if (result != 0)
  {
    for (int i = 0; i < num_packs; ++i)
    {
      expansions[i] = args.results[i].expansion;
      if (is_booster != NULL)
        is_booster[i] = args.results[i].booster;
    }
    return 1;
  }
  return 0;
}

#endif
