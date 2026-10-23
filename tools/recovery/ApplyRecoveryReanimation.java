// Drop source-only GCC parameter references from the concrete mass-reanimation worker.
// @category Manalink
import ghidra.app.script.GhidraScript;
import ghidra.program.model.data.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.SourceType;

public class ApplyRecoveryReanimation extends GhidraScript {
    public void run() throws Exception {
        Function worker=getFunctionAt(toAddr(0x269ba70));
        Function wrapper=getFunctionAt(toAddr(0x269c810));
        if(worker==null || wrapper==null || wrapper.getParameterCount()!=5)
            throw new Exception("Missing native reanimation interfaces");
        byte[] prologue=java.util.HexFormat.of().parseHex("5589e557565350b86c200000e84f07060029c4");
        if(!java.util.Arrays.equals(getBytes(toAddr(0x269ba70),prologue.length),prologue))
            throw new Exception("Unexpected mass-reanimation prologue/stack probe");
        String[] names={"player","card","targ_player","this_test","action"};
        Parameter[] parameters=new Parameter[5];
        for(int i=0;i<5;i++)parameters[i]=new ParameterImpl(names[i],wrapper.getParameter(i).getDataType(),currentProgram);
        worker.setCustomVariableStorage(false);
        worker.setCallingConvention("__regparm3");
        worker.replaceParameters(Function.FunctionUpdateType.DYNAMIC_STORAGE_ALL_PARAMS,true,SourceType.IMPORTED,parameters);
        println("RECOVERY_REANIMATION_COMPLETE physical=5 registers=3 stack_words=2 source_only_parameters=3");
    }
}
