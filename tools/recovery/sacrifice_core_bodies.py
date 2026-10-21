"""Scoped storage reconstructions for two native 2022 sacrifice selectors."""
import re

STORAGE = '''typedef union
{
  int32_t rval;
  target_t target;
  token_generation_2022_t token;
} sacrifice_selection_storage_2022_t;

typedef union
{
  char prompt[200];
  test_definition_2022_t test;
} sacrifice_prompt_storage_2022_t;

_Static_assert(sizeof(sacrifice_selection_storage_2022_t) == 76, "sacrifice shared target/token storage");
_Static_assert(sizeof(sacrifice_prompt_storage_2022_t) == 216, "sacrifice shared prompt/test storage");
'''


def reconstruct(text, address):
    changes=[]
    def replace(old,new,kind,occurrences=1,**detail):
        nonlocal text
        assert text.count(old)==occurrences,(address,kind,old,text.count(old))
        text=text.replace(old,new)
        changes.append(dict(original=old,replacement=new,kind=kind,occurrences=occurrences,**detail))
    if address=='26e4110':
        replace('test_definition_t *ptStack_234;','const char *prompt;','prompt pointer type')
        replace('ptStack_234 = (test_definition_t *)default_prompt;','prompt = default_prompt;','default prompt view')
        replace('ptStack_234 = test;','prompt = test->message;','test message view')
        replace('ptStack_234->message','prompt','selected prompt view')
        start=text.index('  pcVar12 = marked + 5;')
        end=text.index('  if ((int)uStack_220 < 1)',start)
        old=text[start:end]
        assert old.count("'\\0'")==18
        replace(old,'  uVar1 = pcVar6->number_of_targets;\n  iVar3 = pcVar6->targets[0].player;\n  iVar4 = pcVar6->targets[0].card;\n  memset(marked, 0, sizeof(marked));\n',
                '151-byte marked clear',ranges=[['26e428d','26e42f0'],['26e4998','26e49a8']])
        start=text.index('      pcVar12 = marked;')
        end=text.index('\n    }\n  }\n  else {',start)
        old=text[start:end]
        assert 'uVar10 = 0x97;' in old and old.count('return iStack_218;')==2
        replace(old,'      memcpy(marked_for_sacrifice, marked, sizeof(marked));\n      spell_fizzled = iVar5;\n      return iStack_218;',
                '151-byte marked output copy',ranges=[['26e457d','26e45c5'],['26e49e2','26e4a11']])
        replace('int rval;','sacrifice_selection_storage_2022_t selection;','76-byte shared selection storage')
        replace('(token_generation_t *)&rval','&selection.token','token object view',occurrences=2)
        replace('(target_t *)&rval','&selection.target','target object view')
        packed='CONCAT13((undefined1)iStack_22c,CONCAT12((undefined1)iStack_23c,(short)iStack_218))'
        replace(packed,'(int32_t)((uint16_t)iStack_218 | ((uint32_t)(uint8_t)iStack_23c << 16) | ((uint32_t)(uint8_t)iStack_22c << 24))',
                'native packed sacrifice result',ranges=[['26e4540','26e4576']])
        for old in ['  int iStack_20c;\n','  int iStack_1fc;\n','  uchar uStack_1e5;\n','  token_action_t tStack_1e0;\n',
                    '  char *pcVar12;\n','  char *pcVar14;\n','  char *pcVar15;\n','  bool bVar16;\n','  byte bVar17;\n','  bVar17 = 0;\n']:
            replace(old,'','removed lowered storage temporary')
        for old,new in [('rval','selection.rval'),('iStack_20c','selection.target.card'),
                        ('iStack_1fc','selection.token.t_player'),('uStack_1e5','selection.token.qty'),
                        ('tStack_1e0','selection.token.action')]:
            # Only identifier occurrences remain. The replacement strings above
            # already contain selection.rval, so protect member identifiers.
            pattern=r'(?<![.\w])'+old+r'\b'
            count=len(re.findall(pattern,text))
            assert count
            replace(old,new,'typed shared member '+old,occurrences=count)
    if address=='26e4c50':
        replace('char prompt [200];','sacrifice_prompt_storage_2022_t storage;','216-byte shared prompt/test storage')
        replace('(test_definition_t *)prompt','&storage.test','shared test object view',occurrences=2)
        members={108:('type_flag',2),104:('subtype',4),110:('subtype_flag',2),112:('color',4),124:('color_flag',2),
                 116:('id',4),126:('id_flag',2),128:('cmc',4),136:('cmc_flag',2)}
        for offset,(member,size) in members.items():
            replace(f'prompt._{offset}_{size}_',f'storage.test.{member}','typed test member',offset=offset,bytes=size,member=member)
        replace('get_sacrifice_prompt_text_part_2022(prompt,','get_sacrifice_prompt_text_part_2022(storage.prompt,','shared prompt constructor')
        replace('select_target(player,card,&td,prompt,','select_target(player,card,&td,storage.prompt,','shared prompt selection')
    return text,changes
