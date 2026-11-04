"""Restore historical card comments and source placement without rewriting bodies."""
import argparse,bisect,collections,csv,gzip,json,re,subprocess,unicodedata,zipfile
from pathlib import Path
from canonicalize_helpers import read,mask,sha
from promote_recovery import LITERAL,strip_comments

BASELINE='cad36b4'
BASELINE_ZIP=Path('.recovery/source-baseline-c743.zip')
MARKER=re.compile(r'/\* ManalinkEh 2022: (?P<address>0x[0-9a-f]+); original (?P<source>[^;\n]+?):(?P<line>\d+)\. \*/')
COMMENT=re.compile(r'/\*.*?\*/',re.S)
DEF=re.compile(r'(?m)^[ \t]*(?:[A-Za-z_]\w*[ \t\r\n*]+)+(?P<name>[A-Za-z_]\w*)\s*\([^;{}]*\)(?:\s|\\\r?\n)*\{')
MACRO=re.compile(r'(?m)^[ \t]*ARCANE(?:_WITH_SPLICE)?\s*\(\s*(?P<name>[A-Za-z_]\w*)[^;{}]*\)\s*\{')

def key(name):
 name=name.replace("'",'').replace('\u2019','').replace('\x92','').replace('\u00c6','AE').replace('\u00e6','ae')
 name=unicodedata.normalize('NFKD',name).encode('ascii','ignore').decode().lower()
 return re.sub(r'[^a-z0-9]+','_',name).strip('_')

def functions(text):
 code=mask(text);stack=[];pairs={};outer=set()
 for m in re.finditer('[{}]',code):
  if m[0]=='{':
   if not stack:outer.add(m.start())
   stack.append(m.start())
  elif stack:pairs[stack.pop()]=m.start()+1
 assert not stack,'Unbalanced source braces'
 result=[]
 for pattern,kind in [(DEF,'function'),(MACRO,'macro')]:
  for m in pattern.finditer(code):
   brace=m.end()-1
   if brace not in outer:continue
   result.append(dict(name=m['name'],start=m.start(),brace=brace,end=pairs[brace],kind=kind,line=text.count('\n',0,m.start())+1))
 result.sort(key=lambda f:f['start'])
 assert all(a['end']<=b['start'] for a,b in zip(result,result[1:])), 'Overlapping function parse'
 return result

def card_description(comment):
 lines=[re.sub(r'^\s*\* ?','',s).strip() for s in comment[2:-2].strip().splitlines()]
 if not lines:return None
 if lines[0].startswith('CARD_ID_'):
  if len(lines)<3:return None
  title=lines[1].split('\t')[0].strip()
  if not re.search(r'\b(?:Creature|Summon|Enchantment|Artifact|Land|Sorcery|Instant|Planeswalker|Tribal|Conspiracy|Plane|Phenomenon|World|Vanguard|Scheme|Hero|Emblem|Token|Battle|Dungeon|Attraction|Sticker)\b',lines[2],re.I):return None
 else:
  if len(lines)<2:return None
  heading=re.split(r'\t|\s+(?=\||""\s*$)',lines[0],maxsplit=1)
  if len(heading)!=2:return None
  title=heading[0].strip()
  if not re.search(r'\b(?:Creature|Summon|Enchantment|Artifact|Land|Sorcery|Instant|Planeswalker|Tribal|Conspiracy|Plane|Phenomenon|World|Vanguard|Scheme|Hero|Emblem|Token|Battle|Dungeon|Attraction|Sticker)\b',lines[1],re.I):return None
 return dict(title=title,key=key(title),text=comment)

def first_description(text,f):
 start=f['brace']+1
 while start<f['end'] and text[start].isspace():start+=1
 m=COMMENT.match(text,start)
 return card_description(m[0]) if m else None

def plain(text):
 return text.replace('\u2014','-').replace('\u2013','-').replace('\u2019',"'").replace('\u2018',"'").replace('\u201c','"').replace('\u201d','"').replace('\u2022','*').replace('\u221e','infinity').replace('\u2212','-').replace('\u00bd','1/2').replace('\u00c6','AE').replace('\u00e6','ae').encode('latin1','xmlcharrefreplace').decode('latin1')

def mana(text):
 return re.sub(r'\{([^{}]+)\}',lambda m:'|'+m[1].replace('/',''),text)

