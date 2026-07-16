#pragma once

#include <world.hpp>
#include <graphics/shader.hpp>
#include <fastgltf/core.hpp>
#include <string>

class ModelLoader {
    public:
        static Entity& LoadModelIntoWorld(World& world, const std::string& path, Shader& shader, Entity* parent = nullptr);
    private:
        static void attachMeshAndMaterial(
            Entity& entity, 
            const fastgltf::Asset& asset, 
            const fastgltf::Primitive& primitive, 
            const std::string& baseDir, 
            Shader& shader
        );

        static void applyNodeTransform(const fastgltf::Node& node, Transform& transform);
        
        static void createNodeEntity(
            World& world, 
            const fastgltf::Asset& asset, 
            size_t nodeIndex,
            Entity* parent, 
            const std::string& baseDir, 
            Shader& shader
        );

        static std::optional<std::string> resolveTexturePath(
            const fastgltf::Asset& asset, 
            const std::optional<size_t>& textureIndex, 
            const std::string& baseDirectory
        );
};