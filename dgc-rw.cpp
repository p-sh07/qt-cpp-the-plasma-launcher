//{
//  dgc-rw.cpp
//  gzdml
//
//  Created by ps on 02.05.2024.
//
#include "dgc-rw.h"

#include <cmath>
#include <iterator>

namespace dgc {

using namespace std::literals;

ModSet::ModSet(std::string label, std::string iwad, std::vector<std::string> mod_filenames)
    : label(std::move(label))
    , iwad(std::move(iwad))
    , selected_mods(mod_filenames) {
}

//open default file if empty path
DgcParser::DgcParser(const fs::path settings_file)
    : file_(std::fstream(settings_file, std::ios_base::in)) {

    std::string line;
    std::getline(file_,line);
    if(line.empty()) {
#ifdef WIN_BUILD
        Write(_default_win);
#elif defined(MAC_BUILD)
        Write(_default_mac);
#endif
    }
}

bool DgcParser::open(const fs::path& settings_file) {
    file_.open(settings_file);
    return file_.is_open();
}
LaunchSettings DgcParser::ParseModSettings(const fs::path& gdc_path) {
    LaunchSettings s;



    return s;
}


std::vector<ModSet> DgcParser::ParsePresetsFromFile(const fs::path& gdc_path) {
        std::vector<ModSet> games;

    return games;
}

void DgcParser::MakeDefaultsWin() {

}

void DgcParser::MakeDefaultsMac() {

}

_node Load_node(std::istream& input);
_node LoadString(std::istream& input);

std::string LoadLiteral(std::istream& input) {
    std::string s;
    while (std::isalpha(input.peek())) {
        s.push_back(static_cast<char>(input.get()));
    }
    return s;
}

_node LoadLabel(std::istream& input) {
    std::vector<_node> result;

    for (char c; input >> c && c != ']';) {
        if (c != ',') {
            input.putback(c);
        }
        result.push_back(Load_node(input));
    }
    if (!input) {
        throw ParsingError("Array parsing error"s);
    }
    return _node(std::move(result));
}

_node LoadMap(std::istream& input) {
    Map map;
    for (char c; input >> c && c != '}';) {
        if (c == '"') {
            std::string key = LoadString(input).AsString();
            if (input >> c && c == ':') {
                if (map.find(key) != map.end()) {
                    throw ParsingError("Duplicate key '"s + key + "' have been found");
                }
                map.emplace(std::move(key), Load_node(input));
            } else {
                throw ParsingError(": is expected but '"s + c + "' has been found"s);
            }
        } else if (c != ',') {
            throw ParsingError(R"(',' is expected but ')"s + c + "' has been found"s);
        }
    }
    if (!input) {
        throw ParsingError("Mapionary parsing error"s);
    }
    return _node(std::move(map));
}

_node LoadString(std::istream& input) {
    auto it = std::istreambuf_iterator<char>(input);
    auto end = std::istreambuf_iterator<char>();
    std::string s;
    while (true) {
        if (it == end) {
            throw ParsingError("String parsing error");
        }
        const char ch = *it;
        if (ch == '"') {
            ++it;
            break;
        } else if (ch == '\\') {
            ++it;
            if (it == end) {
                throw ParsingError("String parsing error");
            }
            const char escaped_char = *(it);
            switch (escaped_char) {
            case 'n':
                s.push_back('\n');
                break;
            case 't':
                s.push_back('\t');
                break;
            case 'r':
                s.push_back('\r');
                break;
            case '"':
                s.push_back('"');
                break;
            case '\\':
                s.push_back('\\');
                break;
            default:
                throw ParsingError("Unrecognized escape sequence \\"s + escaped_char);
            }
        } else if (ch == '\n' || ch == '\r') {
            throw ParsingError("Unexpected end of line"s);
        } else {
            s.push_back(ch);
        }
        ++it;
    }

    return _node(std::move(s));
}

_node LoadBool(std::istream& input) {
    const auto s = LoadLiteral(input);
    if (s == "true"sv) {
        return _node{true};
    } else if (s == "false"sv) {
        return _node{false};
    } else {
        throw ParsingError("Failed to parse '"s + s + "' as bool"s);
    }
}

_node Load_node(std::istream& input) {
    char c;
    if (!(input >> c)) {
        throw ParsingError("Unexpected EOF"s);
    }
    switch (c) {
    case '[':
        return LoadLabel(input);
    case '{':
        return LoadMap(input);
    case '"':
        return LoadString(input);
    case 't':
        [[fallthrough]];
    case 'f':
        input.putback(c);
        return LoadBool(input);
    default:
        return std::monostate();
    }
}

struct PrintContext {
    std::ostream& out;
    int indent_step = 4;
    int indent = 0;

    void PrintIndent() const {
        for (int i = 0; i < indent; ++i) {
            out.put(' ');
        }
    }

