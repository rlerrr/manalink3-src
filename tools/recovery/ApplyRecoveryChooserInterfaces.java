// Correct the scalar C++ member return and its concrete ECX this parameter.
// @category Manalink
import java.util.*;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.data.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.*;
public class ApplyRecoveryChooserInterfaces extends GhidraScript {
    public void run() throws Exception {
        Function f = getFunctionAt(toAddr(0x26eea80L));
        f.setCallingConvention("__thiscall");
        f.setReturnType(IntegerDataType.dataType, SourceType.IMPORTED);
        f.setCustomVariableStorage(true);
        List<Variable> args = new ArrayList<>();
        args.add(new ParameterImpl("self", new PointerDataType(VoidDataType.dataType),
                 new VariableStorage(currentProgram, currentProgram.getRegister("ECX")), currentProgram));
        f.replaceParameters(args, Function.FunctionUpdateType.CUSTOM_STORAGE, true, SourceType.IMPORTED);
        getFunctionAt(toAddr(0x26c11b0L)).setNoReturn(true);
        println("RECOVERY_CHOOSER_MEMBER " + f.getSignature() + " " + f.getParameters()[0].getVariableStorage());
    }
}
