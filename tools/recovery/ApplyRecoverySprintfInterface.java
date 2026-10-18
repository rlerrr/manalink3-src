// The exact FF25 import thunk was matched to msvcrt.dll!sprintf in the PE.
// @category Manalink
import ghidra.app.script.GhidraScript;
import ghidra.program.model.data.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.SourceType;
import java.util.*;
public class ApplyRecoverySprintfInterface extends GhidraScript {
    public void run() throws Exception {
        currentProgram.getOptions("Manalink recovery").setBoolean("ExternalInterfacesReady", false);
        byte[] expected = {(byte)0xff, 0x25, (byte)0xf0, 0x54, 0x7b, 0x02};
        byte[] actual = getBytes(toAddr(0x2701df0), 6);
        if (!Arrays.equals(expected, actual)) throw new Exception("sprintf import thunk mismatch");
        Function function = getFunctionAt(toAddr(0x2701df0));
        if (function == null) function = createFunction(toAddr(0x2701df0), "sprintf");
        if (function == null) throw new Exception("Cannot define sprintf import thunk");
        function.setName("sprintf", SourceType.IMPORTED);
        function.setCallingConvention("__cdecl");
        function.setReturnType(IntegerDataType.dataType, SourceType.IMPORTED);
        DataType pointer = new PointerDataType(CharDataType.dataType, currentProgram.getDataTypeManager());
        List<Variable> parameters = new ArrayList<>();
        parameters.add(new ParameterImpl("buffer", pointer, currentProgram));
        parameters.add(new ParameterImpl("format", pointer, currentProgram));
        function.replaceParameters(parameters, Function.FunctionUpdateType.DYNAMIC_STORAGE_ALL_PARAMS,
                                   true, SourceType.IMPORTED);
        function.setVarArgs(true);
        function.setInline(false);
        currentProgram.getOptions("Manalink recovery").setBoolean("ExternalInterfacesReady", true);
        println("RECOVERY_SPRINTF_INTERFACE_COMPLETE " + function.getSignature());
    }
}
