#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../defs.h"

// Highest usable number in expansions.h is 8 less than this, because of the 8 format expansions
#define EXPANSIONS	179
#define MAXCARDS	30000

int linenum = 0;
const char* line = NULL;

void fatal(const char* fmt, ...) __attribute__((format(printf, 1, 2)));
void fatalerr(const char* fmt, ...) __attribute__((format(printf, 1, 2)));

void
fatal(const char* fmt, ...)
{
  char buf[8000];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buf, 8000, fmt, args);
  va_end(args);

  fprintf(stderr, "%s\n", buf);
  fprintf(stderr, "At line %d%s\n", linenum, line ? ":" : "");
  if (line)
    fprintf(stderr, "%s\n", line);
  exit(1);
}

void
fatalerr(const char* fmt, ...)
{
  int e = errno;
  char buf[8000];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buf, 8000, fmt, args);
  va_end(args);

  fprintf(stderr, "%s: %s\n", buf, strerror(e));
  if (linenum >= 0)
    fprintf(stderr, "At line %d%s\n", linenum, line ? ":" : "");
  if (line)
    fprintf(stderr, "%s\n", line);
  exit(1);
}

const char no_accents[] = ("E_,f\".||^%S<E_Z_"
			   "_''\"\".---_s>e_zY"
			   "_!cL_Y|_\"_a<--_-"
			   "_+23'u_.,1o>___?"
			   "AAAAAAECEEEEIIII"
			   "DNOOOOOxOUUUUY_S"
			   "aaaaaaeceeeeiiii"
			   "dnooooo:ouuuuy_y");

char rarities[256];
void
setup_rarities(void)
{
  int i;
  for (i = 0; i < 256; ++i)
    rarities[i] = -1;
  rarities[(int)('-')] = 0;
  rarities[(int)('C')] = 1;
  rarities[(int)('U')] = 2;
  rarities[(int)('R')] = 3;
  rarities[(int)('M')] = 4;
  rarities[(int)('L')] = 5;
  rarities[(int)('S')] = 6;
}

void
my_fseek(FILE* f, const char* filename, long offset, int whence)
{
  if (fseek(f, offset, whence))
    fatalerr("Couldn't seek in %s", filename);
}

void
my_fclose(FILE* f, const char* filename)
{
  if (fclose(f))
    fatalerr("Couldn't close %s", filename);
}

void
my_fgets(char* buf, int sz, FILE* f, const char* filename)
{
  ++linenum;
  if (!fgets(buf, sz, f))
    {
      if (feof(f))
	buf[0] = 0;
      else
	fatalerr("Couldn't read from %s", filename);
    }
}

void
fput_str(FILE* f, const char* filename, const char* str)
{
  if (fputs(str, f) == EOF)
    fatalerr("Couldn't write to %s", filename);
}

void
fput_data(FILE* f, const char* filename, void* buf, size_t size, size_t count)
{
  if (fwrite(buf, size, count, f) != count)
    fatalerr("Couldn't write to %s", filename);
}

void
fput_dword(FILE* f, const char* filename, int dword)
{
  fput_data(f, filename, &dword, 4, 1);
}

int
grabnumber(const char** p)
{
  const char* q = *p;
  int neg;
  if (*q == '-')
    {
      neg = -1;
      ++q;
    }
  else
    neg = 1;

  int tot = 0;
  while (*q >= '0' && *q <= '9')
    tot = (10 * tot) + (*q++ - '0');

  if (*q == ';')
    ++q;
  else if (*q != '\n')
    fatal("Expected number, got \"%s\"", *p);

  *p = q;
  return neg * tot;
}

int
grabhexnumber(const char** p)
{
  const char* q = *p;
  if (*q == ';')
    {
      *p = q + 1;
      return 0;
    }

  if (*q == '-' && q[1] == '1')
    {
      if (q[2] == ';')
	q += 3;
      else if (q[2] == '\n')
	q += 2;
      else
	fatal("Expected hex number, got \"%s\"", *p);

      *p = q;
      return -1;
    }

  if (*q == '0')
    {
      if (q[1] == ';')
	{
	  q += 2;
	  *p = q;
	  return 0;
	}
      else if (q[1] == '\n')
	{
	  q += 1;
	  *p = q;
	  return 0;
	}
    }

  int tot = 0;
  while (1)
    {
      char c = *q;
      if (c >= '0' && c <= '9')
	c -= '0';
      else if (c >= 'A' && c <= 'F')
	c -= 'A' - 10;
      else if (c >= 'a' && c <= 'f')
	c -= 'a' - 10;
      else if (c == 'h')
	{
	  if (q[1] == ';')
	    q += 2;
	  else if (q[1] == '\n')
	    q += 1;
	  else
	    fatal("Expected hex number, got \"%s\"", *p);

	  break;
	}
      else
	fatal("Expected hex number, got \"%s\"", *p);

      tot = (16 * tot) + c;
      ++q;
    }

  *p = q;
  return tot;
}

