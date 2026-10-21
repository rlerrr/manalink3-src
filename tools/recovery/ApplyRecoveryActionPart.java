// Concrete seven-argument worker: EAX/EDX/ECX and four stack words, plus abort cold range.
// @category Manalink
import ghidra.app.script.GhidraScript;
import ghidra.program.model.data.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.*;

public class ApplyRecoveryActionPart extends GhidraScript {
    public void run() throws Exception {
        Function worker=getFunctionAt(toAddr(0x2666bc0));
        Function cook=getFunctionAt(toAddr(0x2666f10));
        if(worker==null || cook==null)throw new Exception("Missing action worker or decoder");
        byte[] expected=java.util.HexFormat.of().parseHex("e8ab1effff");
        if(!java.util.Arrays.equals(getBytes(toAddr(0x2710008),5),expected))
            throw new Exception("Unexpected native action abort cold range");
        Function cold=getFunctionAt(toAddr(0x2710008));
        if(cold!=null && !cold.equals(worker))removeFunction(cold);
        DataType integer=IntegerDataType.dataType;
        DataType action=cook.getParameter(2).getDataType();
        DataType counter=((Pointer)cook.getParameter(3).getDataType()).getDataType();
        String[] names={"player","card","p","c","action","counter_type","counter_num"};
        DataType[] types={integer,integer,integer,integer,action,counter,integer};
        Parameter[] parameters=new Parameter[7];
        for(int i=0;i<7;i++)parameters[i]=new ParameterImpl(names[i],types[i],currentProgram);
        worker.setCustomVariableStorage(false);
        worker.setCallingConvention("__regparm3");
        worker.setName("action_on_card_impl_2022",SourceType.IMPORTED);
        worker.setReturnType(integer,SourceType.IMPORTED);
        worker.replaceParameters(Function.FunctionUpdateType.DYNAMIC_STORAGE_ALL_PARAMS,true,SourceType.IMPORTED,parameters);
        println("RECOVERY_ACTION_PART_COMPLETE physical=7 registers=3 stack_words=4 cold_bytes=5");
    }
}
