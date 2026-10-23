// Use concrete DWARF: the abstract ai argument has no physical entry storage.
// @category Manalink
import ghidra.app.script.GhidraScript;
import ghidra.program.model.data.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.SourceType;
import java.util.*;

public class ApplyRecoveryMemorial extends GhidraScript {
    public void run() throws Exception {
        Function f=getFunctionAt(toAddr(0x22ca5c0));
        if(f==null || f.getParameterCount()!=15 || !f.getParameter(11).getName().equals("ai"))
            throw new Exception("Expected unmodified abstract Memorial signature");
        List<Variable> parameters=new ArrayList<>();
        for(int i=0;i<15;i++) {
            if(i==11)continue;
            Parameter p=f.getParameter(i);
            parameters.add(new ParameterImpl(p.getName(),p.getDataType(),currentProgram));
        }
        f.setCustomVariableStorage(false);
        f.setCallingConvention("__regparm3");
        f.setName("dominaria_memorial_2022_impl",SourceType.IMPORTED);
        f.setReturnType(IntegerDataType.dataType,SourceType.IMPORTED);
        f.replaceParameters(parameters,Function.FunctionUpdateType.DYNAMIC_STORAGE_ALL_PARAMS,true,SourceType.IMPORTED);
        for(Parameter p:f.getParameters())println("RECOVERY_MEMORIAL_PARAMETER "+p.getName()+" "+p.getVariableStorage());
        println("RECOVERY_MEMORIAL_COMPLETE physical=14 registers=3 stack_words=11 ai=nonphysical");
    }
}
