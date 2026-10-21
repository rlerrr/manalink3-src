// Concrete DWARF proves the private Level Up entry omits l1power/l1toughness.
// Buddy's sole direct caller discards the private result and returns zero.
// @category Manalink
import ghidra.app.script.GhidraScript;
import ghidra.program.model.data.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.*;
import java.util.*;

public class ApplyRecoveryLevelBuddy extends GhidraScript {
    public void run() throws Exception {
        Function level=getFunctionAt(toAddr(0x24ac850));
        Function buddy=getFunctionAt(toAddr(0x258e270));
        if(level==null || buddy==null)throw new Exception("Missing private mechanic entry");
        Map<String,DataType> types=new HashMap<>();
        for(Parameter p:level.getParameters())types.put(p.getName(),p.getDataType());
        String[] names={"player","card","event","l2","l2power","l2toughness","l2key","l2sp",
                        "l3","l3power","l3toughness","l3key","l3sp"};
        Parameter[] parameters=new Parameter[names.length];
        for(int i=0;i<names.length;i++) {
            if(!types.containsKey(names[i]))throw new Exception("Missing original Level Up type "+names[i]);
            parameters[i]=new ParameterImpl(names[i],types.get(names[i]),currentProgram);
        }
        level.setCustomVariableStorage(false);
        level.setName("level_up_effects_part_2022",SourceType.IMPORTED);
        level.setCallingConvention("__regparm3");
        level.setReturnType(VoidDataType.dataType,SourceType.IMPORTED);
        level.replaceParameters(Function.FunctionUpdateType.DYNAMIC_STORAGE_ALL_PARAMS,true,SourceType.IMPORTED,parameters);
        int callers=0;
        for(Reference reference:getReferencesTo(buddy.getEntryPoint())) {
            if(!reference.getReferenceType().isCall())continue;
            Function caller=getFunctionContaining(reference.getFromAddress());
            println("RECOVERY_BUDDY_PRIVATE_REFERENCE "+reference.getFromAddress()+" "+caller);
            if(!reference.getFromAddress().equals(toAddr(0x258f70c)) || caller==null ||
               !caller.getEntryPoint().equals(toAddr(0x258f6c0)))
                throw new Exception("Unverified Buddy private caller "+reference);
            callers++;
        }
        if(callers!=1)throw new Exception("Expected one Buddy private caller; got "+callers);
        buddy.setName("buddy_creature_part_2022",SourceType.IMPORTED);
        buddy.setReturnType(VoidDataType.dataType,SourceType.IMPORTED);
        println("RECOVERY_LEVEL_BUDDY_COMPLETE level_physical=13 level_nonphysical=2 buddy_physical=5 buddy_result=discarded");
    }
}
