#pragma once

enum class SelectionCallbackResult {
    CONTINUE        = 0,
    INTERRUPT       = 1 << 0,
    UPDATE_MAP      = 1 << 1,
    DELETE_CALLBACK = 1 << 2,
};

inline SelectionCallbackResult operator|(SelectionCallbackResult a, SelectionCallbackResult b) {
    return static_cast<SelectionCallbackResult>(static_cast<int>(a) | static_cast<int>(b));
}


inline SelectionCallbackResult operator&(SelectionCallbackResult a, SelectionCallbackResult b) {
    return static_cast<SelectionCallbackResult>(static_cast<int>(a) & static_cast<int>(b));
}

inline SelectionCallbackResult operator~(SelectionCallbackResult a) {
    return static_cast<SelectionCallbackResult>(static_cast<int>(~a));
}

inline SelectionCallbackResult& operator|=(SelectionCallbackResult& a, SelectionCallbackResult b) {
    return a = a | b;
}

inline SelectionCallbackResult& operator&=(SelectionCallbackResult& a, SelectionCallbackResult b) {
    return a = a & b;
}