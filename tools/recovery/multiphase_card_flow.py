"""Restore event dispatch around shared Najeela and Biomancer label tails."""
import re
from structured_flow_source import block


def najeela(text):
    marker='  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))\n';assert text.count(marker)==1;start=text.index(marker);label='LAB_02027d1a:\n';assert text.count(label)==1
    ctor=re.search(r'(?m)^  default_test_definition\(&(\w+), 2\);',text[start:]);assert ctor;name=ctor[1];body_start=start+ctor.start();body=text[body_start:text.index(label)]
    prefix=text[:start];decl='  test_definition_t '+name+';\n';assert prefix.count(decl)==1;prefix=prefix.replace(decl,'')
    old=text[start:body_start]
    expected='''  if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
  {
  LAB_02027d11:
    if (event != EVENT_RESOLVE_ACTIVATION)
      goto LAB_02027d1a;
  }
  else if (event != EVENT_RESOLVE_ACTIVATION)
  {
    if (event != EVENT_CLEANUP)
    {
      return 0;
    }
    goto LAB_02027d11;
  }
''';assert old==expected
    wrapped='  if (event == EVENT_RESOLVE_ACTIVATION)\n  {\n    test_definition_t '+name+';\n'+''.join('  '+line+'\n' for line in body.splitlines())+'  }\n  else if ((event != EVENT_CAN_ACTIVATE) && (event != EVENT_ACTIVATE) &&\n           (event != EVENT_CLEANUP))\n  {\n    return 0;\n  }\n'
    return prefix+wrapped+text[text.index(label)+len(label):]


def biomancer(text):
    resolve=block(text,'  if (event == EVENT_RESOLVE_SPELL)\n  {');tail='''    leaves_play(player, card, EVENT_RESOLVE_SPELL, 0, 0);
  LAB_025bc6ef:
    if ((event == EVENT_CAN_ACTIVATE) || (event == EVENT_ACTIVATE))
      goto LAB_025bc73e;
''';assert resolve.count(tail)==1;resolve=resolve.replace(tail,'')
    cast=block(text,'    if ((affected_card_controller == player) &&\n')
    leave=block(text,'      if (event == EVENT_LEAVES_PLAY_ABILITY)\n');leave=''.join(line[4:]+'\n' for line in leave.splitlines()).rstrip('\n')
    head=text[:text.index('  if (event == EVENT_RESOLVE_SPELL)\n')]
    label='LAB_025bc73e:\n';assert text.count(label)==1;tail=text[text.index(label)+len(label):]
    new=head+resolve+'\n  else if (event == EVENT_CAST_SPELL)\n  {\n'+cast+'\n  }\n  leaves_play(player, card, event, 0, 0);\n'+leave+'\n'
    new+='''  if ((event != EVENT_CAN_ACTIVATE) && (event != EVENT_ACTIVATE) &&
      (event != EVENT_RESOLVE_ACTIVATION) && (event != EVENT_CLEANUP))
  {
    return 0;
  }
'''+tail
    return new


def project(name,text):
    if name=='card_najeela_the_blade_blossom':return najeela(text)
    if name=='card_biomancers_familiar':return biomancer(text)
    return text


def traces():
    events=['can','activate','resolve_activation','cleanup','resolve_spell','cast','leaves','other'];rows=[]
    for e in events:
        # Old Najeela tail has two labels. Calls before it remain unchanged.
        old=[];pc='dispatch'
        while pc!='end':
            if pc=='dispatch':pc='11' if e in ['can','activate','cleanup'] else 'resolve' if e=='resolve_activation' else 'end'
            elif pc=='11':pc='1a' if e!='resolve_activation' else 'resolve'
            elif pc=='resolve':old.append('combat_effects');pc='1a'
            elif pc=='1a':old.append('generic_activation');pc='end'
            else:raise AssertionError(pc)
        new=[]
        if e=='resolve_activation':new.append('combat_effects')
        if e in ['can','activate','resolve_activation','cleanup']:new.append('generic_activation')
        assert old==new;rows.append(dict(handler='najeela',event=e,actions=old))
        # Biomancer's affected-card/type predicate can be arbitrary. These
        # helpers may change global facts but cannot change local event.
        for cast_match in [False,True]:
            old=[]
            if e=='resolve_spell':old+=['resolve_filter','set_global_cost','leaves']
            elif e!='cast':
                old.append('leaves')
                if e=='leaves':old+=['leave_filter','remove_global_cost']
            else:
                if cast_match:old.append('set_cast_cost')
                old.append('leaves')
            if e in ['can','activate','resolve_activation','cleanup']:
                old.append('target')
                if e=='resolve_activation':old.append('legacy')
                old.append('generic_activation')
            new=[]
            if e=='resolve_spell':new+=['resolve_filter','set_global_cost']
            elif e=='cast' and cast_match:new.append('set_cast_cost')
            new.append('leaves')
            if e=='leaves':new+=['leave_filter','remove_global_cost']
            elif e in ['can','activate','resolve_activation','cleanup']:
                new.append('target')
                if e=='resolve_activation':new.append('legacy')
                new.append('generic_activation')
            assert old==new;rows.append(dict(handler='biomancer',event=e,cast_match=cast_match,actions=old))
    return dict(cases=rows,scope='Finite event-dispatch classes and both cast predicates for the two recorded rewrites. Common effect blocks remain source-identical apart from indentation/declaration placement; event is a by-value local and is unchanged by calls. This does not assert callee purity, dynamic argument provenance or complete gameplay.')
