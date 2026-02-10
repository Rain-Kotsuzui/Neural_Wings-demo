#version 330
in vec2 fragTexCoord;
out vec4 finalColor;

uniform sampler2D u_debug;
uniform sampler2D u_screen;

void main() {
    // vec4 centerColor = texture(rawScreen, fragTexCoord);
    vec4 screenColor = texture(u_screen, fragTexCoord);
    vec4 debugColor = texture(u_debug, fragTexCoord);
    finalColor = vec4(screenColor.rgb + debugColor.rgb, 1.0);
}