import { Terminal } from "https://cdn.jsdelivr.net/npm/@xterm/xterm@6.0.0/+esm";
import { FitAddon } from "https://cdn.jsdelivr.net/npm/@xterm/addon-fit@0.11.0/+esm";

const NO_SCROLLBACK = 0;

export function openTerminal(element) {
    const terminal = new Terminal({ scrollback: NO_SCROLLBACK });
    const fitAddon = new FitAddon();

    terminal.loadAddon(fitAddon);
    terminal.open(element);
    fitAddon.fit();

    return { terminal, fitToElement: () => fitAddon.fit() };
}
