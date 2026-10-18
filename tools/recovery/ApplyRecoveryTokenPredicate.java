// @category Manalink
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;
import ghidra.program.model.symbol.SourceType;
public class ApplyRecoveryTokenPredicate extends GhidraScript {
    public void run() throws Exception {
        Function function = getFunctionAt(toAddr(0x22d6fa0));
        if (function == null || function.getParameterCount() != 5)
            throw new Exception("Unexpected token predicate interface");
        function.setName("target_is_token_too_2022", SourceType.IMPORTED);
        println("RECOVERY_TOKEN_PREDICATE_COMPLETE count=1");
    }
}
