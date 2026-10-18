// Keep the target nine-argument split interface separate from the old API.
// @category Manalink
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;
import ghidra.program.model.symbol.SourceType;

public class ApplyRecoverySplitInterfaces extends GhidraScript {
    public void run() throws Exception {
        Function worker = getFunctionAt(toAddr(0x02485f50));
        Function wrapper = getFunctionAt(toAddr(0x02488e80));
        if (worker == null || wrapper == null || worker.getParameterCount() != 10 || wrapper.getParameterCount() != 9)
            throw new Exception("Missing expected split-card interfaces");
        wrapper.setName("generic_split_card_2022", SourceType.IMPORTED);
        println("RECOVERY_SPLIT_INTERFACES_COMPLETE renamed=1 existing_worker_retained");
    }
}
