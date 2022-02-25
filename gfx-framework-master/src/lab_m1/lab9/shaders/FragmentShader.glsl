#version 330

// Input
in vec2 texcoord;

// Uniform properties
uniform sampler2D texture_1;
uniform sampler2D texture_2;

// TODO(student): Declare various other uniforms
uniform int use_texture_2;

// Output
layout(location = 0) out vec4 out_color;

float myLerp(float a, float b, float t)
{
    // Linear interpolation
    return (1-t) * a + t * b;
}

void main()
{
    // TODO(student): Calculate the out_color using the texture2D() function.
    vec4 color1 = texture2D(texture_1, texcoord);
    
    if (color1.a < 0.8) {
        discard;
    }

   vec4 color2 = texture2D(texture_2, texcoord);

   if (use_texture_2 == 1) {
        out_color = mix(color1, color2, 0.5);
   } else {
        out_color = color1;
   }

}
