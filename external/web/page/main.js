import createCppWarships from "./cpp_warships.js";
import { requireCrossOriginIsolation } from "./isolation.js";
import { forwardLetterKeysByPosition } from "./layout.js";
import { forwardScrollingToGame } from "./scrolling.js";
import { openStandardStreams } from "./streams.js";
import { openTerminal } from "./terminal.js";


const INPUT_NEVER_ENDS = null;
const FAREWELL = "\r\n[game finished — reload to play again]\r\n";

const element = document.getElementById("terminal");
const { terminal, fitToElement } = openTerminal(element);
requireCrossOriginIsolation(terminal);
const { readTypedByte, writeDrawnByte, typeText } = openStandardStreams(terminal);
forwardScrollingToGame(terminal, typeText);
forwardLetterKeysByPosition(terminal, typeText);

let hasRuntimeExited = false;
const game = await createCppWarships({
    preRun: [(runtime) => runtime.FS.init(readTypedByte, writeDrawnByte, INPUT_NEVER_ENDS)],
    onExit: () => {
        hasRuntimeExited = true;
        terminal.write(FAREWELL);
    },
});

const tellGameItsNewSize = () => {
    fitToElement();
    if (!hasRuntimeExited) {
        game._ftxui_on_resize(terminal.cols, terminal.rows);
    }
};
new ResizeObserver(tellGameItsNewSize).observe(element);
tellGameItsNewSize();
