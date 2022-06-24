#version 430
//mtl data
struct MaterialInfo{
	vec3 Ka;
	vec3 Kd;
	vec3 Ks;
};
uniform MaterialInfo Material;

//lighting color, a uniform
uniform vec4 ambientColor;
uniform vec4 diffuseColor;
uniform vec4 specularColor;
//for material specular, also called specular strength
uniform float Shininess;


//uniform samplerCube shadowMap;
uniform sampler2D shadowMap;

//attenuation constants
uniform float Kc;
uniform float Kl;
uniform float Kq;

//light box coordinate
uniform vec3 vLightPosition;

//input output
out vec4 vFragColor;
in vec3 inverseTransposeNormal;
in vec2 UV;
in vec3 viewDir;
in vec3 halfwayDir;
in vec4 lightSpacePosition;
in vec3 fragPos;
in vec3 lightDir;

float ShadowCalculation(vec4 lightSpacePosition, vec3 lightDir){
    //normalize with alpha, to [-1,1]
    vec3 normLSPosition = lightSpacePosition.xyz / lightSpacePosition.w;

    //depth map is in range[0,1], since this is the coordinate we use for texture
    normLSPosition = normLSPosition * 0.5 + 0.5; 

    //sample mapping from the texture we have made, call the .r to find the depth that is closest to light 
    //(in other words, displayed, not shadw)
    float closestDepth = texture(shadowMap, normLSPosition.xy).r;  
    
    //compare with current depth
    float currentDepth = normLSPosition.z; 

    //fix shadow acne,caused by light coming from different angle, which is not parallel with shadow texture
    //simply offset the depth of the surface (or the shadow map) by a small bias amount
    //such that the fragments are not incorrectly considered above the surface.
    //we will try fix exploding bias with peter panning
    float bias = max(0.05 * (1.0 - dot(inverseTransposeNormal, lightDir)), 0.005);

    //partially fix the jagged edges by PCF(partially close filtering)
    //produce softer shadows by sample more than once from the depth map, 3x3 size, and average the result based on the 9 coordinates
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, normLSPosition.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // this is to fix the oversampling, when the particle is more than the border capacity, just make it no shadow
    if(normLSPosition.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

void main(void)
{ 
    //attenuation for point lighting
    float lightDistance = length(lightDir);
    float attenuation = 1.0 / (Kc + Kl * lightDistance + Kq * (lightDistance * lightDistance)); 


    //shadowing
    float shadow = ShadowCalculation(lightSpacePosition, lightDir);


    // diffuse intensity
    float diff = max(0.0, dot(normalize(inverseTransposeNormal), normalize(lightDir)));

    //diff*diffuse results in difuse color, and Kd is the objectColor
    vFragColor = (1.0 - shadow)*attenuation*diffuseColor*(diff*vec4(Material.Kd,1.0));
    
    // Add in ambient light, note that shadow doesn't affect ambient
    vFragColor += (1.0)*attenuation*ambientColor*vec4(Material.Ka,1);
   

    // Specular Light, for shininess
    float spec = max(0.0, dot(normalize(inverseTransposeNormal), halfwayDir));
    if(diff != 0) {
		spec = pow(spec, Shininess);
		vFragColor += (1.0 - shadow)*attenuation*specularColor*(spec*vec4(Material.Ks,1));
    }
    //ofcourse, if diffuse is 0, means no diffuse light present, don't  create any specular shininess
    
}
	
    