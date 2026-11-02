"""Restore source locals hidden by the remaining target/test/token unions."""
import re
from canonicalize_helpers import mask

NAMES=set('''card_ovinomancer card_angelic_favor card_mechanized_production
card_gremlin_infestation card_baffling_end card_leafdrake_roost card_tetravus
card_batterskull card_inalla_archmage_ritualist card_edh card_skyclave_apparition
card_kiora_bests_the_sea_god card_ghired_conclave_exile card_grand_ossuary
card_security_blockade card_alharu_solemn_ritualist card_lullmage_mentor
new_sacrifice_impl'''.split())


def source_changes(name,text):
    assert name in NAMES;changes=[]
    def replace(old,new,count=1):
        nonlocal text
        assert text.count(old)==count,(name,old,text.count(old),count)
        changes.append(dict(before=old,after=new,occurrences=count));text=text.replace(old,new)
    def union(new=''):
        pattern=r'  /\* DWARF locals share this frame offset; keep every typed view\. \*/\n  union\n  \{\n.*?\n  \} (recovered_stack_\w+);\n'
        found=list(re.finditer(pattern,text,re.S));assert len(found)==1
        replace(found[0][0],new);return found[0][1]
    def decl_token():
        found=list(re.finditer(r'(?m)^( +)(?:default_token_definition|copy_token_definition)\([^\n]*&token',text))
        assert len(found)==1,(name,len(found))
        m=found[0];replace(m[0],m[1]+'token_generation_t token;\n'+m[0])
    def decl_target(local='td',count=1):
        found=list(re.finditer(r'(?m)^( +)(?:default_target_definition|base_target_definition)\([^\n]*&'+local+r'\b',text))
        assert len(found)==count,(name,local,len(found))
        before=text;after=text
        for m in reversed(found):after=after[:m.start()]+m[1]+'target_definition_t '+local+';\n'+after[m.start():]
        replace(before,after)
    if name=='card_edh':
        old='''  union
  {
    char text[600];
    test_definition_t test;
    token_generation_t token;
    target_definition_t target;
  } buffer_views;
  union
  {
    char text[100];
    token_generation_t token;
  } msg_views;
'''
        replace(old,'');replace('  target_definition_t td;\n','')
        # Two short error buffers have their own native declarations. Duplicate
        # only the existing shared exit tail so their addresses never escape.
        tail='do_dialog(0, player, card, -1, -1, buffer, 0);\n{0}lose_the_game(0);\n{0}return 0;'
        replace('    if ((csvid & csvid_00) == 0xffffffff)\n    {','    if ((csvid & csvid_00) == 0xffffffff)\n    {\n      char buffer[600];')
        replace('\n      goto LAB_022c77fe;', '\n      '+tail.format('      '))
        replace('        snprintf(buffer_views.text, 600, "Two Commanders are allowed only if both have Partner.");',
                '        char buffer[600];\n        snprintf(buffer_views.text, 600, "Two Commanders are allowed only if both have Partner.");')
        replace('\n        goto LAB_022c77fe;','\n        '+tail.format('        '))
        replace('    LAB_022c77fe:\n','')
        replace('    iVar15 = snprintf(buffer_views.text, 600, "%s general is %s\\n", puVar23, pcVar17);',
                '    char buffer[600];\n    iVar15 = snprintf(buffer_views.text, 600, "%s general is %s\\n", puVar23, pcVar17);')
        # Activation owns two100-byte prompts, rather than the larger deck text.
        lo=text.index('  else if (event == EVENT_ACTIVATE)\n  {');code=mask(text);start=code.index('{',lo);depth=1;end=start+1
        while depth:depth+=(code[end]=='{')-(code[end]=='}');end+=1
        block=text[lo:end];new=block.replace('  else if (event == EVENT_ACTIVATE)\n  {','  else if (event == EVENT_ACTIVATE)\n  {\n    char msg[100];\n    char msg2[100];')
        new=new.replace('msg_views.text','msg').replace('buffer_views.text','msg2')
        new=new.replace('      base_target_definition(player, card, &td, 2);','      target_definition_t td;\n      base_target_definition(player, card, &td, 2);')
        replace(block,new)
        # Byte fragments are exact little-endian native fields, not strings.
        for old,value in [('-0x2a','0xd6'),('-0x33','0xcd')]:
            prefix='        ' if old=='-0x2a' else '          '
            rows=[('68',old),('69',"'\\0'"),('6a',"'\\0'"),('6b',"'\\0'"),('aa',"'\\x01'"),('ab',"'\\0'")]
            if old=='-0x2a':rows += [('6c',"'\\x03'"),('6d',"'\\0'")]
            before=''.join(prefix+'buffer_views.text[0x'+off+'] = '+v+';\n' for off,v in rows)
            after=prefix+'test.subtype = '+value+';\n'+prefix+'test.not_me = 1;\n'
            if old=='-0x2a':after+=prefix+'test.type_flag = 3;\n'
            replace(before,after)
        for indent in ['        ','          ']:
            replace('\n'+indent+'default_test_definition(&buffer_views.test, 2);','\n'+indent+'test_definition_t test;\n'+indent+'default_test_definition(&test, 2);')
        replace('buffer_views.test','test',2)
        replace('          copy_token_definition(player, card, &buffer_views.token,','          token_generation_t token;\n          copy_token_definition(player, card, &buffer_views.token,')
        replace('buffer_views.token','token',3)
        replace('            default_token_definition(player, card, 0x49a0, &msg_views.token);','            token_generation_t token;\n            default_token_definition(player, card, 0x49a0, &token);')
        for offset,field in [('1c','tou'),('18','pow')]:
            off=int(offset,16);before=''.join('            msg_views.text[0x'+format(off+j,'x')+'] = '+("'\\x01'" if j==0 else "'\\0'")+';\n' for j in range(4))
            replace(before,'            token.'+field+' = 1;\n')
        replace('msg_views.token','token')
        replace('  default_target_definition(player, card, &buffer_views.target, 2);','  {\n  target_definition_t td;\n  default_target_definition(player, card, &td, 2);')
        for offset,field,value in [(0x2c,'required_subtype','0x1f'),(0x38,'special','0x4000')]:
            before=''.join('  buffer_views.text[0x'+format(offset+j,'x')+'] = '+("'\\x1f'" if offset==0x2c and j==0 else "'@'" if offset==0x38 and j==1 else "'\\0'")+';\n' for j in range(4))
            replace(before,'  td.'+field+' = '+value+';\n')
        replace('buffer_views.target','td',4)
        replace('LAB_022c6de0:\n','  }\nLAB_022c6de0:\n')
        replace('buffer_views.text','buffer',16)
        assert 'buffer_views' not in text and 'msg_views' not in text
        return changes,text
    if name=='new_sacrifice_impl':
        replace('  sacrifice_selection_storage_t selection;\n','')
        replace('          default_token_definition(player, card, 0x493b, &selection.token);','          token_generation_t token;\n          default_token_definition(player, card, 0x493b, &selection.token);')
        replace('selection.token','token',5)
        replace('''        selection.rval = (int32_t)((uint16_t)iStack_218 | ((uint32_t)(uint8_t)iStack_23c << 16) |
                                   ((uint32_t)(uint8_t)iStack_22c << 24));
        iStack_218 = selection.rval;''','''        iStack_218 = (int32_t)((uint16_t)iStack_218 | ((uint32_t)(uint8_t)iStack_23c << 16) |
                                   ((uint32_t)(uint8_t)iStack_22c << 24));''')
        replace('    do\n    {\n      spell_fizzled = 0;','    do\n    {\n      target_t tgt;\n      spell_fizzled = 0;')
        replace('selection.rval','tgt.player',5);replace('selection.target','tgt',12)
        assert 'selection.' not in text
        return changes,text
    if name=='card_inalla_archmage_ritualist':base=union('  target_definition_t td2;\n')
    elif name in ['card_mechanized_production','card_gremlin_infestation','card_leafdrake_roost','card_lullmage_mentor']:
        base=union('  target_definition_t td;\n')
    else:base=union()
    td='td2' if name in ['card_angelic_favor','card_inalla_archmage_ritualist'] else 'td1' if name in ['card_alharu_solemn_ritualist','card_security_blockade'] else 'td'
    for member,local in [('token','token'),('target',td),('test','test')]:
        old=base+'.'+member
        if old in text:replace(old,local,text.count(old))
    if name=='card_ovinomancer':
        replace('    td.illegal_type = instance->targets[0].player;','    token.t_player = instance->targets[0].player;')
        replace('  default_target_definition(player, card, &td, 1);','  {\n  target_definition_t td;\n  default_target_definition(player, card, &td, 1);')
        replace('  return 0;\n}','  return 0;\n  }\n}')
    elif name=='card_angelic_favor':
        replace('''        td2.illegal_state =
            (int32_t)haste_and_remove_token_at_end_of_next_turn;''','''        token.special_code_on_generation =
            haste_and_remove_token_at_end_of_next_turn;''')
        decl_target('td2',2)
    elif name=='card_gremlin_infestation':
        replace('    td.illegal_abilities = 2;','    token.tou = 2;');replace('    td.required_abilities = 2;','    token.pow = 2;')
    elif name=='card_baffling_end':
        replace('      td.illegal_type = instance->targets[0].player;','      token.t_player = instance->targets[0].player;');decl_target()
    elif name=='card_tetravus':
        replace('      token.tou = 0;','      td.illegal_abilities = 0;');replace('      token.keep_track_of_tokens_generated = 0x8000;','      td.special = 0x8000;');decl_target()
    elif name=='card_batterskull':
        replace('          token.s_card = player;','          td.preferred_controller = player;');replace('          token.s_player = player;','          td.allowed_controller = player;');decl_target()
    elif name=='card_inalla_archmage_ritualist':
        replace('    default_test_definition(&test, 2);','    test_definition_t test;\n    default_test_definition(&test, 2);')
        replace('''        td2.illegal_state =
            (int32_t)haste_and_remove_token_at_end_of_next_turn;''','''        token.special_code_on_generation =
            haste_and_remove_token_at_end_of_next_turn;''')
    elif name=='card_skyclave_apparition':
        replace('    td.required_abilities = iVar3.data;','    token.pow = iVar3.data;')
        replace('    td.illegal_abilities = td.required_abilities;','    token.tou = token.pow;')
        replace('    td.required_subtype = 0x42;','    token.special_infos = 0x42;')
        replace('    td.illegal_type =\n','    token.t_player =\n');decl_target()
    elif name=='card_kiora_bests_the_sea_god':decl_target()
    elif name=='card_ghired_conclave_exile':
        replace('  target_t tgt;\n  token_generation_t token_1;\n','')
        lo=text.index('  if (event == EVENT_DAT_ABILITY)\n  {');code=mask(text);start=code.index('{',lo);end=start+1;depth=1
        while depth:depth+=(code[end]=='{')-(code[end]=='}');end+=1
        replace(text[lo:end],'  if (event == EVENT_DAT_ABILITY)\n  {\n    ghireds_special_populate(player, card);\n  }')
    elif name=='card_grand_ossuary':
        replace('  char msg[100];\n','')
        lo=text.index('code_r0x02418132:\n');tail=text[lo:-2];assert text.endswith('\n}') and tail.endswith('  goto LAB_02418119;')
        replace(tail,'')
        replace('    LAB_024181bf:\n','      goto LAB_024181bf;\n'+tail+'\n    LAB_024181bf:\n')
        replace('      default_target_definition(player, card, &td, 2);','      target_definition_t td;\n      char msg[100];\n      default_target_definition(player, card, &td, 2);')
        replace('      td.illegal_type = t_player;','      token.t_player = t_player;')
    elif name=='card_security_blockade':
        # The decompiler pointer temporary and the original aura local had the
        # same spelling; retain both roles under distinct ordinary identifiers.
        old=text;new=re.sub(r'\btd\b','ptd',text);replace(old,new)
        before=''.join('      token.'+f+' = '+v+';\n' for f,v in [('color_forced','(uint8_t)damage_card'),('no_sleight','((unsigned int)damage_card >> 8 & 0xff)'),('legacy','((unsigned int)damage_card >> 16 & 0xff)'),('qty','((unsigned int)damage_card >> 24 & 0xff)')])
        replace(before,'      td1.extra = damage_card;\n');replace('      token.keep_track_of_tokens_generated = 0x20;','      td1.special = 0x20;')
        replace('  token.s_card = player;','  td.preferred_controller = player;');replace('  token.s_player = player;','  td.allowed_controller = player;')
        replace('  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);','  {\n  target_definition_t td1;\n  etb_ability(player, card, event, RESOLVE_TRIGGER_MANDATORY);')
        replace('LAB_0245db10:\n','  }\nLAB_0245db10:\n  {\n  target_definition_t td;\n')
        # The final aura branch uses the separate native td, not activated td1.
        replace('  default_target_definition(player, card, &td1, 1);','  default_target_definition(player, card, &td, 1);')
        replace('event, &td1,\n','event, &td,\n')
        replace('  return iVar2;\n}','  return iVar2;\n  }\n}')
        # End the token's scope before the subsequent activation-local setup.
        replace('    default_token_definition(player, card, 0x4958, &token);','    {\n    default_token_definition(player, card, 0x4958, &token);')
        replace('    generate_token(&token);\n','    generate_token(&token);\n    }\n')
    elif name=='card_alharu_solemn_ritualist':
        replace('  int tgs[3];\n','');replace('    state_untargettable(player, card, 1);','    int tgs[3];\n    state_untargettable(player, card, 1);')
        replace('    td1.required_abilities = 1;','    token.pow = 1;');replace('    td1.illegal_abilities = 1;','    token.tou = 1;');replace('    td1.required_color = 0x20;','    token.key_plus = 0x20;');decl_target('td1')
    elif name=='card_lullmage_mentor':
        replace('        td.illegal_abilities = 1;','        token.tou = 1;')
        replace('        td.required_abilities = 1;','        token.pow = 1;')
    decl_token()
    assert 'recovered_stack_' not in text and base not in text
    return changes,text


