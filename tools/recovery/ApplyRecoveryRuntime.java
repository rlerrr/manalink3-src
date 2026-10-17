// Apply verified external signatures (CRT imports or address-matched EXE bindings).
// @category Manalink
import java.nio.file.*;
import java.util.*;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.data.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.SourceType;
import ghidra.util.data.DataTypeParser;
import ghidra.util.data.DataTypeParser.AllowedDataTypes;

public class ApplyRecoveryRuntime extends GhidraScript {
    public void run() throws Exception {
        currentProgram.getOptions("Manalink recovery").setBoolean("ExternalInterfacesReady", false);
        DataTypeManager manager = currentProgram.getDataTypeManager();
        DataTypeParser parser = new DataTypeParser(manager, manager, null, AllowedDataTypes.ALL);
        List<String[]> rows = new ArrayList<>();
        // Validate the entire input before changing any interface.
        for (String line : Files.readAllLines(Paths.get(getScriptArgs()[0]))) {
            String[] row = line.split("\t");
            for (int i = 2; i < row.length; i++) parser.parse(row[i]);
            rows.add(row);
        }
        int count = 0;
        for (String[] row : rows) {
            DataType returnType = parser.parse(row[2]);
            List<Variable> parameters = new ArrayList<>();
            for (int i = 3; i < row.length; i++) {
                parameters.add(new ParameterImpl("argument" + (i - 2), parser.parse(row[i]), currentProgram));
            }
            Function function = getFunctionAt(toAddr(Long.decode(row[0])));
            if (function == null) function = createFunction(toAddr(Long.decode(row[0])), row[1]);
            if (function == null) throw new Exception("Cannot create " + row[1]);
            function.setName(row[1], SourceType.IMPORTED);
            function.setCallingConvention("__cdecl");
            function.setReturnType(returnType, SourceType.IMPORTED);
            function.replaceParameters(parameters, Function.FunctionUpdateType.DYNAMIC_STORAGE_ALL_PARAMS, true, SourceType.IMPORTED);
            function.setInline(false);
            println("RECOVERY_RUNTIME " + String.join(" ", row));
            count++;
        }
        println("RECOVERY_RUNTIME_COMPLETE " + count);
        currentProgram.getOptions("Manalink recovery").setBoolean("ExternalInterfacesReady", true);
    }
}
