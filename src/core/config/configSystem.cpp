#include <core/config/configSystem.hpp>
#include <rapidjson/document.h>
#include <fstream>
#include <string>
#include <sstream>

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

void ConfigSystem::load(std::string filename) {
    /*std::string extension = filename.substr(filename.find_last_of(".") + 1);
    if (extension == "json") {
        load_json(filename);
    } else {
        throw std::runtime_error("Unsupported file format: " + extension);
    }*/
    config["window.width"] = "800";
    config["window.height"] = "800";
}

void ConfigSystem::load_json(std::string filename) {
    std::string json = readFile(filename);
    rapidjson::Document document;
    document.Parse(json.c_str());

    for (auto it = document.MemberBegin(); it != document.MemberEnd(); ++it) {
        std::string key = it->name.GetString();
        std::string value = it->value.GetString();
        config[key] = value;
    }
}

}