#version 410 core

out vec4 FragColor;
in vec3 WorldPos;
in vec2 TexCoord;
in mat3 TBN;

#define PI 3.14159265

struct MaterialData {
    vec3 Albedo;
    vec3 Normal;

    float Metallic;
    float Roughness;
    float AO;
};

struct Light {
    vec3 Radiance;
    vec3 Position;
};

uniform vec3 uCameraPosition;
uniform Light uLight;

uniform vec3 uAmbientColor;
uniform float uAmbientIntensity;

uniform sampler2D uAlbedoMap;
uniform sampler2D uNormalMap;
uniform sampler2D uARMMap;

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

MaterialData getMaterial() {
    MaterialData data;
    data.Albedo = texture(uAlbedoMap, TexCoord).rgb;

    vec3 arm = texture(uARMMap, TexCoord).rgb;

    data.AO = arm.r;
    data.Roughness = arm.g;
    data.Metallic = arm.b;

    vec3 tangentNormal = texture(uNormalMap, TexCoord).rgb;
    tangentNormal = tangentNormal * 2.0 - 1.0;
    data.Normal = normalize(TBN * tangentNormal);

    return data;
}

vec3 calculateAmbient(MaterialData data) {
    return uAmbientColor * uAmbientIntensity * data.Albedo * data.AO;
}

vec3 calculateDirectLight(MaterialData data, vec3 normal) {
    vec3 lightDirection = normalize(uLight.Position - WorldPos);
    vec3 viewDirection = normalize(uCameraPosition - WorldPos);
    vec3 halfWay = normalize(viewDirection + lightDirection);

    vec3 f0 = mix(vec3(0.04), data.Albedo, data.Metallic);

    float ndotv = max(dot(normal, viewDirection), 0.0);
    float ndotl = max(dot(normal, lightDirection), 0.0);

    float d = distributionGGX(normal, halfWay, data.Roughness);
    float g = geometrySmith(normal, viewDirection, lightDirection, data.Roughness);
    vec3 f = calculateFresnelSchlick(max(dot(halfWay, viewDirection), 0.0), f0);

    vec3 num = (d * g) * f;
    float denom = 4.0 * ndotv * ndotl + 0.0001;
    vec3 specular = num / denom;

    vec3 ks = f;
    vec3 kd = vec3(1.0) - ks;
    kd *= 1.0 - data.Metallic;

    vec3 diffuse = kd * data.Albedo / PI;

    float dist = length(uLight.Position - WorldPos);
    float attenuation = 1.0 / (dist * dist);
    vec3 radiance = uLight.Radiance * attenuation;

    vec3 result = (diffuse + specular) * radiance * ndotl;
    result *= data.AO;

    return result;
}

vec3 tonemap(vec3 color) {
    return color / (color + vec3(1.0));
}

vec3 gammaCorrection(vec3 color) {
    return pow(color, vec3(1.0 / 2.2));
}

void main() {
    MaterialData material = getMaterial();
    
    vec3 ambient = calculateAmbient(material);
    vec3 direct = calculateDirectLight(material, material.Normal);

    vec3 color = ambient + direct;
    color = tonemap(color);
    color = gammaCorrection(color);
    
    FragColor = vec4(color, 1.0);
}