def oracle_description(card,face):
 name=plain(face['name']);cost=mana(face.get('mana_cost','')) or '""'
 typ=plain(face.get('type_line',card.get('type_line','')))
 if 'power' in face and 'toughness' in face:typ+=' '+plain(face['power'])+'/'+plain(face['toughness'])
 elif 'loyalty' in face:typ+=' ('+plain(face['loyalty'])+')'
 text=plain(mana(face.get('oracle_text',card.get('oracle_text',''))))
 if card.get('hand_modifier') is not None:text+='\nStarting hand size modifier: '+str(card['hand_modifier'])+'.'
 if card.get('life_modifier') is not None:text+='\nStarting life total modifier: '+str(card['life_modifier'])+'.'
 text=re.sub(r'(?<![A-Za-z])'+re.escape(name)+r'(?![A-Za-z])','~',text)
 lines=[name+'\t'+cost,typ]+text.splitlines()
 # Variable power such as */5 must never terminate the C comment.
 content=(lines[0]+'\n'+'\n'.join(' * '+line for line in lines[1:])).replace('*/','* / ').replace('/*','/ *')
 comment='/* '+content+' */'
 return dict(title=name,key=key(name),text=comment,oracle_id=card.get('oracle_id'),scryfall_id=card['id'],scryfall_uri=card.get('scryfall_uri'),oracle_snapshot='2026-11-04',card_face=face['name'],parent_card_name=card['name'],parent_type_line=card.get('type_line',''),layout=card.get('layout'))

def oracle_library(path):
 result=collections.defaultdict(list)
 if path is None:return result
 for card in json.loads(path.read_bytes()):
  if card.get('layout')=='art_series' or card.get('type_line')=='Card':continue
  for face in card.get('card_faces') or [card]:
   if face.get('name'):
    d=oracle_description(card,face);result[d['key']].append(d)
  if card.get('card_faces') and ' // ' in card['name']:
   faces=card['card_faces']
   combined=dict(name=card['name'],mana_cost=' // '.join(f.get('mana_cost','') for f in faces),type_line=card.get('type_line',' // '.join(f.get('type_line','') for f in faces)),oracle_text='\n\n'.join(f['name']+':\n'+f.get('oracle_text','') for f in faces))
   d=oracle_description(card,combined);result[d['key']].append(d)
 # Repeated token printings may have separate Oracle IDs but identical text.
 return {k:list({r['text']:r for r in rows}.values()) for k,rows in result.items()}

