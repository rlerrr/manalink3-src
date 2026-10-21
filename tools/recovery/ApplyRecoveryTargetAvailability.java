// Remove a constant source-only parameter from the concrete targeting worker.
// @category Manalink
import java.util.*;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.SourceType;

public class ApplyRecoveryTargetAvailability extends GhidraScript {
    public void run() throws Exception {
        Function function=getFunctionAt(toAddr(0x265f1f0));
        if(function==null)throw new Exception("Missing target availability specialization");
        List<Variable> parameters=new ArrayList<>();
        boolean removed=false;
        for(Parameter p:function.getParameters()) {
            if(p.getName().equals("test_damage_target_or_source")) {removed=true;continue;}
            parameters.add(new ParameterImpl(p.getName(),p.getDataType(),currentProgram));
        }
        if(parameters.size()!=22 || (removed && function.getParameterCount()!=23))
            throw new Exception("Unexpected target availability physical parameter list");
        function.setCustomVariableStorage(false);
        function.setCallingConvention("__regparm3");
        function.replaceParameters(parameters,Function.FunctionUpdateType.DYNAMIC_STORAGE_ALL_PARAMS,true,SourceType.IMPORTED);
        println("RECOVERY_TARGET_AVAILABILITY_COMPLETE physical=22 test_damage_target_or_source=0");
    }
}
