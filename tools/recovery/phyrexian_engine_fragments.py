"""Native K'rrik payment blocks inserted into the existing mana charger."""
LOCALS='int life_to_pay_for_krrik_2022 = 0;'

SELECTION='''if ((card_flags[player] & 2) != 0 && pay_mana_xbugrwaU[COLOR_BLACK] != 0 &&
    charge_mana_called_from_pay_phyrexian_mana == 0)
{
  int can_pay = can_pay_life(player, 2);
  int max_life = how_much_life_can_i_pay(player);
  if (can_pay)
  {
    int maximum = max_life / 2;
    if (pay_mana_xbugrwaU[COLOR_BLACK] < maximum)
      maximum = pay_mana_xbugrwaU[COLOR_BLACK];
    int minimum = pay_mana_xbugrwaU[COLOR_BLACK] - has_mana(player, COLOR_BLACK, 1);
    if (minimum < 0) minimum = 0;
    char buffer[150];
    sprintf(buffer, " Pay 2 life for how much black mana? (max %d, min %d)", maximum, minimum);
    int chosen;
    while (1)
    {
      chosen = choose_a_number(player, buffer, maximum);
      if (chosen == -1)
      {
        spell_fizzled = 1;
        return 0;
      }
      /* The native engine checks only the upper bound here. */
      if (chosen <= maximum) break;
      if (player == 1)
      {
        spell_fizzled = 1;
        ai_modifier -= 100;
        return 0;
      }
    }
    if ((max_life - 6) / 2 <= chosen && player == 1)
    {
      ai_modifier -= 999;
      spell_fizzled = 1;
      return 0;
    }
    life_to_pay_for_krrik_2022 = chosen * 2;
    pay_mana_xbugrwaU[COLOR_BLACK] -= chosen;
  }
}'''

SETTLEMENT='''if ((card_flags[player] & 2) != 0)
  lose_life(player, life_to_pay_for_krrik_2022);'''

BLOCKS=[('locals',LOCALS,'  if (color == COLOR_ANY)',2),
        ('selection',SELECTION,'      if (autotap_matching_colors_in_pool',6),
        ('settlement',SETTLEMENT,'  max_x_value = -1;',2)]