def inventory(oracle=None):
 oracle_cards=oracle_library(oracle)
 alias_path=Path('config/recovery/card-comment-aliases.json')
 aliases={(r['source'],r['function']):r for r in json.loads(alias_path.read_bytes())['functions']} if alias_path.exists() else {}
 original={};library=collections.defaultdict(list);original_by_function={}
 with zipfile.ZipFile(BASELINE_ZIP) as z:
  for name in z.namelist():
   if not name.endswith(('.c','.cpp')):continue
   text=z.read(name).decode('latin1').replace('\r\n','\n');original[name]=text
   for m in COMMENT.finditer(text):
    description=card_description(m[0])
    if description:library[description['key']].append(dict(description,source=name,baseline=BASELINE))
   for f in functions(text):
    d=first_description(text,f)
    if d:original_by_function[(name,f['name'])]=dict(d,source=name,baseline=BASELINE)
 native=collections.defaultdict(set)
 for r in csv.DictReader(Path('.recovery/2022-functions.csv').open()):
  if r['line'] and r['name']:
   source=r['source'].replace('cards/odyssey.c','cards/odissey.c');native[(source,r['name'])].add(int(r['line']))
 files=[];counts=collections.Counter()
 for p in sorted(list(Path('cards').glob('*.c'))+list(Path('functions').glob('*.c'))+list(Path('functions').glob('*.cpp'))):
  text=read(p);fs=functions(text);starts=[f['start'] for f in fs];comments=[]
  for m in COMMENT.finditer(text):
   d=card_description(m[0])
   if not d:continue
   j=bisect.bisect_right(starts,m.start())-1
   if j<0 or not fs[j]['start']<=m.start()<fs[j]['end']:comments.append(dict(d,start=m.start(),end=m.end()))
  rows=[]
  for f in fs:
   row=dict(f)
   line_set=native[(p.as_posix(),f['name'])]
   if len(line_set)==1:row['native_line']=next(iter(line_set))
   prefix=text[max(0,f['start']-250):f['start']];matches=list(MARKER.finditer(prefix))
   if matches and not prefix[matches[-1].end():].strip():
    mm=matches[-1];row['marker']=dict(address=mm['address'],source=mm['source'],line=int(mm['line']),start=max(0,f['start']-250)+mm.start());row['native_line']=int(mm['line']);counts['marked_functions']+=1
   if f['name'].startswith('card_'):
    counts['card_definitions']+=1;desc=first_description(text,f);row['has_description']=desc is not None
    if desc:counts['card_descriptions_present']+=1
    else:
     counts['card_descriptions_missing']+=1
     inside=[]
     for cm in COMMENT.finditer(text,f['brace']+1,f['end']):
      cd=card_description(cm[0])
      if cd:inside.append(dict(cd,start=cm.start(),end=cm.end()))
     candidates=[d for d in comments if d['key']==key(f['name'][5:])]
     if len(inside)==1:row['description_source']='existing_body';row['description']=inside[0];counts['restorable_from_existing_body']+=1
     elif len(candidates)==1:row['description_source']='placeholder';row['description']=candidates[0];counts['restorable_from_placeholder']+=1
     elif (p.as_posix(),f['name']) in original_by_function:row['description_source']='baseline_function';row['description']=original_by_function[(p.as_posix(),f['name'])];counts['restorable_from_baseline_function']+=1
     elif key(f['name'][5:]) in library:
      options=library[key(f['name'][5:])];local=[d for d in options if d['source']==p.as_posix()];row['description_source']='baseline_title';row['description']=(local or options)[0];counts['restorable_from_baseline_title']+=1
     else:
      card_key=key(f['name'][5:]);choices=oracle_cards.get(card_key,[])
      reviewed_alias=aliases.get((p.as_posix(),f['name']))
      if reviewed_alias:
       choices=oracle_cards.get(key(reviewed_alias['card_name']),[])
       if reviewed_alias.get('scryfall_id'):choices=[c for c in choices if c['scryfall_id']==reviewed_alias['scryfall_id']]
      if not choices:
       alias_key=re.sub(r'(?:_impl|_?2)$','',card_key)
       choices=oracle_cards.get(alias_key,[]) if alias_key!=card_key else []
      if len(choices)>1:
       wanted=reviewed_alias['card_name'] if reviewed_alias else f['name'][5:].replace('_',' ')
       exact=[c for c in choices if c['card_face'].casefold()==wanted.casefold()]
       if exact:choices=exact
      if len(choices)>1:
       primary=[c for c in choices if c['parent_card_name']==c['card_face'] and not c['parent_type_line'].startswith('Token')]
       if len(primary)==1:choices=primary
      if len(choices)==1:
       origin='scryfall_reviewed_name' if reviewed_alias else 'scryfall_oracle'
       row['description_source']=origin;row['description']=choices[0];counts['restorable_from_'+origin]+=1
      else:counts['needs_card_data_or_manual_name']+=1
   rows.append(row)
  files.append(dict(source=p.as_posix(),sha256=sha(p.read_bytes()),functions=rows,placeholders=comments))
 return dict(format=1,baseline=BASELINE,baseline_archive_sha256=sha(BASELINE_ZIP.read_bytes()),oracle_snapshot_sha256=sha(oracle.read_bytes()) if oracle else None,counts=dict(counts),files=files)

def tokens(text):
 return re.findall(LITERAL+r'|[A-Za-z_]\w*|0[xX][0-9a-fA-F]+|\d+|>>=|<<=|\.\.\.|->|\+\+|--|&&|\|\||==|!=|<=|>=|<<|>>|[+*/%&|^!-]=|\S',strip_comments(text))

def conditions(text):
 result=[];stack=[]
 for m in re.finditer(r'(?m)^[ \t]*#[ \t]*(if|ifdef|ifndef|elif|else|endif)\b[^\n]*',text):
  kind=m[1];line=m[0].strip()
  if kind in ('if','ifdef','ifndef'):stack.append([line])
  elif kind in ('elif','else'):
   assert stack,'Unmatched conditional';stack[-1].append(line)
  else:
   assert stack,'Unmatched endif';stack.pop()
  result.append((m.end(),tuple(tuple(s) for s in stack)))
 assert not stack,'Unclosed conditionals'
 return result

def context_at(contexts,at):
 i=bisect.bisect_right([p for p,c in contexts],at)-1
 return contexts[i][1] if i>=0 else ()

