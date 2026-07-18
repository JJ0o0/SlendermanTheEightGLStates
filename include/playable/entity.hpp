#pragma once

#include <graphics/mesh.hpp>
#include <graphics/shader.hpp>
#include <graphics/material.hpp> 
#include <playable/transform.hpp>
#include <playable/collider.hpp>
#include <optional>
#include <vector>
#include <memory>

class World;
class Skeleton;
class Entity {
    public:
        Entity(const std::string& name) : m_name(name) {}

        void SetName(const std::string& name) { m_name = name; }
        const std::string& GetName() const { return m_name; }

        bool HasMesh() const { return m_mesh != nullptr; }
        void SetMesh(const std::shared_ptr<Mesh>& mesh) { m_mesh = mesh; }
        const std::shared_ptr<Mesh>& GetMesh() const { return m_mesh; }

        void SetMaterial(const std::shared_ptr<Material>& material) { m_material = material; }
        const std::shared_ptr<Material>& GetMaterial() const { return m_material; }

        template <typename T>
        std::shared_ptr<T> GetMaterialAs() const {
            return std::dynamic_pointer_cast<T>(m_material);
        }

        std::optional<AABB> GetWorldBounds() const;
        AABB GetColliderAABB() const { return m_collider->GetAABB(GetWorldPosition()); }
        bool HasCollider() const { return m_collider.has_value(); }
        void SetCollider(const Collider& collider) { m_collider.emplace(collider); }
        const std::optional<Collider>& GetCollider() const { return m_collider; }

        bool SnapToGround(const World& world, float maxDistance = 100.0f);
        
        bool HasSkeleton() const { return m_skeleton != nullptr; }
        void SetSkeleton(const std::shared_ptr<Skeleton>& skeleton) { m_skeleton = skeleton; }
        const std::shared_ptr<Skeleton>& GetSkeleton() const { return m_skeleton; }

        Transform& GetTransform() { return m_transform; }
        const Transform& GetTransform() const { return m_transform; }

        void SetParent(Entity* parent) { 
            if (m_parent) std::erase(m_parent->m_children, this);

            m_parent = parent;

            if (parent) parent->m_children.push_back(this);
        }

        void ForEachDescendant(const std::function<void(Entity&)>& callback) {
            for (Entity* child : m_children) {
                callback(*child);
                child->ForEachDescendant(callback);
            }
        }

        Entity* GetParent() const { return m_parent; }
        const std::vector<Entity*>& GetChildren() const { return m_children; }

        glm::mat4 GetWorldModel() const {
            glm::mat4 local = m_transform.GetModel();
            return m_parent ? (m_parent->GetWorldModel() * local) : local;
        }

        glm::vec3 GetWorldPosition() const { return glm::vec3(GetWorldModel()[3]); }
    private:
        std::string m_name;
        std::shared_ptr<Mesh> m_mesh;
        std::shared_ptr<Material> m_material;
        Transform m_transform{};
        std::optional<Collider> m_collider;
        std::shared_ptr<Skeleton> m_skeleton;

        Entity* m_parent = nullptr;
        std::vector<Entity*> m_children;
};