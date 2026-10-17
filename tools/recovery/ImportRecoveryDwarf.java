// Import matching DWARF only; leave unrelated whole-program analyzers disabled.
// @category Manalink
import ghidra.app.plugin.core.analysis.DWARFAnalyzer;
import ghidra.app.script.GhidraScript;
import ghidra.app.util.importer.MessageLog;
import ghidra.framework.options.Options;
import ghidra.program.model.listing.Program;

public class ImportRecoveryDwarf extends GhidraScript {
    @Override
    public void run() throws Exception {
        DWARFAnalyzer analyzer = new DWARFAnalyzer();
        Options options = currentProgram.getOptions(Program.ANALYSIS_PROPERTIES).getOptions("DWARF");
        analyzer.registerOptions(options, currentProgram);
        options.setString("Default Calling Convention", "__cdecl");
        options.setBoolean("Import Source Line Info", false);
        analyzer.optionsChanged(options, currentProgram);
        MessageLog log = new MessageLog();
        boolean imported = analyzer.added(currentProgram, currentProgram.getMemory(), monitor, log);
        println(log.toString());
        if (!imported || !DWARFAnalyzer.isAlreadyImported(currentProgram)) {
            throw new Exception("DWARF import did not complete");
        }
        println("RECOVERY_DWARF_COMPLETE functions=" + currentProgram.getFunctionManager().getFunctionCount() +
                " types=" + currentProgram.getDataTypeManager().getDataTypeCount(true));
    }
}
