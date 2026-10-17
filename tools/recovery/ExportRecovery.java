// Export explicitly requested DWARF functions for source reconstruction.
// @category Manalink
import java.io.*;
import java.nio.charset.StandardCharsets;
import java.nio.file.*;
import java.util.*;
import ghidra.app.cmd.disassemble.DisassembleCommand;
import ghidra.app.decompiler.*;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.*;
import ghidra.program.model.data.*;
import ghidra.program.model.listing.*;

public class ExportRecovery extends GhidraScript {
    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        Path directory = Paths.get(args[0]);
        Files.createDirectories(directory);
        List<String> requests = Files.readAllLines(Paths.get(args[1]));
        DecompInterface decompiler = new DecompInterface();
        DecompileOptions options = new DecompileOptions();
        options.grabFromProgram(currentProgram);
        decompiler.setOptions(options);
        decompiler.openProgram(currentProgram);
        int success = 0;
        int failed = 0;
        try (PrintWriter manifest = new PrintWriter(Files.newBufferedWriter(directory.resolve("export.tsv")))) {
            manifest.println("address\tsource\tname\tstatus\tprototype");
            for (String request : requests) {
                monitor.checkCancelled();
                String[] fields = request.split("\t");
                Address start = toAddr(Long.parseUnsignedLong(fields[0], 16));
                Address end = toAddr(Long.parseUnsignedLong(fields[3], 16) - 1);
                Function function = getFunctionAt(start);
                if (function == null) {
                    manifest.println(request + "\tmissing\t");
                    failed++;
                    continue;
                }
                new DisassembleCommand(start, new AddressSet(start, end), true).applyTo(currentProgram, monitor);
                function.setBody(new AddressSet(start, end));
                DecompileResults result = decompiler.decompileFunction(function, 45, monitor);
                String prototype = function.getSignature().getPrototypeString();
                if (result.decompileCompleted()) {
                    Path destination = directory.resolve(fields[0] + ".c");
                    String text = "/* October 2022 ManalinkEh.dll: " + fields[2] + " @ 0x" + fields[0] +
                        "; DWARF unit " + fields[1] + ". Raw analysis output. */\n" +
                        result.getDecompiledFunction().getC();
                    Files.writeString(destination, text, StandardCharsets.UTF_8);
                    manifest.println(fields[0] + "\t" + fields[1] + "\t" + fields[2] + "\tok\t" + prototype);
                    success++;
                } else {
                    manifest.println(fields[0] + "\t" + fields[1] + "\t" + fields[2] + "\tfailed\t" + result.getErrorMessage());
                    failed++;
                }
                manifest.flush();
                if ((success + failed) % 50 == 0) {
                    println("RECOVERY_EXPORT progress=" + (success + failed) + "/" + requests.size() +
                        " success=" + success + " failed=" + failed);
                }
            }
        } finally {
            decompiler.dispose();
        }
        try (Writer writer = Files.newBufferedWriter(directory.resolve("debug-types.h"))) {
            new DataTypeWriter(currentProgram.getDataTypeManager(), writer).write(currentProgram.getDataTypeManager(), monitor);
        }
        println("RECOVERY_EXPORT_COMPLETE success=" + success + " failed=" + failed);
    }
}
