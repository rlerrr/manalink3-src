// Typed EXE boundaries and byte arrays used by native charge_spell_cost.
// @category Manalink
import ghidra.app.script.GhidraScript;
import ghidra.program.model.data.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.SourceType;

public class ApplyRecoverySpellCost extends GhidraScript {
    private void bind(long address,String name,DataType...types) throws Exception {
        Function f=getFunctionAt(toAddr(address));
        if(f==null)f=createFunction(toAddr(address),name);
        if(f==null)throw new Exception("Cannot create EXE boundary "+name);
        f.setCustomVariableStorage(false);
        f.setCallingConvention("__cdecl");
        f.setName(name,SourceType.IMPORTED);
        f.setReturnType(IntegerDataType.dataType,SourceType.IMPORTED);
        Parameter[] parameters=new Parameter[types.length];
        for(int i=0;i<types.length;i++)parameters[i]=new ParameterImpl("argument"+i,types[i],currentProgram);
        f.replaceParameters(Function.FunctionUpdateType.DYNAMIC_STORAGE_ALL_PARAMS,true,SourceType.IMPORTED,parameters);
        f.setInline(false);
    }
    private void bytes(long address,String name) throws Exception {
        clearListing(toAddr(address),toAddr(address+7));
        createData(toAddr(address),new ArrayDataType(SignedByteDataType.dataType,8,1));
        createLabel(toAddr(address),name,true,SourceType.IMPORTED);
    }
    public void run() throws Exception {
        DataType integer=IntegerDataType.dataType;
        bind(0x4302c0,"charge_spell_cost_exe_2022",integer,integer,integer,integer);
        bind(0x4301a0,"record_mana_spent_exe_2022",integer,integer);
        bind(0x430260,"rollback_mana_spent_exe_2022",new PointerDataType(integer,currentProgram.getDataTypeManager()));
        bytes(0x55cf20,"spell_cost_adjustment_bytes_2022");
        bytes(0x55cef0,"spell_cost_paid_snapshot_bytes_2022");
        println("RECOVERY_SPELL_COST_COMPLETE exe_signatures=3 signed_byte_views=2");
    }
}
