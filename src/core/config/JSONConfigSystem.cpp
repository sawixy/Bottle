#include <core/config/JSONConfigSystem.hpp>
#include <rapidjson/document.h>
#include <fstream>
#include <string>
#include <sstream>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <stdexcept>
#include <vector>

std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

namespace bottle::core::config {

// This code has written by AI btw. Pray this wont crush the engine :D
static rapidjson::Value* findValue(rapidjson::Value& root, const std::string& key) {
    rapidjson::Value* cur = &root;
    size_t start = 0;
    while (start <= key.size()) {
        size_t dot = key.find('.', start);
        std::string part = key.substr(start, (dot == std::string::npos ? key.size() : dot) - start);
        if (!cur->IsObject()) return nullptr;
        auto it = cur->FindMember(part.c_str());
        if (it == cur->MemberEnd()) return nullptr;
        cur = &it->value;
        if (dot == std::string::npos) break;
        start = dot + 1;
    }
    return cur;
}

void JSONConfigSystem::load(const std::string& filename) {
    std::string content = readFile(filename);
    config.Parse(content.c_str());
    if (config.HasParseError()) {
        throw std::runtime_error("Failed to parse JSON: " + filename);
    }
}

std::any JSONConfigSystem::getAny(const std::string& key) {
    rapidjson::Value* v = findValue(config, key);
    if (!v) return {};

    if (v->IsBool()) return v->GetBool();
    if (v->IsInt()) return v->GetInt();
    if (v->IsInt64()) return v->GetInt64();
    if (v->IsUint()) return v->GetUint();
    if (v->IsDouble()) return v->GetDouble();
    if (v->IsString()) return std::string(v->GetString());

    rapidjson::StringBuffer buf;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buf);
    v->Accept(writer);
    return std::string(buf.GetString());
}

void JSONConfigSystem::setAny(const std::string& key, const std::any& value) {
    rapidjson::Document::AllocatorType& alloc = config.GetAllocator();
    if (!config.IsObject()) config.SetObject();

    std::vector<std::string> parts;
    size_t start = 0;
    while (start <= key.size()) {
        size_t dot = key.find('.', start);
        parts.push_back(key.substr(start, (dot == std::string::npos ? key.size() : dot) - start));
        if (dot == std::string::npos) break;
        start = dot + 1;
    }

    rapidjson::Value* cur = &config;
    for (size_t i = 0; i + 1 < parts.size(); ++i) {
        auto it = cur->FindMember(parts[i].c_str());
        if (it == cur->MemberEnd()) {
            rapidjson::Value name(parts[i].c_str(), alloc);
            rapidjson::Value obj(rapidjson::kObjectType);
            cur->AddMember(name, obj, alloc);
            it = cur->FindMember(parts[i].c_str());
        }
        cur = &it->value;
    }

    std::string last = parts.back();
    rapidjson::Value name(last.c_str(), alloc);
    rapidjson::Value newVal;

    if (value.type() == typeid(bool)) newVal.SetBool(std::any_cast<bool>(value));
    else if (value.type() == typeid(int)) newVal.SetInt(std::any_cast<int>(value));
    else if (value.type() == typeid(long)) newVal.SetInt64(std::any_cast<long>(value));
    else if (value.type() == typeid(long long)) newVal.SetInt64(std::any_cast<long long>(value));
    else if (value.type() == typeid(unsigned)) newVal.SetUint(std::any_cast<unsigned>(value));
    else if (value.type() == typeid(double)) newVal.SetDouble(std::any_cast<double>(value));
    else if (value.type() == typeid(std::string)) {
        const std::string &s = std::any_cast<const std::string &>(value);
        newVal.SetString(s.c_str(), alloc);
    } else {
        throw std::invalid_argument("Unsupported type for setAny");
    }

    auto it = cur->FindMember(last.c_str());
    if (it != cur->MemberEnd()) {
        cur->RemoveMember(it);
    }
    cur->AddMember(name, newVal, alloc);
}

}