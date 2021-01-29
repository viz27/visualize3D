#version 130
in vec4 vColor;
in vec3 FragPos;
in vec3 Normal;
out vec4 FragColor;

void main()
{
    vec3 lightColor = vec3(1.0, 1.0, 1.0);
    vec3 lightPos = vec3(0.0, 0.0, 3.0);
    //vec3 lightPos = vec3(3.0, 0.0, 2.0);

    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColor;

    vec3 finnormal = Normal;
    vec3 norm = normalize(finnormal);
    vec3 lightDir = normalize(lightPos - FragPos);
    
    float diff = dot(norm, lightDir);
    if (diff < 0)
    {
        diff = -diff;
    }
    
    vec3 diffuse = diff * lightColor;
    
    FragColor = vec4((ambient+diffuse), 1.0) * vColor;
    //FragColor = vec4(FragPos, 1.0);
}