def wrap_condition(text,path):
 if not path:return text
 return '\n'.join(line for branch in path for line in branch)+'\n'+text+'\n'+'\n'.join('#endif' for branch in path)

def render_description(description):
 lines=description['text'][2:-2].strip().splitlines()
 cleaned=[re.sub(r'^\s*\* ?','',line).rstrip() for line in lines]
 return '  /* '+cleaned[0].lstrip()+'\n'+'\n'.join(('   * '+s).rstrip() for s in cleaned[1:])+' */'

def restore_body(text,row):
 body=text[row['start']:row['end']]
 if not row.get('description_source'):return body
 desc=row['description']
 if row.get('remove_descriptions'):
  for old in sorted(row['remove_descriptions'],key=lambda d:d['start'],reverse=True):
   a=old['start']-row['start'];b=old['end']-row['start'];body=body[:a]+body[b:]
 if row.get('remove_description'):
  old=row['remove_description'];a=old['start']-row['start'];b=old['end']-row['start'];body=body[:a]+body[b:]
 if row['description_source']=='existing_body':
  a=desc['start']-row['start'];b=desc['end']-row['start'];body=body[:a]+body[b:]
 brace=row['brace']-row['start']
 tail=re.sub(r'^(?:[ \t]*\n)+','',body[brace+1:])
 return body[:brace+1]+'\n'+render_description(desc)+'\n\n'+tail

def replace_ranges(text,changes):
 changes=sorted(changes)
 assert all(a[1]<=b[0] for a,b in zip(changes,changes[1:])), 'Overlapping source edits'
 for a,b,new in reversed(changes):text=text[:a]+new+text[b:]
 return text

def trim_gaps(text):
 fs=functions(text);changes=[];last=0
 for f in fs+[dict(start=len(text),end=len(text))]:
  gap=text[last:f['start']];clean=re.sub(r'\n(?:[ \t]*\n){2,}','\n\n',gap)
  if clean!=gap:changes.append((last,f['start'],clean))
  last=f['end']
 return replace_ranges(text,changes)

def standalone_comments(text):
 """Whole-line comments outside definitions, excluding literals and trailing notes."""
 fs=functions(text);ends=[r['end'] for r in fs]
 for m in re.finditer(LITERAL+r'|/\*.*?\*/|//[^\r\n]*',text,re.S):
  if not m[0].startswith(('/*','//')):continue
  i=bisect.bisect_right(ends,m.start())
  if i<len(fs) and fs[i]['start']<=m.start()<fs[i]['end']:continue
  a=text.rfind('\n',0,m.start())+1;b=text.find('\n',m.end())
  if text[a:m.start()].strip() or text[m.end():len(text) if b<0 else b].strip():continue
  yield m

def definition_prefix(text,row):
 """Include attached block documentation and simple opening conditional wrappers."""
 start=row['start']
 while True:
  end=start
  while end>0 and text[end-1].isspace():end-=1
  cm=next((m for m in reversed(list(COMMENT.finditer(text,0,end))) if m.end()==end),None)
  if cm is not None and not card_description(cm[0]):start=cm.start();continue
  line=text.rfind('\n',0,end)+1
  if re.fullmatch(r'[ \t]*#(?:if|ifdef|ifndef)\b[^\n]*',text[line:end]):start=line;continue
  return start