int
grabtriads(const char** p, int num, const char* name)
{
  int i, tot = 0;
  for (i = 1; i <= num; ++i)
    {
      int triad = grabnumber(p);
      if (triad < 0 || triad > 7)
	fatal("Expected 0..7 for %s#%d, got %d", name, i, triad);
      tot = (tot << 3) | triad;
    }
  return tot;
}

int
grabtaystes(const char** p, int num, const char* name)
{
  int i, tot = 0;
  for (i = 1; i <= num; ++i)
    {
      int tayste = grabnumber(p);
      if (tayste < 0 || tayste > 3)
	fatal("Expected 0..3 for %s#%d, got %d", name, i, tayste);
      tot = (tot | tayste) << 2;	// Can't possibly be right, but that's the assembly version does
    }
  return tot;
}

const char*
grabstring(const char** p, char* r)
{
  const char* q = *p;
  if (*q == '"')
    {
      ++q;
      while (1)
	switch (*r++ = *q++)
	  {
	    case '"':
	      if (*q == '"')
		++q;
	      else if (*q == ';')
		{
		  ++q;
		  --r;
		  goto epilogue;
		}
	      else if (*q == '\n')
		goto epilogue;
	      break;

	    case '\\':
	      if (*q == '\\' || *q == 'n')
		{
		  ++q;
		  r[-1] = '\n';	// Within quotes, both \\ and \n become newlines
		}
	      break;

	    case 0:
	      fatal("Expected quoted string, got \"%s\"", *p);
	  }
    }
  else
    {
      while (1)
	switch (*r++ = *q++)
	  {
	    case ';':
	    case '\n':
	      --r;
	      goto epilogue;
	    case 0:
	      goto epilogue;

	    case '\\':
	      if (*q == 'n')
		{
		  ++q;
		  r[-1] = '\n';	// Outside quotes, just \n does
		}
	      break;
	  }
    }

 epilogue:
  *p = q;
  *r = 0;
  return r;
}

char* txt;
void
setup_txt(void)
{
  txt = (char*)malloc(16 * 1024 * 1024);
  if (!txt)
    fatal("Couldn't allocate text buffer");
  txt[0] = 0;
}

int curr_txt = 0;

char*
copy_string(const char** p)
{
  int pos = curr_txt;
  const char* end = grabstring(p, &txt[pos]);
  curr_txt = end - txt + 1;
  return (char*)pos;
}

char*
copy_string_with_str(const char** p, const char** str_out)
{
  int pos = curr_txt;
  const char* end = grabstring(p, &txt[pos]);
  if (str_out)
    *str_out = &txt[pos];
  curr_txt = end - txt + 1;
  return (char*)pos;
}

char coded_table[MAXCARDS] = {0};
card_ptr_t bah[MAXCARDS] = {{0}};
char* bah3[MAXCARDS][5] = {{0}};

