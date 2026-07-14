#pragma once

#include <graphics/mesh.hpp>

inline Mesh CreateSphere(int rings = 16, int segments = 32, float radius = 0.5f) {
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	const float PI = 3.14159265f;
	for (int ring = 0; ring <= rings; ring++) {
		float v = static_cast<float>(ring) / static_cast<float>(rings);
		float phi = v * PI;

		for (int segment = 0; segment <= segments; segment++) {
			float u = static_cast<float>(segment) / static_cast<float>(segments);
			float theta = u * (PI * 2);

			glm::vec3 position = glm::vec3(
				std::cos(theta) * std::sin(phi) * radius,
				std::cos(phi) * radius,
				std::sin(theta) * std::sin(phi) * radius
			);

			glm::vec3 normal = glm::normalize(position);
            glm::vec2 textureCoords = glm::vec2(u, 1.0f - v);

            glm::vec3 tangent = glm::normalize(
                glm::vec3(
                    -std::sin(theta),
                    0.0f,
                    std::cos(theta)
                )
            );

			vertices.push_back({position, normal, textureCoords, tangent});
		}
	}

	for (int ring = 0; ring < rings; ring++) {
		for (int segment = 0; segment < segments; segment++) {
			uint32_t current = ring * (segments + 1) + segment;
			uint32_t next = current + segments + 1;

			indices.push_back(current);
			indices.push_back(next);
			indices.push_back(current + 1);

			indices.push_back(current + 1);
			indices.push_back(next);
			indices.push_back(next + 1);
		}
	}

	return Mesh(vertices, indices);
}