// Separate changed counter/death/search interfaces from retained source APIs.
// @category Manalink
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;
import ghidra.program.model.symbol.SourceType;
public class ApplyRecoveryBatch17Interfaces extends GhidraScript {
    public void run() throws Exception {
        long[] addresses = {0x26d4ca0, 0x26d5970, 0x26d5a00, 0x269b2e0,
                            0x2679600, 0x2686130, 0x26d0e90};
        String[] names = {"copy_counters_from_source", "add_counters_predoubled_from_source",
            "move_counters_from_source", "lobotomy_effect_2022", "effect_this_dies_2022",
            "this_dies_trigger_impl_2022", "add_or_remove_counters_from_source_impl"};
        int[] counts = {7, 6, 8, 5, 3, 7, 6};
        for (int i = 0; i < addresses.length; ++i) {
            Function f = getFunctionAt(toAddr(addresses[i]));
            if (f == null || f.getParameterCount() != counts[i])
                throw new Exception("Unexpected interface: " + names[i]);
        }
        for (int i = 0; i < addresses.length; ++i)
            getFunctionAt(toAddr(addresses[i])).setName(names[i], SourceType.IMPORTED);
        println("RECOVERY_BATCH17_INTERFACES_COMPLETE count=7");
    }
}
