// DWARF target_t is returned by value: player in EAX, card in EDX.
// @category Manalink
import ghidra.app.script.GhidraScript;
import ghidra.program.model.data.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.SourceType;
import java.util.*;

public class ApplyRecoveryPairReturn extends GhidraScript {
    public void run() throws Exception {
        Function function = getFunctionAt(toAddr(0x0267b0a0));
        if (function == null) throw new Exception("Missing pair-return helper");
        List<DataType> matches = new ArrayList<>();
        currentProgram.getDataTypeManager().findDataTypes("target_t", matches);
        DataType pair = null;
        for (DataType type : matches) {
            DataType base = type instanceof TypeDef ? ((TypeDef)type).getBaseDataType() : type;
            if (!(base instanceof Structure)) continue;
            Structure structure = (Structure)base;
            if (structure.getLength() == 8 && structure.getNumComponents() == 2 &&
                "player".equals(structure.getComponent(0).getFieldName()) &&
                "card".equals(structure.getComponent(1).getFieldName()) &&
                structure.getComponent(0).getOffset() == 0 && structure.getComponent(1).getOffset() == 4) {
                pair = type;
                break;
            }
        }
        if (pair == null) throw new Exception("Missing verified eight-byte target_t");
        // This join order produces field 0 from EAX and field 4 from EDX in
        // the decompiler. The recovery tool checks both output assignments
        // and a caller's use of .card against the verified machine sequence.
        VariableStorage storage = new VariableStorage(currentProgram,
            currentProgram.getLanguage().getRegister("EDX"), currentProgram.getLanguage().getRegister("EAX"));
        function.setCustomVariableStorage(true);
        function.setReturn(pair, storage, SourceType.IMPORTED);
        println("RECOVERY_PAIR_RETURN " + function.getReturn().getDataType() + " " + function.getReturn().getVariableStorage());
        println("RECOVERY_PAIR_RETURN_COMPLETE");
    }
}
