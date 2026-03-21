#pragma once

#include <core/utils/singleton.hpp>
#include <string>
#include <unordered_map>

namespace bottle::core::resources {

class Paths : public Singleton<Paths> {
private:
    std::unordered_map<std::string, std::string> paths;

public:
    std::string getPath(std::string path_type) { return paths[path_type]; }
    void setPath(std::string path_type, std::string path) { paths[path_type] = path; }
};

}