// Decompile only the explicit DWARF ranges supplied on the command line.
// Analysis pilot: no whole-program analysis, no execution of the target DLL.
// @category Manalink

import java.io.File;
import java.io.PrintWriter;
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.address.AddressSet;
import ghidra.program.model.data.IntegerDataType;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.Parameter;
import ghidra.program.model.listing.ParameterImpl;
import ghidra.program.model.symbol.SourceType;

public class RecoveryPilot extends GhidraScript {
    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        // output path followed by name:hex-address:decimal-size triples.
        DecompInterface decompiler = new DecompInterface();
        decompiler.openProgram(currentProgram);
        try (PrintWriter out = new PrintWriter(new File(args[0]))) {
            out.println("/* Inspection pilot only. Parameter types supplied from DWARF; event is represented as int.");
            out.println("   Callee prototypes and other types have not been imported. This is not buildable recovered source. */");
            for (int i = 1; i < args.length; i++) {
                String[] parts = args[i].split(":");
                Address start = toAddr(Long.parseUnsignedLong(parts[1], 16));
                int size = Integer.parseInt(parts[2]);
                AddressSet body = new AddressSet(start, start.add(size - 1));
                disassemble(start);
                Function function = getFunctionAt(start);
                if (function == null) {
                    function = currentProgram.getFunctionManager().createFunction(parts[0], start, body, SourceType.USER_DEFINED);
                } else {
                    function.setBody(body);
                }
                function.setCallingConvention("__cdecl");
                function.setReturnType(IntegerDataType.dataType, SourceType.USER_DEFINED);
                // The selected card functions all have int(int,int,event_t).
                Parameter[] parameters = new Parameter[] {
                    new ParameterImpl("player", IntegerDataType.dataType, currentProgram),
                    new ParameterImpl("card", IntegerDataType.dataType, currentProgram),
                    new ParameterImpl("event", IntegerDataType.dataType, currentProgram)
                };
                function.replaceParameters(Function.FunctionUpdateType.DYNAMIC_STORAGE_ALL_PARAMS, true, SourceType.USER_DEFINED, parameters);
                DecompileResults result = decompiler.decompileFunction(function, 60, monitor);
                out.println("\n/* " + parts[0] + " @ " + start + ", " + size + " bytes */");
                if (result.decompileCompleted()) {
                    out.println(result.getDecompiledFunction().getC());
                } else {
                    out.println("/* FAILED: " + result.getErrorMessage() + " */");
                    throw new Exception("Decompilation failed: " + parts[0]);
                }
            }
        } finally {
            decompiler.dispose();
        }
    }
}
