// @category Manalink
import ghidra.app.script.GhidraScript;
import ghidra.program.model.data.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.SourceType;
import java.util.*;

public class ApplyRecoveryInstInfoReturn extends GhidraScript {
    public void run() throws Exception {
        Function function=getFunctionAt(toAddr(0x26b3740));
        if (function==null || function.getParameterCount()!=2)
            throw new Exception("Unexpected inst_info interface");
        List<DataType> matches=new ArrayList<>();
        currentProgram.getDataTypeManager().findDataTypes("inst_info_t",matches);
        DataType result=null;
        for (DataType type:matches) {
            DataType base=type instanceof TypeDef ? ((TypeDef)type).getBaseDataType():type;
            if (!(base instanceof Structure)) continue;
            Structure record=(Structure)base;
            if (record.getLength()==8 && record.getNumComponents()==3 &&
                record.getComponent(0).getOffset()==0 && "id".equals(record.getComponent(0).getFieldName()) &&
                record.getComponent(1).getOffset()==2 && "data2".equals(record.getComponent(1).getFieldName()) &&
                record.getComponent(2).getOffset()==4 && "data".equals(record.getComponent(2).getFieldName())) {
                result=type;break;
            }
        }
        if(result==null)throw new Exception("Missing eight-byte information record");
        VariableStorage storage=new VariableStorage(currentProgram,
            currentProgram.getLanguage().getRegister("EDX"),currentProgram.getLanguage().getRegister("EAX"));
        function.setCustomVariableStorage(true);
        function.setReturn(result,storage,SourceType.IMPORTED);
        println("RECOVERY_INST_INFO_RETURN_COMPLETE "+function.getReturn().getVariableStorage());
    }
}
