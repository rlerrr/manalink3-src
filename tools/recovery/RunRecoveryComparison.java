// Build a review-only Version Tracking session and decompile the pilot pairs.
// No match markup is applied. Run only in the dedicated ComparisonPilot project.
// @category Manalink
import java.io.*;
import java.nio.file.*;
import java.util.*;
import ghidra.app.cmd.disassemble.DisassembleCommand;
import ghidra.app.decompiler.*;
import ghidra.app.script.GhidraScript;
import ghidra.feature.vt.api.correlator.program.*;
import ghidra.feature.vt.api.db.VTSessionDB;
import ghidra.feature.vt.api.main.*;
import ghidra.framework.model.*;
import ghidra.program.model.address.*;
import ghidra.program.model.listing.*;

public class RunRecoveryComparison extends GhidraScript {
    private AddressSet prepare(Program program, Path requests, Path output, String year) throws Exception {
        AddressSet selected = new AddressSet();
        int conventions = 0;
        for (String line : Files.readAllLines(output.resolve(year + "-metadata/register-conventions.tsv"))) {
            String[] row = line.split("\t");
            Address address = program.getAddressFactory().getDefaultAddressSpace().getAddress(Long.decode(row[0]));
            Function function = program.getFunctionManager().getFunctionAt(address);
            if (function != null) {
                function.setCustomVariableStorage(false);
                function.setCallingConvention(row[2]);
                conventions++;
            }
        }
        for (Function function : program.getFunctionManager().getFunctions(true)) function.setInline(false);
        println("COMPARISON_ABI " + year + " conventions=" + conventions);
        DecompInterface decompiler = new DecompInterface();
        DecompileOptions options = new DecompileOptions();
        options.grabFromProgram(program);
        decompiler.setOptions(options);
        decompiler.openProgram(program);
        try (PrintWriter manifest = new PrintWriter(Files.newBufferedWriter(output.resolve(year + "-decompiled.tsv")))) {
            manifest.println("name\taddress\tstatus\tprototype");
            for (String line : Files.readAllLines(requests)) {
                String[] fields = line.split("\t");
                String artifact = fields.length > 4 ? fields[4] : fields[2];
                if (!artifact.matches("[A-Za-z0-9_]+")) throw new Exception("Invalid artifact identifier: " + artifact);
                Address start = program.getAddressFactory().getDefaultAddressSpace().getAddress(Long.parseUnsignedLong(fields[0], 16));
                Address end = program.getAddressFactory().getDefaultAddressSpace().getAddress(Long.parseUnsignedLong(fields[3], 16) - 1);
                AddressSet body = new AddressSet(start, end);
                Function function = program.getFunctionManager().getFunctionAt(start);
                if (function == null) throw new Exception("Missing DWARF function: " + year + " " + fields[2]);
                new DisassembleCommand(start, body, true).applyTo(program, monitor);
                function.setBody(body);
                for (Instruction ins : program.getListing().getInstructions(body, true)) {
                    Address[] flows = ins.getFlows();
                    if (ins.getFlowType().isJump() && !ins.getFlowType().isConditional() &&
                        !ins.getFlowType().isComputed() && flows.length == 1 && !body.contains(flows[0]) &&
                        program.getFunctionManager().getFunctionAt(flows[0]) != null) {
                        ins.setFlowOverride(FlowOverride.CALL_RETURN);
                    }
                }
                selected.add(start);
                decompiler.flushCache();
                DecompileResults result = decompiler.decompileFunction(function, 60, monitor);
                boolean success = result.decompileCompleted();
                manifest.println(fields[2] + "\t" + fields[0] + "\t" + (success ? "ok" : "failed") + "\t" + function.getSignature().getPrototypeString());
                manifest.flush();
                if (!success) throw new Exception("Decompile failure: " + year + " " + fields[2] + " " + result.getErrorMessage());
                Files.writeString(output.resolve(year + "-" + artifact + ".c"),
                    "/* Release comparison only; " + year + " " + fields[1] + " @ " + start +
                    ". Unreviewed DWARF decompilation; native external call models are incomplete. */\n" + result.getDecompiledFunction().getC());
                println("COMPARISON_DECOMPILED " + year + " " + fields[2]);
            }
        } finally {
            decompiler.dispose();
        }
        return selected;
    }

    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        Path output = Paths.get(args[0]);
        Files.createDirectories(output);
        DomainFolder folder = state.getProject().getProjectData().getRootFolder();
        DomainFile sourceFile = folder.getFile("2020-ManalinkEh.analysis.dll");
        String sessionName = args.length > 1 ? args[1] : "2020-to-2022-pilot";
        if (folder.getFile(sessionName) != null) throw new Exception("Session already exists; preserve it or choose a new project.");
        Program source = (Program)sourceFile.getDomainObject(this, true, false, monitor);
        VTSessionDB session = null;
        try {
            int transaction = source.startTransaction("Prepare comparison pilot");
            AddressSet sourceSet;
            boolean success = false;
            try {
                sourceSet = prepare(source, output.resolve("2020-requests.tsv"), output, "2020");
                success = true;
            } finally {
                source.endTransaction(transaction, success);
            }
            AddressSet destinationSet = prepare(currentProgram, output.resolve("2022-requests.tsv"), output, "2022");
            source.save("Prepare comparison pilot", monitor);
            end(true);
            currentProgram.save("Prepare comparison pilot", monitor);
            session = new VTSessionDB(sessionName, source, currentProgram, this);
            folder.createFile(sessionName, session, monitor);
            int sessionTransaction = session.startTransaction("Correlate pilot without applying markup");
            success = false;
            try (PrintWriter out = new PrintWriter(Files.newBufferedWriter(output.resolve("version-tracking.tsv")))) {
                out.println("correlator\tsource\tdestination\tsimilarity\tconfidence");
                List<VTProgramCorrelatorFactory> factories = List.of(
                    new SymbolNameProgramCorrelatorFactory(),
                    new ExactMatchBytesProgramCorrelatorFactory(),
                    new ExactMatchInstructionsProgramCorrelatorFactory());
                for (VTProgramCorrelatorFactory factory : factories) {
                    ghidra.feature.vt.api.util.VTOptions vtOptions = factory.createDefaultOptions();
                    if (factory instanceof SymbolNameProgramCorrelatorFactory) {
                        vtOptions.setBoolean(SymbolNameProgramCorrelatorFactory.INCLUDE_EXTERNAL_SYMBOLS, false);
                    }
                    VTMatchSet matches = factory.createCorrelator(source, sourceSet, currentProgram,
                        destinationSet, vtOptions).correlate(session, monitor);
                    for (VTMatch match : matches.getMatches()) {
                        out.println(factory.getName() + "\t" + match.getSourceAddress() + "\t" +
                            match.getDestinationAddress() + "\t" + match.getSimilarityScore() + "\t" + match.getConfidenceScore());
                    }
                    println("COMPARISON_VT " + factory.getName() + " matches=" + matches.getMatchCount());
                }
                success = true;
            } finally {
                session.endTransaction(sessionTransaction, success);
            }
            session.save();
            println("COMPARISON_COMPLETE session=" + sessionName + " markup_applied=false");
        } finally {
            if (session != null) session.release(this);
            source.release(this);
        }
    }
}
