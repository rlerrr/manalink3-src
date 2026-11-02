"""Recover typed helper locals; no shared union/register-word storage in C."""
import re
from canonicalize_helpers import mask,sha
from structured_flow_source import block
from verify_source_spelling_edits import substitute

REVIEWED_AURA_LOOP_SHA='c17b48237bc17acf42937ca877c02707a3cf763079b406f9374cf95a7aea3ab3'
REVIEWED_GRAVE_LOOP_SHA='5f7d3aa3a31236a8007d615db29c705e231d3b09e2b057836820c75db0c7bd45'

TYPEDEFS=[('''typedef union
{
  card_instance_t *instance;
  int32_t number;
  uint32_t bits;
  uint8_t low;
} generic_spell_word_t;

_Static_assert(sizeof(generic_spell_word_t) == 4, "generic spell reused words");
'''),('''typedef union
{
  target_definition_t target;
  test_definition_t test;
  target_t selected;
} activated_storage_t;
''')]


def project(name,before):
    text=before;changes=[]
    def replace(old,new,count=1):
        nonlocal text
        assert text.count(old)==count,(name,old,text.count(old),count);changes.append(dict(before=old,after=new,occurrences=count));text=text.replace(old,new)
    if name=='granted_generic_activated_ability':
        replace('  uint8_t uVar1;','  int old_number_of_targets;')
        new=substitute(text,{'uVar1':'old_number_of_targets'});replace(text,new)
        replace('  activated_storage_t storage;\n','')
        can=block(text,'    if ((mode & 0x400000U) != 0)\n    {')
        new=can.replace('    {\n','    {\n      target_definition_t td_default;\n',1).replace('storage.target','td_default');replace(can,new)
        replace('  hack_what_was_sacced_for_gaa = 0;\n  pcVar3 = get_card_instance(granting_player, granting_card);','  target_definition_t td_default;\n  hack_what_was_sacced_for_gaa = 0;\n  pcVar3 = get_card_instance(granting_player, granting_card);')
        replace('storage.target','td_default',4)
        replace('    new_default_test_definition(&storage.test, 2, "Select a creature to sacrifice.");','    test_definition_t test;\n    new_default_test_definition(&test, 2, "Select a creature to sacrifice.");')
        replace('                                &storage.test);','                                &test);')
        replace('      pcVar7 = get_card_instance(td->player, td->card);','      target_t tgt;\n      pcVar7 = get_card_instance(td->player, td->card);')
        count=len(re.findall(r'\bstorage\.selected\b',mask(text)));replace('storage.selected','tgt',count)
        assert not re.search(r'\bstorage\b',mask(text))
    else:
        assert name=='generic_spell'
        aura_start='              iVar7 = 0;\n              pcStack_f4.instance = pcVar5;\n';a=text.index(aura_start);end='              } while (iVar7 != 2);\n';b=text.index(end,a)+len(end);old=text[a:b]
        assert sha(old.encode())==REVIEWED_AURA_LOOP_SHA
        new='''              for (int p = 0; p < 2; ++p)
              {
                for (int c = 0; c < active_cards_count[p]; ++c)
                {
                  if (in_play(p, c) != NULL && is_what(p, c, 0x47) != 0 &&
                      would_validate_arbitrary_target(td, p, c) != 0 &&
                      is_humiliated(p, c, AL_STATIC) == 0)
                  {
                    int csvid = get_id(p, c);
                    if (csvid == 0x32b || csvid == 0x242)
                    {
                      return 0;
                    }
                  }
                }
              }
''';replace(old,new)
        old=block(text,'    if ((flags & 0x1808U) != 0)\n    {')
        assert sha(old.encode())==REVIEWED_GRAVE_LOOP_SHA
        new='''    if ((flags & 0x1808U) != 0)
    {
      int grave_player = player;
      if ((flags & 0x800U) != 0)
      {
        grave_player = 1 - player;
      }
      if ((flags & 0x1000U) != 0)
      {
        grave_player = 2;
      }
      int found = 0;
      for (int p = 0; p < 2; ++p)
      {
        if ((p == grave_player || grave_player == 2) &&
            new_special_count_grave(p, this_test) > 0 && graveyard_has_shroud(p) == 0)
        {
          found = 1;
          break;
        }
      }
      if (!found)
      {
        return 0;
      }
    }''';replace(old,new)
        replace('  char cVar13;\n','');replace('  bool bVar16;\n','')
        replace('  uint8_t uVar2;','  int old_number_of_targets;')
        replace('        uVar2 = pcVar5->number_of_targets;','        old_number_of_targets = pcVar5->number_of_targets;')
        replace('        pcStack_f4.low = uVar2;\n','')
        replace('        pcVar5->number_of_targets = pcStack_f4.low;','        pcVar5->number_of_targets = old_number_of_targets;')
        replace('  generic_spell_word_t pcStack_fc;\n  generic_spell_word_t pcStack_f8;\n  generic_spell_word_t pcStack_f4;\n','  int aura_spell;\n  int graveyard_flags;\n  int32_t target_limit;\n  int normal_targeting;\n  card_instance_t *storage_inst;\n  int p;\n  int c;\n')
        replace('  int aiStack_114[2];','  int rv;\n  int min_targets;')
        replace('aiStack_114[0]','rv',2);replace('aiStack_114[1]','min_targets',3)
        replace('  pcStack_f4.number = (flags & 0x200);','  aura_spell = (flags & 0x200);')
        count=text.count('pcStack_f4.bits');replace('pcStack_f4.bits','((uint32_t)aura_spell)',count)
        replace('  pcStack_fc.low = ((uint32_t)aura_spell) != 0;\n','')
        replace("pcStack_fc.low != '\\0'",'aura_spell != 0')
        replace('      pcStack_fc.number = (base_target + max_targets);','      target_limit = (base_target + max_targets);')
        replace('pcStack_fc.number <= selected_targets','target_limit <= selected_targets')
        replace('      pcStack_f4.number = selected_targets;\n','')
        count=text.count('pcStack_f4.number <');replace('pcStack_f4.number <','selected_targets <',count)
        replace('base_target == pcStack_f4.number','base_target == selected_targets')
        # Keep the spell instance stable while inspecting cleanup targets.
        replace('        pcStack_fc.instance = pcVar5;\n','')
        replace('            pcVar5 = get_card_instance(ptVar15->player, ptVar15->card);\n            if ((pcVar5->state & 0x100000) != 0)\n            {\n              pcVar5->state = pcVar5->state & 0xffefffff;\n            }',
                '            card_instance_t *cleanup_instance = get_card_instance(ptVar15->player, ptVar15->card);\n            if ((cleanup_instance->state & 0x100000) != 0)\n            {\n              cleanup_instance->state = cleanup_instance->state & 0xffefffff;\n            }')
        replace('          pcVar5 = pcStack_fc.instance;\n','')
        replace('    pcVar5 = get_card_instance(pcVar5->targets[0].player, pcVar5->targets[0].card);\n    if ((pcVar5->state & 0x100000) != 0)\n    {\n      pcVar5->state = pcVar5->state & 0xffefffff;',
                '    pcVar10 = get_card_instance(pcVar5->targets[0].player, pcVar5->targets[0].card);\n    if ((pcVar10->state & 0x100000) != 0)\n    {\n      pcVar10->state = pcVar10->state & 0xffefffff;')
        replace('pcStack_f4.number = attacking_card_controller','p = attacking_card_controller')
        replace('pcStack_fc.number = attacking_card','c = attacking_card')
        replace('pcStack_fc.number = card','c = card')
        replace('pcStack_f4.number = player','p = player')
        count=text.count('pcStack_f4.number');replace('pcStack_f4.number','p',count)
        # Native0x26a125c writes offset0 (who_chooses), not offset72 (player).
        replace('          td9.player = p;','          td9.who_chooses = p;')
        replace('      pcStack_fc.number = 0x0;','      graveyard_flags = 0x0;')
        replace('pcStack_fc.number = (flags & 0x1808)','graveyard_flags = (flags & 0x1808)',2)
        count=text.count('pcStack_fc.bits');replace('pcStack_fc.bits','((uint32_t)graveyard_flags)',count)
        # The remaining full-word uses are the graveyard caller card.
        count=text.count('pcStack_fc.number');replace('pcStack_fc.number','c',count)
        replace('          base_target = pcVar5->targets[0].player;\n          uStack_100 = pcVar5->targets[0].card;\n          pcStack_fc.low = pcVar5->number_of_targets;',
                '          target_t old_t0 = pcVar5->targets[0];\n          int old_nt = pcVar5->number_of_targets;')
        replace('            pcVar5->targets[0].player = base_target;\n            pcVar5->targets[0].card = uStack_100;',
                '            pcVar5->targets[0].player = old_t0.player;\n            pcVar5->targets[0].card = old_t0.card;')
        replace('          pcVar5->targets[0].card = uStack_100;\n          pcVar5->targets[0].player = base_target;',
                '          pcVar5->targets[0].card = old_t0.card;\n          pcVar5->targets[0].player = old_t0.player;')
        replace('pcStack_fc.low','old_nt',2)
        count=text.count('pcStack_f8.instance');replace('pcStack_f8.instance','storage_inst',count)
        count=text.count('pcStack_f8.low');replace('pcStack_f8.low','normal_targeting',count)
        # No address of these scalar pointer variables ever escapes.
        assert not re.search(r'&\s*(?:pcVar5|selected_targets|uVar2)\b',mask(before))
        assert len(re.findall(r'\bpcVar5\s*=(?!=)',mask(text)))==1
        assert not re.search(r'\b(?:pcStack_f[4c8]|generic_spell_word_t|aiStack_114|cVar13|bVar16|uVar2)\b',mask(text))
        new=substitute(text,{'pcVar5':'instance','pcVar10':'target_instance'});replace(text,new)
    assert re.findall(r'"(?:\\.|[^"\\])*"',before)==re.findall(r'"(?:\\.|[^"\\])*"',text)
    return text,changes
