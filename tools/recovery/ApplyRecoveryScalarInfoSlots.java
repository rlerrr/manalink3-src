// @category Manalink
// Scoped union-facet selection follows Ghidra 12.0 ForceUnionAction and
// HighFunctionDBUtil (Apache License 2.0, National Security Agency).
// https://www.apache.org/licenses/LICENSE-2.0
import java.nio.file.*;
import java.io.*;
import java.util.*;
import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.*;
import ghidra.program.model.address.Address;
import ghidra.program.model.data.*;
import ghidra.program.model.listing.Function;
import ghidra.program.model.pcode.*;
import ghidra.program.model.symbol.SourceType;

public class ApplyRecoveryScalarInfoSlots extends GhidraScript {
    private DataType related(Varnode vn, Union union) {
        if (vn == null || vn.getHigh() == null) return null;
        HighVariable high = vn.getHigh();
        DataType dt = high.getDataType();
        if (dt instanceof TypeDef) dt = ((TypeDef)dt).getBaseDataType();
        DataType inner = dt;
        if (inner instanceof Pointer) inner = ((Pointer)inner).getDataType();
        else if (inner instanceof PartialUnion) {
            inner = ((PartialUnion)inner).getParent();
            if (inner instanceof TypeDef) inner = ((TypeDef)inner).getBaseDataType();
        }
        if (inner == union) return dt;
        HighSymbol symbol = high.getSymbol();
        if (symbol == null) return null;
        dt = symbol.getDataType();
        if (dt instanceof TypeDef) dt = ((TypeDef)dt).getBaseDataType();
        return dt == union ? dt : null;
    }

    private static class Facet {
        Function function; DataType type; int ordinal; Address address; long hash;
        Facet(Function f, DataType t, int n, Address a, long h) {
            function=f; type=t; ordinal=n; address=a; hash=h;
        }
    }

    private Facet facet(Function function, ClangFieldToken token, HighFunction high, String desired) throws Exception {
        DataType type = token.getDataType();
        if (type instanceof TypeDef) type = ((TypeDef)type).getBaseDataType();
        if (!(type instanceof Union) || type.getLength() != 4)
            throw new Exception("Unexpected info-slot union " + type);
        Union union = (Union)type;
        DataTypeComponent field = null;
        for (DataTypeComponent component : union.getDefinedComponents())
            if (desired.equals(component.getFieldName())) field = component;
        if (field == null || field.getLength() != 4)
            throw new Exception("Missing four-byte scalar info_slot field");
        PcodeOp op = token.getPcodeOp();
        if (op == null) throw new Exception("Missing info-slot p-code");
        int opcode = op.getOpcode(); int slot = -1; DataType parent = null;
        if (opcode == PcodeOp.PTRSUB) {
            parent = related(op.getInput(0), union);
            if (parent != null) {
                slot = 0;
                if (op.getInput(1).getOffset() == 0) {
                    do {
                        Varnode vn = op.getOutput();
                        PcodeOp next = vn.getLoneDescend();
                        if (next == null) break;
                        op = next; slot = op.getSlot(vn);
                    } while (op.getOpcode() == PcodeOp.PTRSUB && op.getInput(1).getOffset() == 0);
                }
            }
        } else {
            for (slot = 0; slot < op.getNumInputs(); ++slot) {
                parent = related(op.getInput(slot), union);
                if (parent != null) break;
            }
            if (parent != null && opcode == PcodeOp.SUBPIECE && slot == 0 && !(parent instanceof Pointer))
                slot = -1;
        }
        if (parent == null) { slot = -1; parent = related(op.getOutput(), union); }
        if (parent == null) throw new Exception("Cannot resolve union facet at " + token.getMinAddress());
        DynamicHash hash = new DynamicHash(op, slot, high);
        if (hash.getAddress().equals(Address.NO_ADDRESS) || hash.getHash() == 0)
            throw new Exception("No unique union-facet hash");
        return new Facet(function, parent, field.getOrdinal(), hash.getAddress(), hash.getHash());
    }

    public void run() throws Exception {
        String[] args = getScriptArgs();
        DecompInterface decompiler = new DecompInterface();
        DecompileOptions options = new DecompileOptions(); options.grabFromProgram(currentProgram);
        decompiler.setOptions(options); decompiler.openProgram(currentProgram);
        List<Facet> facets = new ArrayList<>();
        try {
            for (String request : Files.readAllLines(Paths.get(args[0]))) {
                String[] fields = request.split("\t");
                String selected = fields.length > 4 ? fields[4] : "info_slot_fn";
                String desired = fields.length > 5 ? fields[5] : "info_slot";
                Address access = fields.length > 6 ? toAddr(Long.parseUnsignedLong(fields[6],16)) : null;
                Function function = getFunctionAt(toAddr(Long.parseUnsignedLong(fields[0],16)));
                if (function == null) throw new Exception("Missing requested function");
                DecompileResults result = decompiler.decompileFunction(function, 120, monitor);
                if (!result.decompileCompleted()) throw new Exception("Decompilation failed: " + function.getName());
                List<ClangNode> tokens = new ArrayList<>(); result.getCCodeMarkup().flatten(tokens);
                int count = 0;
                for (ClangNode node : tokens)
                    if (node instanceof ClangFieldToken && selected.equals(node.toString())) {
                        Facet choice = facet(function, (ClangFieldToken)node, result.getHighFunction(), desired);
                        if (access != null && !choice.address.equals(access)) continue;
                        facets.add(choice); ++count;
                    }
                if (count == 0) throw new Exception("No info_slot_fn access: " + function.getName());
            }
        } finally { decompiler.dispose(); }
        try (PrintWriter output = new PrintWriter(Files.newBufferedWriter(Paths.get(args[1])))) {
            output.println("function\taccess\tfield\thash\tparent_type");
            for (Facet facet : facets) {
                HighFunctionDBUtil.writeUnionFacet(facet.function, facet.type, facet.ordinal,
                    facet.address, facet.hash, SourceType.USER_DEFINED);
                output.println(facet.function.getEntryPoint()+"\t"+facet.address+"\t"+facet.ordinal+"\t"+
                    Long.toUnsignedString(facet.hash,16)+"\t"+facet.type.getPathName());
            }
        }
        println("RECOVERY_SCALAR_INFO_SLOTS_COMPLETE count=" + facets.size());
    }
}
