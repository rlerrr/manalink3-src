// Separate a GCC .part.0 worker from its public wrapper.
// @category Manalink
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;
import ghidra.program.model.symbol.SourceType;
public class ApplyRecoveryProtectionWorker extends GhidraScript {
    public void run() throws Exception {
        Function worker = getFunctionAt(toAddr(0x26dd200));
        Function wrapper = getFunctionAt(toAddr(0x26e09a0));
        if (worker == null || wrapper == null || worker.getParameterCount() != 3 ||
            wrapper.getParameterCount() != 3 || !worker.getCallingConventionName().equals("__regparm3"))
            throw new Exception("Unexpected protection worker/wrapper interfaces");
        worker.setName("get_protection_source_impl", SourceType.IMPORTED);
        println("RECOVERY_PROTECTION_WORKER_COMPLETE count=1");
    }
}
