#pragma once

#include <graphics/mesh.hpp>
#include <graphics/shader.hpp>
#include <graphics/materials/pbr_material.hpp>
#include <playable/transform.hpp>
#include <playable/collider.hpp>
#include <optional>
#include <memory>

class Entity {
    public:
        Entity(const std::string& name) : m_name(name) {}

        void SetName(const std::string& name) { m_name = name; }
        const std::string& GetName() const { return m_name; }

        void SetMesh(const std::shared_ptr<Mesh>& mesh) { m_mesh = mesh; }
        const std::shared_ptr<Mesh>& GetMesh() const { return m_mesh; }

        void SetMaterial(const std::shared_ptr<PBRMaterial>& material) { m_material = material; }
        const std::shared_ptr<PBRMaterial>& GetMaterial() const { return m_material; }

        AABB GetColliderAABB() const { return m_collider->GetAABB(m_transform.Position); }
        bool HasCollider() const { return m_collider.has_value(); }
        void SetCollider(const Collider& collider) { m_collider.emplace(collider); }
        const std::optional<Collider>& GetCollider() const { return m_collider; }

        Transform& GetTransform() { return m_transform; }
        const Transform& GetTransform() const { return m_transform; }
    private:
        std::string m_name;
        std::shared_ptr<Mesh> m_mesh; // alguma classe *Model* para lidar com várias meshes, por enquanto só uma
        std::shared_ptr<PBRMaterial> m_material;
        Transform m_transform{};
        std::optional<Collider> m_collider;
};