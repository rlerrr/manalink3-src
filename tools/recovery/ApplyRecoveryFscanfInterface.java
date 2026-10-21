// The runtime FF25 thunk and IAT entry match msvcrt.dll!fscanf.
// @category Manalink
import java.util.*;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.data.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.SourceType;
import ghidra.util.data.DataTypeParser;
import ghidra.util.data.DataTypeParser.AllowedDataTypes;

public class ApplyRecoveryFscanfInterface extends GhidraScript {
    public void run() throws Exception {
        byte[] expected = {(byte)0xff, 0x25, (byte)0xb4, 0x54, 0x7b, 0x02};
        if (!Arrays.equals(expected, getBytes(toAddr(0x2701e58), 6)))
            throw new Exception("fscanf import thunk mismatch");
        DataTypeManager manager = currentProgram.getDataTypeManager();
        DataTypeParser parser = new DataTypeParser(manager, manager, null, AllowedDataTypes.ALL);
        DataType filePointer = parser.parse("FILE *");
        DataType charPointer = new PointerDataType(CharDataType.dataType, manager);
        Function function = getFunctionAt(toAddr(0x2701e58));
        if (function == null) function = createFunction(toAddr(0x2701e58), "fscanf");
        if (function == null) throw new Exception("Cannot create fscanf import thunk");
        function.setName("fscanf", SourceType.IMPORTED);
        function.setCallingConvention("__cdecl");
        function.setReturnType(IntegerDataType.dataType, SourceType.IMPORTED);
        List<Variable> parameters = new ArrayList<>();
        parameters.add(new ParameterImpl("stream", filePointer, currentProgram));
        parameters.add(new ParameterImpl("format", charPointer, currentProgram));
        function.replaceParameters(parameters, Function.FunctionUpdateType.DYNAMIC_STORAGE_ALL_PARAMS, true, SourceType.IMPORTED);
        function.setVarArgs(true);
        function.setInline(false);
        println("RECOVERY_FSCANF_INTERFACE_COMPLETE " + function.getSignature());
    }
}
