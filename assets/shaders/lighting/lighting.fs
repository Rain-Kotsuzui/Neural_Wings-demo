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
};

#define MAX_LIGHTS 16
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

void main() {
    vec3 norm = normalize(fragNormal);
    vec3 viewDir = normalize(viewPos - fragPosition);
    vec3 totalLight = vec3(0.1); // 基础环境光 0.1

    for(int i = 0; i < MAX_LIGHTS; i++) {
        if(i >= lightCounts) {
            break;
        }

        if(lights[i].type == 0) {
            totalLight += CalcDirectionalLight(lights[i], norm, viewDir);
        } else {
            totalLight += CalcPointLight(lights[i], norm, fragPosition, viewDir);
        }
    }

    float currentFrame = floor(mod(gameTime * u_diffuseMap_animSpeed, float(u_diffuseMap_frameCount)));
    vec2 animatedUV = fragTexCoord;
    animatedUV.y /= u_diffuseMap_frameCount;

    animatedUV.y += (currentFrame / u_diffuseMap_frameCount);

    vec4 texColor = texture(u_diffuseMap, animatedUV);
    vec3 emission = emissiveColor * emissiveIntensity;
    finalColor = vec4(totalLight * texColor.rgb + emission, texColor.a);
}