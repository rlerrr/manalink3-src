/* Inspection pilot only. Parameter types supplied from DWARF; event is represented as int.
   Callee prototypes and other types have not been imported. This is not buildable recovered source. */

/* card_black_lotus @ 024bf4e0, 49 bytes */

int __cdecl card_black_lotus(int player,int card,int event)

{
  int iVar1;
  
  iVar1 = func_0x0266c7a0(player,card,event,3,1);
  return iVar1;
}



/* card_winter_orb @ 024cff90, 84 bytes */

int __cdecl card_winter_orb(int player,int card,int event)

{
  int iVar1;
  
  iVar1 = func_0x004e81fe(player,card);
  if (iVar1 != 0) {
    return 0;
  }
  func_0x024bdef0(player,card,event,2,1);
  return 0;
}



/* card_professor_onyx @ 023d7d50, 948 bytes */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int __cdecl card_professor_onyx(int player,int card,int event)

{
  undefined4 uVar1;
  char cVar2;
  int iVar3;
  int iStack_f4;
  undefined1 auStack_e8 [216];
  
  func_0x02663890(player,card,event);
  func_0x026a8160(player,event,2,0x38,0,player,card);
  if (event == 0x1e00) {
    func_0x0268f1b0(1 - player,2);
    func_0x0268f900(player,2);
    goto LAB_023d7e98;
  }
  if ((event == 0x6d) || (event == 0x73)) {
    cVar2 = (6 < *(int *)(&DAT_004ef528 + player * 4)) * '\x05' + '\x05';
LAB_023d7ed1:
    iVar3 = func_0x026e9c50(player,card,event,&UNK_02746dd2,&UNK_027476a9,&UNK_02746ed3,
                            &UNK_02747b6a,1,cVar2,1,&UNK_02747b4c,1,0x14,0xfffffffd,&UNK_02747b30,1,
                            0x32,0xfffffff8,0,0,0,0);
    if (event == 0x73) {
      if (iVar3 == 0) {
        return 0;
      }
      goto LAB_023d7e98;
    }
  }
  else {
    if ((event != 0x72) && (event != 0x22)) goto LAB_023d7e98;
    cVar2 = '\x05';
    if (*(int *)(&DAT_004ef528 + player * 4) < 7) goto LAB_023d7ed1;
    iVar3 = func_0x026e9c50(player,card,event,&UNK_02746dd2,&UNK_027476a9,&UNK_02746ed3,
                            &UNK_02747b6a,1,10,1,&UNK_02747b4c,1,0x14,0xfffffffd,&UNK_02747b30,1,
                            0x32,0xfffffff8,0,0,0,0);
  }
  uVar1 = _DAT_004ef194;
  if (event == 0x72) {
    if (iVar3 == 1) {
      func_0x0268f1b0(player,1);
      func_0x026bea60(auStack_e8,0xff,&UNK_02747064);
      func_0x02655580(player,card,player,3,1,1,0,3,0,auStack_e8);
    }
    else if (iVar3 == 2) {
      func_0x021a3da0(player,card,1 - player);
    }
    else if (iVar3 == 3) {
      iStack_f4 = 0x15;
      iVar3 = 1 - player;
      while (*(int *)(&DAT_004ef1c4 + iVar3 * 4) != 0) {
        _DAT_004ef194 = 0;
        func_0x0269a440(iVar3,6,player);
        if ((_DAT_004ef194 == 1) || (iStack_f4 = iStack_f4 + -3, iStack_f4 == 0)) break;
      }
      _DAT_004ef194 = uVar1;
      func_0x0268f1b0(iVar3,iStack_f4);
    }
  }
LAB_023d7e98:
  iVar3 = func_0x0215f420(player,card,event,5);
  return iVar3;
}


