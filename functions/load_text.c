#include "manalink.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

typedef struct
{
  const char *key;
  int num;
  const char **lines;
} LoadText;

_Static_assert(sizeof(LoadText) == 12, "LoadText size");
_Static_assert(offsetof(LoadText, key) == 0 && offsetof(LoadText, num) == 4 &&
               offsetof(LoadText, lines) == 8, "LoadText fields");

/* Recovered private caches; retained raw buffers own the borrowed strings. */
static LoadText *loadtexts;
static int num_loadtexts;
static char last_loadtext_from_file[262];
static LoadText *loadtexts_from_file;
static int num_loadtexts_from_file;

/* ManalinkEh 2022: 0x2675d70; original functions/load_text.c:52. */
static void load_text_stash(const char *filename, LoadText **out_loadtexts, int *out_num_loadtexts)
{
  FILE *argument1;
  char *pcVar3;
  char *pcVar4;
  char *pcVar5;
  LoadText *pLVar7;
  const char **ppcVar8;
  char *pcVar9;
  const char **ppcVar10;
  int *piVar11;
  int iVar12;
  char cVar13;
  unsigned int uVar14;
  int iVar17;
  LoadText *lt;
  int iStack_214;
  int iStack_210;
  int iStack_20c;
  char path[500];

  EXE_FN(void, 0x466020, char *)(path);
  sprintf(path + strlen(path), "\\%s", filename);
  argument1 = fopen(path, "rt");
  if (argument1 == NULL)
  {
    piVar11 = (int *)_errno();
    pcVar3 = strerror(*piVar11);
    fatal("Couldn\'t open \"%s\": %s", path, pcVar3);
  }
  fseek(argument1, 0, 2);
  uVar14 = ftell(argument1);
  pcVar3 = malloc(uVar14 + 2);
  if (pcVar3 == NULL)
  {
    fatal("Couldn\'t malloc %d bytes for raw_file", uVar14 + 2);
  }
  fseek(argument1, 0, 0);
  uVar14 = fread(pcVar3, 1, uVar14, argument1);
  pcVar3[uVar14] = '\n';
  pcVar3[uVar14 + 1] = '\0';
  lt = malloc(0x1c20);
  if (lt == NULL)
  {
    fatal("Couldn\'t malloc %d*%d entries for LoadText", 600, 0xc);
  }
  pcVar4 = strchr(pcVar3, 10);
  iStack_20c = 0;
  iStack_214 = 600;
  iStack_210 = 1;
LAB_02675eb0:
  do
  {
    iVar12 = iStack_20c * 0xc;
    if (pcVar4 == NULL)
    {
    LAB_02675f5f:
      if (iStack_214 == iStack_20c)
      {
        lt = realloc(lt, (int)((double)iStack_214 * 1.5) * 0xc);
        if (lt == NULL)
        {
          fatal("Couldn\'t realloc %d*%d entries for LoadText", (int)((double)iStack_214 * 1.5),
                0xc);
        }
      }
      LoadText *sentinel = (LoadText *)((char *)lt + iVar12);
      sentinel->key = "";
      sentinel->num = 250;
      const char **empty_lines = malloc(250 * sizeof(*empty_lines));
      sentinel->lines = empty_lines;
      if (empty_lines != NULL)
      {
        const char **end_lines = empty_lines + 250;
        do
        {
          *empty_lines = "";
          empty_lines = empty_lines + 1;
        } while (empty_lines != end_lines);
        pLVar7 = malloc(iStack_210 * 0xc);
        *out_loadtexts = pLVar7;
        if (pLVar7 != NULL)
        {
          memcpy(pLVar7, lt, iStack_210 * 0xc);
          free(lt);
          *out_num_loadtexts = iStack_210;
          return;
        }
        fatal("Couldn\'t malloc %d*%d entries for LoadText reduction", iStack_210, 0xc);
      }
      fatal("Couldn\'t malloc %d lines for LoadText(ai speculation)", 0xfa);
    }
    *pcVar4 = '\0';
    pcVar9 = pcVar4 + 1;
    if ((*pcVar3 == '@') && (pcVar3[1] != '\0'))
    {
      if (iStack_214 == iStack_20c)
      {
        iStack_214 = (int)((double)iStack_214 * 1.5);
        lt = realloc(lt, iStack_214 * 0xc);
        if (lt == NULL)
        {
          fatal("Couldn\'t realloc %d*%d entries for LoadText", iStack_214, 0xc);
        }
      }
      pcVar5 = strchr(pcVar9, 10);
      if (pcVar5 == NULL)
      {
        iVar12 = iStack_20c * 0xc;
        goto LAB_02675f5f;
      }
      *pcVar5 = '\0';
      pcVar5 = pcVar5 + 1;
      if ((unsigned char)(pcVar4[1] - 0x31U) < 9)
      {
        pLVar7 = lt + iStack_20c;
        pLVar7->key = pcVar3 + 1;
        iVar12 = atoi(pcVar9);
        pLVar7->num = iVar12;
        ppcVar8 = malloc(iVar12 * 4);
        pLVar7->lines = ppcVar8;
        if (ppcVar8 == NULL)
        {
          fatal("Couldn\'t malloc %d lines for LoadText(\"@%s\")", iVar12, pcVar3 + 1);
        }
        pcVar4 = strchr(pcVar5, 10);
        if (0 < iVar12)
        {
          iVar17 = 0;
          while (pcVar4 != NULL)
          {
            *pcVar4 = '\0';
            pcVar3 = pcVar4 + 1;
            pcVar4 = pcVar5;
            while (pcVar9 = strchr(pcVar4, 0x5c), pcVar9 != NULL)
            {
              cVar13 = pcVar9[1];
              pcVar4 = pcVar9 + 1;
              if (cVar13 == 'n')
              {
                cVar13 = '\n';
                goto LAB_02676119;
              }
              if (cVar13 == 't')
              {
                cVar13 = '\t';
                goto LAB_02676119;
              }
              if (cVar13 == '\\')
              {
              LAB_02676119:
                *pcVar9 = cVar13;
                cVar13 = pcVar9[2];
                pcVar9[1] = cVar13;
                pcVar9 = pcVar4;
                while (cVar13 != '\0')
                {
                  cVar13 = pcVar9[2];
                  pcVar9 = pcVar9 + 1;
                  *pcVar9 = cVar13;
                }
              }
            }
            ppcVar8[iVar17] = pcVar5;
            iVar17 = iVar17 + 1;
            if (iVar12 == iVar17)
            {
              pcVar4 = strchr(pcVar3, 10);
              iStack_20c = iStack_210;
              iStack_210 = iStack_210 + 1;
              goto LAB_02675eb0;
            }
            pcVar4 = strchr(pcVar3, 10);
            pcVar5 = pcVar3;
          }
          if (iVar12 <= iVar17)
          {
            iVar17 = iStack_210;
            iVar12 = iStack_210 * 0xc;
            iStack_210 = iStack_20c + 2;
            iStack_20c = iVar17;
            goto LAB_02675f5f;
          }
          ppcVar10 = ppcVar8 + iVar17;
          do
          {
            *ppcVar10 = "";
            ppcVar10 = ppcVar10 + 1;
          } while (ppcVar8 + iVar12 != ppcVar10);
          pcVar4 = strchr(pcVar5, 10);
        }
        iStack_20c = iStack_210;
        iStack_210 = iStack_210 + 1;
        pcVar3 = pcVar5;
      }
      else
      {
        pcVar4 = strchr(pcVar5, 10);
        pcVar3 = pcVar5;
      }
      goto LAB_02675eb0;
    }
    pcVar4 = strchr(pcVar9, 10);
    pcVar3 = pcVar9;
  } while (true);
}