int
main(int argc, char** argv)
{
  setup_rarities();
  setup_txt();

#define CR "\r\n"

  char linebuf[8192];
  linebuf[0] = 0;
  line = linebuf;
  FILE* mlcsv = fopen("Manalink.csv", "rb");
  if (!mlcsv)
    fatalerr("Couldn't open Manalink.csv");
#define MLCSV mlcsv, "Manalink.csv"
  my_fgets(linebuf, 8192, MLCSV);	// skip the columns line

  FILE* cardid = fopen("card_id.h", "wb");
  if (!cardid)
    fatalerr("Couldn't open card_id.h");
#define CARDID cardid, "card_id.h"
  fput_str(CARDID, "#ifndef CARD_ID_H" CR "#define CARD_ID_H" CR CR "typedef enum" CR "{" CR);

  FILE* dbinfo = fopen("DBinfo.dat", "wb");
  if (!dbinfo)
    fatalerr("Couldn't open DBinfo.dat");
#define DBINFO dbinfo, "DBinfo.dat"
  fput_dword(DBINFO, 0);

  FILE* raritydat = fopen("Rarity.dat", "wb");
  if (!raritydat)
    fatalerr("Couldn't open Rarity.dat");
#define RARITYDAT raritydat, "Rarity.dat"
  fput_dword(RARITYDAT, 0);
  fput_dword(RARITYDAT, EXPANSIONS);

  int csvid = 0;

  char rarityline[(((EXPANSIONS * 3) + 7) / 8) + 5] = {0};

  while (my_fgets(linebuf, 8192, MLCSV), linebuf[0] && linebuf[0] != '\n' && linebuf[0] != '#')
    {
      const char* l = linebuf;
      const char** p = &l;
      if (grabnumber(p) != csvid)
	fatal("csvid mismatch: expected %d", csvid);

      card_ptr_t* cp = &bah[csvid];

      const char* str_out;
      cp->full_name = copy_string_with_str(p, &str_out);

      if (strcmp(str_out, "None") && strcmp(str_out, "Empty"))
	{
	  char cid_line[160];
	  char* c = cid_line + sprintf(cid_line, "  CARD_ID_");
	  const char* s = str_out;
	  unsigned char ch;
	  while ((ch = *s++))
	    {
	      if (ch >= 0x80)
		ch = no_accents[ch - 0x80];

	      if (ch == '\'')
		continue;

	      if (ch >= 'a' && ch <= 'z')
		ch += 'A' - 'a';
	      else if (!(ch >= 'A' && ch <= 'Z')
		       && !(ch >= '0' && ch <= '9'))
		{
		  ch = '_';
		  if (c[-1] == '_')
		    continue;
		}
	      *c++ = ch;
	    }
	  *c = 0;

	  int any_underscores_reaped = 0;
	  while (c[-1] == '_')
	    {
	      *--c = 0;
	      any_underscores_reaped = 1;
	    }
	  /* Special case for UST cards Monkey- and Rhino-, which would otherwise conflict with the Monkey and Rhino
	   * tokens - if the last character in the unaltered name is -, then retain the trailing underscore. */
	  if (any_underscores_reaped
	      && str_out[strlen(str_out) - 1] == '-')
	    *c++ = '_';

	  while (c - cid_line < 40)
	    *c++ = ' ';

	  sprintf(c, "=%5d," CR, csvid);
	  fput_str(CARDID, cid_line);
	}

      int coded = grabnumber(p);
      if (coded < 0 || coded > 1)
	fatal("Bad value for CODED CARD: %d", coded);
      coded_table[csvid] = coded;

      cp->id = csvid;
      cp->name = copy_string(p);
      cp->mana_cost_text = copy_string(p);
      cp->expansion = grabhexnumber(p);
      cp->color = grabnumber(p);
      cp->card_type = grabnumber(p);
      cp->rarity = grabnumber(p);
      cp->req_colorless = grabnumber(p);
      cp->req_black = grabnumber(p);
      cp->req_blue = grabnumber(p);
      cp->req_green = grabnumber(p);
      cp->req_red = grabnumber(p);
      cp->req_white = grabnumber(p);
      cp->num_pics = grabnumber(p);
      cp->expansion_rarity = grabhexnumber(p);
      cp->power = grabnumber(p);
      cp->toughness = grabnumber(p);
      cp->sleight_color = grabnumber(p);
      cp->sleight_color = 0;	/* bug-for-bug compatibility with csv2dat.exe - it's overwritten later in the
				 * ai_dependencies assignment */
      cp->hack_colors = grabnumber(p);
      cp->currently_zero_for_all_cards = 0;
      cp->enchant_type = 0;
      cp->hack_mode = grabnumber(p);

      int enchant_type_own = grabnumber(p);
      int enchant_type_opp = grabnumber(p);
      cp->enchant_type = enchant_type_own | (enchant_type_opp << 4);

      int i;
      for (i = 0; i < 7; ++i)
	cp->types[i] = grabhexnumber(p);

      cp->subtype1 = grabnumber(p);
      cp->subtype2 = grabnumber(p);
      cp->mana_source_colors = grabnumber(p);
      cp->type_text = copy_string(p);
      cp->rules_text = copy_string(p);
      cp->flavor_text = copy_string(p);

      for (i = 0; i < 5; ++i)	// Damage Text, Effect Title Text, Effect Text, Legacy Title Text, Legacy Text
	bah3[csvid][i] = copy_string(p);

      cp->ai_modifiers1 = grabtriads(p, 10, "ai_modifiers1");
      cp->ai_modifiers2 = grabtriads(p, 10, "ai_modifiers2");
      cp->ai_inc_power = grabnumber(p);
      cp->ai_inc_toughness = grabnumber(p);
      cp->ai_power = grabnumber(p);
      cp->ai_toughness = grabnumber(p);
      cp->ai_base_value = grabnumber(p);
      cp->ai_dependencies = grabtriads(p, 3, "ai_dependencies");
      cp->ai_against_color = grabtaystes(p, 5, "ai_against_color");
      cp->ai_for_color = grabtaystes(p, 5, "ai_for_color");
      cp->ai_counts_as_color = grabtaystes(p, 5, "ai_counts_as_color");
      cp->ai_against_land = grabtaystes(p, 5, "ai_against_land");
      cp->ai_for_land = grabtaystes(p, 5, "ai_for_land");
      cp->ai_counts_as_land = grabtaystes(p, 5, "ai_counts_as_land");
      cp->ai_abilities = grabtriads(p, 6, "ai_abilities");
      cp->inflatable = grabnumber(p);

      char dbline[16];
      int n = grabnumber(p);	// DB Card Type 2
      memcpy(dbline, &n, 4);
      n = grabnumber(p);	// Num of DB Abilities
      memcpy(&dbline[4], &n, 4);
      for (i = 0; i < 8; ++i)
	dbline[8 + i] = grabnumber(p);	// DBA1..DBA8
      fput_data(DBINFO, dbline, 1, 16);

      int bits = 0;
      int offset = 0;

      char* r = rarityline;
      for (i = EXPANSIONS; i > 0; --i)
	{
	  int rarity = rarities[(int)(*l++)];
	  if (rarity < 0)
	    fatal("Unknown rarity \"%c\"", l[-1]);
	  rarity <<= offset;
	  bits |= rarity;
	  offset += 3;
	  if (offset >= 8)
	    {
	      *r++ = (unsigned char)(bits & 0xff);
	      bits >>= 8;
	      offset -= 8;
	    }
	  if (i != 1)
	    {
	      char ch = *l++;
	      if (ch != ';')
		fatal("Expected ';' after rarity, got '%c'", ch);
	    }
	}
      *r++ = (unsigned char)(bits & 0xff);
      if (*l && *l != '\n' && *l != '\r')
	fatal("Excess characters after last rarity: \"%s\"", l);

      int proper_len = ((EXPANSIONS * 3) + 7) / 8;
      while (r - rarityline < proper_len)
	*r++ = 0;
      if (r - rarityline > proper_len)
	fatal("rarityline should be %d bytes, was %d", r - rarityline, proper_len);

      fput_data(RARITYDAT, rarityline, 1, proper_len);

      ++csvid;
    }

  int lenn = csvid;

  my_fseek(DBINFO, 0, SEEK_SET);
  fput_dword(DBINFO, lenn);
  my_fclose(DBINFO);

  my_fseek(RARITYDAT, 0, SEEK_SET);
  fput_dword(RARITYDAT, lenn);
  my_fclose(RARITYDAT);

  csvid = 0;
  char coded_table2[(MAXCARDS + 7) / 8];
  int coded_table2_len = (lenn + 7) / 8;
  char* ct2 = coded_table2;
  int bytes;
  for (bytes = coded_table2_len; bytes > 0; --bytes)
    {
      csvid += 8;
      int bits = 0;
      int i;
      for (i = 0; i < 8; ++i)
	{
	  bits <<= 1;
	  bits |= coded_table[--csvid];
	}

      *ct2++ = bits;
      csvid += 8;
    }

  FILE* cardsdat = fopen("Cards.dat", "wb");
  if (!cardsdat)
    fatalerr("Couldn't open Cards.dat");
#define CARDSDAT cardsdat, "Cards.dat"

  fput_dword(CARDSDAT, lenn);
  fput_dword(CARDSDAT, curr_txt);

  fput_data(CARDSDAT, bah, sizeof(card_ptr_t), lenn);
  fput_data(CARDSDAT, txt, 1, curr_txt);

  fput_dword(CARDSDAT, coded_table2_len);
  fput_data(CARDSDAT, coded_table2, 1, coded_table2_len);

  fput_data(CARDSDAT, bah3, 5 * sizeof(int), lenn);
  my_fclose(CARDSDAT);

  my_fseek(CARDID, -3, SEEK_CUR);
  fput_str(CARDID, CR "} card_id_t;" CR CR "#endif" CR);
  my_fclose(CARDID);
  
  return 0;
}
