// @category Manalink
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.*;
public class InspectRecoveryDrawingImports extends GhidraScript {
    public void run() throws Exception {
        FunctionIterator functions = currentProgram.getFunctionManager().getExternalFunctions();
        int count = 0;
        while (functions.hasNext()) {
            Function f = functions.next(); ++count;
            if (f.getName().contains("rarity") || f.getName().contains("watermark") || f.getName().contains("border"))
                println("DRAWING_EXTERNAL " + f.getEntryPoint() + " " + f.getName(true));
        }
        println("EXTERNAL_COUNT " + count);
        for (long slot : new long[]{0x27b536c,0x27b5374,0x27b5378}) {
            for (Symbol symbol : currentProgram.getSymbolTable().getSymbols(toAddr(slot)))
                println("DRAWING_SYMBOL " + symbol.getAddress() + " " + symbol.getName(true));
            Data data = getDataAt(toAddr(slot));
            println("DRAWING_DATA " + data);
            if (data != null) println("DRAWING_VALUE " + data.getValue());
        }
    }
}
