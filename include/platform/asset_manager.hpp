#pragma once

#include <unordered_map>
#include <functional>
#include <string>
#include <memory>

template<typename T>
class AssetManager {
    public:
        static std::shared_ptr<T> GetOrLoad(
            const std::string& key,
            const std::function<std::shared_ptr<T>()>& factory
        ) {
            auto it = s_cache.find(key);
            if (it != s_cache.end()) return it->second;

            auto asset = factory();
            s_cache[key] = asset;
            return asset;
        }

        static std::shared_ptr<T> TryGet(const std::string& key) {
            auto it = s_cache.find(key);
            return it != s_cache.end() ? it->second : nullptr;
        }

        static void Store(const std::string& key, std::shared_ptr<T> asset) {
            s_cache[key] = asset;
        }

    private:
        static std::unordered_map<std::string, std::shared_ptr<T>> s_cache;
};

template <typename T>
std::unordered_map<std::string, std::shared_ptr<T>> AssetManager<T>::s_cache;