#pragma once

#include <world.hpp>
#include <graphics/shader.hpp>
#include <graphics/texture.hpp>
#include <graphics/vertex.hpp>
#include <graphics/skeleton.hpp>
#include <graphics/animation/animation_clip.hpp>
#include <fastgltf/core.hpp>
#include <unordered_map>
#include <string>
#include <vector>

class ModelLoader {
    public:
        static Entity& LoadModelIntoWorld(
            World& world, 
            const std::string& path, 
            Shader& shader, 
            Entity* parent = nullptr,
            std::vector<AnimationClip>* outAnimations = nullptr
        );
    private:
        static void attachMeshAndMaterial(
            Entity& entity, 
            const fastgltf::Asset& asset, 
            const fastgltf::Primitive& primitive, 
            const std::string& baseDir, 
            Shader& shader
        );

        static Skeleton parseSkin(
            const fastgltf::Asset& asset,
            const fastgltf::Skin& skin,
            const std::unordered_map<size_t, Entity*>& nodeToEntity
        );

        static std::vector<AnimationClip> parseAnimations(
            const fastgltf::Asset& asset,
            const std::unordered_map<size_t, Entity*>& nodeToEntity
        );

        static void generateTangents(std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);

        static void applyNodeTransform(const fastgltf::Node& node, Transform& transform);
        
        static void createNodeEntity(
            World& world, 
            const fastgltf::Asset& asset, 
            size_t nodeIndex,
            Entity* parent, 
            const std::string& baseDir, 
            Shader& shader,
            std::unordered_map<size_t, Entity*>& nodeToEntity,
            std::unordered_map<size_t, std::vector<Entity*>>& skinnedMeshEntities
        );

        static AABB computeMeshLocalAABB(const fastgltf::Asset& asset, const fastgltf::Mesh& mesh);

        static std::shared_ptr<Texture> loadTexture(
            const fastgltf::Asset& asset,
            size_t textureIndex,
            const std::string& baseDirectory,
            bool srgb
        );
};