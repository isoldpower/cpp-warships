# cpp-warships — architecture

Two layers, each its own CMake target. The model never draws and never reads input;
the head never decides anything. What crosses between them is `ApplicationContext`,
handed to the head as `const`.

| Directory | Target | Layer |
|---|---|---|
| `cpp_warships/application/core` | `warships_core` | rules, error root |
| `cpp_warships/application/flow` | `warships_flow` | match orchestration |
| `cpp_warships/application/persistence` | `warships_persistence` | saves |
| `cpp_warships/application/model` | `warships_model` | events, intents, scenarios, game state |
| `cpp_warships/application/head/{common,plain}` | `warships_head` | presentation, no drawing library |
| `cpp_warships/application/head/ftxui` | `warships_head_ftxui` | FTXUI renderers |

`warships_head` links without FTXUI, and the build proves it rather than a checklist:
`plain/` sits in that target, so leaking an FTXUI include into `common/` breaks the
build. Measured: 27 translation units in `warships_head` see no FTXUI include path,
10 in `warships_head_ftxui` see one.

## Structure

```mermaid
classDiagram
    direction TB

    namespace PresentationLayer {
        class PresentationShell {
            <<abstract>>
            Owns the loop and the terminal
            +run(PresentationContext) void
            +requestQuit() void
        }
        class EventBus {
            Maps raw input to game events
            Owns the cursor and the grid geometry
            +interpret(Keystroke) optional~GameEvent~
        }
        class GridGeometry {
            Where each board landed on screen
            +cellAt(int, int) optional~Coordinate~
        }
        class PresentationContext {
            The one place presentation state lives
            Owns the theme, the state and the geometry
            +application() const ApplicationContext&
            +state() PresentationState&
            +geometry() GridGeometry&
            +theme() const Theme&
            +currentScreen() ScreenKind
        }
        class EventPipeline {
            One turn of the machine
            +offer(Keystroke) void
            +settle() bool
        }
        class RendererSet {
            A renderer for every screen there is
            +render(ScreenKind, int, int) Frame
        }
        class Renderer {
            <<abstract>>
            Draws one screen from the context
            +render(int, int) Frame
        }
    }

    namespace ModelLayer {
        class EventQueue {
            Game events in the order they arrived
            +push(GameEvent) void
            +drain() vector~GameEvent~
        }
        class EventRouter {
            Subscribers and scopes
            Distributes events to handlers
            +subscribe(Scope, EventHandler) Subscription
            +dispatch(GameEvent) void
        }
        class EventHandler {
            <<abstract>>
            +isHandled(GameEvent) bool
            +scenarioFor(GameEvent) IntendedGameScenario
        }
        class IntendedGameScenario {
            An ordered branch of simple actions
            Iterator: the next intent may depend
            on what the previous one returned
            +next(IntentResult) optional~GameIntent~
        }
        class ScenarioQueue {
            <<abstract>>
            +submit(IntendedGameScenario) void
            +start() void
            +join() void
            +isIdle() bool
        }
        class SyncScenarioQueue {
            start drains inline, join returns at once
        }
        class IntentProcessor {
            Runs each scenario to its end
            Catches every layer error below it
            Nothing escapes upwards
            +run(IntendedGameScenario) void
        }
        class IntentFactory {
            The intent-to-context boundary
            Hands each intent only what it needs
            +fireAt(Coordinate) GameIntent
            +saveMatch() GameIntent
        }
        class GameIntent {
            <<abstract>>
            +applyTo() IntentResult
        }
        class IntentResult {
            Succeeded, or why not
        }
        class ApplicationContext {
            What an intent may reach
            +game() WarshipsGame&
            +saves() SaveArchive&
            +isFinished() bool
        }
        class WarshipsGame {
            The match in play, its log and its rules
            +match() const Match&
            +journal() const BattleJournal&
        }
        class MatchBehavior {
            start, place, shuffle, fire, use skill
        }
        class SaveBehavior {
            save, load, hasSavedMatch
        }
        class BattleJournal {
            What has happened, in words
        }
    }

    PresentationShell --> RendererSet : draws through
    PresentationShell --> EventPipeline : feeds
    PresentationShell --> PresentationContext : renders
    RendererSet o-- Renderer
    EventPipeline --> EventBus
    EventPipeline --> EventQueue
    EventPipeline --> EventRouter
    EventPipeline --> ScenarioQueue
    EventBus *-- GridGeometry
    EventBus --> PresentationContext : moves the cursor
    Renderer --> PresentationContext : reads (const)

    EventBus --> EventQueue : GameEvent
    EventQueue --> EventRouter : on tick
    EventRouter --> EventHandler : dispatches, in scope
    EventHandler --> IntendedGameScenario : builds
    EventHandler --> IntentFactory : asks for intents
    IntendedGameScenario o-- GameIntent
    EventHandler --> ScenarioQueue : submits
    ScenarioQueue <|-- SyncScenarioQueue
    ScenarioQueue --> IntentProcessor : resolves through
    IntentProcessor --> IntendedGameScenario : iterates
    IntentProcessor --> IntentResult : collects
    IntentFactory o-- ApplicationContext
    GameIntent --> ApplicationContext : acts on
    ApplicationContext o-- WarshipsGame
    WarshipsGame *-- MatchBehavior
    WarshipsGame *-- SaveBehavior
    WarshipsGame *-- BattleJournal

    PresentationContext o-- ApplicationContext : const ref
```

