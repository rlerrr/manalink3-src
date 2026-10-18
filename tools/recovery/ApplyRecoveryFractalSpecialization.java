// Concrete DWARF amount=1 has no ABI storage; the third register holds counters.
// @category Manalink
import ghidra.app.script.GhidraScript;
import ghidra.program.model.data.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.SourceType;
import java.util.*;

public class ApplyRecoveryFractalSpecialization extends GhidraScript {
    public void run() throws Exception {
        Function function = getFunctionAt(toAddr(0x023c8b60));
        if (function == null) throw new Exception("Missing specialized Fractal helper");
        function.setCustomVariableStorage(false);
        function.setCallingConvention("__regparm3");
        function.setName("make_one_fractal", SourceType.IMPORTED);
        function.setReturnType(VoidDataType.dataType, SourceType.IMPORTED);
        List<Variable> parameters = new ArrayList<>();
        for (String name : new String[]{"player", "card", "number_of_p1_p1_counters"}) {
            parameters.add(new ParameterImpl(name, IntegerDataType.dataType, currentProgram));
        }
        function.replaceParameters(parameters, Function.FunctionUpdateType.DYNAMIC_STORAGE_ALL_PARAMS,
                                   true, SourceType.IMPORTED);
        function.setInline(false);
        for (Parameter parameter : function.getParameters()) {
            println("RECOVERY_FRACTAL_PARAMETER " + parameter.getName() + " " + parameter.getVariableStorage());
        }
        println("RECOVERY_FRACTAL_SPECIALIZATION_COMPLETE quantity=1");
    }
}
