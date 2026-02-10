#version 330

in vec2 fragTexCoord;
in vec4 fragColor;
in float fragLifeRatio;
in vec3 vViewPos;
in vec3 vPosition;
flat in uint vID;
in float vRemainingLife;
in vec3 vDir;
in float vRadius;
in float vNear;
in float vFar;

out vec4 finalColor;

uniform sampler2D dataTex;
uniform int maxParticles;
vec4 GetPos(int id) {
    return texelFetch(dataTex, ivec2(0, id), 0);
}
vec3 GetVel(int id) {
    return texelFetch(dataTex, ivec2(1, id), 0).xyz;
}
vec4 GetAcc(int id) {
    return texelFetch(dataTex, ivec2(2, id), 0);
}
vec4 GetColor(int id) {
    return texelFetch(dataTex, ivec2(3, id), 0);
}
vec4 GetSize(int id) {
    return texelFetch(dataTex, ivec2(4, id), 0);
}
vec4 GetLife(int id) {
    return texelFetch(dataTex, ivec2(5, id), 0);
}

uniform sampler2D sceneDepth;
uniform sampler2D tex;

// 自定义

void main() {
    // 务必让fragTexCoord参与结果运算，否则内存访问会出错
    // float depth = texture(sceneDepth, fragTexCoord).r;

    vec2 normalCoords = fragTexCoord * 2.0 - 1;
    float r2 = dot(normalCoords, normalCoords);
    if(r2 > 1.0)
        discard;
    float z = sqrt(1.0 - r2);
    float thickness = 2.0 * vRadius * z;
    vec3 view = vPosition - vViewPos;
    float depth = length(view);
    depth = depth - z * vRadius;
    float far = vFar;
    float near = vNear;
    float nonLinearDepth = ((far + near) / (far - near)) + ((2.0 * far * near) / (far - near) / -depth);
    gl_FragDepth = nonLinearDepth * 0.5 + 0.5;
    // float t = 1.0;
    // t = length(GetVel(int(vID)));
    // t = clamp(t / 10, 0, 1);
    // vec4 texColor = texture(tex, fragTexCoord);
    // finalColor = texColor * (1 - t) + vec4(1, 0, 0, 1) * (t);
    finalColor = vec4(vec3(thickness), 1.0);
}
