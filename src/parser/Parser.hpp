#pragma once

#include "parser/Lexer.hpp"

namespace Parser {
    using PValue = SharedPtr<Value>;
    using Key = std::variant<double, std::string>;

    class Value {
        public:
            virtual bool IsLeaf() const = 0;
            virtual int Count() const = 0;

            virtual int GetInt() const = 0;
            virtual double GetDouble() const = 0;
            virtual bool GetBool() const = 0;
            virtual std::string GetString() const = 0;
            
            virtual PValue Get(const Key& key) = 0;
            virtual PValue Get(const Key& key) const = 0;

            virtual void Set(const Key& key, int value) = 0;
            virtual void Set(const Key& key, double value) = 0;
            virtual void Set(const Key& key, bool value) = 0;
            virtual void Set(const Key& key, std::string value) = 0;
            virtual void Set(const Key& key, const PValue& value) = 0;

            virtual PValue operator=(int v) = 0;
            virtual PValue operator=(double v) = 0;
            virtual PValue operator=(bool v) = 0;
            virtual PValue operator=(std::string v) = 0;
            virtual PValue operator=(PValue v) = 0;

            virtual PValue operator[](const Key& key) = 0;
            virtual PValue operator[](const Key& key) const = 0;

            virtual PValue Copy() const = 0;
    };

    class Node : public Value {
        public:
            Node();
            Node(const Node& n);
            Node(const std::map<Key, PValue>& values);

            virtual bool IsLeaf() const;
            virtual int Count() const;

            virtual int GetInt() const;
            virtual double GetDouble() const;
            virtual bool GetBool() const;
            virtual std::string GetString() const;
            
            virtual PValue Get(const Key& key);
            virtual PValue Get(const Key& key) const;

            virtual void Set(const Key& key, int value);
            virtual void Set(const Key& key, double value);
            virtual void Set(const Key& key, bool value);
            virtual void Set(const Key& key, std::string value);
            virtual void Set(const Key& key, const PValue& value);

            virtual PValue operator=(int v);
            virtual PValue operator=(double v);
            virtual PValue operator=(bool v);
            virtual PValue operator=(std::string v);
            virtual PValue operator=(PValue v);

            virtual PValue operator[](const Key& key);
            virtual PValue operator[](const Key& key) const;

            virtual PValue Copy() const;

        private:
            std::map<Key, PValue> m_Values;        
    };

    class Leaf : public Value {
        public:
            Leaf();
            Leaf(const Leaf& l);
            Leaf(int v);
            Leaf(double v);
            Leaf(bool v);
            Leaf(std::string v);

            virtual bool IsLeaf() const;
            virtual int Count() const;

            virtual int GetInt() const;
            virtual double GetDouble() const;
            virtual bool GetBool() const;
            virtual std::string GetString() const;
            
            virtual PValue Get(const Key& key);
            virtual PValue Get(const Key& key) const;

            virtual void Set(const Key& key, int value);
            virtual void Set(const Key& key, double value);
            virtual void Set(const Key& key, bool value);
            virtual void Set(const Key& key, std::string value);
            virtual void Set(const Key& key, const PValue& value);

            virtual PValue operator=(int v);
            virtual PValue operator=(double v);
            virtual PValue operator=(bool v);
            virtual PValue operator=(std::string v);
            virtual PValue operator=(PValue v);

            virtual PValue operator[](const Key& key);
            virtual PValue operator[](const Key& key) const;

            virtual PValue Copy() const;

        private:
            std::variant<double, bool, std::string> m_Value;
    };

    PValue Parse(const std::string& content);
    PValue Parse(std::queue<PToken>& tokens);

    void Benchmark();
}