/* ManalinkEh 2022: 0x2676360; original functions/load_text.c:180. */
const char **load_text_nocopy(const char *header, int *num_lines)
{
  int iVar1;
  LoadText *pLVar2;
  int iVar3;
  int iVar4;
  LoadText *pLVar5;
  int iStack_14;

  if (loadtexts == NULL)
  {
    load_text_stash("Text.res", &loadtexts, &num_loadtexts);
  }
  pLVar2 = loadtexts;
  iVar1 = num_loadtexts;
  if (ai_is_speculating == 1)
  {
    if (num_lines != NULL)
    {
      *num_lines = 0;
    }
    return pLVar2[num_loadtexts + -1].lines;
  }
  iVar4 = 0;
  iStack_14 = num_loadtexts;
  pLVar5 = loadtexts;
  if (0 < num_loadtexts)
  {
    do
    {
      iVar3 = strcmp(pLVar5->key, header);
      if (iVar3 == 0)
      {
        if (num_lines != NULL)
        {
          *num_lines = pLVar5->num;
        }
        return pLVar5->lines;
      }
      iVar4 = iVar4 + 1;
      pLVar5 = pLVar5 + 1;
    } while (iVar4 != iVar1);
  }
  if (num_lines != NULL)
  {
    *num_lines = 0;
    iStack_14 = num_loadtexts;
  }
  return pLVar2[iStack_14 + -1].lines;
}

