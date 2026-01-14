#pragma once

enum class SelectionCallbackResult : uint8_t {
    CONTINUE        = 0,
    INTERRUPT       = 1 << 0,
    UPDATE_MAP      = 1 << 1,
    DELETE_CALLBACK = 1 << 2,
};

inline SelectionCallbackResult operator|(SelectionCallbackResult a, SelectionCallbackResult b) {
    using T = std::underlying_type_t<SelectionCallbackResult>;
    return static_cast<SelectionCallbackResult>(static_cast<T>(a) | static_cast<T>(b));
}


inline SelectionCallbackResult operator&(SelectionCallbackResult a, SelectionCallbackResult b) {
    using T = std::underlying_type_t<SelectionCallbackResult>;
    return static_cast<SelectionCallbackResult>(static_cast<T>(a) & static_cast<T>(b));
}

inline SelectionCallbackResult operator~(SelectionCallbackResult a) {
    using T = std::underlying_type_t<SelectionCallbackResult>;
    return static_cast<SelectionCallbackResult>(static_cast<T>(~a));
}

inline SelectionCallbackResult& operator|=(SelectionCallbackResult& a, SelectionCallbackResult b) {
    return a = a | b;
}

inline SelectionCallbackResult& operator&=(SelectionCallbackResult& a, SelectionCallbackResult b) {
    return a = a & b;
}

inline bool SelectionCallbackHasFlag(SelectionCallbackResult value, SelectionCallbackResult flag) {
    return (value & flag) != SelectionCallbackResult::CONTINUE;
}