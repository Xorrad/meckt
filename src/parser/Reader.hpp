#pragma once

class Reader {
public:
    Reader(std::string value) {
        m_Content = value;
        m_CursorStart = 0;
        m_Cursor = 0;
        m_CursorLine = 1;
    }

    bool IsEmpty() const {
        return m_Cursor >= m_Content.size();
    }

    void Start() {
        m_CursorStart = m_Cursor;
    }

    std::string End() const {
        return m_Content.substr(m_CursorStart, m_Cursor - m_CursorStart);
    }

    std::string At(int pos) const {
        int length = String::UTF8CharLength(m_Content[pos]);
        return m_Content.substr(pos, length);
    }

    void CheckNewLine() {
        if(m_Content.at(m_Cursor) == '\n')
            m_CursorLine++;
    }

    char Advance() {
        this->CheckNewLine();
        return m_Content.at(m_Cursor++);
    }

    bool Match(char ch) {
        if(this->IsEmpty())
            return false;
        if(m_Content.at(m_Cursor) != ch)
            return false;
        this->CheckNewLine();
        m_Cursor++;
        return true;
    }

    char Peek() {
        if(this->IsEmpty())
            return '\0';
        return m_Content.at(m_Cursor);
    }
    
    void SkipTo(char ch) {
        while(!this->IsEmpty() && this->Peek() != ch) {
            this->CheckNewLine();
            m_Cursor++;
        }
    }

    int GetCursor() const {
        return m_Cursor;
    }

    int GetLine() const {
        return m_CursorLine;
    }

    std::size_t Length() const {
        return m_Content.size() - m_Cursor;
    }

    std::string GetContent() const {
        return m_Content;
    }

private:
    std::string m_Content;
    int m_CursorStart;
    int m_Cursor;
    int m_CursorLine;
};