/* ManalinkEh 2022: 0x2676450; original functions/load_text.c:216. */
/* The released fallback indexes this cache using num_loadtexts from the main cache. */
const char **load_text_nocopy_from_file(const char *filename, const char *header, int *num_lines)
{
  LoadText *pLVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  LoadText *pLVar5;

  if (ai_is_speculating == 1)
  {
    if (num_lines != NULL)
    {
      *num_lines = 0;
    }
    if (loadtexts == NULL)
    {
      load_text_stash("Text.res", &loadtexts, &num_loadtexts);
    }
    return loadtexts[num_loadtexts + -1].lines;
  }
  iVar2 = strncmp(last_loadtext_from_file, filename, 0x106);
  pLVar1 = loadtexts_from_file;
  if (iVar2 != 0)
  {
    strncpy(last_loadtext_from_file, filename, 0x106);
    last_loadtext_from_file[0x105] = '\0';
    if (pLVar1 != NULL)
    {
      free(pLVar1);
    }
    load_text_stash(filename, &loadtexts_from_file, &num_loadtexts_from_file);
  }
  pLVar1 = loadtexts_from_file;
  iVar2 = num_loadtexts_from_file;
  if (0 < num_loadtexts_from_file)
  {
    iVar4 = 0;
    pLVar5 = loadtexts_from_file;
    do
    {
      iVar3 = strcmp(pLVar5->key, header);
      if (iVar3 == 0)
      {
        if (num_lines != NULL)
        {
          *num_lines = pLVar5->num;
        }
        return pLVar5->lines;
      }
      iVar4 = iVar4 + 1;
      pLVar5 = pLVar5 + 1;
    } while (iVar4 != iVar2);
  }
  if (num_lines != NULL)
  {
    *num_lines = 0;
  }
  return pLVar1[num_loadtexts + -1].lines;
}

/* ManalinkEh 2022: 0x26765b0; original functions/load_text.c:257. */
/* Preserve the released 250-row cap; the native declaration has only 249 rows. */
int load_text(const char *ignored, const char *header)
{
  const char **ppcVar1;
  const char **ppcVar2;
  int result;
  char (*pacVar4)[300];
  int num_lines;

  ppcVar2 = load_text_nocopy(header, &num_lines);
  if (num_lines < 0xfb)
  {
    if (num_lines < 1)
      goto LAB_02676615;
  }
  else
  {
    num_lines = 0xfa;
  }
  pacVar4 = text_lines;
  result = 0;
  do
  {
    ppcVar1 = ppcVar2 + result;
    result = result + 1;
    _snprintf(*pacVar4, 300, "%s", *ppcVar1);
    pacVar4 = pacVar4 + 1;
  } while (result < num_lines);
LAB_02676615:
  if (num_lines < 0xf9)
  {
    pacVar4 = text_lines + num_lines;
    do
    {
      (*pacVar4)[0] = '\0';
      pacVar4 = pacVar4 + 1;
    } while (pacVar4 != (text_lines + 249));
  }
  return num_lines;
}
