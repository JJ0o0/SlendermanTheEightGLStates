#include <graphics/model_loader.hpp>
#include <fastgltf/types.hpp>
#include <fastgltf/tools.hpp>
#include <fastgltf/glm_element_traits.hpp>
#include <platform/error_handling.hpp>
#include <glm/gtc/quaternion.hpp>
#include <iostream>
#include <filesystem>

Entity& ModelLoader::LoadModelIntoWorld(
    World& world, 
    const std::string& path,
    Shader& shader, 
    Entity* parent
) {
    std::string baseDir = std::filesystem::path(path).parent_path().string();
    Entity& root = world.CreateEntity(std::filesystem::path(path).stem().string(), parent);

    auto data = fastgltf::GltfDataBuffer::FromPath(path);
    if (data.error() != fastgltf::Error::None) {
        ErrorHandling::ShowError("FastGLTF Error", "Error opening {}", path);
        return root;
    }

    fastgltf::Parser parser;
    auto asset = parser.loadGltf(
        data.get(),
        std::filesystem::path(path).parent_path(),
        fastgltf::Options::LoadExternalBuffers | fastgltf::Options::DecomposeNodeMatrices
    );

    if (asset.error() != fastgltf::Error::None) {
        ErrorHandling::ShowError("FastGLTF Error", "Error parsing {}", path);
        return root;
    }

    size_t sceneIndex = asset->defaultScene.value_or(0);
    for (size_t nodeIndex : asset->scenes[sceneIndex].nodeIndices) {
        createNodeEntity(
            world, 
            asset.get(), 
            nodeIndex, 
            &root, 
            baseDir, 
            shader
        );
    }

    return root;
}

void ModelLoader::attachMeshAndMaterial(
    Entity& entity, 
    const fastgltf::Asset& asset, 
    const fastgltf::Primitive& primitive, 
    const std::string& baseDir, 
    Shader& shader
) {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    // Vertex Position
    auto posIt = primitive.findAttribute("POSITION");
    if (posIt == primitive.attributes.end()) return;

    const auto& positionAccessor = asset.accessors[posIt->accessorIndex];
    vertices.resize(positionAccessor.count);

    fastgltf::iterateAccessorWithIndex<glm::vec3>(asset, positionAccessor, [&](glm::vec3 pos, size_t index) {
        vertices[index].Position = pos;
    });

    // Vertex Normal
    auto normalIt = primitive.findAttribute("NORMAL");
    if (normalIt != primitive.attributes.end()) {
        const auto& accessor = asset.accessors[normalIt->accessorIndex];
        fastgltf::iterateAccessorWithIndex<glm::vec3>(asset, accessor,[&](glm::vec3 n, size_t idx) { 
            vertices[idx].Normal = n; 
        });
    }

    // Vertex TexCoord
    auto uvIt = primitive.findAttribute("TEXCOORD_0");
    if (uvIt != primitive.attributes.end()) {
        const auto& accessor = asset.accessors[uvIt->accessorIndex];
        fastgltf::iterateAccessorWithIndex<glm::vec2>(asset, accessor,[&](glm::vec2 uv, size_t idx) { 
            glm::vec2 invertedUV = {
                uv.x,
                1.0f - uv.y
            };

            vertices[idx].TexCoord = invertedUV; 
        });
    }

    // Vertex Tangent
    auto tanIt = primitive.findAttribute("TANGENT");
    bool hasTangents = tanIt != primitive.attributes.end();
    if (hasTangents) {
        const auto& accessor = asset.accessors[tanIt->accessorIndex];
        fastgltf::iterateAccessorWithIndex<glm::vec4>(asset, accessor,[&](glm::vec4 t, size_t idx) { 
            vertices[idx].Tangent = glm::vec3(t); 
        });
    }

    // Indices
    if (primitive.indicesAccessor.has_value()) {
        const auto& accessor = asset.accessors[*primitive.indicesAccessor];
        indices.resize(accessor.count);

        fastgltf::iterateAccessorWithIndex<uint32_t>(asset, accessor,[&](uint32_t index, size_t idx) { 
            indices[idx] = index; 
        });

        if (!hasTangents) generateTangents(vertices, indices);
    }

    entity.SetMesh(std::make_shared<Mesh>(vertices, indices));

    // Materials
    if (primitive.materialIndex.has_value()) {
        const auto& material = asset.materials[*primitive.materialIndex];
        auto pbrMaterial = std::make_shared<PBRMaterial>(shader);

        PBRTextureSet textures;
        auto albedoPath = material.pbrData.baseColorTexture
            ? resolveTexturePath(asset, material.pbrData.baseColorTexture->textureIndex, baseDir)
            : std::nullopt;

        auto normalPath = material.normalTexture
            ? resolveTexturePath(asset, material.normalTexture->textureIndex, baseDir)
            : std::nullopt;

        auto armPath = material.pbrData.metallicRoughnessTexture
            ? resolveTexturePath(asset, material.pbrData.metallicRoughnessTexture->textureIndex, baseDir)
            : std::nullopt;
        
        if (albedoPath) textures.Albedo = std::make_shared<Texture>(TextureProperties{ .SRGB = true, .ImagePath = *albedoPath });
        if (normalPath) textures.Normal = std::make_shared<Texture>(TextureProperties{ .ImagePath = *normalPath });
        if (armPath) textures.ARM = std::make_shared<Texture>(TextureProperties{ .ImagePath = *armPath });

        pbrMaterial->SetTextures(textures);
        entity.SetMaterial(pbrMaterial);
    }
}

