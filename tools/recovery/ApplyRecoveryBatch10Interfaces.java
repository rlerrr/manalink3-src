// Resolve public/private name collisions and a constant-only compiler clone.
// @category Manalink
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.*;
import ghidra.program.model.data.IntegerDataType;
import ghidra.program.model.symbol.SourceType;
import java.util.*;

public class ApplyRecoveryBatch10Interfaces extends GhidraScript {
    public void run() throws Exception {
        Function cycling = getFunctionAt(toAddr(0x0222bd80));
        Function commander = getFunctionAt(toAddr(0x022c64f0));
        Function color = getFunctionAt(toAddr(0x02488800));
        if (cycling == null || commander == null || color == null ||
            cycling.getParameterCount() != 9 || commander.getParameterCount() != 1)
            throw new Exception("Missing expected shared-helper interfaces");
        cycling.setName("basic_landcycling_2022", SourceType.IMPORTED);
        commander.setName("locate_edh_card_2022", SourceType.IMPORTED);
        color.setCustomVariableStorage(false);
        color.setCallingConvention("__cdecl");
        color.setReturnType(IntegerDataType.dataType, SourceType.IMPORTED);
        color.setName("most_common_color_all_permanents", SourceType.IMPORTED);
        color.replaceParameters(new ArrayList<Variable>(), Function.FunctionUpdateType.DYNAMIC_STORAGE_ALL_PARAMS,
                                true, SourceType.IMPORTED);
        println("RECOVERY_BATCH10_INTERFACES_COMPLETE count=3 color_parameters=0");
    }
}
