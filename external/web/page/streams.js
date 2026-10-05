const FRAME_END_BYTE = 0;
const NOTHING_TYPED_YET = undefined;

export function openStandardStreams(terminal) {
    const typedBytes = [];
    const typeText = (text) => typedBytes.push(...new TextEncoder().encode(text));
    terminal.onData(typeText);

    const readTypedByte = () =>
        typedBytes.length === 0 ? NOTHING_TYPED_YET : typedBytes.shift();

    const bytesOfFrameSoFar = [];
    const writeDrawnByte = (byte) => {
        if (byte !== FRAME_END_BYTE) {
            bytesOfFrameSoFar.push(byte);
            return;
        }

        terminal.write(new Uint8Array(bytesOfFrameSoFar));
        bytesOfFrameSoFar.length = 0;
    };

    return { readTypedByte, writeDrawnByte, typeText };
}
