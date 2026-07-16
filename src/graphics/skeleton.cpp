#define GLM_ENABLE_EXPERIMENTAL
#include <graphics/skeleton.hpp>
#include <playable/entity.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

std::vector<glm::mat4> Skeleton::GetSkinningMatrices() const {
    std::vector<glm::mat4> matrices;
    matrices.reserve(m_joints.size());

    for (const auto& joint : m_joints) {
        glm::mat4 worldPose = joint.JointEntity ? joint.JointEntity->GetWorldModel() : glm::mat4(1.0f);
        matrices.push_back(worldPose * joint.InverseBindMatrix);
    }

    return matrices;
}

void Skeleton::DebugPrint() const {
    std::cout << "\n=== Skeleton: " << m_joints.size() << " joints ===\n";

    for (const auto& joint : m_joints) {
        glm::vec3 worldPos = joint.JointEntity ? joint.JointEntity->GetWorldPosition() : glm::vec3(0.0f);

        std::cout
            << "    Joint Name: " << joint.Name
            << " | World Position: " << glm::to_string(worldPos)
            << "\n";
    }

    std::cout << "=== Skeleton Debug End ===\n\n";
}