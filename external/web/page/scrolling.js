// Scrolling, from a wheel, a touchpad or a finger, turned into the wheel reports the game reads.
//
// xterm.js reports at most one wheel step per event, damps small pixel deltas to almost
// nothing and drops sideways and shifted rolls altogether, and it does not turn a finger
// dragging across a touchscreen into anything at all. So the page does it: it adds up how
// far the content was pushed on each axis and reports a step for every few cells of it.

const WHEEL_UP = 64;
const WHEEL_DOWN = 65;
const WHEEL_LEFT = 66;
const WHEEL_RIGHT = 67;

// The game scrolls a panel this many lines or columns per reported step, so a step is
// reported for each this many cells of travel and the content follows the fingers.
const CELLS_PER_STEP = 3;

const PIXELS_PER_LINE_STEP = 16;

// A finger has to move this far before a touch counts as a drag rather than a tap.
const DRAG_THRESHOLD_PIXELS = 8;

// How a flick carries on once the finger lifts: the share of its speed kept each frame,
// and the speed, in pixels per millisecond, below which it stops.
const FLICK_FRICTION = 0.95;
const FLICK_STOPPING_SPEED = 0.02;

const reportOf = (button, column, row) => `\x1b[<${button};${column};${row}M`;

/** Turns pixels of travel into wheel reports aimed at the cell where scrolling began. */
function createScrollReporter(terminal, typeText) {
    const travelled = { across: 0, down: 0 };

    const cellAt = (clientX, clientY) => {
        const screen = terminal.element.querySelector(".xterm-screen").getBoundingClientRect();
        const width = screen.width / terminal.cols;
        const height = screen.height / terminal.rows;
        const column = Math.floor((clientX - screen.left) / width) + 1;
        const row = Math.floor((clientY - screen.top) / height) + 1;

        return {
            width,
            height,
            column: Math.min(Math.max(column, 1), terminal.cols),
            row: Math.min(Math.max(row, 1), terminal.rows),
        };
    };

    const reportTravel = (axis, pixelsPerStep, towardsStart, towardsEnd, cell) => {
        const steps = Math.trunc(travelled[axis] / pixelsPerStep);
        travelled[axis] -= steps * pixelsPerStep;

        const button = steps < 0 ? towardsStart : towardsEnd;
        typeText(reportOf(button, cell.column, cell.row).repeat(Math.abs(steps)));
    };

    const travel = (across, down, cell) => {
        travelled.across += across;
        travelled.down += down;

        reportTravel("across", CELLS_PER_STEP * cell.width, WHEEL_LEFT, WHEEL_RIGHT, cell);
        reportTravel("down", CELLS_PER_STEP * cell.height, WHEEL_UP, WHEEL_DOWN, cell);
    };

    const forget = () => {
        travelled.across = 0;
        travelled.down = 0;
    };

    return { cellAt, travel, forget };
}

function forwardWheel(terminal, reporter) {
    const pixelsOf = (delta, deltaMode, cellSize) => {
        if (deltaMode === WheelEvent.DOM_DELTA_LINE) {
            return delta * PIXELS_PER_LINE_STEP;
        }
        if (deltaMode === WheelEvent.DOM_DELTA_PAGE) {
            return delta * cellSize * terminal.rows;
        }

        return delta;
    };

    terminal.attachCustomWheelEventHandler((event) => {
        event.preventDefault();

        const cell = reporter.cellAt(event.clientX, event.clientY);
        const isShiftedRoll = event.shiftKey && event.deltaX === 0;
        const across = isShiftedRoll ? event.deltaY : event.deltaX;
        const down = isShiftedRoll ? 0 : event.deltaY;

        reporter.travel(
            pixelsOf(across, event.deltaMode, cell.width),
            pixelsOf(down, event.deltaMode, cell.height),
            cell,
        );
        return false;
    });
}

function forwardTouch(element, reporter) {
    let drag = null;
    let flick = null;

    const stopFlick = () => {
        if (flick !== null) {
            cancelAnimationFrame(flick.frame);
            flick = null;
        }
    };

    const carryFlick = (time) => {
        const elapsed = time - flick.time;
        flick.time = time;
        flick.speedX *= FLICK_FRICTION;
        flick.speedY *= FLICK_FRICTION;

        if (Math.hypot(flick.speedX, flick.speedY) < FLICK_STOPPING_SPEED) {
            flick = null;
            return;
        }

        reporter.travel(flick.speedX * elapsed, flick.speedY * elapsed, flick.cell);
        flick.frame = requestAnimationFrame(carryFlick);
    };

    element.addEventListener("touchstart", (event) => {
        stopFlick();
        if (event.touches.length !== 1) {
            drag = null;
            return;
        }

        const touch = event.touches[0];
        reporter.forget();
        drag = {
            cell: reporter.cellAt(touch.clientX, touch.clientY),
            startX: touch.clientX,
            startY: touch.clientY,
            lastX: touch.clientX,
            lastY: touch.clientY,
            lastTime: event.timeStamp,
            speedX: 0,
            speedY: 0,
            isDragging: false,
        };
    }, { capture: true, passive: true });

    element.addEventListener("touchmove", (event) => {
        if (drag === null || event.touches.length !== 1) {
            return;
        }

        const touch = event.touches[0];
        const distance = Math.hypot(touch.clientX - drag.startX, touch.clientY - drag.startY);
        if (!drag.isDragging && distance < DRAG_THRESHOLD_PIXELS) {
            return;
        }

        event.preventDefault();
        event.stopPropagation();
        drag.isDragging = true;

        const across = drag.lastX - touch.clientX;
        const down = drag.lastY - touch.clientY;
        const elapsed = Math.max(event.timeStamp - drag.lastTime, 1);
        drag.speedX = across / elapsed;
        drag.speedY = down / elapsed;
        drag.lastX = touch.clientX;
        drag.lastY = touch.clientY;
        drag.lastTime = event.timeStamp;

        reporter.travel(across, down, drag.cell);
    }, { capture: true, passive: false });

    element.addEventListener("touchend", (event) => {
        if (drag?.isDragging) {
            event.preventDefault();
            flick = { cell: drag.cell, speedX: drag.speedX, speedY: drag.speedY, time: performance.now() };
            flick.frame = requestAnimationFrame(carryFlick);
        }
        drag = null;
    }, { capture: true, passive: false });

    element.addEventListener("touchcancel", () => {
        drag = null;
    }, { capture: true, passive: true });
}

export function forwardScrollingToGame(terminal, typeText) {
    const reporter = createScrollReporter(terminal, typeText);

    forwardWheel(terminal, reporter);
    forwardTouch(terminal.element, reporter);
}
