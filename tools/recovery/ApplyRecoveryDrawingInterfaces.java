// Type existing drawing-library imports; IAT slots are data, not function bodies.
// @category Manalink
import ghidra.app.script.GhidraScript;
import ghidra.program.model.data.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.*;
import ghidra.util.data.DataTypeParser;
import ghidra.util.data.DataTypeParser.AllowedDataTypes;
import java.util.*;

public class ApplyRecoveryDrawingInterfaces extends GhidraScript {
    public void run() throws Exception {
        currentProgram.getOptions("Manalink recovery").setBoolean("ExternalInterfacesReady", false);
        String[][] signatures = {{"get_rarity", "Rarities *", "int"},
                                 {"get_watermarks", "Watermark *"},
                                 {"get_border_color", "border_t", "uint32_t"}};
        long[] slots = {0x27b5374, 0x27b5378, 0x27b536c};
        DataTypeManager manager = currentProgram.getDataTypeManager();
        DataTypeParser parser = new DataTypeParser(manager, manager, null, AllowedDataTypes.ALL);
        Map<String, Function> imports = new HashMap<>();
        FunctionIterator iterator = currentProgram.getFunctionManager().getExternalFunctions();
        while (iterator.hasNext()) {
            Function function = iterator.next();
            for (String[] signature : signatures) {
                if (function.getName().equals(signature[0])) {
                    if (imports.put(signature[0], function) != null)
                        throw new Exception("Ambiguous external import " + signature[0]);
                }
            }
        }
        for (int index = 0; index < signatures.length; ++index) {
            String[] signature = signatures[index];
            boolean namedSlot = false;
            for (Symbol symbol : currentProgram.getSymbolTable().getSymbols(toAddr(slots[index])))
                namedSlot |= symbol.getName().equals("__imp__" + signature[0]);
            if (!namedSlot || getDataAt(toAddr(slots[index])) == null)
                throw new Exception("Missing verified IAT slot " + signature[0]);
            for (int i = 1; i < signature.length; ++i) parser.parse(signature[i]);
        }
        for (int index = 0; index < signatures.length; ++index) {
            String[] signature = signatures[index];
            Function function = imports.get(signature[0]);
            if (function == null)
                function = currentProgram.getExternalManager().addExtFunction(
                    "Drawcardlib.dll", signature[0], null, SourceType.IMPORTED).getFunction();
            function.setCallingConvention("__cdecl");
            function.setReturnType(parser.parse(signature[1]), SourceType.IMPORTED);
            List<Variable> parameters = new ArrayList<>();
            for (int i = 2; i < signature.length; ++i)
                parameters.add(new ParameterImpl("csvid", parser.parse(signature[i]), currentProgram));
            function.replaceParameters(parameters, Function.FunctionUpdateType.DYNAMIC_STORAGE_ALL_PARAMS,
                                       true, SourceType.IMPORTED);
            function.setInline(false);
            DataType pointer = new PointerDataType(new FunctionDefinitionDataType(function, true), manager);
            removeDataAt(toAddr(slots[index]));
            createData(toAddr(slots[index]), pointer);
            ExternalLocation location = currentProgram.getExternalManager().getExternalLocation(function.getSymbol());
            currentProgram.getReferenceManager().addExternalReference(toAddr(slots[index]), 0, location,
                                                                     SourceType.IMPORTED, RefType.DATA);
            println("RECOVERY_DRAWING_IMPORT " + function.getEntryPoint() + " " + function.getSignature());
        }
        currentProgram.getOptions("Manalink recovery").setBoolean("ExternalInterfacesReady", true);
        println("RECOVERY_DRAWING_INTERFACES_COMPLETE count=3");
    }
}
