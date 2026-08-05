#pragma once

namespace Components::Filters {
    static int TitleName(ImGuiInputTextCallbackData* data) { 
        ImWchar c = data->EventChar;
        if((c >= 'a' && c <= 'z') || c >= '_') return 0;
        if((c >= '0' && c <= '9')) return 0;
        if(c >= 'A' && c <= 'Z') { data->EventChar += 'A'-'a'; return 0; }
        return 1;
    }

    static int Numeric(ImGuiInputTextCallbackData* data) { 
        ImWchar c = data->EventChar;
        if((c >= '0' && c <= '9')) return 0;
        return 1;
    }
}