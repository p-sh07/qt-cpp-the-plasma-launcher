//
//  Doom-game-config reader/writer
//  gzdml
//
//  Created by ps on 02.05.2024.
//
#pragma once

#include <cmath>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <map>
#include <string>
#include <stack>
#include <unordered_set>
#include <variant>
#include <vector>





using std::ofstream;
using std::ifstream;

namespace fs = std::filesystem;

namespace dgc {
//================================================================//
//================ GzdoomLauncher - Interface/Abstract Class =====//
struct LaunchSettings {
    fs::path gzdoom_folder;
    fs::path gzdoom_ini_path;
    fs::path mod_config_path;

    fs::path iwad_folder;
    fs::path mod_folder;

    size_t chosen_iwad = 0;
    size_t chosen_mod_set = 0;
    std::vector<size_t> chosen_mods;

    std::vector<std::string> iwad_filenames;
    std::vector<std::string> mod_filenames;

    fs::path gzdoom_exe;
    fs::path gzdoom_app;
};

//================================================================//
//================ GzdoomLauncher - Interface/Abstract Class =====//
struct ModSet
{
    ModSet() = default;
    ModSet(std::string label, std::string iwad = "DOOM.WAD", std::vector<std::string> mod_filenames = {});

    //Required:
    std::string label = "Default";
    std::string iwad = "DOOM.WAD";
    std::vector<std::string> selected_mods;

    //Optional, if needed to specify:
    fs::path gzdoom_dir;
    fs::path config;
    fs::path iwad_dir;
    fs::path mod_dir;
};

//================================================================//
//================ GzdoomLauncher - Interface/Abstract Class =====//
class _node;
using Map = std::map<std::string, _node>;
using Array = std::vector<_node>;


//================================================================//
//================ GzdoomLauncher - Interface/Abstract Class =====//
class ParsingError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};


//================================================================//
//================ GzdoomLauncher - Interface/Abstract Class =====//
class _node  final
    : private std::variant<std::monostate, bool, std::string, Array, Map> {
public:
    using variant::variant;
    using Value = variant;

    bool IsBool() const {
        return std::holds_alternative<bool>(*this);
    }
    bool AsBool() const {
        using namespace std::literals;
        if (!IsBool()) {
            throw std::logic_error("Not a bool"s);
        }

        return std::get<bool>(*this);
    }

    bool IsNull() const {
        return std::holds_alternative<std::monostate>(*this);
    }

    bool IsArray() const {
        return std::holds_alternative<Array>(*this);
    }
    const Array& AsArray() const {
        using namespace std::literals;
        if (!IsArray()) {
            throw std::logic_error("Not an array"s);
        }

        return std::get<Array>(*this);
    }

    Array& GetArray() {
        using namespace std::literals;
        if (!IsArray()) {
            throw std::logic_error("Not an array"s);
        }
        return std::get<Array>(*this);
    }

    bool IsString() const {
        return std::holds_alternative<std::string>(*this);
    }
    const std::string& AsString() const {
        using namespace std::literals;
        if (!IsString()) {
            throw std::logic_error("Not a string"s);
        }

        return std::get<std::string>(*this);
    }

    bool IsMap() const {
        return std::holds_alternative<Map>(*this);
    }

    const Map& AsMap() const {
        using namespace std::literals;
        if (!IsMap()) {
            throw std::logic_error("Not a Map"s);
        }

        return std::get<Map>(*this);
    }

    Map& GetMap() {
        using namespace std::literals;
        if (!IsMap()) {
            throw std::logic_error("Not a Map"s);
        }
        return std::get<Map>(*this);
    }

    bool operator==(const _node& rhs) const {
        return GetValue() == rhs.GetValue();
    }

    const Value& GetValue() const {
        return *this;
    }

    Value& GetValue() {
        return *this;
    }
};
inline bool operator!=(const _node& lhs, const _node& rhs) {
    return !(lhs == rhs);
}


//================================================================//
//================ GzdoomLauncher - Interface/Abstract Class =====//
class DgcDoc {
public:
    explicit DgcDoc(_node root)
        : root_(std::move(root)) {
    }

    const _node& GetRoot() const {
        return root_;
    }

private:
    _node root_;
};

