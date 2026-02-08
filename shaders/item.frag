#version 430 core

in vec3 frag_color;
in vec3 frag_pos;
in vec3 frag_normal;
in vec2 frag_uv;

out vec4 FragColor;

uniform sampler2D tex;
uniform sampler2D tex_spec;

uniform vec3 viewPos; // Camera position in world space
// Lights
// Ambient
vec3 ambientLight = vec3(0.3f);

// Directional
vec3 directionalLight = normalize(vec3(1.0, -1.0, 1.0)); // Direction where light rays are travelling
vec3 directionalLightColor = vec3(1.0, 1.0, 1.0);

// Point
uniform int activePointLights = 0;
uniform vec3 pointLightPosition[10];
uniform vec3 pointLightColor[10];
uniform float pointLightStrength[10];

// Spotlight
vec3 spotlightColor = vec3(1.0, 1.0, 1.0);
vec3 spotlightPosition = vec3(2.0, 0.0, 0.0);
vec3 spotlightDirection = normalize(vec3(-1.0, -1.0, 0.0));
float spotlightCutoff = radians(30.0);
float spotlightSofteningAngle = radians(40.0);
float spotlightStrengtheningFactor = 2.0;

float CA = 1.0;
float LA = 0.09;
float QA = 0.032;

void main() {
    vec3 normal = normalize(frag_normal);

    // Directional Light
    vec3 directionalLightContri = max(0.0, dot(-directionalLight, normal)) * directionalLightColor;

    vec3 netPointLightContri = vec3(0.0);
    vec3 netSpecularContri = vec3(0.0);
    vec3 albedo = texture(tex, frag_uv).xyz;

    // For all light sources (for now point light sources)
    for(int i = 0;i < activePointLights;i += 1) {
        // Diffuse
        vec3 pointLightDir = pointLightPosition[i] - frag_pos;
        float pointLightDistance = length(pointLightDir);
        pointLightDir = normalize(pointLightDir);
        float attenuation = 1 / (CA + pointLightDistance * LA + pointLightDistance * pointLightDistance * QA);

        netPointLightContri += max(dot(pointLightDir, normal), 0.0) * attenuation * pointLightStrength[i] * pointLightColor[i];

        // Specular
        float specFactor = pow(max(dot(normalize(viewPos - frag_pos), reflect(-pointLightDir, normal)), 0.0), 32.0);
        vec3 specular = pointLightColor[i] * specFactor * attenuation * texture(tex_spec, frag_uv).xyz;

        netSpecularContri += specular;
    }

    // Spotlight calculation
    vec3 spotlightColorContri = vec3(0.0);
    vec3 lightToFrag = frag_pos - spotlightPosition;
    vec3 lightToFragDir = normalize(lightToFrag);
    
    float lightToFragDis = length(lightToFrag);
    float theta = dot(spotlightDirection, lightToFragDir);
    float isFacingSpotlight = max(dot(normal, -lightToFragDir), 0.0);
    
    float innerCutoff = cos(spotlightCutoff); // spotlightCutoff
    float outerCutoff = cos(spotlightSofteningAngle); // spotlightCutoff + some softening angle

    float attenuation = 1 / (CA + lightToFragDis * LA + lightToFragDis * lightToFragDis * QA);
    float intensity = smoothstep(outerCutoff, innerCutoff, theta); 

    if(theta > cos(spotlightCutoff)) {
        spotlightColorContri += spotlightColor * intensity * attenuation * isFacingSpotlight * spotlightStrengtheningFactor;
    }

    vec3 lighting = albedo * ( directionalLightContri + netPointLightContri + ambientLight + spotlightColorContri) + netSpecularContri;

    FragColor = vec4(lighting, 1.0);
}