#version 130
in vec3 Position;

out vec4 vColor;
void main()
{
    gl_Position = vec4(Position, 1.0);
    vColor = vec4(1, 1, 1, 1);
}
