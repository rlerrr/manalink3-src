// @category Manalink
import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.*;
import ghidra.program.model.data.*;
import ghidra.program.model.listing.Function;
import ghidra.program.model.pcode.*;
import ghidra.program.model.symbol.SourceType;
import java.util.*;
public class ApplyRecoveryTextLocals extends GhidraScript {
    public void run() throws Exception {
        Function function = getFunctionAt(toAddr(0x2675d70));
        if (function == null || function.getParameterCount() != 3)
            throw new Exception("Missing text parser");
        DataType output = function.getParameter(1).getDataType();
        if (!(output instanceof Pointer)) throw new Exception("Unexpected table output type");
        DataType pointer = ((Pointer)output).getDataType();
        if (!(pointer instanceof Pointer) || ((Pointer)pointer).getDataType().getLength() != 12)
            throw new Exception("Unexpected text-entry pointer layout");
        DecompInterface decompiler = new DecompInterface();
        DecompileOptions options = new DecompileOptions(); options.grabFromProgram(currentProgram);
        decompiler.setOptions(options); decompiler.openProgram(currentProgram);
        try {
            DecompileResults result = decompiler.decompileFunction(function,120,monitor);
            if (!result.decompileCompleted()) throw new Exception("Text parser decompilation failed");
            HighSymbol table = null;
            Iterator<HighSymbol> symbols = result.getHighFunction().getLocalSymbolMap().getSymbols();
            while (symbols.hasNext()) {
                HighSymbol symbol = symbols.next();
                if (symbol.getName().equals("pvStack_218")) table = symbol;
            }
            if (table == null) throw new Exception("Missing parser table local");
            println("RECOVERY_TEXT_TABLE_STORAGE " + table.getStorage());
            HighFunctionDBUtil.updateDBVariable(table, "lt", pointer, SourceType.USER_DEFINED);
        } finally { decompiler.dispose(); }
        println("RECOVERY_TEXT_LOCALS_COMPLETE count=1");
    }
}
