#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <string>

class Entity;

struct Joint {
    std::string Name;
    Entity* JointEntity = nullptr;
    glm::mat4 InverseBindMatrix{1.0f};
};

class Skeleton {
    public:
        void AddJoint(const Joint& joint) { m_joints.push_back(joint); }

        const std::vector<Joint>& GetJoints() const { return m_joints; }
        size_t GetJointsCount() const { return m_joints.size(); }

        std::vector<glm::mat4> GetSkinningMatrices() const;

        // DEBUG
        void DebugPrint() const;
    private:
        std::vector<Joint> m_joints;
};