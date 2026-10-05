#include <application/head/common/input/KeyCodes.h>

#include <algorithm>
#include <array>
#include <charconv>
#include <map>
#include <string>
#include <vector>

namespace cpp_warships::head::common::input {
    namespace {
        constexpr std::string_view REPORT_START = "\x1b[";
        constexpr std::string_view ANSWER_START = "\x1b[?";
        constexpr char REPORT_END = 'u';

        /** @brief Modifier bits as the protocol counts them, after taking one off its number. */
        constexpr int SHIFT_BIT = 1;
        constexpr int ALT_BIT = 2;
        constexpr int CONTROL_BIT = 4;
        constexpr int SUPER_BIT = 8;
        constexpr int HYPER_BIT = 16;
        constexpr int META_BIT = 32;

        /** @brief The event type the protocol gives a key being let go. */
        constexpr int RELEASE_EVENT = 3;

        /** @brief Where the protocol keeps the keys that type nothing, such as Shift on its own. */
        constexpr int FIRST_FUNCTIONAL_CODE = 57344;
        constexpr int LAST_FUNCTIONAL_CODE = 63743;

        /** @brief The protocol's code for Enter on the number pad. */
        constexpr int KEYPAD_ENTER_CODE = 57414;

        constexpr int CODE_C = 'c';
        constexpr int CODE_LOWER_A = 'a';
        constexpr int CODE_LOWER_Z = 'z';
        constexpr int CASE_DISTANCE = 'a' - 'A';

        /** @brief The parts of a report: the key, its shifted form and its US-layout key, then
         * the modifiers and the event type. A part the terminal left out stays absent. */
        struct Report {
            int code = 0;
            std::optional<int> shiftedCode{};
            std::optional<int> layoutCode{};
            int modifierBits = 0;
            int eventType = 1;
        };

        [[nodiscard]] std::vector<std::string_view> split(std::string_view text, const char by) {
            std::vector<std::string_view> parts;
            while (true) {
                const std::size_t at = text.find(by);
                parts.push_back(text.substr(0, at));
                if (at == std::string_view::npos) {
                    return parts;
                }
                text.remove_prefix(at + 1);
            }
        }

        [[nodiscard]] std::optional<int> numberOf(const std::string_view text) {
            int number = 0;
            const auto [end, error] =
                std::from_chars(text.data(), text.data() + text.size(), number);
            if (error != std::errc{} || end != text.data() + text.size()) {
                return std::nullopt;
            }

            return number;
        }

        [[nodiscard]] std::optional<Report> reportOf(std::string_view sequence) {
            if (!sequence.starts_with(REPORT_START) || !sequence.ends_with(REPORT_END)) {
                return std::nullopt;
            }

            sequence.remove_prefix(REPORT_START.size());
            sequence.remove_suffix(1);
            const std::vector<std::string_view> fields = split(sequence, ';');
            const std::vector<std::string_view> keys = split(fields[0], ':');

            const std::optional<int> code = numberOf(keys[0]);
            if (!code.has_value()) {
                return std::nullopt;
            }

            Report report{.code = *code};
            if (keys.size() > 1) {
                report.shiftedCode = numberOf(keys[1]);
            }
            if (keys.size() > 2) {
                report.layoutCode = numberOf(keys[2]);
            }

            if (fields.size() > 1) {
                const std::vector<std::string_view> modifiers = split(fields[1], ':');
                report.modifierBits = numberOf(modifiers[0]).value_or(1) - 1;
                if (modifiers.size() > 1) {
                    report.eventType = numberOf(modifiers[1]).value_or(1);
                }
            }

            return report;
        }

        [[nodiscard]] std::string utf8Of(const int codePoint) {
            const auto byte = [](const int value) {
                return static_cast<char>(static_cast<unsigned char>(value));
            };

            if (codePoint < 0x80) {
                return std::string(1, byte(codePoint));
            }
            if (codePoint < 0x800) {
                return {byte(0xC0 | (codePoint >> 6)), byte(0x80 | (codePoint & 0x3F))};
            }
            if (codePoint < 0x10000) {
                return {
                    byte(0xE0 | (codePoint >> 12)),
                    byte(0x80 | ((codePoint >> 6) & 0x3F)),
                    byte(0x80 | (codePoint & 0x3F))
                };
            }

            return {
                byte(0xF0 | (codePoint >> 18)),
                byte(0x80 | ((codePoint >> 12) & 0x3F)),
                byte(0x80 | ((codePoint >> 6) & 0x3F)),
                byte(0x80 | (codePoint & 0x3F))
            };
        }

