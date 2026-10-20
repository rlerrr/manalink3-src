// Keep the native color dialog as a typed tail-call boundary.
// @category Manalink
import java.util.*;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.data.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.SourceType;

public class ApplyRecoveryManaChooser extends GhidraScript {
    public void run() throws Exception {
        Function dialog = getFunctionAt(toAddr(0x4721c0));
        if (dialog == null) dialog = createFunction(toAddr(0x4721c0), "choose_a_color_exe");
        if (dialog == null) throw new Exception("Missing native color chooser");
        dialog.setName("choose_a_color_exe", SourceType.USER_DEFINED);
        dialog.setInline(false);
        dialog.setCallingConvention("__cdecl");
        dialog.setReturnType(IntegerDataType.dataType, SourceType.USER_DEFINED);
        List<Variable> parameters = new ArrayList<>();
        parameters.add(new ParameterImpl("player", IntegerDataType.dataType, currentProgram));
        parameters.add(new ParameterImpl("prompt", new PointerDataType(CharDataType.dataType), currentProgram));
        parameters.add(new ParameterImpl("use_color_names", IntegerDataType.dataType, currentProgram));
        parameters.add(new ParameterImpl("ai_choice", IntegerDataType.dataType, currentProgram));
        parameters.add(new ParameterImpl("available", IntegerDataType.dataType, currentProgram));
        dialog.replaceParameters(parameters, Function.FunctionUpdateType.DYNAMIC_STORAGE_ALL_PARAMS, true, SourceType.USER_DEFINED);
        Instruction tail = getInstructionAt(toAddr(0x266afff));
        if (tail == null || !Arrays.equals(tail.getBytes(), new byte[]{(byte)0xe9,(byte)0xbc,0x71,(byte)0xe0,(byte)0xfd}))
            throw new Exception("Native chooser tail-jump mismatch");
        tail.setFlowOverride(FlowOverride.CALL_RETURN);
        println("RECOVERY_MANA_CHOOSER_BOUNDARY_COMPLETE");
    }
}
