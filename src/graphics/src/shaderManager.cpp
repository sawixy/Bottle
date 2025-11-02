#include "ShaderManager.h"
#include "Shader.h"
#include "Render.h"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>

ShaderManager::ShaderManager(Render* render) : render(render) {}

// Загрузить шейдер по имени и пути
void ShaderManager::load_shader(const std::string& shader_name, const std::string& shader_path) {
    if (shaders.find(shader_name) != shaders.end()) {
        std::cerr << "Shader with name '" << shader_name << "' already loaded.\n";
        return;
    }

    if (!std::filesystem::exists(shader_path)) {
        std::cerr << "Shader file not found: " << shader_path << '\n';
        return;
    }

    compile_shader(shader_path);
}

void ShaderManager::compile_shader(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open shader file: " << filename << '\n';
        return;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();
    file.close();

    Shader shader;

    std::string shader_name = std::filesystem::path(filename).stem().string();
    shaders[shader_name] = shader;

    std::cout << "Shader compiled and loaded: " << shader_name << '\n';
}

void ShaderManager::compile_all_shaders() {
    for (const auto& path : shaders_paths) {
        if (std::filesystem::is_regular_file(path)) {
            compile_shader(path);
        }
    }

    for (const auto& folder : shaders_folders) {
        if (std::filesystem::exists(folder) && std::filesystem::is_directory(folder)) {
            for (const auto& entry : std::filesystem::directory_iterator(folder)) {
                if (entry.path().extension() == ".vert" || 
                    entry.path().extension() == ".frag" ||
                    entry.path().extension() == ".glsl") {
                    compile_shader(entry.path().string());
                }
            }
        }
    }
}

// Получить шейдер по имени
Shader ShaderManager::get_shader(const std::string& shader_name) {
    auto it = shaders.find(shader_name);
    if (it != shaders.end()) {
        return it->second;
    } else {
        std::cerr << "Shader not found: " << shader_name << '\n';
        // Возвращаем пустой шейдер или можно бросить исключение
        return Shader();
    }
}

// Добавить путь к отдельному файлу шейдера
void ShaderManager::add_shader_path(const std::string& path) {
    shaders_paths.push_back(path);
}

// Добавить папку с шейдерами
void ShaderManager::add_shaders_folder(const std::string& folder_path) {
    if (std::find(shaders_folders.begin(), shaders_folders.end(), folder_path) == shaders_folders.end()) {
        shaders_folders.push_back(folder_path);
        std::cout << "Shader folder added: " << folder_path << '\n';
    }
}

// Удалить папку с шейдерами
void ShaderManager::remove_shader_folder(const std::string& folder_path) {
    auto it = std::find(shaders_folders.begin(), shaders_folders.end(), folder_path);
    if (it != shaders_folders.end()) {
        shaders_folders.erase(it);
        std::cout << "Shader folder removed: " << folder_path << '\n';
    } else {
        std::cerr << "Shader folder not found: " << folder_path << '\n';
    }
}

// Сканировать и загрузить все зарегистрированные пути
void ShaderManager::load_shaders_paths() {
    compile_all_shaders();
}

// Вывести текущие папки с шейдерами (или можно использовать для отладки)
void ShaderManager::get_shaders_folders() {
    std::cout << "Shader folders:\n";
    for (const auto& folder : shaders_folders) {
        std::cout << "  " << folder << '\n';
    }
}
