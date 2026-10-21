"""Reconstruct native generic-spell storage while retaining the exported control flow."""
import re

STORAGE='''typedef union
{
  card_instance_t *instance;
  int32_t number;
  uint32_t bits;
  uint8_t low;
} generic_spell_word_2022_t;

typedef union
{
  test_definition_2022_t test;
  target_definition_2022_t target;
  char error[200];
} generic_spell_scratch_2022_t;

_Static_assert(sizeof(generic_spell_word_2022_t) == 4, "generic spell reused words");
_Static_assert(sizeof(generic_spell_scratch_2022_t) == 216, "generic spell shared scratch");
'''


def reconstruct(text,prompt_literal):
    changes=[]
    def replace(old,new,kind,occurrences=1,**proof):
        nonlocal text
        assert text.count(old)==occurrences,(kind,old,text.count(old))
        text=text.replace(old,new)
        changes.append(dict(original=old,replacement=new,kind=kind,occurrences=occurrences,**proof))
    # Reconstruct the outgoing frame of the one VLA branch as a whole; all
    # stores and calls in this interval are checked against its native ranges.
    start=text.index('      pcStack_f4 = (card_instance_t *)&stack0xfffffecc;')
    end=text.index('      return iVar7;',start)+len('      return iVar7;')
    old=text[start:end]
    assert old.count('get_internal_card_id_from_csv_id(')==2 and old.count('get_card_instance(')==1 and old.count('show_deck(')==1
    new='''      uVar8 = (uint)pcVar5->number_of_targets;
      int target_cards[uVar8];
      iVar6 = 0;
      while (iVar6 < (int)(uint)pcVar5->number_of_targets) {
        iVar12 = pcVar5->targets[iVar6].card;
        iVar3 = pcVar5->targets[iVar6].player;
        iVar6 = iVar6 + 1;
        if (iVar12 == -1) {
          iVar12 = get_internal_card_id_from_csv_id(iVar3 == player ? 0x615a : 0x6161);
          target_cards[iVar6 - 1] = iVar12;
        } else {
          pcVar9 = get_card_instance(iVar3,iVar12);
          target_cards[iVar6 - 1] = pcVar9->internal_card_id;
        }
      }
      iVar7 = show_deck_2022(player,target_cards,iVar6,PROMPT_LITERAL,0,ENGINE_DONE_LABEL);
      return iVar7;'''.replace('PROMPT_LITERAL',prompt_literal)
    replace(old,new,'retarget VLA and outgoing arguments',ranges=[['26a0dc6','26a0e39'],['26a1048','26a105f'],['26a11a6','26a11d7']])
    for match in list(re.finditer(r'(?m)^  +uStackY_138 = 0x[0-9a-f]+;\n',text))[::-1]:
        replace(match[0],'','removed native return-address scaffolding')
    replace('  undefined4 uStackY_138;\n','','removed call-frame declaration')
    replace('test_definition_t test;','generic_spell_scratch_2022_t scratch;','216-byte scratch object')
    replace('(target_definition_t *)&test','&scratch.target','shared target view',occurrences=2)
    start=text.index("          test.message[0xc] = '\\0';")
    end=text.index('          pcStack_108 = ',start)
    old=text[start:end]
    assert old.count('test.message[')==8 and 'test.message._0_4_ = pcStack_f4;' in old
    replace(old,'          scratch.target.zone = 0x1000;\n          scratch.target.illegal_abilities = 0;\n          scratch.target.player = pcStack_f4.number;\n',
            'native graveyard chooser fields',ranges=[['26a1242','26a1262']])
    replace('sprintf(test.message,(char *)0x73940c);','sprintf(scratch.error,EXE_STR(0x73940c),EXE_STR(0x785e65));',
            'native error format and omitted variadic operand',occurrences=2,ranges=[['26a0cb5','26a0cda'],['26a144a','26a1468']])
    replace('display_error_message(test.message);','display_error_message(scratch.error);','error buffer view',occurrences=2)
    replace('&test','&scratch.test','shared test view',occurrences=2)
    # Scalar address arithmetic was spelled as card-instance member addresses
    # by the decompiler. Preserve its native additions (including cleanup's
    # base+count end) explicitly rather than dereferencing invented instances.
    replace('  card_instance_t *pcStack_108;','  int32_t base_target;\n  int32_t selected_targets;\n  int32_t cleanup_end;', 'scalar target storage')
    for old,new,n in [
        ('(int)&pcStack_108->counters2','(base_target + 1)',1),
        ('pcStack_108->color_id + max_targets + -0x40','base_target + max_targets',2),
        ('pcStack_108->color_id + (aiStack_114[1] - 0x40)','base_target + aiStack_114[1]',1),
        ('(int)(puVar1 + (int)(pcVar9->color_id + -0x80))','cleanup_end',2),
        ('puVar1 = pcStack_108->color_id;','cleanup_end = base_target + selected_targets;',1),
        ('pcVar9 = pcStack_108;','selected_targets = base_target;',1),
        ('((int)pcStack_fc <= (int)pcVar9)','(pcStack_fc.number <= selected_targets)',1),
        ('pcStack_f8->targets + (int)pcVar9','pcStack_f8.instance->targets + selected_targets',1),
        ('pcVar9 = (card_instance_t *)&pcVar9->counters2;','selected_targets = selected_targets + 1;',3),
        ('pcStack_f4 = pcVar9;','pcStack_f4.number = selected_targets;',1),
        ('pcStack_108 = (card_instance_t *)0x1;','base_target = 1;',2),
        ('pcStack_108 = (card_instance_t *)0x0;','base_target = 0;',2),
        ('pcStack_108 = (card_instance_t *)pcVar5->targets[0].player;','base_target = pcVar5->targets[0].player;',1),
        ('(int32_t)pcStack_108','base_target',2),
        ('(int)pcStack_108','base_target',4),
        ('pcStack_108 == pcStack_f4','base_target == pcStack_f4.number',1)]:
        replace(old,new,'native scalar target arithmetic',occurrences=n)
    assert 'pcStack_108' not in text
    replace('uint8_t *puVar1;','target_t *selected_target;','selected target typed address')
    replace('uint8_t *puStack_10c;','target_t *saved_target;','saved target typed address')
    for old,new,n in [
        ('puVar1 = pcStack_f8->color_id + (int)pcVar9 * 8 + -0x40;','selected_target = &pcStack_f8.instance->targets[selected_targets];',1),
        ('*(int *)(puVar1 + 0x78)','selected_target->card',1),
        ('((target_t *)(puVar1 + 0x74))->player','selected_target->player',1),
        ('puStack_10c = puVar1;','saved_target = selected_target;',1),
        ('*(int *)(puStack_10c + 0x78)','saved_target->card',2),
        ('((target_t *)(puStack_10c + 0x74))->player','saved_target->player',2)]:
        replace(old,new,'native target field view',occurrences=n)
    # Three four-byte slots have pointer, integer and byte lifetimes. Do not
    # read uninitialized upper bytes when only a native low byte is live.
    for name in ['pcStack_f4','pcStack_f8','pcStack_fc']:
        replace('card_instance_t *'+name+';','generic_spell_word_2022_t '+name+';','reused word union')
    replace('pcStack_f8 = (card_instance_t *)((uint)pcStack_f8 & 0xffffff00);','pcStack_f8.low = 0;','low-byte initialization')
    for name in ['pcStack_f4','pcStack_f8','pcStack_fc']:
        pattern=name+r' = \(card_instance_t \*\)CONCAT31\('+name+r'\._1_3_,([^;]+)\);'
        for match in list(re.finditer(pattern,text))[::-1]:replace(match[0],name+'.low = '+match[1]+';','low-byte spill')
        for cast in ['uint8_t','char']:
            old='('+cast+')'+name
            count=text.count(old)
            if count:replace(old,('((char)'+name+'.low)' if cast=='char' else name+'.low'),'low-byte read',occurrences=count)
        old='(int)'+name
        count=text.count(old)
        if count:replace(old,name+'.number','native signed word',occurrences=count)
        for cmp in ['==','!=']:
            old=name+' '+cmp+' (card_instance_t *)0x0'
            count=text.count(old)
            if count:replace(old,name+'.bits '+cmp+' 0','native zero-word comparison',occurrences=count)
    stores={m[0]:m[1]+'.number = '+m[2]+';' for m in re.finditer(r'(pcStack_f[48c]) = \(card_instance_t \*\)([^;]+);',text)}
    for old,new in stores.items():replace(old,new,'native scalar word store',occurrences=text.count(old))
    stores={m[0]:'uStack_100 = '+m[1]+';' for m in re.finditer(r'uStack_100 = CONCAT31\(uStack_100\._1_3_,([^;]+)\);',text)}
    for old,new in stores.items():replace(old,new,'boolean spill with unused upper bytes',occurrences=text.count(old))
    # Every remaining bare reference to these slots is a native pointer use.
    # Declarations and already-qualified word/byte views are protected.
    for name in ['pcStack_f4','pcStack_f8','pcStack_fc']:
        declaration='generic_spell_word_2022_t '+name+';'
        replace(declaration,'GENERIC_SLOT_DECL_'+name,'protect word declaration')
        pattern=r'\b'+name+r'\b(?!\.)'
        # Record exact enclosing lines so replay never rewrites typed members.
        for line in list(dict.fromkeys(text.splitlines())):
            if re.search(pattern,line):replace(line,re.sub(pattern,name+'.instance',line),'native pointer word view',occurrences=text.count(line))
        replace('GENERIC_SLOT_DECL_'+name,declaration,'restore word declaration')
    replace('char **ppcVar11;','const char **ppcVar11;','readonly text table result')
    replace('char *prompt,','const char *prompt,','readonly prompt parameter')
    assert not re.search(r'CONCAT|stack0x|\._\d|color_id|counters2|puVar1|puStack_10c|pcStack_108',text.replace('land_can_be_played._1_1_','land_can_be_played'))
    return text,changes
