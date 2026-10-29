// Extract the pure native card-id -> preferred-counter mapping by emulation.
// No game process or DLL entry point is executed.
// @category Manalink
import java.io.*;
import java.nio.file.*;
import java.security.MessageDigest;
import ghidra.app.emulator.EmulatorHelper;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;

public class ExportCounterTypeMap extends GhidraScript {
    @Override
    public void run() throws Exception {
        if (!"x86:LE:32:default".equals(currentProgram.getLanguageID().toString()))
            throw new Exception("Requires the 32-bit October 2022 program");
        Address entry = toAddr(0x26cef50L);
        Address stop = toAddr(0);
        byte[] bytes = new byte[0x26d0e27 - 0x26cef50];
        currentProgram.getMemory().getBytes(entry, bytes);
        StringBuilder digest = new StringBuilder();
        for (byte b : MessageDigest.getInstance("SHA-256").digest(bytes))
            digest.append(String.format("%02x", b & 255));
        if (!digest.toString().equals("994b7e79717faf88a5945f117009433405c5c4083af1f12b5e98608e39e766ba"))
            throw new Exception("Native preference-function bytes differ from the pinned release");
        // Covers every indirect switch table referenced by this function.
        byte[] lookupBytes = new byte[0x276af6c - 0x2767f9c];
        currentProgram.getMemory().getBytes(toAddr(0x2767f9cL), lookupBytes);
        StringBuilder lookupDigest = new StringBuilder();
        for (byte b : MessageDigest.getInstance("SHA-256").digest(lookupBytes))
            lookupDigest.append(String.format("%02x", b & 255));
        EmulatorHelper emu = new EmulatorHelper(currentProgram);
        try (PrintWriter out = new PrintWriter(Files.newBufferedWriter(Paths.get(getScriptArgs()[0])))) {
            emu.setBreakpoint(stop);
            out.println("# native=0x26cef50 end=0x26d0e27 sha256=" + digest);
            out.println("# lookup=0x2767f9c end=0x276af6c sha256=" + lookupDigest);
            out.println("csvid\tcounter_type");
            for (int input = 0; input < 65536; ++input) {
                monitor.checkCancelled();
                int result = evaluate(emu, entry, stop, input);
                out.println(input + "\t" + result);
                if ((input & 8191) == 8191) println("COUNTER_MAP progress=" + (input + 1));
            }
            for (int input : new int[] {Integer.MIN_VALUE, -65536, -1, 65536, 100000, Integer.MAX_VALUE}) {
                int result = evaluate(emu, entry, stop, input);
                if (result != -1) throw new Exception("Unexpected out-of-domain result: " + input + " -> " + result);
                out.println(input + "\t" + result);
            }
            println("COUNTER_MAP_COMPLETE inputs=65542");
        } finally { emu.dispose(); }
    }

    private int evaluate(EmulatorHelper emu, Address entry, Address stop, int input) throws Exception {
        emu.writeRegister("ESP", 0x10000000L);
        emu.writeRegister("EBP", 0x10000100L);
        emu.writeRegister("EAX", 0);
        emu.writeRegister("ECX", 0);
        emu.writeRegister("EDX", 0);
        emu.writeStackValue(0, 4, 0);
        emu.writeStackValue(4, 4, input & 0xffffffffL);
        emu.writeRegister(emu.getPCRegister(), entry.getOffset());
        if (!emu.run(monitor) || !emu.getExecutionAddress().equals(stop))
            throw new Exception("Emulation failed for " + input + ": " + emu.getLastError());
        return emu.readRegister("EAX").intValue();
    }
}
