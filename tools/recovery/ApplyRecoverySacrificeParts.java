// Apply the physical signatures established by concrete sacrifice DWARF locations.
// @category Manalink
import ghidra.app.script.GhidraScript;
import ghidra.program.model.data.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.SourceType;

public class ApplyRecoverySacrificeParts extends GhidraScript {
    public void run() throws Exception {
        Function cause=getFunctionAt(toAddr(0x26e3990));
        Function prompt=getFunctionAt(toAddr(0x26e3a30));
        if(cause==null || prompt==null)throw new Exception("Missing sacrifice private entries");
        // Only t_player has physical entry storage (EAX). The original player
        // exists solely as a DW_OP_GNU_parameter_ref expression, not an argument.
        cause.setCustomVariableStorage(false);
        cause.setName("can_cause_sacrifice_part_2022",SourceType.IMPORTED);
        cause.setCallingConvention("__regparm1");
        cause.setReturnType(IntegerDataType.dataType,SourceType.IMPORTED);
        cause.replaceParameters(Function.FunctionUpdateType.DYNAMIC_STORAGE_ALL_PARAMS,true,SourceType.IMPORTED,
            new ParameterImpl("t_player",IntegerDataType.dataType,currentProgram));
        // The source maxlen is a constant 200 with no location. Physical
        // parameters are prompt/type/invert in EAX/EDX/ECX, respectively.
        prompt.setCustomVariableStorage(false);
        prompt.setName("get_sacrifice_prompt_text_part_2022",SourceType.IMPORTED);
        prompt.setCallingConvention("__regparm3");
        prompt.setReturnType(VoidDataType.dataType,SourceType.IMPORTED);
        prompt.replaceParameters(Function.FunctionUpdateType.DYNAMIC_STORAGE_ALL_PARAMS,true,SourceType.IMPORTED,
            new ParameterImpl("prompt",new PointerDataType(CharDataType.dataType,currentProgram.getDataTypeManager()),currentProgram),
            new ParameterImpl("type",IntegerDataType.dataType,currentProgram),
            new ParameterImpl("invert",IntegerDataType.dataType,currentProgram));
        println("RECOVERY_SACRIFICE_PARTS_COMPLETE cause=1 prompt=3 maxlen=200 source_player=nonphysical");
    }
}
