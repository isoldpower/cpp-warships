// Shortcuts are letters, but a terminal only ever hears the character a key produced, so
// with a Russian, Greek or any other non-Latin layout the game would hear "л" where it
// listens for "k". The browser knows which physical key was pressed, so when a letter key
// produces something outside ASCII, the game is sent that key's Latin letter instead.
// Latin layouts such as AZERTY or Dvorak are left alone: their own letters are what the
// player reads on the keys and in the legend.

const LETTER_KEY = /^Key([A-Z])$/;
const LAST_ASCII_CODE = 0x7f;

export function forwardLetterKeysByPosition(terminal, typeText) {
    terminal.attachCustomKeyEventHandler((event) => {
        const letter = LETTER_KEY.exec(event.code);
        const isPlainPress =
            event.type === "keydown" && !event.ctrlKey && !event.altKey && !event.metaKey;
        const isNonLatinCharacter =
            event.key.length === 1 && event.key.charCodeAt(0) > LAST_ASCII_CODE;

        if (letter === null || !isPlainPress || !isNonLatinCharacter) {
            return true;
        }

        event.preventDefault();
        typeText(event.shiftKey ? letter[1] : letter[1].toLowerCase());
        return false;
    });
}
