// DWARF proves for_life=1 has no storage; retain six physical arguments.
// @category Manalink
import ghidra.app.script.GhidraScript;
import ghidra.program.model.data.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.*;

public class ApplyRecoveryCannotLosePart extends GhidraScript {
    public void run() throws Exception {
        Function worker=getFunctionAt(toAddr(0x26a3060));
        Function wrapper=getFunctionAt(toAddr(0x26a33c0));
        if(worker==null || wrapper==null)throw new Exception("Missing cannot-lose worker or wrapper");
        DataType integer=IntegerDataType.dataType;
        String[] names={"player","card","event","who_cant_lose","for_poison","for_draw_on_empty_deck"};
        Parameter[] parameters=new Parameter[6];
        for(int i=0;i<6;i++)parameters[i]=new ParameterImpl(names[i],i==2?wrapper.getParameter(2).getDataType():integer,currentProgram);
        worker.setCustomVariableStorage(false);
        worker.setCallingConvention("__regparm3");
        worker.setName("cannot_lose_the_game_impl_2022",SourceType.IMPORTED);
        worker.setReturnType(VoidDataType.dataType,SourceType.IMPORTED);
        worker.replaceParameters(Function.FunctionUpdateType.DYNAMIC_STORAGE_ALL_PARAMS,true,SourceType.IMPORTED,parameters);
        println("RECOVERY_CANNOT_LOSE_PART_COMPLETE physical=6 registers=3 stack_words=3 for_life=1");
    }
}
