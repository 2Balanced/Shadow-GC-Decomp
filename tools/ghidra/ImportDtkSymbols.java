// Applies names from a decomp-toolkit symbols.txt to the current Ghidra program.
//
// Only symbols that carry information are applied: auto-generated dtk names
// (fn_, lbl_, gap_, jumptable_, @stringBase, ...) are skipped so that existing
// Ghidra names are not clobbered by placeholders. Function sizes are not
// enforced; Ghidra's own boundaries are kept.
//
// Headless use (any Ghidra install with a GameCube loader extension):
//   analyzeHeadless <projDir> <projName> -process <program> -scriptPath tools/ghidra
//       -postScript ImportDtkSymbols.java config/GUPE8P/symbols.txt
// GUI use: run from the Script Manager and pick symbols.txt when prompted.
//@category GameCube.Decomp

import ghidra.app.cmd.disassemble.DisassembleCommand;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.symbol.SourceType;
import ghidra.program.model.symbol.Symbol;
import ghidra.program.model.symbol.SymbolTable;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class ImportDtkSymbols extends GhidraScript {
    private static final Pattern LINE = Pattern.compile(
        "^(\\S+)\\s*=\\s*([.\\w]+):0x([0-9A-Fa-f]+);\\s*//(.*)$");
    private static final Pattern PLACEHOLDER = Pattern.compile(
        "^(fn|lbl|gap|jumptable|switch|auto|dtor|ctor)_[0-9A-Fa-f_]+.*|^@.*|^\\.\\..*");

    @Override
    protected void run() throws Exception {
        File file;
        String[] args = getScriptArgs();
        if (args.length > 0) {
            file = new File(args[0]);
        } else {
            file = askFile("decomp-toolkit symbols.txt", "Import");
        }

        SymbolTable st = currentProgram.getSymbolTable();
        int applied = 0, functions = 0, skipped = 0;
        try (BufferedReader r = new BufferedReader(new FileReader(file))) {
            String line;
            while ((line = r.readLine()) != null) {
                Matcher m = LINE.matcher(line.trim());
                if (!m.matches()) continue;
                String name = m.group(1);
                if (PLACEHOLDER.matcher(name).matches()) { skipped++; continue; }
                Address addr = toAddr(Long.parseLong(m.group(3), 16));
                boolean isFunc = m.group(4).contains("type:function");

                if (isFunc) {
                    Function f = getFunctionAt(addr);
                    if (f == null) {
                        if (getInstructionAt(addr) == null) {
                            new DisassembleCommand(addr, null, true).applyTo(currentProgram, monitor);
                        }
                        f = createFunction(addr, name);
                    }
                    if (f != null) {
                        if (!f.getName().equals(name)) f.setName(name, SourceType.IMPORTED);
                        functions++;
                        applied++;
                        continue;
                    }
                }
                Symbol existing = st.getPrimarySymbol(addr);
                if (existing != null && existing.getName().equals(name)) { applied++; continue; }
                st.createLabel(addr, name, SourceType.IMPORTED).setPrimary();
                applied++;
            }
        }
        println(String.format("Applied %d symbols (%d functions), skipped %d placeholders", applied, functions, skipped));
    }
}