def restore_loose_comment_positions(text,original):
 """Repair comments stranded above definitions by an earlier layout pass.

 Match original whole-line comments, then anchor each to the next surviving
 definition in its original source. File headers and declaration notes stay put.
 The transformation moves comments only, preserving the complete token stream.
 """
 fs=functions(text);oldfs=functions(original)
 if not fs or not oldfs:return text,[]
 oldctx=conditions(original);ctx=conditions(text);byname=collections.defaultdict(list)
 for f in fs:byname[(f['name'],context_at(ctx,f['start']))].append(f)
 oldcomments=collections.defaultdict(list);current=collections.defaultdict(list)
 for m in standalone_comments(original):oldcomments[m[0]].append(m)
 for m in standalone_comments(text):
  if m.start()<fs[0]['start']:current[m[0]].append(m)
 changes=[];insertions=collections.defaultdict(list);receipt=[]
 for value,matches in current.items():
  if value.startswith('/* ManalinkEh 2022:') or value.startswith('/* Recovered October 2022') or card_description(value):continue
  origins=oldcomments[value]
  if len(origins)!=len(matches):continue
  for m,old in zip(matches,origins):
   if old.start()<oldfs[0]['start']:continue
   following=next((f for f in oldfs if f['start']>old.end()),None)
   if following is None:continue
   # A comment explaining a forward declaration belongs with that declaration.
   between=mask(original[old.end():following['start']])
   between=re.sub(r'(?m)^[ \t]*#(?:if|ifdef|ifndef|elif|else|endif)\b[^\n]*','',between)
   if between.strip() and '-->' not in value:continue
   targets=byname[(following['name'],context_at(oldctx,following['start']))]
   if len(targets)!=1:continue
   at=definition_prefix(text,targets[0]);changes.append((m.start(),m.end(),''));insertions[at].append((old.start(),value))
   receipt.append(dict(comment=value,original_line=original.count('\n',0,old.start())+1,before_function=following['name']))
 for at,comments in insertions.items():changes.append((at,at,'\n\n'.join(value for _,value in sorted(comments))+'\n\n'))
 out=trim_gaps(replace_ranges(text,changes))
 assert tokens(text)==tokens(out),'Loose-comment repair changed code'
 assert body_fingerprints(text)==body_fingerprints(out),'Loose-comment repair changed definitions or guards'
 return out,receipt

def arrange_simple_unit(text,file):
 """Order units whose non-function code consists only of includes/prototypes.

 Preserve each definition's original preprocessor path. Unknown card slots
 interpolate between neighboring known slots; no absolute line padding.
 """
 rows=file['functions']
 if not rows:return trim_gaps(text)
 assert all(r.get('native_line') is not None or r.get('layout_order') is not None for r in rows)
 contexts=conditions(text);changes=[];nodes=[];bycard={}
 for r in rows:
  body=restore_body(text,r);path=context_at(contexts,r['start'])
  start=r.get('marker',{}).get('start',r['start'])
  # Keep adjacent helper documentation and native provenance with its body.
  while True:
   end=start
   while end>0 and text[end-1].isspace():end-=1
   previous=next((m for m in reversed(list(COMMENT.finditer(text,0,end))) if m.end()==end),None)
   if previous is None or card_description(previous[0]) or previous[0].startswith('/* Recovered October 2022'):break
   start=previous.start()
  prefix=text[start:r['start']].strip()
  if not prefix and r.get('restore_marker'):
   m=r['restore_marker'];prefix=f"/* ManalinkEh 2022: {m['address']}; original {m['source']}:{m['line']}. */"
  node_text=prefix+'\n'+body if prefix else body
  if r['name'].startswith('card_'):bycard[key(r['name'][5:])]=r
  nodes.append(dict(order=float(r.get('native_line',r.get('layout_order'))),tie=r['start'],text=wrap_condition(node_text,path),name=r['name']))
  changes.append((start,r['end'],''))
 slots=[]
 for r in rows:
  if r['name'].startswith('card_') and not r.get('marker'):slots.append((r['start'],r.get('native_line',r.get('layout_order')),None))
 for comment in file['placeholders']:
  r=bycard.get(comment['key']);slots.append((comment['start'],r.get('native_line',r.get('layout_order')) if r else None,comment))
  changes.append((comment['start'],comment['end'],''))
 slots.sort(key=lambda s:s[0])
 for i,(at,line,comment) in enumerate(slots):
  if comment is None or comment['key'] in bycard:continue
  if comment['key'] in file.get('placeholder_order',{}):
   nodes.append(dict(order=file['placeholder_order'][comment['key']],tie=at,text=comment['text'],placeholder=comment['title']));continue
  previous=next(((j,slots[j][1]) for j in range(i-1,-1,-1) if slots[j][1] is not None),None)
  following=next(((j,slots[j][1]) for j in range(i+1,len(slots)) if slots[j][1] is not None),None)
  if previous and following:
   pi,pl=previous;ni,nl=following;order=pl+(nl-pl)*(i-pi)/(ni-pi)
  elif following:order=following[1]-(following[0]-i)*0.01
  elif previous:order=previous[1]+(i-previous[0])*0.01
  else:order=min(n['order'] for n in nodes)-1+i*0.000001
  nodes.append(dict(order=order,tie=at,text=comment['text'],placeholder=comment['title']))
 # Loose notes and section headings are ordering items too. Leaving them in
 # the remainder would collect all of them above the sorted definitions.
 for cm in standalone_comments(text):
  if cm.start()<rows[0]['start'] or any(a<cm.end() and b>cm.start() for a,b,_ in changes):continue
  if cm[0].startswith(('/* ManalinkEh 2022:','/* Recovered October 2022')):continue
  following=next((r for r in rows if r['start']>cm.end()),None)
  previous=next((r for r in reversed(rows) if r['end']<=cm.start()),None)
  neighbor=following or previous
  if neighbor is None:continue
  between=mask(text[cm.end():following['start']]) if following else ''
  between=re.sub(r'(?m)^[ \t]*#(?:if|ifdef|ifndef|elif|else|endif)\b[^\n]*','',between)
  if between.strip() and '-->' not in cm[0]:continue
  order=float(neighbor.get('native_line',neighbor.get('layout_order')))+(-0.0001 if following else 0.0001)
  nodes.append(dict(order=order,tie=cm.start(),text=wrap_condition(cm[0],context_at(contexts,cm.start())),loose_comment=True))
  changes.append((cm.start(),cm.end(),''))
 header=replace_ranges(text,changes)
 header=re.sub(r'/\* Recovered October 2022.*?\*/','',header,flags=re.S)
 # Definitions have moved; remove empty conditionals while retaining guarded
 # declarations. Only simple #ifndef SHANDALAR wrappers occur in these units.
 assert not re.search(r'(?m)^\s*#(?!include\b|if\b|ifdef\b|ifndef\b|elif\b|else\b|endif\b)\w+',header), 'Non-conditional preprocessor directives need dedicated placement'
 while True:
  stack=[];empty=[]
  for directive in re.finditer(r'(?m)^[ \t]*#[ \t]*(if|ifdef|ifndef|elif|else|endif)\b[^\n]*',header):
   if directive[1] in ('if','ifdef','ifndef'):stack.append(directive)
   elif directive[1]=='endif':
    start=stack.pop();middle=mask(header[start.end():directive.start()]);middle=re.sub(r'(?m)^\s*#(?:if|ifdef|ifndef|elif|else|endif)\b[^\n]*','',middle)
    if not middle.strip():empty.append((start.start(),directive.end(),''))
  outer=[r for r in empty if not any(s[0]<r[0] and s[1]>=r[1] for s in empty)]
  cleaned=replace_ranges(header,outer)
  if cleaned==header:break
  header=cleaned
 code=mask(header)
 code=re.sub(r'(?m)^\s*#(?:include|if|ifdef|ifndef|elif|else|endif)\b[^\n]*','',code)
 for prototype in re.finditer(r'(?m)^[ \t]*(?:[A-Za-z_]\w*[ \t\r\n*]+)+[A-Za-z_]\w*\s*\([^;{}]*\)\s*;',code):
  code=code[:prototype.start()]+' '*len(prototype[0])+code[prototype.end():]
 assert not code.strip(),('Global data/type declarations need dedicated placement',file['source'],code[:400])
 return trim_gaps(header).strip()+'\n\n'+'\n\n'.join(n['text'] for n in sorted(nodes,key=lambda n:(n['order'],n['tie'])))+'\n'

