#version 410 core
in vec2 texCoord;
uniform sampler2D theTexture;
uniform float offsetX;
out vec4 color;
void main () {
   vec4 texel = texture(theTexture, vec2(texCoord.x + offsetX, texCoord.y));
   color = texel;
}