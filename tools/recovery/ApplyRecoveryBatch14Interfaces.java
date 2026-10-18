// Preserve old public APIs while naming the target's incompatible interfaces.
// @category Manalink
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;
import ghidra.program.model.symbol.SourceType;
public class ApplyRecoveryBatch14Interfaces extends GhidraScript {
    public void run() throws Exception {
        long[] addresses = {0x22fabf0, 0x2697910, 0x26a3630};
        String[] names = {"cascade_2022", "beginning_of_combat_2022", "other_leaves_play_2022"};
        int[] counts = {3, 4, 7};
        for (int i = 0; i < addresses.length; ++i) {
            Function function = getFunctionAt(toAddr(addresses[i]));
            if (function == null || function.getParameterCount() != counts[i])
                throw new Exception("Unexpected interface for " + names[i]);
        }
        for (int i = 0; i < addresses.length; ++i)
            getFunctionAt(toAddr(addresses[i])).setName(names[i], SourceType.IMPORTED);
        println("RECOVERY_BATCH14_INTERFACES_COMPLETE count=3");
    }
}
