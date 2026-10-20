// @category Manalink
import ghidra.app.script.GhidraScript;
import ghidra.program.model.symbol.Reference;
import ghidra.program.model.mem.MemoryBlock;
import java.util.*;
public class InspectRecoveryCardArrays extends GhidraScript {
    public void run() throws Exception {
        for (long address : new long[]{0x27210c0L,0x27210e0L}) {
            int count = address == 0x27210c0L ? 7 : 9;
            MemoryBlock block=currentProgram.getMemory().getBlock(toAddr(address));
            println("CARD_ARRAY address="+Long.toHexString(address)+" count="+count+" block="+block.getName()+" writable="+block.isWrite());
            for(int i=0;i<count;i++) {
                long cell=address+4*i;
                println("CARD_ARRAY_CELL "+Long.toHexString(cell)+" "+currentProgram.getMemory().getInt(toAddr(cell)));
                for(Reference reference:getReferencesTo(toAddr(cell))) {
                    println("CARD_ARRAY_REFERENCE "+Long.toHexString(cell)+" from="+reference.getFromAddress()+" type="+reference.getReferenceType()+" write="+reference.getReferenceType().isWrite());
                }
            }
        }
    }
}
