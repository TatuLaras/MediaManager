#pragma once


struct Serializable {
    virtual void SerializeJSON(nlohmann::json& json) = 0;
    virtual void DeserializeJSON(const nlohmann::json& json) = 0;
};

struct SubSerializable {
    template<typename T>
    static std::vector<std::map<std::string, nlohmann::json>> ArrayToSerializable(std::vector<T> data);

    template <typename T>
    static std::vector<T> ArrayFromSerializable(std::vector<std::map<std::string, nlohmann::json>> data, bool tmdb_mode = false);

    virtual std::map<std::string, nlohmann::json> ToSerializableType() = 0;
    virtual void FromSerializableType(std::map<std::string, nlohmann::json> data, bool tmdb_mode = false) = 0;
};