// Separate the two-argument GCC keyword worker from its event-gated wrapper.
// @category Manalink
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.*;
import ghidra.program.model.data.*;
import ghidra.program.model.symbol.SourceType;

public class ApplyRecoveryKeywordWorker extends GhidraScript {
    public void run() throws Exception {
        Function worker = getFunctionAt(toAddr(0x26dd130));
        Function wrapper = getFunctionAt(toAddr(0x26de880));
        if (worker == null || wrapper == null ||
            worker.getBody().getNumAddresses() != 32 || wrapper.getParameterCount() != 3)
            throw new Exception("Unexpected misc protection worker/wrapper");
        worker.setName("set_misc_protection_2022_impl", SourceType.IMPORTED);
        worker.setReturnType(VoidDataType.dataType, SourceType.IMPORTED);
        worker.setCallingConvention("__regparm2");
        worker.replaceParameters(Function.FunctionUpdateType.DYNAMIC_STORAGE_ALL_PARAMS, true,
            SourceType.IMPORTED,
            new ParameterImpl("player", IntegerDataType.dataType, currentProgram),
            new ParameterImpl("card", IntegerDataType.dataType, currentProgram));
        println("RECOVERY_KEYWORD_WORKER_COMPLETE private=2 public=3");
    }
}
