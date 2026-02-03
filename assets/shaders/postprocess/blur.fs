#version 330

in vec2 fragTexCoord;
out vec4 finalColor;

// 引擎内置参数
uniform float gameTime;
uniform float realTime;
uniform float deltaRealTime;
uniform float deltaGameTime;
uniform sampler2D screenTexture;
uniform vec2 screenResolution;

// 自定义参数
uniform sampler2D u_screen;
uniform vec2 u_direction;
uniform float u_radius;

const float weight[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);
// TODO:BUGGGGGGGGGGGGGGGGGGGGGGG!!
void main() {
    vec2 tex_offset = 1.0 / screenResolution;
    vec3 result = texture(screenTexture, fragTexCoord).rgb * weight[0];

    for(int i = 1; i < 5; i++) {
        vec2 offset = u_direction * tex_offset * u_radius * float(i);

        result += texture(u_screen, fragTexCoord + offset).rgb * weight[i];
        result += texture(u_screen, fragTexCoord - offset).rgb * weight[i];
    }
    finalColor = vec4(result, 1.0);
}