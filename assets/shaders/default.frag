#version 410 core

out vec4 FragColor;
in vec3 WorldPos;
in vec2 TexCoord;
in mat3 TBN;

struct Light {
    vec3 Radiance;
    vec3 Position;
};

struct Material {
    vec3 Albedo;
    float Metallic;
    float Roughness;
    float AO;
};

uniform vec3 uCameraPosition;
uniform Light uLight;
uniform Material uMaterial;
uniform sampler2D uNormalMap;

vec3 calculateFresnelSchlick(float cosTheta, vec3 f0) {
    return f0 + (1.0 - f0) * pow(1.0 - cosTheta, 5.0);
}

float distributionGGX(vec3 n, vec3 h, float r) {
    float a = r * r;
    float a2 = a * a;

    float ndoth = max(dot(n, h), 0.0);
    float ndoth2 = ndoth * ndoth;

    float num = a2;
    float denom = (ndoth2 * (a2 - 1.0) + 1.0);
    denom = 3.14159265 * denom * denom;

    return num / denom;
}

float geometrySchlickGGX(float ndotv, float r) {
    float k = pow(r + 1.0, 2.0) / 8.0;

    return ndotv / (ndotv * (1.0 - k) + k);
}

float geometrySmith(vec3 n, vec3 v, vec3 l, float r) {
    float ndotv = max(dot(n, v), 0.0);
    float ndotl = max(dot(n, l), 0.0);

    float ggx1 = geometrySchlickGGX(ndotv, r);
    float ggx2 = geometrySchlickGGX(ndotl, r);

    return ggx1 * ggx2;
}

void main() {
    vec3 tangentNormal = texture(uNormalMap, TexCoord).rgb;
    tangentNormal = tangentNormal * 2.0 - 1.0;

    vec3 normal = normalize(TBN * tangentNormal);
    vec3 lightDirection = normalize(uLight.Position - WorldPos);
    vec3 viewDirection = normalize(uCameraPosition - WorldPos);
    vec3 halfWay = normalize(viewDirection + lightDirection);

    vec3 f0 = mix(vec3(0.04), uMaterial.Albedo, uMaterial.Metallic);

    float ndotv = max(dot(normal, viewDirection), 0.0);
    float ndotl = max(dot(normal, lightDirection), 0.0);

    float d = distributionGGX(normal, halfWay, uMaterial.Roughness);
    float g = geometrySmith(normal, viewDirection, lightDirection, uMaterial.Roughness);
    vec3 f = calculateFresnelSchlick(max(dot(halfWay, viewDirection), 0.0), f0);
    
    vec3 num = (d * g) * f;
    float denom = 4.0 * ndotv * ndotl + 0.0001;
    vec3 specular = num / denom;

    vec3 ks = f;
    vec3 kd = vec3(1.0) - ks;
    kd *= 1.0 - uMaterial.Metallic;

    vec3 diffuse = kd * uMaterial.Albedo / 3.14159265;

    // Point Light
    float dist = length(uLight.Position - WorldPos);
    float attenuation = 1.0 / (dist * dist);
    vec3 radiance = uLight.Radiance * attenuation;

    vec3 result = (diffuse + specular) * radiance * ndotl;

    // Tonemapping
    vec3 color = result;
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0 / 2.2));

    FragColor = vec4(color, 1.0);
}