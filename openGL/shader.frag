#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture_one;
uniform sampler2D texture_two;

void main()
{
  FragColor = mix(texture(texture_one, TexCoord), texture(texture_two, TexCoord), 0.2);
}
