#version 330

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexCoord;

out vec4 finalColor;

struct Light {
    int type; // 0 = Directional, 1 = Point
    vec3 position;
    vec3 direction;
    vec3 color;
    float intensity;
    float range;

    // 阴影贴图索引
    int shadowIndex;
    float shadowBias;
};

#define MAX_LIGHTS 16
#define MAX_SHADOWS 4 

uniform sampler2D shadowMaps[MAX_SHADOWS];
uniform mat4 lightVPs[MAX_SHADOWS];

uniform Light lights[MAX_LIGHTS];
uniform int lightCounts;
uniform vec3 emissiveColor;
uniform float emissiveIntensity;

uniform vec3 viewPos;
uniform vec4 baseColor;

uniform sampler2D u_diffuseMap;
uniform int u_diffuseMap_frameCount;
uniform float u_diffuseMap_animSpeed;
uniform float gameTime;
uniform float realTime;

// --- 计算方向光 ---
vec3 CalcDirectionalLight(Light light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);

    // 漫反射
    float diff = max(dot(normal, lightDir), 0.0);

    // 高光 (Blinn-Phong)
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

    vec3 result = (diff + spec) * light.color * light.intensity;
    return result;
}

// --- 计算点光源 ---
vec3 CalcPointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);

    // 衰减 (Attenuation)
    float distance = length(light.position - fragPos);
    // 使用简单的线性+二次衰减，或者基于 range 的平滑衰减
    // 这里使用基于 range 的简单公式：1 - (dist/range)^2
    float att = 1.0 - clamp(distance / light.range, 0.0, 1.0);
    att = att * att; // 平方让衰减更自然

    // 漫反射
    float diff = max(dot(normal, lightDir), 0.0);

    // 高光
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

    vec3 result = (diff + spec) * light.color * light.intensity * att;
    return result;
}

float SampleShadowMap(int index, vec2 coords) {
    if(index == 0)
        return texture(shadowMaps[0], coords).r;
    if(index == 1)
        return texture(shadowMaps[1], coords).r;
    if(index == 2)
        return texture(shadowMaps[2], coords).r;
    if(index == 3)
        return texture(shadowMaps[3], coords).r;
    return 1.0;
}

float CalculateShadow(int shadowIndex, vec3 worldPos, float bias) {
    if(shadowIndex < 0 || shadowIndex >= MAX_SHADOWS)
        return 0.0;

    // 1. 变换到该光源的裁剪空间
    vec4 fragPosLightSpace = lightVPs[shadowIndex] * vec4(worldPos, 1.0);

    // 2. 透视除法
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    // 3. 越界检查
    if(projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.x > 1.0 || projCoords.y < 0.0 || projCoords.y > 1.0)
        return 0.0;

    // 4. PCF 采样 (简单 3x3)
    float currentDepth = projCoords.z;
    float shadow = 0.0;
    vec2 texSize = vec2(1.0) / vec2(textureSize(shadowMaps[0], 0)); // 假设所有阴影图大小一致

    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {

            // float pcfDepth = SampleShadowMap(shadowIndex, projCoords.xy + vec2(x, y) * texSize);
            float pcfDepth = 0.0;
            if(shadowIndex == 0)
                pcfDepth = texture(shadowMaps[0], projCoords.xy + vec2(x, y) * texSize).r;
            else if(shadowIndex == 1)
                pcfDepth = texture(shadowMaps[1], projCoords.xy + vec2(x, y) * texSize).r;
            else if(shadowIndex == 2)
                pcfDepth = texture(shadowMaps[2], projCoords.xy + vec2(x, y) * texSize).r;
            else if(shadowIndex == 3)
                pcfDepth = texture(shadowMaps[3], projCoords.xy + vec2(x, y) * texSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    return shadow;
}

void main() {
    vec3 norm = normalize(fragNormal);
    vec3 viewDir = normalize(viewPos - fragPosition);
    vec3 totalLight = vec3(0.1); // 基础环境光 0.1

    for(int i = 0; i < MAX_LIGHTS; i++) {
        if(i >= lightCounts) {
            break;
        }
        vec3 lightContribution = vec3(0.0);
        float shadow = 0.0;
        if(lights[i].shadowIndex >= 0) {
            shadow = CalculateShadow(lights[i].shadowIndex, fragPosition, lights[i].shadowBias);
        }

        if(lights[i].type == 0) {
            lightContribution = CalcDirectionalLight(lights[i], norm, viewDir);
        } else {
            lightContribution = CalcPointLight(lights[i], norm, fragPosition, viewDir);
        }

        totalLight += lightContribution * (1.0 - shadow);
    }

    float currentFrame = floor(mod(gameTime * u_diffuseMap_animSpeed, float(u_diffuseMap_frameCount)));
    vec2 animatedUV = fragTexCoord;
    animatedUV.y /= u_diffuseMap_frameCount;

    animatedUV.y += (currentFrame / u_diffuseMap_frameCount);

    vec4 texColor = texture(u_diffuseMap, animatedUV);
    vec3 emission = emissiveColor * emissiveIntensity;
    finalColor = vec4(totalLight * texColor.rgb + emission, texColor.a);
}