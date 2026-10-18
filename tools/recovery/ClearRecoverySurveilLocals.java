// Remove an aggregate hint that incorrectly covers restored outgoing arguments.
// @category Manalink
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.*;
public class ClearRecoverySurveilLocals extends GhidraScript {
    public void run() throws Exception {
        Function function = getFunctionAt(toAddr(0x26198e0));
        if (function == null || !function.getName().equals("surveil"))
            throw new Exception("Missing Surveil function");
        int count = 0;
        for (Variable local : function.getLocalVariables()) {
            println("RECOVERY_SURVEIL_LOCAL_REMOVED " + local.getName() + " " + local.getVariableStorage());
            function.removeVariable(local); ++count;
        }
        println("RECOVERY_SURVEIL_LOCALS_COMPLETE count=" + count);
    }
}
