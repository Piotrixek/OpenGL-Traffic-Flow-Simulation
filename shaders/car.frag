#version 330 core
in vec2 TexCoords;
out vec4 FragColor;
uniform sampler2D texSampler;
uniform bool exploding;
uniform float explosionProgress;
void main(){
    vec4 texColor = texture(texSampler, TexCoords);
    if(exploding){
        // Blend from the texture color to an explosion color (orange) as explosion progresses.
        vec4 explosionColor = mix(texColor, vec4(1.0, 0.5, 0.0, 0.0), explosionProgress);
        FragColor = explosionColor;
    } else {
        FragColor = texColor;
    }
}
