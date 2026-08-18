#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D uTexture;

// Blinn-Phong lighting parameters (match CPU path exactly)
uniform vec3 viewPos;
uniform vec3 lightPositions[2]  = vec3[2](vec3(20.0, 20.0, 20.0), vec3(-20.0, 20.0, 0.0));
uniform vec3 lightIntensities[2] = vec3[2](vec3(500.0, 500.0, 500.0), vec3(500.0, 500.0, 500.0));
uniform vec3 ambientLight       = vec3(10.0, 10.0, 10.0);
uniform float specularPower     = 150.0;
uniform vec3 ka                 = vec3(0.005, 0.005, 0.005);
uniform vec3 ks                 = vec3(0.7937, 0.7937, 0.7937);

void main()
{
    // Sample texture; kd is texture color normalized to [0,1]
    vec3 textureColor = texture(uTexture, TexCoord).rgb;
    vec3 kd = textureColor / 255.0;

    vec3 point = FragPos;
    vec3 n = normalize(Normal);
    vec3 v = normalize(viewPos - point);

    // Ambient
    vec3 ambient = ka * ambientLight;

    // Accumulate diffuse + specular for each light
    vec3 result = vec3(0.0);
    for (int i = 0; i < 2; i++)
    {
        vec3 l = normalize(lightPositions[i] - point);
        vec3 h = normalize(l + v);
        float r2 = dot(lightPositions[i] - point, lightPositions[i] - point);

        vec3 diffuse  = (kd * lightIntensities[i] / r2) * max(0.0, dot(n, l));
        vec3 specular = (ks * lightIntensities[i] / r2) * pow(max(0.0, dot(n, h)), specularPower);
        result += diffuse + specular;
    }

    result += ambient;
    FragColor = vec4(result, 1.0);
}