def body_fingerprints(text):
 contexts=conditions(text)
 return collections.Counter((f['name'],context_at(contexts,f['start']),sha(repr(tokens(text[f['start']:f['end']])).encode())) for f in functions(text))

def arrange_barrier_unit(text,file):
 """Sort definition slots between data/type/macro barriers.

 Gaps, prototypes, conditional directives and standalone comments stay in place.
 Only slots in identical conditional branches exchange bodies. File-local
 dependencies remain before callers, even where native line metadata disagrees.
 """
 rows=file['functions'];groups=[];group=[];previous=0;contexts=conditions(text)
 for r in rows:
  start=r.get('marker',{}).get('start',r['start'])
  while True:
   end=start
   while end>previous and text[end-1].isspace():end-=1
   cm=next((m for m in reversed(list(COMMENT.finditer(text,previous,end))) if m.end()==end),None)
   if cm is None or card_description(cm[0]) or cm[0].startswith('/* Recovered October 2022'):break
   start=cm.start()
  r=dict(r,attached_start=start,condition=context_at(contexts,r['start']))
  gap=mask(text[previous:start])
  gap=re.sub(r'(?m)^[ \t]*#(?:if|ifdef|ifndef|elif|else|endif)\b[^\n]*','',gap)
  gap=re.sub(r'(?m)^[ \t]*(?:[A-Za-z_]\w*[ \t\r\n*]+)+[A-Za-z_]\w*\s*\([^;{}]*\)\s*;','',gap)
  if gap.strip() and group:groups.append(group);group=[]
  group.append(r);previous=r['end']
 if group:groups.append(group)
 changes=[];report=[]
 for group in groups:
  bycontext=collections.defaultdict(list)
  for r in group:bycontext[r['condition']].append(r)
  for subset in bycontext.values():
   if len(subset)<2 or any(r.get('native_line',r.get('layout_order')) is None for r in subset):continue
   if len({r['name'] for r in subset})!=len(subset):continue
   names={r['name'] for r in subset};original={r['name']:i for i,r in enumerate(subset)}
   deps={r['name']:(set(re.findall(r'\b[A-Za-z_]\w*\b',mask(text[r['brace']+1:r['end']-1])))&names)-{r['name']} for r in subset}
   deps={n:{d for d in ds if original[d]<original[n]} for n,ds in deps.items()}
   pending=list(subset);ordered=[];done=set()
   while pending:
    ready=[r for r in pending if deps[r['name']]<=done]
    r=min(ready,key=lambda r:(r.get('native_line',r.get('layout_order')),r['start']))
    ordered.append(r);pending.remove(r);done.add(r['name'])
   if [r['name'] for r in subset]==[r['name'] for r in ordered]:continue
   for slot,node in zip(subset,ordered):
    if slot['name']!=node['name']:changes.append((slot['attached_start'],slot['end'],text[node['attached_start']:node['end']]))
   report.append(dict(before=[r['name'] for r in subset],after=[r['name'] for r in ordered]))
 file['barrier_regions_reordered']=report
 return trim_gaps(replace_ranges(text,changes))