void ModelLoader::generateTangents(std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) {
    std::vector<glm::vec3> accumulator(vertices.size(), glm::vec3(0.0f));

    for (size_t i = 0; i + 2 < indices.size(); i += 3) {
        uint32_t i0 = indices[i]; 
        uint32_t i1 = indices[i + 1];
        uint32_t i2 = indices[i + 2];

        const glm::vec3& pos0 = vertices[i0].Position;
        const glm::vec3& pos1 = vertices[i1].Position;
        const glm::vec3& pos2 = vertices[i2].Position;

        const glm::vec2& uv0 = vertices[i0].TexCoord;
        const glm::vec2& uv1 = vertices[i1].TexCoord;
        const glm::vec2& uv2 = vertices[i2].TexCoord;

        glm::vec3 edge1 = pos1 - pos0;
        glm::vec3 edge2 = pos2 - pos0;

        glm::vec2 deltaUV1 = uv1 - uv0;
        glm::vec2 deltaUV2 = uv2 - uv0;

        float denom = deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y;
        
        // Se os três pontos tem o mesmo UV, denom vira infinito.
        if (std::abs(denom) < 1e-8f) continue;

        float f = 1.0f / denom;
        glm::vec3 tangent = f * (deltaUV2.y * edge1 - deltaUV1.y * edge2);

        accumulator[i0] += tangent;
        accumulator[i1] += tangent;
        accumulator[i2] += tangent;
    }

    for (size_t i = 0; i < vertices.size(); i++) {
        if (glm::length(accumulator[i]) < 1e-8f) {
            glm::vec3 result = glm::cross(vertices[i].Normal, glm::vec3(0.0f, 1.0f, 0.0f));

            if (glm::length(result) < 1e-8f) {
                vertices[i].Tangent = glm::normalize(
                    glm::cross(vertices[i].Normal, glm::vec3(1.0f, 0.0f, 0.0f))
                );

                continue;
            }

            vertices[i].Tangent = glm::normalize(result);
        } else {
            vertices[i].Tangent = glm::normalize(accumulator[i]);
        }
    }
}

void ModelLoader::applyNodeTransform(const fastgltf::Node& node, Transform& transform) {
    // Obrigado DecomposeNodeMatrices!
    const auto& trs = std::get<fastgltf::TRS>(node.transform);

    transform.Position = { trs.translation.x(), trs.translation.y(), trs.translation.z() };
    transform.Scale = { trs.scale.x(), trs.scale.y(), trs.scale.z() };

    glm::quat rotation { trs.rotation.w(), trs.rotation.x(), trs.rotation.y(), trs.rotation.z() };
    transform.Rotation = glm::degrees(glm::eulerAngles(rotation));
}

void ModelLoader::createNodeEntity(
    World& world, 
    const fastgltf::Asset& asset, 
    size_t nodeIndex,
    Entity* parent, 
    const std::string& baseDir, 
    Shader& shader
) {
    const auto& node = asset.nodes[nodeIndex];
    std::string name = node.name.empty() ? ("Node_" + std::to_string(nodeIndex)) : std::string(node.name);

    Entity& entity = world.CreateEntity(name, parent);
    applyNodeTransform(node, entity.GetTransform());

    if (node.meshIndex.has_value()) {
        const auto& mesh = asset.meshes[*node.meshIndex];

        for (size_t i = 0; i < mesh.primitives.size(); i++) {
            Entity& target = (mesh.primitives.size() == 1) 
                ? entity 
                : world.CreateEntity(name + "_part" + std::to_string(i), &entity);

            attachMeshAndMaterial(target, asset, mesh.primitives[i], baseDir, shader);
        }
    }

    for (size_t childIndex : node.children) {
        createNodeEntity(
            world, 
            asset, 
            childIndex, 
            &entity, 
            baseDir, 
            shader
        );
    }
}

std::optional<std::string> ModelLoader::resolveTexturePath(
    const fastgltf::Asset& asset, 
    const std::optional<size_t>& textureIndex, 
    const std::string& baseDirectory
) {
    if (!textureIndex.has_value()) return std::nullopt;

    const auto& texture = asset.textures[*textureIndex];
    if (!texture.imageIndex.has_value()) return std::nullopt;

    const auto& image = asset.images[*texture.imageIndex];
    if (const auto* uri = std::get_if<fastgltf::sources::URI>(&image.data)) {
        return (std::filesystem::path(baseDirectory) / uri->uri.fspath()).string();
    }

    std::cerr << "Embedded texture ignored, export with external textures.\n";
    return std::nullopt;
}