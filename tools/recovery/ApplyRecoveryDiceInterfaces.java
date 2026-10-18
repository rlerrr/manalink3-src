// @category Manalink
import ghidra.app.script.GhidraScript;
import ghidra.program.model.data.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.SourceType;
import java.util.*;

public class ApplyRecoveryDiceInterfaces extends GhidraScript {
    public void run() throws Exception {
        Function abort = getFunctionAt(toAddr(0x26c11b0));
        if (abort == null || !abort.getName().equals("backtrace_and_abort"))
            throw new Exception("Missing terminating backtrace helper");
        byte[] expected = java.util.HexFormat.of().parseHex("ff25e4547b02");
        if (!Arrays.equals(expected, getBytes(toAddr(0x2701e00), 6)))
            throw new Exception("qsort import thunk mismatch");
        DataTypeManager manager = currentProgram.getDataTypeManager();
        DataType pointer = new PointerDataType(VoidDataType.dataType, manager);
        FunctionDefinitionDataType comparator = new FunctionDefinitionDataType("RecoveryQsortComparator");
        comparator.setReturnType(IntegerDataType.dataType);
        comparator.setArguments(new ParameterDefinition[] {
            new ParameterDefinitionImpl("lhs", pointer, null),
            new ParameterDefinitionImpl("rhs", pointer, null)
        });
        DataType callback = new PointerDataType(manager.resolve(comparator, DataTypeConflictHandler.DEFAULT_HANDLER), manager);
        Function qsort = getFunctionAt(toAddr(0x2701e00));
        if (qsort == null) qsort = createFunction(toAddr(0x2701e00), "qsort");
        if (qsort == null) throw new Exception("Cannot create qsort");
        qsort.setName("qsort", SourceType.IMPORTED);
        qsort.setCallingConvention("__cdecl");
        qsort.setReturnType(VoidDataType.dataType, SourceType.IMPORTED);
        qsort.replaceParameters(Arrays.asList(
            new ParameterImpl("base", pointer, currentProgram),
            new ParameterImpl("count", UnsignedIntegerDataType.dataType, currentProgram),
            new ParameterImpl("size", UnsignedIntegerDataType.dataType, currentProgram),
            new ParameterImpl("compare", callback, currentProgram)),
            Function.FunctionUpdateType.DYNAMIC_STORAGE_ALL_PARAMS, true, SourceType.IMPORTED);
        qsort.setVarArgs(false);
        qsort.setInline(false);
        abort.setNoReturn(true);
        println("RECOVERY_DICE_INTERFACES_COMPLETE count=2");
    }
}
