// Keep legacy source APIs separate from the concrete 2022 source-aware entries.
// @category Manalink
import ghidra.app.script.GhidraScript;
import ghidra.program.model.data.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.SourceType;
import java.util.*;

public class ApplyRecoveryCounterRemoval extends GhidraScript {
    public void run() throws Exception {
        long[] addresses = {0x026ceef0, 0x026d48d0, 0x026d4c10, 0x026d4c50};
        String[] names = {"clear_all_counters_2022", "remove_counters_from_source_2022_impl",
                          "remove_counters_from_source", "remove_all_counters_from_source"};
        for (long address : addresses)
            if (getFunctionAt(toAddr(address)) == null) throw new Exception("Missing counter entry");
        for (int i = 0; i < addresses.length; ++i)
            getFunctionAt(toAddr(addresses[i])).setName(names[i], SourceType.IMPORTED);
        Function clear = getFunctionAt(toAddr(addresses[0]));
        clear.setCustomVariableStorage(false);
        clear.setCallingConvention("__regparm2");
        List<Variable> parameters = new ArrayList<>();
        parameters.add(new ParameterImpl("t_player", IntegerDataType.dataType, currentProgram));
        parameters.add(new ParameterImpl("t_card", IntegerDataType.dataType, currentProgram));
        clear.replaceParameters(parameters, Function.FunctionUpdateType.DYNAMIC_STORAGE_ALL_PARAMS,
                                true, SourceType.IMPORTED);
        for (Parameter parameter : clear.getParameters())
            println("RECOVERY_COUNTER_CLEAR_PARAMETER " + parameter.getName() + " " + parameter.getVariableStorage());
        println("RECOVERY_COUNTER_REMOVAL_COMPLETE count=4");
    }
}
