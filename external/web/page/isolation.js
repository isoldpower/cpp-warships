const WHY_THE_GAME_CANNOT_START =
    "\r\nThis page is not cross-origin isolated, so the game cannot start.\r\n" +
    "Its event loop runs on a worker thread, which needs shared memory, which a\r\n" +
    "browser grants only to a page served with\r\n" +
    "Cross-Origin-Opener-Policy: same-origin and\r\n" +
    "Cross-Origin-Embedder-Policy: require-corp.\r\n";

export function requireCrossOriginIsolation(terminal) {
    if (globalThis.crossOriginIsolated) {
        return;
    }

    terminal.write(WHY_THE_GAME_CANNOT_START);
    throw new Error("the page is not cross-origin isolated");
}
