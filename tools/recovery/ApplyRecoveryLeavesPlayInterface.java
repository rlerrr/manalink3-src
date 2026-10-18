// The target has five arguments; retain the source's old three-argument API.
// @category Manalink
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;
import ghidra.program.model.symbol.SourceType;
public class ApplyRecoveryLeavesPlayInterface extends GhidraScript {
    public void run() throws Exception {
        Function function = getFunctionAt(toAddr(0x26a2080));
        if (function == null || function.getParameterCount() != 5)
            throw new Exception("Missing five-argument leaves_play interface");
        function.setName("leaves_play_2022", SourceType.IMPORTED);
        println("RECOVERY_LEAVES_PLAY_INTERFACE_COMPLETE " + function.getSignature());
    }
}
