// Resolve decompiler-sanitized data aliases using the caller's operands.
// @category Manalink
import java.io.*;
import java.nio.file.*;
import java.util.*;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.scalar.Scalar;
import ghidra.program.model.symbol.Symbol;

public class ExportRecoveryStrings extends GhidraScript {
    @Override
    public void run() throws Exception {
        Path root = Paths.get(getScriptArgs()[0]);
        Set<Address> aliases = new HashSet<>();
        try (PrintWriter out = new PrintWriter(Files.newBufferedWriter(root.resolve("rdata-symbols.tsv")))) {
            for (Symbol symbol : currentProgram.getSymbolTable().getAllSymbols(true)) {
                if (symbol.getName().replaceAll("[^A-Za-z0-9_]", "_").equals("_rdata")) {
                    aliases.add(symbol.getAddress());
                    out.println(symbol.getAddress() + "\t" + symbol.getName());
                }
            }
        }
        int resolved = 0;
        try (PrintWriter out = new PrintWriter(Files.newBufferedWriter(root.resolve("string-aliases.tsv")))) {
            out.println("function\talias\taddress");
            for (String row : Files.readAllLines(root.resolve("decompiled-abi/export.tsv"))) {
                String[] fields = row.split("\t");
                if (!fields[0].matches("[0-9a-fA-F]+")) continue;
                Path source = root.resolve("decompiled-abi/" + fields[0] + ".c");
                if (!Files.exists(source) || !Files.readString(source).contains("&_rdata")) continue;
                Function function = getFunctionAt(toAddr(Long.parseLong(fields[0], 16)));
                if (function == null) continue;
                Set<Address> used = new HashSet<>();
                for (Instruction instruction : currentProgram.getListing().getInstructions(function.getBody(), true)) {
                    for (int operand = 0; operand < instruction.getNumOperands(); operand++) {
                        for (Object object : instruction.getOpObjects(operand)) {
                            Address address = null;
                            if (object instanceof Scalar) address = toAddr(((Scalar)object).getUnsignedValue());
                            else if (object instanceof Address) address = (Address)object;
                            if (address != null && aliases.contains(address)) used.add(address);
                        }
                    }
                }
                if (used.size() == 1) {
                    out.println(fields[0] + "\t_rdata\t" + used.iterator().next());
                    resolved++;
                }
            }
        }
        println("RECOVERY_STRINGS aliases=" + aliases.size() + " callers=" + resolved);
    }
}
