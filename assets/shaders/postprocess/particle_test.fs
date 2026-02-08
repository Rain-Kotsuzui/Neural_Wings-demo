#version 330

in vec2 fragTexCoord;
out vec4 fragColor;

uniform sampler2D u_screen;
uniform sampler2D u_sparks;
uniform float u_1;
uniform float u_bloomIntensity;

void main() {
    vec4 sceneColor = texture(u_screen, fragTexCoord);
    vec4 sparkColor = texture(u_sparks, fragTexCoord);

    vec3 toneColor = sceneColor.rgb * u_1 + sparkColor.rgb * u_bloomIntensity;

    fragColor = vec4(toneColor, sceneColor.a);
}