`PresentationContext` holding a `const ApplicationContext&` is the whole seam. The head
can read everything and write nothing; the compiler enforces it rather than discipline.

## One tick

```mermaid
sequenceDiagram
    autonumber
    participant Player
    participant Shell as PresentationShell
    participant Bus as EventBus
    participant EQ as EventQueue
    participant Router as EventRouter
    participant Handler as EventHandler
    participant SQ as ScenarioQueue
    participant Proc as IntentProcessor
    participant Intent as GameIntent
    participant Game as WarshipsGame

    Player->>Shell: keystroke or click
    Shell->>Bus: interpret(Keystroke)
    Note over Bus: arrow keys move the cursor<br/>and emit nothing at all
    Bus->>EQ: push(CellSelectedEvent)

    Shell->>EQ: drain()
    EQ->>Router: dispatch(event)
    Router->>Handler: handler claiming it, in scope
    Handler->>SQ: submit(scenario)

    Shell->>SQ: start()
    SQ->>Proc: run(scenario)
    loop until the scenario is spent
        Proc->>Intent: applyTo()
        Intent->>Game: mutate
        Intent-->>Proc: IntentResult
        Note over Proc: an error below becomes a failed<br/>result and a notification on the context<br/>it never leaves this loop
    end
    Shell->>SQ: join()

    Shell->>Shell: render PresentationContext
```

Navigation is not an intent. The head picks its renderer from `MatchPhase`, so a phase
change *is* the navigation. Quitting is a scenario that ends by setting
`ApplicationContext::isFinished`, which the shell loop observes — that keeps quit
sequenced behind a save without making it a navigation step.

## Why a scenario, in one case

Saving is offered from the menu and nowhere else, and never on its own: it always ends
the session with it. Inside a match there is no key for it at all, so putting a match
away is always deliberate — step out to the menu first. That pairing is a scenario of
two steps, and `SequenceScenario` stops at the first that does not come off:

```
saving and leaving  =  [ saveMatch, finishSession ]
```

A save that fails therefore leaves the session running, with the match still there and
a notice saying why. Nothing had to be written to arrange that: the ordering and the
stop-on-failure are what a scenario already is.

Telling the two failures apart mattered enough to change a signature. `SaveBehavior::
saveMatch` once returned `false` both for "no match" and for "the write failed", so a
full disk reported "there is no match to save". It now returns `false` only for the
first and throws `SaveWriteException` for the second, which the processor turns into
an accurate notice.

## Errors

