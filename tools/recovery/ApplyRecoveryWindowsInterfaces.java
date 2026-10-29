// Apply x86 Win32/IAT contracts before decompiling the original Windows frontends.
// @category Manalink
import java.nio.file.*;
import java.util.*;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.data.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.*;
import ghidra.util.data.DataTypeParser;
import ghidra.util.data.DataTypeParser.AllowedDataTypes;

public class ApplyRecoveryWindowsInterfaces extends GhidraScript {
    public void run() throws Exception {
        currentProgram.getOptions("Manalink recovery").setBoolean("ExternalInterfacesReady", false);
        DataTypeManager manager = currentProgram.getDataTypeManager();
        DataTypeParser parser = new DataTypeParser(manager, manager, null, AllowedDataTypes.ALL);
        List<String[]> rows = new ArrayList<>();
        for (String line : Files.readAllLines(Paths.get(getScriptArgs()[0]))) {
            String[] row = line.split("\t");
            for (int i = 4; i < row.length; i++) parser.parse(row[i]);
            rows.add(row);
        }
        Map<String, Function> imports = new HashMap<>();
        FunctionIterator iterator = currentProgram.getFunctionManager().getExternalFunctions();
        while (iterator.hasNext()) {
            Function f = iterator.next();
            if (imports.put(f.getName(), f) != null)
                throw new Exception("Ambiguous external import " + f.getName());
        }
        for (String[] row : rows) {
            long at = Long.decode(row[0]);
            Function function;
            int stackBytes = 0;
            for (int i = 5; i < row.length; i++) stackBytes += parser.parse(row[i]).getLength();
            String decorated = row[2] + (row[3].equals("__stdcall") ? "@" + stackBytes : "");
            if (row[1].equals("direct")) {
                function = getFunctionAt(toAddr(at));
                if (function == null) function = createFunction(toAddr(at), row[2]);
            } else {
                boolean matched = false;
                String labels = "";
                for (Symbol symbol : currentProgram.getSymbolTable().getSymbols(toAddr(at))) {
                    labels += symbol.getName() + " ";
                    matched |= symbol.getName().equals("__imp__" + decorated);
                }
                if (!matched) throw new Exception("IAT name/address mismatch: " + row[2] + " actual=" + labels);
                function = imports.get(decorated);
                if (function == null) function = imports.get(row[2]);
                if (function == null)
                    function = currentProgram.getExternalManager().addExtFunction(
                        row[1], row[2], null, SourceType.IMPORTED).getFunction();
            }
            if (function == null) throw new Exception("Cannot bind " + row[2]);
            function.setCallingConvention(row[3]);
            function.setReturnType(parser.parse(row[4]), SourceType.IMPORTED);
            List<Variable> parameters = new ArrayList<>();
            for (int i = 5; i < row.length; i++)
                parameters.add(new ParameterImpl("argument" + (i - 4), parser.parse(row[i]), currentProgram));
            function.replaceParameters(parameters, Function.FunctionUpdateType.DYNAMIC_STORAGE_ALL_PARAMS,
                                       true, SourceType.IMPORTED);
            function.setInline(false);
            if (row[2].equals("abort")) function.setNoReturn(true);
            if (!row[1].equals("direct")) {
                DataType pointer = new PointerDataType(new FunctionDefinitionDataType(function, true), manager);
                removeDataAt(toAddr(at));
                createData(toAddr(at), pointer);
                ExternalLocation location = currentProgram.getExternalManager().getExternalLocation(function.getSymbol());
                currentProgram.getReferenceManager().addExternalReference(toAddr(at), 0, location,
                                                                         SourceType.IMPORTED, RefType.DATA);
            }
            println("RECOVERY_WINDOWS_INTERFACE " + row[0] + " " + function.getSignature());
        }
        currentProgram.getOptions("Manalink recovery").setBoolean("ExternalInterfacesReady", true);
        println("RECOVERY_WINDOWS_INTERFACES_COMPLETE count=" + rows.size());
    }
}
