// Apply prevalidated concrete clone interfaces prepared from DWARF locations.
// @category Manalink
import ghidra.app.script.GhidraScript;
import ghidra.program.model.data.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.SourceType;
import java.nio.file.*;
import java.util.*;

public class ApplyRecoveryCloneInterfaces extends GhidraScript {
    public void run() throws Exception {
        List<String> lines = Files.readAllLines(Path.of(getScriptArgs()[0]));
        for (String line : lines) {
            String[] fields = line.split("\t");
            if (fields.length < 5 || getFunctionAt(toAddr(fields[0])) == null)
                throw new Exception("Invalid clone interface: " + line);
            for (int i = 4; i < fields.length; ++i)
                if (!fields[i].matches("(?:int|int_ptr|int_ptr_ptr):[A-Za-z_][A-Za-z_0-9]*"))
                    throw new Exception("Invalid clone parameter: " + fields[i]);
            if (!fields[2].equals("int") && !fields[2].equals("void"))
                throw new Exception("Invalid clone return type");
        }
        for (String line : lines) {
            String[] fields = line.split("\t");
            Function function = getFunctionAt(toAddr(fields[0]));
            function.setCustomVariableStorage(false);
            function.setCallingConvention(fields[3]);
            function.setName(fields[1], SourceType.IMPORTED);
            function.setReturnType(fields[2].equals("int") ? IntegerDataType.dataType : VoidDataType.dataType,
                                   SourceType.IMPORTED);
            List<Variable> parameters = new ArrayList<>();
            for (int i = 4; i < fields.length; ++i) {
                String[] parameter = fields[i].split(":");
                DataType type = parameter[0].equals("int") ? IntegerDataType.dataType :
                    new PointerDataType(parameter[0].equals("int_ptr_ptr") ? new PointerDataType(IntegerDataType.dataType, currentProgram.getDataTypeManager()) : IntegerDataType.dataType, currentProgram.getDataTypeManager());
                parameters.add(new ParameterImpl(parameter[1], type, currentProgram));
            }
            function.replaceParameters(parameters, Function.FunctionUpdateType.DYNAMIC_STORAGE_ALL_PARAMS,
                                       true, SourceType.IMPORTED);
            function.setInline(false);
            for (Parameter parameter : function.getParameters())
                println("RECOVERY_CLONE_PARAMETER " + fields[1] + " " + parameter.getName() + " " + parameter.getVariableStorage());
        }
        println("RECOVERY_CLONE_INTERFACES_COMPLETE count=" + lines.size());
    }
}
