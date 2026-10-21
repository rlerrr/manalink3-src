// Apply physical parameters proved by concrete keyword-helper DWARF locations.
// @category Manalink
import ghidra.app.script.GhidraScript;
import ghidra.program.model.data.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.SourceType;

public class ApplyRecoveryAbilityParts extends GhidraScript {
    public void run() throws Exception {
        long[] addresses={0x26dd080,0x26dd0c0,0x26dd110,0x26dd150,0x26dd1a0};
        String[] names={"deathtouch_part_2022","hexproof_part_2022","indestructible_part_2022","attack_if_able_part_2022","vigilance_part_2022"};
        for(int i=0;i<addresses.length;i++) {
            Function f=getFunctionAt(toAddr(addresses[i]));
            if(f==null)throw new Exception("Missing keyword private entry "+names[i]);
            f.setCustomVariableStorage(false);
            f.setName(names[i],SourceType.IMPORTED);
            f.setCallingConvention("__regparm2");
            f.replaceParameters(Function.FunctionUpdateType.DYNAMIC_STORAGE_ALL_PARAMS,true,SourceType.IMPORTED,
                new ParameterImpl("player",IntegerDataType.dataType,currentProgram),
                new ParameterImpl("card",IntegerDataType.dataType,currentProgram));
        }
        Function flanking=getFunctionAt(toAddr(0x26df8e0));
        if(flanking==null)throw new Exception("Missing granted-flanking private entry");
        flanking.setCustomVariableStorage(false);
        flanking.setName("granted_flanking_part_2022",SourceType.IMPORTED);
        flanking.setCallingConvention("__regparm3");
        flanking.replaceParameters(Function.FunctionUpdateType.DYNAMIC_STORAGE_ALL_PARAMS,true,SourceType.IMPORTED,
            new ParameterImpl("s_player",IntegerDataType.dataType,currentProgram),
            new ParameterImpl("s_card",IntegerDataType.dataType,currentProgram),
            new ParameterImpl("t_player",IntegerDataType.dataType,currentProgram),
            new ParameterImpl("t_card",IntegerDataType.dataType,currentProgram));
        getFunctionAt(toAddr(0x26dcca0)).setName("lure_effect_part_2022",SourceType.IMPORTED);
        getFunctionAt(toAddr(0x26dcfb0)).setName("lure_effect_2022",SourceType.IMPORTED);
        println("RECOVERY_ABILITY_PARTS_COMPLETE keyword_parts=5 physical_parameters=2 flanking=4 lure=9");
    }
}
