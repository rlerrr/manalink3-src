// The concrete DWARF parameter amount=1 is constant, not an ABI argument.
// @category Manalink
import ghidra.app.script.GhidraScript;
import ghidra.program.model.data.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.SourceType;
import java.util.*;

public class ApplyRecoverySpiritSpecialization extends GhidraScript {
    public void run() throws Exception {
        Function function = getFunctionAt(toAddr(0x023c8990));
        if (function == null) throw new Exception("Missing specialized Spirit helper");
        function.setCustomVariableStorage(false);
        function.setCallingConvention("__regparm2");
        function.setName("make_one_spirit_strixhaven", SourceType.IMPORTED);
        function.setReturnType(VoidDataType.dataType, SourceType.IMPORTED);
        List<Variable> parameters = new ArrayList<>();
        parameters.add(new ParameterImpl("player", IntegerDataType.dataType, currentProgram));
        parameters.add(new ParameterImpl("card", IntegerDataType.dataType, currentProgram));
        function.replaceParameters(parameters, Function.FunctionUpdateType.DYNAMIC_STORAGE_ALL_PARAMS,
                                   true, SourceType.IMPORTED);
        function.setInline(false);
        for (Parameter parameter : function.getParameters()) {
            println("RECOVERY_SPIRIT_PARAMETER " + parameter.getName() + " " + parameter.getVariableStorage());
        }
        println("RECOVERY_SPIRIT_SPECIALIZATION_COMPLETE quantity=1");
    }
}