Rooted in `application/core`, one subtree per layer, each caught at that layer's
entry point so nothing below leaks above it.

```mermaid
classDiagram
    direction LR

    class WarshipsException {
        <<abstract>>
        +layer() ErrorLayer
        +what() const char*
    }

    class CoreException
    class FlowException
    class PersistenceException
    class ModelException
    class PresentationException

    WarshipsException <|-- CoreException
    WarshipsException <|-- FlowException
    WarshipsException <|-- PersistenceException
    WarshipsException <|-- ModelException
    WarshipsException <|-- PresentationException

    CoreException <|-- CoordinateOutOfBoardException
    CoreException <|-- ShipOverlapException
    CoreException <|-- ShipOutOfBoundsException
    CoreException <|-- CellAlreadyAttackedException

    FlowException <|-- WrongPhaseException
    FlowException <|-- NotYourTurnException
    FlowException <|-- FleetIncompleteException
    FlowException <|-- SkillUnavailableException

    PersistenceException <|-- SaveNotFoundException
    PersistenceException <|-- SaveWriteException
    PersistenceException <|-- SaveCorruptException

    ModelException <|-- IntentFailedException
    ModelException <|-- ScenarioAbortedException

    PresentationException <|-- ShellException
    PresentationException <|-- EventMappingException
```

| Entry point | Catches | Emits |
|---|---|---|
| `Match` methods | `CoreException` | wraps as `FlowException` |
| `SaveArchive` | `serialization::*` | wraps as `PersistenceException` |
| **`IntentProcessor`** | core, flow, persistence | **an `IntentResult` — never rethrows** |
| shell loop | `PresentationException` | its own message |

Leaves carry typed fields, not just text — `CoordinateOutOfBoardException` holds the
offending `Coordinate` and the board size. Wrapping keeps the cause as a string rather
than `std::nested_exception`. The existing `serialization::exceptions::*` stay where
they are and keep their own shape: that library is generic and should not know the
word "warships".

Because `IntentProcessor` never lets an exception past it, the head learns of failure
by reading a notification off the context, not by catching anything.

## What lives where, and why

| Thing | Layer | Reason |
|---|---|---|
| cursor, aim, selected ship length, board size, theme, log scroll | head | input affordances, not game concepts — mouse play has no cursor at all |
| panel focus, panel scroll offsets, narrow-or-wide layout | head | how the screen is arranged; the game never knows a panel exists. Keys write offsets, renderers clamp them to what fits and record where each panel landed in `GridGeometry` |
| grid geometry and hit-testing | head | whoever drew the grid is the only one who knows where it landed |
| which screen shows | head | derived from `MatchPhase`; nothing to decide |
| ordering of actions within one event | model | `IntendedGameScenario`, locally and visibly, not a global priority table |
| what an intent may touch | model | `IntentFactory` injects at construction; intents never see the whole context |

## Settled along the way

- **Menu versus in-match** is not a `MatchPhase`, and does not need to be.
  `PresentationState::isAtMenu` carries it, on the presentation side, because a match
  can be in play while the player is looking at the menu. Everything else about which
  screen shows is read off the match.
- **The journal is saved.** `MatchSnapshot` carries it as `flow::MatchEventLog` rather
  than as a `BattleJournal`, since persistence sits below the layer that keeps journals.
  A save written before this loads with an empty log rather than failing.
- **Segment health reaches every fleet.** `placeFleetRandomly` takes it as an argument
  instead of defaulting, so shuffled and computer fleets are as tough as hand-laid ones.
- **`ScreenKind::GameOver` is gone.** A finished match is the battle screen, saying so.

## Still open

- The renderers are still named `MenuView`, `PlainBattleView` and so on, while the
  interface they implement is `Renderer`. Worth one rename.
- `Queries.h` is thinner than it was: `MatchQuery` and `BoardQuery` have no callers left
  now that everything reads the context.
- Nothing shows a notice except a failed save, because every other failure is guarded
  before it can happen. That is the right trade, but it leaves the strip nearly unused.
