// Three EXE-owned EVENT_CHECK_ABILITIES accumulators verified in setup_ai_combat_data.
// @category Manalink
import ghidra.app.script.GhidraScript;
import ghidra.program.model.data.IntegerDataType;
import ghidra.program.model.symbol.SourceType;

public class ApplyRecoveryCombatGlobals extends GhidraScript {
    public void run() throws Exception {
        long[] addresses={0x739b50,0x7a371c,0x78fef0};
        String[] names={"check_abilities_power_2022","check_abilities_toughness_2022","check_abilities_keywords_2022"};
        for(int i=0;i<addresses.length;i++) {
            clearListing(toAddr(addresses[i]),toAddr(addresses[i]+3));
            createData(toAddr(addresses[i]),IntegerDataType.dataType);
            createLabel(toAddr(addresses[i]),names[i],true,SourceType.IMPORTED);
        }
        println("RECOVERY_COMBAT_GLOBALS_COMPLETE exe_int32_accumulators=3");
    }
}
