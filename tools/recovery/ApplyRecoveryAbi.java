// Map executable interfaces and apply DWARF-proven GCC register conventions.
// @category Manalink
import java.io.*;
import java.nio.file.*;
import java.util.*;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.*;
import ghidra.program.model.data.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.mem.MemoryBlock;
import ghidra.program.model.symbol.SourceType;
import ghidra.util.data.DataTypeParser;
import ghidra.util.data.DataTypeParser.AllowedDataTypes;

public class ApplyRecoveryAbi extends GhidraScript {
    @Override
    public void run() throws Exception {
        Path root = Paths.get(getScriptArgs()[0]);
        for (String line : Files.readAllLines(root.resolve("magic-sections.tsv"))) {
            String[] row = line.split("\t");
            Address start = toAddr(Long.decode(row[1]));
            if (currentProgram.getMemory().getBlock(start) != null) continue;
            byte[] bytes = Files.readAllBytes(Paths.get(row[3]));
            MemoryBlock block = currentProgram.getMemory().createInitializedBlock("Magic" + row[0], start,
                new ByteArrayInputStream(bytes), bytes.length, monitor, false);
            long flags = Long.parseLong(row[2]);
            block.setRead((flags & 0x40000000L) != 0);
            block.setWrite((flags & 0x80000000L) != 0);
            block.setExecute((flags & 0x20000000L) != 0);
        }
        DataTypeManager dtm = currentProgram.getDataTypeManager();
        DataTypeParser parser = new DataTypeParser(dtm, dtm, null, AllowedDataTypes.ALL);
        int globals = 0, interfaces = 0, conventions = 0;
        try (PrintWriter log = new PrintWriter(Files.newBufferedWriter(root.resolve("abi-application.tsv")))) {
            for (String line : Files.readAllLines(root.resolve("absolute-interfaces.tsv"))) {
                String[] row = line.split("\t");
                try {
                    Address address = toAddr(Long.decode(row[0]));
                    createLabel(address, row[1], true, SourceType.IMPORTED);
                    DataType type = parser.parse(row[3]);
                    if (row[2].equals("global")) {
                        if (type.getLength() > 0) {
                            clearListing(address, address.add(type.getLength() - 1));
                            createData(address, type);
                            globals++;
                        }
                    } else {
                        Function function = getFunctionAt(address);
                        if (function == null) function = createFunction(address, row[1]);
                        if (function == null) throw new Exception("Cannot create interface function");
                        function.setCallingConvention("__cdecl");
                        function.setReturnType(type, SourceType.IMPORTED);
                        List<Variable> parameters = new ArrayList<>();
                        for (int i = 4; i < row.length; i++) {
                            parameters.add(new ParameterImpl("argument" + (i - 3), parser.parse(row[i]), currentProgram));
                        }
                        function.replaceParameters(parameters, Function.FunctionUpdateType.DYNAMIC_STORAGE_ALL_PARAMS, true, SourceType.IMPORTED);
                        interfaces++;
                    }
                    log.println("ok\t" + line);
                } catch (Exception e) {
                    log.println("unresolved\t" + line + "\t" + e.toString());
                }
            }
            for (String line : Files.readAllLines(root.resolve("register-conventions.tsv"))) {
                String[] row = line.split("\t");
                Function function = getFunctionAt(toAddr(Long.decode(row[0])));
                if (function != null) {
                    function.setCustomVariableStorage(false);
                    function.setCallingConvention(row[2]);
                    conventions++;
                }
            }
        }
        for (Function function : currentProgram.getFunctionManager().getFunctions(true)) {
            function.setInline(false);
        }
        println("RECOVERY_ABI_COMPLETE globals=" + globals + " interfaces=" + interfaces + " register_conventions=" + conventions);
    }
}
