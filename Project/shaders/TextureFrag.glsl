#version 330 core
// this is a pointer to the current 2D texture object
uniform sampler2D tex;

uniform vec2 scaleMod;

// the vertex UV
in vec2 vertUV;
// the final fragment colour
layout (location =0) out vec4 outColour;
void main ()
{
 // set the fragment colour to the current texture
 outColour = vec4(texture(tex,vertUV*scaleMod).rgb,1.0);
}
