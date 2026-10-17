// MinGW ___chkstk_ms preserves general registers and does not allocate the VLA.
// Model the compiler's page touches without inventing a returned allocation size.
// @category Manalink
import ghidra.app.script.GhidraScript;
import ghidra.program.database.SpecExtension;
import ghidra.program.model.listing.Function;
import ghidra.program.model.symbol.SourceType;

public class ApplyRecoveryStackProbe extends GhidraScript {
    public void run() throws Exception {
        // Exact target body: push ECX/EAX, touch each page, pop EAX/ECX, RET.
        byte[] expected = java.util.HexFormat.of().parseHex(
            "51503d001000008d4c240c721581e9001000008309002d001000003d0010000077eb29c18309005859c3");
        byte[] actual = getBytes(toAddr(0x026fc1d0), expected.length);
        if (!java.util.Arrays.equals(expected, actual)) throw new Exception("Unexpected stack probe");
        // CALL p-code already pushed a return address. Model RET's four-byte
        // pop; the caller's following SUB ESP,EAX performs the allocation.
        String xml = "<callfixup name=\"manalink_stack_probe\">" +
            "<target name=\"___chkstk_ms\"/><pcode><body><![CDATA[ ESP = ESP + 4; ]]></body></pcode></callfixup>";
        new SpecExtension(currentProgram).addReplaceCompilerSpecExtension(xml, monitor);
        Function probe = getFunctionAt(toAddr(0x026fc1d0));
        if (probe == null) probe = createFunction(toAddr(0x026fc1d0), "___chkstk_ms");
        probe.setName("___chkstk_ms", SourceType.IMPORTED);
        probe.setCallFixup("manalink_stack_probe");
        println("RECOVERY_STACK_PROBE register-preserving page touch modeled at " + probe.getEntryPoint());
    }
}
