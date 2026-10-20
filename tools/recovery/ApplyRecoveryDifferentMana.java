// @category Manalink
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.SourceType;
import java.util.*;
public class ApplyRecoveryDifferentMana extends GhidraScript {
    public void run() throws Exception {
        Function function=getFunctionAt(toAddr(0x2670e00));
        if(function==null || function.getParameterCount()!=6)throw new Exception("Unexpected distinct-colors specialization");
        List<Variable> parameters=new ArrayList<>();
        for(int i=0;i<5;i++){
            Parameter p=function.getParameter(i);
            parameters.add(new ParameterImpl(p.getName(),p.getDataType(),currentProgram));
        }
        function.setCustomVariableStorage(false);
        function.setName("produce_mana_tapped_all_different_colors_2022_impl",SourceType.IMPORTED);
        function.setCallingConvention("__regparm3");
        function.replaceParameters(parameters,Function.FunctionUpdateType.DYNAMIC_STORAGE_ALL_PARAMS,true,SourceType.IMPORTED);
        println("RECOVERY_DIFFERENT_MANA_COMPLETE physical=5 mca=1");
    }
}
