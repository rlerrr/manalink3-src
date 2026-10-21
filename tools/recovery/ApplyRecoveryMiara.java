// Miara's optimized worker takes player/amount in EAX/EDX; card is nonphysical.
// @category Manalink
import ghidra.app.script.GhidraScript;
import ghidra.program.model.data.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.*;
import java.util.*;

public class ApplyRecoveryMiara extends GhidraScript {
    public void run() throws Exception {
        Function worker=getFunctionAt(toAddr(0x25a22a0));
        if(worker==null)throw new Exception("Missing Miara worker");
        Map<String,DataType> types=new HashMap<>();
        for(Parameter p:worker.getParameters())types.put(p.getName(),p.getDataType());
        Parameter[] parameters=new Parameter[2];
        String[] names={"player","amount"};
        for(int i=0;i<names.length;i++) {
            if(!types.containsKey(names[i]))throw new Exception("Missing original Miara type "+names[i]);
            parameters[i]=new ParameterImpl(names[i],types.get(names[i]),currentProgram);
        }
        int callers=0;
        for(Reference reference:getReferencesTo(worker.getEntryPoint())) {
            if(!reference.getReferenceType().isCall())continue;
            Function caller=getFunctionContaining(reference.getFromAddress());
            boolean effect=reference.getFromAddress().equals(toAddr(0x25a246b)) && caller!=null &&
               caller.getEntryPoint().equals(toAddr(0x25a2340));
            boolean card=reference.getFromAddress().equals(toAddr(0x25aca41)) && caller!=null &&
               caller.getEntryPoint().equals(toAddr(0x25ac8e0));
            if(!effect && !card)
                throw new Exception("Unverified Miara worker caller "+reference);
            callers++;
        }
        if(callers!=2)throw new Exception("Expected two Miara worker callers; got "+callers);
        worker.setCustomVariableStorage(false);
        worker.setCallingConvention("__regparm2");
        worker.setReturnType(VoidDataType.dataType,SourceType.IMPORTED);
        worker.replaceParameters(Function.FunctionUpdateType.DYNAMIC_STORAGE_ALL_PARAMS,true,SourceType.IMPORTED,parameters);
        println("RECOVERY_MIARA_COMPLETE physical=2 nonphysical=1 direct_callers=2");
    }
}