    PrintContext Indented() const {
        return {out, indent_step, indent_step + indent};
    }
};

void Print_node(const _node& value, const PrintContext& ctx);

template <typename Value>
void PrintValue(const Value& value, const PrintContext& ctx) {
    ctx.out << value;
}

void PrintString(const std::string& value, std::ostream& out) {
    out.put('"');
    for (const char c : value) {
        switch (c) {
        case '\r':
            out << "\\r"sv;
            break;
        case '\n':
            out << "\\n"sv;
            break;
        case '\t':
            out << "\\t"sv;
            break;
        case '"':
            // Символы " и \ выводятся как \" или \\, соответственно
            [[fallthrough]];
        case '\\':
            out.put('\\');
            [[fallthrough]];
        default:
            out.put(c);
            break;
        }
    }
    out.put('"');
}

template <>
void PrintValue<std::string>(const std::string& value, const PrintContext& ctx) {
    PrintString(value, ctx.out);
}

template <>
void PrintValue<bool>(const bool& value, const PrintContext& ctx) {
    ctx.out << (value ? "true"sv : "false"sv);
}

template <>
void PrintValue<Array>(const Array& nodes, const PrintContext& ctx) {
    std::ostream& out = ctx.out;
    out << "[\n"sv;
    bool first = true;
    auto inner_ctx = ctx.Indented();
    for (const _node& node : nodes) {
        if (first) {
            first = false;
        } else {
            out << ",\n"sv;
        }
        inner_ctx.PrintIndent();
        Print_node(node, inner_ctx);
    }
    out.put('\n');
    ctx.PrintIndent();
    out.put(']');
}

template <>
void PrintValue<Map>(const Map& nodes, const PrintContext& ctx) {
    std::ostream& out = ctx.out;
    out << "{\n"sv;
    bool first = true;
    auto inner_ctx = ctx.Indented();
    for (const auto& [key, node] : nodes) {
        if (first) {
            first = false;
        } else {
            out << ",\n"sv;
        }
        inner_ctx.PrintIndent();
        PrintString(key, ctx.out);
        out << ": "sv;
        Print_node(node, inner_ctx);
    }
    out.put('\n');
    ctx.PrintIndent();
    out.put('}');
}

void Print_node(const _node& node, const PrintContext& ctx) {
    std::visit(
        [&ctx](const auto& value) {
            PrintValue(value, ctx);
        },
        node.GetValue());
}

DgcDoc Load(std::istream& input) {
    return DgcDoc{Load_node(input)};
}

void Print(const DgcDoc& doc, std::ostream& output) {
    Print_node(doc.GetRoot(), PrintContext{output});
}

///Конструктор по умолчанию, добавляет пустой Node root в tree
Builder::Builder()
    : root_()
    , tree_({&root_}) {
}
Builder::KeyReturnItem Builder::Key(std::string key) {
    if(!KeyExpected()) {
        throw std::logic_error("A Map Key is not expected");
    }
    //add key & store ptr to value at tree top
    auto [it, _ ] = tree_.top()->GetMap().insert({std::move(key), Node()});
    tree_.push(&it->second);

    return RetItm(*this);
}

Builder::RetItm Builder::Value(Node node) {
    if(!ValueExpected()) {
        throw std::logic_error("A Value is not expected");
    }
    if(!PushIfArray(node)) {
        *tree_.top() = std::move(node);
        tree_.pop();
    }
    return RetItm(*this);
}

Builder::MapReturnItem Builder::StartMap() {
    if(!ValueExpected()) {
        throw std::logic_error("Cannot make a Map");
    }
    if(!PushIfArray(Map{})) {
        tree_.top()->GetValue() = Map{};
    }
    return RetItm(*this);
}

Builder::ArrayReturnItem Builder::StartArray() {
    if(!ValueExpected()) {
        throw std::logic_error("Cannot Start an Array");
    }
    if(!PushIfArray(Array{})) {
        tree_.top()->GetValue() = Array{};
    }

    return RetItm(*this);
}

Builder::RetItm Builder::EndMap() {
    if(!KeyExpected() ) {
        throw std::logic_error("Cannot Finish a Map");
    }
    tree_.pop();
    return RetItm(*this);
}

Builder::RetItm Builder::EndArray() {
    if(!tree_.empty() && !tree_.top()->IsArray()) {
        throw std::logic_error("Cannot Finish an Array");
    }
    tree_.pop();
    return RetItm(*this);
}

Node Builder::Build() {
    if(!tree_.empty()) {
        throw std::logic_error("Unfinished Nodes left when calling Build");
    }
    return root_;
}

///Текущий Node - массив
bool Builder::PushIfArray(Node node) {
    if(tree_.top()->IsArray()) {
        bool push_to_stack = node.IsArray() || node.IsMap();
        tree_.top()->GetArray().push_back(std::move(node));

        if(push_to_stack) {
            tree_.push(&tree_.top()->GetArray().back());
        }
        return true;
    }
    return false;
}

///Ожидается значение либо словарь/массив (новый Node)
bool Builder::ValueExpected() {
    return !tree_.empty() && (tree_.top()->IsNull() || tree_.top()->IsArray());
}

///Ожидается ключ словаря
bool Builder::KeyExpected() {
    return !tree_.empty() && tree_.top()->IsMap();
}


}//namespace dgc
