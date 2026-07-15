#version 410 core

out vec4 FragColor;
in vec3 WorldPos;
in vec2 TexCoord;
in mat3 TBN;
in vec4 FragPosLightSpace;

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
    vec3 Direction;

    float InnerCutoff;
    float OuterCutoff;

    bool Enabled;
};

uniform vec3 uCameraPosition;
uniform Light uLight;

uniform vec3 uAmbientColor;
uniform float uAmbientIntensity;

uniform sampler2D uAlbedoMap;
uniform sampler2D uNormalMap;
uniform sampler2D uARMMap;
uniform sampler2D uFlashlightCookie;

uniform sampler2D uShadowMap;

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
    denom = PI * denom * denom;

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

float calculateSpotIntensity(vec3 lightDirection) {
    float theta = dot(lightDirection, normalize(-uLight.Direction));
    float epsilon = uLight.InnerCutoff - uLight.OuterCutoff;
    return clamp((theta - uLight.OuterCutoff) / epsilon, 0.0, 1.0);
}

float calculateCookie() {
    vec3 forward = normalize(uLight.Direction);

    vec3 right = normalize(cross(forward, vec3(0.0, 1.0, 0.0)));
    vec3 up = normalize(cross(right, forward));

    vec3 local = WorldPos - uLight.Position;

    float z = dot(local, forward);

    if (z <= 0.0) return 0.0;

    float radius = tan(acos(uLight.OuterCutoff));

    vec2 uv;
    uv = vec2(dot(local, right), dot(local, up)) / (z * radius);
    uv = uv * 0.5 + 0.5;

    if (uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0) return 0.0;
    return texture(uFlashlightCookie, uv).r;
}

float calculateAttenuation(float dist) {
    return 1.0 / (
        1.0 +
        0.09 * dist +
        0.032 * dist * dist
    );
}

float calculateShadow() {
    vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if( projCoords.z > 1.0 ||
        projCoords.x < 0.0 ||
        projCoords.x > 1.0 ||
        projCoords.y < 0.0 ||
        projCoords.y > 1.0) {
        return 0.0;
    }

    float currDepth = projCoords.z;
    float bias = 0.0005;

    vec2 texelSize = 1.0 / vec2(textureSize(uShadowMap, 0));

    float shadowSum = 0.0;
    for (int y = -1; y <= 1; y++) {
        for (int x = -1; x <= 1; x++) {
            vec2 offset = vec2(x, y) * texelSize;
            float neighbor = texture(uShadowMap, projCoords.xy + offset).r;
            shadowSum += (currDepth - bias > neighbor) ? 1.0 : 0.0;
        }
    }

    float shadow = shadowSum / 9.0;

    return shadow;
}

vec3 calculateDirectLight(MaterialData data, vec3 normal) {
    if (!uLight.Enabled) return vec3(0.0);

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
    float attenuation = calculateAttenuation(dist);
    float spot = calculateSpotIntensity(lightDirection);
    float cookie = calculateCookie();

    vec3 radiance = uLight.Radiance * attenuation * spot * cookie;

    float shadow = calculateShadow();

    vec3 result = (diffuse + specular) * radiance * ndotl * (1.0 - shadow);

    return result;
}

vec3 tonemap(vec3 x) {
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;

    return clamp((x*(a*x+b))/(x*(c*x+d)+e),0.0,1.0);
}

vec3 gammaCorrection(vec3 color) {
    return pow(color, vec3(1.0 / 2.2));
}

vec3 applyFog(vec3 color) {
    vec3 fogColor = vec3(0.01, 0.015, 0.02);
    float dist = length(uCameraPosition - WorldPos);
    float fog = exp(-dist * 0.08);

    fog = clamp(fog, 0.0, 1.0);
    return mix(fogColor, color, fog);
}

void main() {
    MaterialData material = getMaterial();
    
    vec3 ambient = calculateAmbient(material);
    vec3 direct = calculateDirectLight(material, material.Normal);

    vec3 color = ambient + direct;
    color = applyFog(color);
    color = tonemap(color);
    color = gammaCorrection(color);
    
    FragColor = vec4(color, 1.0);
}