def apply_inventory(plan,layout,evidence,batch='C743'):
 assert not evidence.exists(),'Evidence already exists'
 ledger_path=Path('docs/recovery-evidence/helper-canonicalization.json');ledger=json.loads(ledger_path.read_bytes())
 files=[];outputs={};comment_counts=collections.Counter();descriptions=[]
 for file in plan['files']:
  path=Path(file['source']);raw=path.read_bytes();assert sha(raw)==file['sha256'],str(path)
  text=read(path);layout_file=file['source'] in layout
  if layout_file:out=arrange_simple_unit(text,file)
  else:
   changes=[];consumed=set()
   for row in file['functions']:
    if row.get('description_source'):
     body=restore_body(text,row);changes.append((row['start'],row['end'],body))
     if row['description_source']=='placeholder':consumed.add((row['description']['start'],row['description']['end']))
   # A matched existing body comment makes its old standalone slot redundant.
   card_keys={key(r['name'][5:]) for r in file['functions'] if r['name'].startswith('card_')}
   consumed.update((c['start'],c['end']) for c in file['placeholders'] if c['key'] in card_keys)
   changes.extend((a,b,'') for a,b in consumed)
   out=trim_gaps(replace_ranges(text,changes))
  if file.get('barrier_layout'):
   refreshed=dict(file,functions=functions(out))
   old={r['name']:r for r in file['functions']}
   for r in refreshed['functions']:
    original=old[r['name']]
    for k in ['native_line','layout_order']:
     if k in original:r[k]=original[k]
    prefix=out[max(0,r['start']-250):r['start']];matches=list(MARKER.finditer(prefix))
    if matches and not prefix[matches[-1].end():].strip():r['marker']=dict(start=max(0,r['start']-250)+matches[-1].start())
   out=arrange_barrier_unit(out,refreshed);file['barrier_regions_reordered']=refreshed.get('barrier_regions_reordered',[])
  if file.get('forward_declarations'):
   pos=re.search(r'(?m)^#include[^\n]*\n',out).end()
   out=out[:pos]+'\n'+'\n'.join(file['forward_declarations'])+'\n'+out[pos:]
  if out==text:continue
  assert body_fingerprints(text)==body_fingerprints(out),('Function tokens/guards changed',str(path))
  if not layout_file and not file.get('barrier_layout') and not file.get('forward_declarations'):assert tokens(text)==tokens(out),('Non-comment tokens changed',str(path))
  for row in file['functions']:
   if row.get('description_source'):
    comment_counts[row['description_source']]+=1
    descriptions.append(dict(source=str(path).replace('\\','/'),name=row['name'],origin=row['description_source'],description=row['description']))
  out_raw=out.replace('\n','\r\n').encode('latin1');outputs[path]=out_raw
  before_functions=functions(text);after_functions=functions(out)
  file_record=dict(source=file['source'],before_bytes_sha256=sha(raw),after_bytes_sha256=sha(out_raw),before_canonical_sha256=sha(text.encode()),after_canonical_sha256=sha(out.encode()),functions=len(before_functions),layout_reordered=layout_file,function_tokens_and_conditions_unchanged=True)
  if layout_file:file_record['function_order_before']=[f['name'] for f in before_functions];file_record['function_order_after']=[f['name'] for f in after_functions]
  if file.get('barrier_layout'):file_record['barrier_regions_reordered']=file['barrier_regions_reordered']
  if file.get('forward_declarations'):file_record['forward_declarations']=file['forward_declarations']
  files.append(file_record)
  pin=next((f for f in ledger['files'] if f['source']==file['source']),None)
  if pin:
   assert pin['canonical_sha256']==sha(text.encode()),file['source'];pin['canonical_sha256']=sha(out.encode())
  else:ledger['files'].append(dict(source=file['source'],predecessor_sha256=sha(text.encode()),canonical_sha256=sha(out.encode()),source_layout_comments_only=True))
 record=dict(format=1,batch=batch,status='Applied; grouped build pending.',base_commit=subprocess.check_output(['git','rev-parse','HEAD'],text=True).strip(),files=files,comment_counts=dict(comment_counts),descriptions=descriptions,layout_files=sorted(layout),inventory_counts=plan['counts'],baseline=plan['baseline'],baseline_archive_sha256=plan['baseline_archive_sha256'],oracle_snapshot_sha256=plan['oracle_snapshot_sha256'],tool_sha256=sha(Path(__file__).read_bytes()),tool_source=Path(__file__).read_text(encoding='utf-8-sig'),scope='Historical source comments take precedence; Scryfall fallback uses the pinned current Oracle snapshot as documentation only. Function token streams, literals and enclosing conditional branches are unchanged. Original 2022 line metadata orders functions in the selected simple units; unimplemented comment slots interpolate between known original anchors. Functions without a native line may carry an explicit recorded relative order inferred from baseline/neighbor anchors. Other units receive comment restoration and top-level blank-gap cleanup only. No exact line-number padding or native-behavior edits.')
 record['relative_order_inferences']=plan.get('relative_order_inferences',[])
 backup=Path(f'.recovery/source-layout-before-{batch.lower()}.zip');assert not backup.exists()
 with zipfile.ZipFile(backup,'w',compression=zipfile.ZIP_DEFLATED) as archive:
  for path in outputs:archive.writestr(path.as_posix(),path.read_bytes())
 record['before_archive_sha256']=sha(backup.read_bytes())
 tmp=Path('.recovery/layout-write-c743.tmp')
 for path,raw in outputs.items():tmp.write_bytes(raw);tmp.replace(path)
 evidence.write_bytes(gzip.compress((json.dumps(record,indent=2)+'\n').encode(),mtime=0))
 ledger['interface_supporting_inputs'].append(dict(path=evidence.as_posix(),sha256=sha(evidence.read_bytes())))
 ledger.setdefault('source_layout_batches',[]).append(dict(path=evidence.as_posix(),batch=batch,comment_counts=dict(comment_counts),layout_files=sorted(layout)))
 tmp.write_bytes((json.dumps(ledger,separators=(',',':'))+'\n').encode());tmp.replace(ledger_path)
 print(json.dumps(dict(changed_files=len(files),comment_counts=dict(comment_counts),layout_files=sorted(layout)),indent=2))

def main():
 parser=argparse.ArgumentParser(description=__doc__);mode=parser.add_mutually_exclusive_group(required=True);mode.add_argument('--inventory',type=Path);mode.add_argument('--apply',type=Path);parser.add_argument('--oracle',type=Path);parser.add_argument('--layout',nargs='*',default=[]);parser.add_argument('--evidence',type=Path);parser.add_argument('--batch',default='C743');args=parser.parse_args()
 if args.inventory:
  result=inventory(args.oracle);args.inventory.write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(result['counts'],indent=2))
 else:
  assert args.evidence,'--evidence is required with --apply';apply_inventory(json.loads(args.apply.read_bytes()),set(args.layout),args.evidence,args.batch)

if __name__=='__main__':main()
