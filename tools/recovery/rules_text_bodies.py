"""Typed allocation/pattern setup for the native rules-text walker."""
RULES_PREFIX = r'''  iVar3 = get_id(player, card);
  if (iVar3 == 0x33c4) {
    *out_buf = '\0';
    return;
  }
  const char *orig_rules = cards_ptr[iVar3]->rules_text;
  unsigned int rules_size = strlen(orig_rules) + 1;
  char rules[rules_size];
  memcpy(rules, orig_rules, rules_size);
  pcVar12 = rules;
  pcVar5 = strstr(rules, "\n--FLIP--\n");
  if (pcVar5 != NULL) {
    *pcVar5 = '\0';
  }
  dfcbackpat1[0] = '\n';
  dfcbackpat1[1] = '(';
  dfcbackpat1[2] = '\0';
  /* Native base+4 indexes the following card's name, not this card's. */
  strcpy(dfcbackpat1 + 2, cards_ptr[iVar3 + 1]->name);
  strcpy(dfcbackpat2, dfcbackpat1);
  strcat(dfcbackpat1, " - ");
  strcat(dfcbackpat2, ")");
  pcVar5 = strstr(rules, dfcbackpat1);
  if (pcVar5 != NULL) {
    *pcVar5 = '\0';
  }
  pcVar5 = strstr(rules, dfcbackpat2);
  if (pcVar5 != NULL) {
    *pcVar5 = '\0';
  }
'''
