#pragma once
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class Config {
public:
    virtual ~Config() = default;

    /**
     * @brief [模板方法] 从指定的 JSON 文件加载配置。
     *        这是所有派生类共用的、最终的加载接口。
     * @param filePath 配置文件的路径。
     * @return 如果加载和解析成功，返回 true；否则返回 false。
     */
    bool load(const std::string& filePath);

protected:
    /**
     * @brief 
     * @param configJson 已成功加载的 JSON 对象。
     */
    virtual void ParseJson(const json& configJson) = 0;
};