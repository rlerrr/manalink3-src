"""Structured source projection of the Guild Summit shared tails."""
from canonicalize_helpers import mask
from verify_source_spelling_edits import substitute


def block(text,marker):
    assert text.count(marker)==1;lo=text.index(marker);code=mask(text);start=code.index('{',lo);depth=1;end=start+1
    while depth:depth+=(code[end]=='{')-(code[end]=='}');end+=1
    return text[lo:end]


def guild_summit(text):
    gate=block(text,'    if (trigger_cause == card)\n    {')
    gate=gate.replace('    if (trigger_cause == card)','    if ((event == EVENT_ETB) && (trigger_cause_controller == player) &&\n        (trigger_cause == card))',1)
    assert gate.count('      goto LAB_0261d6f8;')==1;gate=gate.replace('      goto LAB_0261d6f8;\n','')
    gate=substitute(gate,{'iStack_ec':'count'});assert gate.count('      count = 0;')==1;gate=gate.replace('      count = 0;','      int count = 0;')
    gate='\n'.join(line[2:] if line.startswith('  ') else line for line in gate.split('\n'))
    trigger=block(text,'    if (player == affected_card_controller)\n    {')
    trigger=trigger.replace('    if (player == affected_card_controller)','    if ((trigger_condition == 0xdb) && (card == affected_card) &&\n        (player == affected_card_controller))',1)
    trigger='\n'.join(line[2:] if line.startswith('  ') else line for line in trigger.split('\n'))
    prefix=text[:text.index('  if (event == EVENT_ETB)\n')];assert prefix.count('  int iStack_ec;\n')==1;prefix=prefix.replace('  int iStack_ec;\n','')
    suffix='''  if ((event == EVENT_ETB) && (trigger_cause != card))
  {
    draw_cards(player, 1);
  }
  result = global_enchantment(player, card, event);
  return result;
}'''
    assert text.endswith('LAB_0261d644:\n  result = global_enchantment(player, card, event);\n  return result;\n}')
    return prefix+gate+'\n'+trigger+'\n'+suffix


def trace_proof():
    """Exhaust gate/trigger facts, allowing opaque calls to change later facts.

    The unchanged gate chooser is one action. Its post-call trigger facts are
    arbitrary; the unchanged trigger helper can independently change the later
    cause/card comparison. No purity assumption is made for either action.
    """
    from itertools import product
    checked=0
    for event,controller,cause,condition,affected,affected_controller in product([False,True],repeat=6):
        gate=event and controller and cause
        for post in product([False,True],repeat=4) if gate else [(cause,condition,affected,affected_controller)]:
            c,t,a,p=post
            for final_cause in [False,True]:
                old=['gate'] if gate else [];new=['gate'] if gate else []
                old_c=c
                if not event:pc='668' if t and a else '644'
                elif not controller or gate:pc='f8'
                else:pc='704' if t else 'draw'
                while pc!='644':
                    if pc=='f8':pc='704' if t else '6c8'
                    elif pc=='704':pc='668' if a else '6c8'
                    elif pc=='668':
                        if p:old.append('trigger');old_c=final_cause
                        pc='6c8' if event else '644'
                    elif pc=='6c8':pc='644' if old_c else 'draw'
                    elif pc=='draw':old.append('draw_one');pc='644'
                    else:raise AssertionError(pc)
                new_c=c
                if t and a and p:new.append('trigger');new_c=final_cause
                if event and not new_c:new.append('draw_one')
                old.append('global_enchantment');new.append('global_enchantment');assert old==new;checked+=1
    return dict(abstract_path_cases=checked,scope='Exhaustive event/trigger predicate cases for the recorded Guild Summit projection. The chooser and trigger blocks remain byte-for-byte source blocks apart from indentation, count spelling and local declaration placement. Calls may change subsequent trigger/cause facts. This is not a full DLL or gameplay proof.')