def ghired_helper_changes(text):
    """The native inline helper has a separate copy token in each branch."""
    assert text.startswith('static void ghireds_special_populate(int player, int card)')
    after='''static void ghireds_special_populate(int player, int card)
{
  if (player == AI || ai_is_speculating == 1)
  {
    test_definition_t this_test;
    default_test_definition(&this_test, TYPE_CREATURE);
    this_test.type_flag = F1_IS_TOKEN;
    int selected = check_battlefield_for_special_card(player, card, player, 1, &this_test);
    if (selected >= 0)
    {
      token_generation_t token;
      copy_token_definition(player, card, &token, player, selected);
      token.action = TOKEN_ACTION_ATTACKING;
      generate_token(&token);
    }
  }
  else
  {
    target_definition_t td;
    base_target_definition(player, card, &td, TYPE_CREATURE);
    td.allowed_controller = td.preferred_controller = player;
    td.allow_cancel = 0;
    td.special = TARGET_SPECIAL_EXTRA_FUNCTION;
    td.extra = (int32_t)target_is_token_too;
    target_t tgt;
    if (can_target(&td) && select_target(player, card - 1000, &td,
                                        "Select a creature token you control.", &tgt))
    {
      token_generation_t token;
      copy_token_definition(player, card, &token, tgt.player, tgt.card);
      token.action = TOKEN_ACTION_ATTACKING;
      generate_token(&token);
    }
  }
}'''
    return [dict(before=text,after=after,occurrences=1)],after
