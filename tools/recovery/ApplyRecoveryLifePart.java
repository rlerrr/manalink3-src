// Identify the private can-lose compiler part before exporting its public wrapper.
// @category Manalink
import ghidra.app.script.GhidraScript;
import ghidra.program.model.data.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.*;

public class ApplyRecoveryLifePart extends GhidraScript {
    public void run() throws Exception {
        Function worker=getFunctionAt(toAddr(0x2685c90));
        if(worker==null)throw new Exception("Missing private can-lose worker");
        byte[] expected=java.util.HexFormat.of().parseHex("e98a34ffff");
        if(!java.util.Arrays.equals(getBytes(toAddr(0x2692801),5),expected))
            throw new Exception("Unexpected public can-lose tail destination");
        worker.setCustomVariableStorage(false);
        worker.setCallingConvention("__regparm1");
        worker.setName("can_lose_the_game_impl_2022",SourceType.IMPORTED);
        worker.setReturnType(IntegerDataType.dataType,SourceType.IMPORTED);
        worker.replaceParameters(Function.FunctionUpdateType.DYNAMIC_STORAGE_ALL_PARAMS,true,SourceType.IMPORTED,
            new ParameterImpl("player",IntegerDataType.dataType,currentProgram));
        println("RECOVERY_LIFE_PART_COMPLETE physical=1 register=EAX");
    }
}
