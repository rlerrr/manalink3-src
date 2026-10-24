// Restore Win32 stdcall import interfaces before exporting the pack dialog.
// The TSV is generated from the native PE imports and local SDK signatures.
// @category Manalink
import ghidra.app.script.GhidraScript;
import ghidra.program.model.data.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.*;
import ghidra.util.data.DataTypeParser;
import ghidra.util.data.DataTypeParser.AllowedDataTypes;
import java.nio.file.*;
import java.util.*;

public class ApplyRecoveryPackDialogInterfaces extends GhidraScript {
    public void run() throws Exception {
        DataTypeManager manager = currentProgram.getDataTypeManager();
        DataTypeParser parser = new DataTypeParser(manager, manager, null, AllowedDataTypes.ALL);
        List<String[]> rows = new ArrayList<>();
        Map<String, Function> imports = new HashMap<>();
        FunctionIterator iterator = currentProgram.getFunctionManager().getExternalFunctions();
        while (iterator.hasNext()) {
            Function function = iterator.next();
            if (imports.put(function.getName(), function) != null)
                imports.remove(function.getName());
        }
        for (String line : Files.readAllLines(Path.of(getScriptArgs()[0]))) {
            String[] row = line.split("\t");
            if (row.length < 4) throw new Exception("Invalid native import: " + line);
            for (int i = 3; i < row.length; ++i) parser.parse(row[i]);
            rows.add(row);
        }
        for (String[] row : rows) {
            Function function = imports.get(row[1]);
            if (function == null)
                function = currentProgram.getExternalManager().addExtFunction(
                    row[2], row[1], null, SourceType.IMPORTED).getFunction();
            function.setCustomVariableStorage(false);
            function.setCallingConvention("__stdcall");
            function.setReturnType(parser.parse(row[3]), SourceType.IMPORTED);
            List<Variable> parameters = new ArrayList<>();
            for (int i = 4; i < row.length; ++i)
                parameters.add(new ParameterImpl("argument" + (i - 3), parser.parse(row[i]), currentProgram));
            function.replaceParameters(parameters, Function.FunctionUpdateType.DYNAMIC_STORAGE_ALL_PARAMS,
                                       true, SourceType.IMPORTED);
            function.setStackPurgeSize((row.length - 4) * 4);
            function.setVarArgs(false);
            function.setInline(false);
            DataType pointer = new PointerDataType(new FunctionDefinitionDataType(function, true), manager);
            clearListing(toAddr(row[0]), toAddr(row[0]).add(3));
            createData(toAddr(row[0]), pointer);
            ExternalLocation location = currentProgram.getExternalManager().getExternalLocation(function.getSymbol());
            currentProgram.getReferenceManager().addExternalReference(toAddr(row[0]), 0, location,
                                                                     SourceType.IMPORTED, RefType.DATA);
            println("RECOVERY_PACK_WIN32_IMPORT " + row[0] + " " + function.getSignature());
        }
        Function callback = getFunctionAt(toAddr(0x26ecc50));
        if (callback == null) throw new Exception("Missing pack dialog callback");
        callback.setCallingConvention("__stdcall");
        callback.setStackPurgeSize(16);
        println("RECOVERY_PACK_DIALOG_INTERFACES_COMPLETE count=" + rows.size());
    }
}