inline bool operator==(const DgcDoc& lhs, const DgcDoc& rhs) {
    return lhs.GetRoot() == rhs.GetRoot();
}
inline bool operator!=(const DgcDoc& lhs, const DgcDoc& rhs) {
    return !(lhs == rhs);
}
DgcDoc Load(std::istream& input);
void Print(const DgcDoc& doc, std::ostream& output);


//================================================================//
//================ GzdoomLauncher - Interface/Abstract Class =====//
class DgcParser {
public:
    //open default file if empty path
    DgcParser(const fs::path settings_file = {});

    void InitPathsForMac();
    void WinParseAndInit(const fs::path& gdc_path);

    void Write(const ModSet& mset);
    void Write(const LaunchSettings& lcfg);

    void MakeDefaultsWin();
    void MakeDefaultsMac();

    bool open(const fs::path& settings_file);

    LaunchSettings ParseModSettings(const fs::path& gdc_path);
    std::vector<ModSet> ParsePresetsFromFile(const fs::path& gdc_path);

    bool AddWadDirToGzdoomIni(const fs::path& iwad_full_path);
private:
    std::fstream file_;
    static const LaunchSettings _default_win;
    static const LaunchSettings _default_mac;

};

//================================================================//
//================ Dgc Builder - based on Json builder YaCpp24 ===//
class Builder {
private:
    class RetItm;
    class KeyReturnItem;
    class MapReturnItem;
    class ArrayReturnItem;

public:
    ///Конструктор по умолчанию
    Builder();
    ///Передать ключ в текущий словарь
    KeyReturnItem Key(std::string key);
    ///Передать значение в текущий _node
    RetItm Value(_node node);
    ///Начать новый соварь
    MapReturnItem StartMap();
    ///Начать новый массив
    ArrayReturnItem StartArray();
    ///Завершить текущий словарь
    RetItm EndMap();
    ///Завершить текущий массив
    RetItm EndArray();
    ///Вернуть готовый Json
    _node Build();

private:
    bool has_nodes_ = false;
    _node root_;
    std::stack<_node*> tree_;

    ///Текущий _node - массив, и удалось в него положить новый _node
    bool PushIfArray(_node node);
    ///Ожидается значение либо словарь/массив (новый _node)
    bool ValueExpected();
    ///Ожидается ключ словаря
    bool KeyExpected();

    //===================== ReturnItmems =====================//
    class RetItm {
    public:
        ///Конструктор по умолчанию
        RetItm(Builder& bd)
            : bldr_(bd) {}

        KeyReturnItem Key(std::string key) {
            return bldr_.Key(std::move(key));
        }
        RetItm Value(_node node) {
            return bldr_.Value(std::move(node));
        }
        MapReturnItem StartMap() {
            return bldr_.StartMap();
        }
        ArrayReturnItem StartArray() {
            return bldr_.StartArray();
        }
        RetItm EndMap() {
            return bldr_.EndMap();
        }
        RetItm EndArray() {
            return bldr_.EndArray();
        }
        _node Build() {
            return bldr_.Build();
        }
    private:
        //Builder& GetBuilder();
        Builder& bldr_;
    };

    //Возврат после добавления ключа
    class KeyReturnItem : public RetItm {
    public:
        KeyReturnItem(RetItm ret)
            : RetItm(ret) {}

        MapReturnItem Value(_node node) {
            return RetItm::Value(std::move(node));
        }

        KeyReturnItem Key(std::string key) = delete;
        RetItm EndMap() = delete;
        RetItm EndArray() = delete;
        _node Build() = delete;
    };
    //Возврат после создания словаря
    class MapReturnItem : public RetItm {
    public:
        MapReturnItem(RetItm ret)
            : RetItm(ret) {}

        RetItm Value(_node node) = delete;
        MapReturnItem StartMap() = delete;
        ArrayReturnItem StartArray() = delete;
        RetItm EndArray() = delete;
        _node Build() = delete;
    };
    //Возврат после создания массива
    class ArrayReturnItem : public RetItm {
    public:
        ArrayReturnItem(RetItm ret)
            : RetItm(ret) {}

        ArrayReturnItem Value(_node node) {
            return RetItm::Value(std::move(node));
        }

        KeyReturnItem Key(std::string key) = delete;
        RetItm EndMap() = delete;
        _node Build() = delete;
    };
};

}

