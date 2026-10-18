// The concrete historic-spell clone has no ABI storage for trigger_mode.
// @category Manalink
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.SourceType;
import java.util.*;

public class ApplyRecoveryHistoricInterface extends GhidraScript {
    public void run() throws Exception {
        Function function = getFunctionAt(toAddr(0x022ca3f0));
        if (function == null || function.getParameterCount() < 3)
            throw new Exception("Missing historic-spell clone");
        List<Variable> parameters = new ArrayList<>();
        for (int i = 0; i < 3; ++i) {
            Parameter parameter = function.getParameter(i);
            parameters.add(new ParameterImpl(parameter.getName(), parameter.getDataType(), currentProgram));
        }
        function.setCustomVariableStorage(false);
        function.setCallingConvention("__regparm3");
        function.replaceParameters(parameters, Function.FunctionUpdateType.DYNAMIC_STORAGE_ALL_PARAMS,
                                   true, SourceType.IMPORTED);
        function.setName("cast_historic_spell_2022", SourceType.IMPORTED);
        for (Parameter parameter : function.getParameters())
            println("RECOVERY_HISTORIC_PARAMETER " + parameter.getName() + " " + parameter.getVariableStorage());
        println("RECOVERY_HISTORIC_INTERFACE_COMPLETE parameters=3");
    }
}
