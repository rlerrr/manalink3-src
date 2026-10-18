// Separate player-aware target subtype APIs from the older source signatures.
// @category Manalink
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;
import ghidra.program.model.symbol.SourceType;
public class ApplyRecoverySubtypeInterfaces extends GhidraScript {
    public void run() throws Exception {
        Function report = getFunctionAt(toAddr(0x2673320));
        Function test = getFunctionAt(toAddr(0x26736e0));
        if (report == null || test == null || report.getParameterCount() != 3 || test.getParameterCount() != 3)
            throw new Exception("Missing player-aware subtype interfaces");
        report.setName("get_subtypes_by_csvid_2022", SourceType.IMPORTED);
        test.setName("has_subtype_by_id_2022", SourceType.IMPORTED);
        println("RECOVERY_SUBTYPE_INTERFACES_COMPLETE count=2");
    }
}
