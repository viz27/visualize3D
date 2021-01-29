#version 130
in vec3 Position;
in vec3 aNormal;
in vec4 Color;

uniform mat4 gWorld;
uniform mat4 gView;
uniform mat4 gProj;

out vec4 vColor;
out vec3 FragPos;  
out vec3 Normal;

void main()
{
    vec4 pos1 = gWorld * vec4(Position, 1.0);
    gl_Position = gProj * gView * gWorld * vec4(Position, 1.0);
    //gl_Position = gWorld * vec4(Position, 1.0);
    vColor = Color+vec4(-pos1.z, 0.0, 0.0, 0.0);
    FragPos = vec3(gWorld * vec4(Position, 1.0));
    Normal = vec3(gWorld * vec4(aNormal, 0.0));
}