        /** @brief The character a report types: its US-layout key when the terminal named one,
         * shifted the way that key would be. */
        [[nodiscard]] int typedCodeOf(const Report& report) {
            const bool isShifted = (report.modifierBits & SHIFT_BIT) != 0;
            const int key = report.layoutCode.value_or(report.code);

            if (isShifted && key >= CODE_LOWER_A && key <= CODE_LOWER_Z) {
                return key - CASE_DISTANCE;
            }
            if (isShifted && !report.layoutCode.has_value() && report.shiftedCode.has_value()) {
                return *report.shiftedCode;
            }

            return key;
        }

        [[nodiscard]] bool isCommand(const Report& report) {
            constexpr int COMMAND_BITS = CONTROL_BIT | ALT_BIT | SUPER_BIT | HYPER_BIT | META_BIT;
            return (report.modifierBits & COMMAND_BITS) != 0;
        }

        /** @brief A key let go, which asks for nothing. */
        [[nodiscard]] std::optional<Keystroke> readRelease(const Report& report) {
            if (report.eventType != RELEASE_EVENT) {
                return std::nullopt;
            }

            return Keystroke{};
        }

        /** @brief Ctrl+C, on whichever key the C sits in the layout in use. */
        [[nodiscard]] std::optional<Keystroke> readInterrupt(const Report& report) {
            const bool isControlHeld = (report.modifierBits & CONTROL_BIT) != 0;
            if (!isControlHeld || report.layoutCode.value_or(report.code) != CODE_C) {
                return std::nullopt;
            }

            return Keystroke{.key = Key::Interrupt};
        }

        /** @brief A key that types nothing but has a name of its own, such as Enter. */
        [[nodiscard]] std::optional<Keystroke> readNamedKey(const Report& report) {
            static const std::map<int, Key> NAMED_KEYS = {
                {'\r', Key::Enter},
                {'\x1b', Key::Escape},
                {'\t', Key::Tab},
                {'\x7f', Key::Backspace},
                {'\b', Key::Backspace},
                {KEYPAD_ENTER_CODE, Key::Enter},
            };

            const auto named = NAMED_KEYS.find(report.code);
            if (named == NAMED_KEYS.end()) {
                return std::nullopt;
            }

            return isCommand(report) ? Keystroke{} : Keystroke{.key = named->second};
        }

        /** @brief A key the game has no use for: held with a command modifier, or one such as
         * Shift that types nothing on its own. */
        [[nodiscard]] std::optional<Keystroke> readUnusedKey(const Report& report) {
            const bool isFunctional =
                report.code >= FIRST_FUNCTIONAL_CODE && report.code <= LAST_FUNCTIONAL_CODE;
            if (!isCommand(report) && !isFunctional) {
                return std::nullopt;
            }

            return Keystroke{};
        }

        /** @brief A key that types a character, read as its US-layout letter. */
        [[nodiscard]] std::optional<Keystroke> readTypedKey(const Report& report) {
            return Keystroke{.key = Key::Character, .character = utf8Of(typedCodeOf(report))};
        }
    }  // namespace

    bool isKeyCodesAnswer(std::string_view sequence) {
        const auto isDigit = [](const char character) {
            return character >= '0' && character <= '9';
        };

        if (!sequence.starts_with(ANSWER_START) || !sequence.ends_with(REPORT_END)) {
            return false;
        }

        sequence.remove_prefix(ANSWER_START.size());
        sequence.remove_suffix(1);
        return !sequence.empty() && std::all_of(sequence.begin(), sequence.end(), isDigit);
    }

    std::optional<Keystroke> keystrokeOfKeyCode(const std::string_view sequence) {
        using ReportReader = std::optional<Keystroke> (*)(const Report&);
        static constexpr std::array<ReportReader, 5> READERS_IN_ORDER{
            readRelease,
            readInterrupt,
            readNamedKey,
            readUnusedKey,
            readTypedKey,
        };

        const std::optional<Report> report = reportOf(sequence);
        if (!report.has_value()) {
            return std::nullopt;
        }

        for (const ReportReader reader : READERS_IN_ORDER) {
            if (const std::optional<Keystroke> stroke = reader(*report)) {
                return stroke;
            }
        }

        return Keystroke{};
    }
}  // namespace cpp_warships::head::common::input
