// Concrete DWARF stores player/card/who_gains in EAX/EDX/ECX; event is abstract.
// @category Manalink
import ghidra.app.script.GhidraScript;
import ghidra.program.model.data.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.SourceType;
public class ApplyRecoverySpiritLinkPart extends GhidraScript {
    public void run() throws Exception {
        Function f=getFunctionAt(toAddr(0x21ecf20));
        if(f==null)throw new Exception("Missing private Spirit Link entry");
        f.setCustomVariableStorage(false);
        f.setName("spirit_link_effect_part_2022",SourceType.IMPORTED);
        f.setCallingConvention("__regparm3");
        f.setReturnType(VoidDataType.dataType,SourceType.IMPORTED);
        f.replaceParameters(Function.FunctionUpdateType.DYNAMIC_STORAGE_ALL_PARAMS,true,SourceType.IMPORTED,
            new ParameterImpl("player",IntegerDataType.dataType,currentProgram),
            new ParameterImpl("card",IntegerDataType.dataType,currentProgram),
            new ParameterImpl("who_gains",IntegerDataType.dataType,currentProgram));
        println("RECOVERY_SPIRIT_LINK_PART_COMPLETE physical=3 event=nonphysical");
    }
}
