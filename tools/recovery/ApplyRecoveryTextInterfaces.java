// @category Manalink
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;
import ghidra.program.model.symbol.SourceType;
public class ApplyRecoveryTextInterfaces extends GhidraScript {
    public void run() throws Exception {
        Function text = getFunctionAt(toAddr(0x26765b0));
        Function fatal = getFunctionAt(toAddr(0x26c11d0));
        if (text == null || text.getParameterCount() != 2 || fatal == null || !fatal.hasVarArgs())
            throw new Exception("Unexpected text/fatal interfaces");
        text.setName("load_text_2022", SourceType.IMPORTED);
        fatal.setNoReturn(true);
        println("RECOVERY_TEXT_INTERFACES_COMPLETE count=2");
    }
}
