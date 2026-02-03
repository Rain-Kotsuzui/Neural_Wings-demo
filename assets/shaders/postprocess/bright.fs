#version 330

in vec2 fragTexCoord;
out vec4 finalColor;

uniform sampler2D u_screen;

uniform float u_threshold;

void main() {
    vec4 color = texture(u_screen, fragTexCoord);
    float brightness = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > u_threshold) {
        finalColor = color;
    } else {
        finalColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
}