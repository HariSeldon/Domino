#version 330

uniform sampler2D texture;

in vec3 position;
in vec3 normal;
in vec2 textureCoordinates;

// Don't use functions glBindFragDataLocation and glGetFragDataLocation
// Rely on the default behaviour: output is associated to location 0.
out vec4 outputColor;

const int MAX_LIGHTS = 4;

struct MaterialInfo {
  vec4 emission;
  vec4 ambient;
  vec4 diffuse;
  vec4 specular;
  float shininess;
};
uniform MaterialInfo material;

struct LightInfo {
  // Light position is in camera space.
  // If the 4th coordinate is 0.0 then the light is directional.
  vec4 position;
  // Color.
  vec4 ambient;
  vec4 diffuse;
  vec4 specular;
  // Attenuations.
  float constantAttenuation;
  float linearAttenuation;
  float quadraticAttenuation;
  // Spot info.
  float spotExponent;
  float spotCosCutOff;
  float spotCutOff;
  vec3 spotDirection;
};
uniform LightInfo lights[MAX_LIGHTS];
uniform int lightsNumber;
uniform int lightMask;

uniform vec4 ambientColor;

const float DEFAULT_SPOT_CUTOFF = 180.f;

// -----------------------------------------------------------------------------
// Light-independent shading.
vec4 shadeAmbientColor() {
  vec4 emissionMaterialColor = material.emission;
  vec4 ambientMaterialColor = material.ambient;

  vec4 result = ambientColor * ambientMaterialColor + emissionMaterialColor;
  return result;
}

// -----------------------------------------------------------------------------
vec4 shadeDirectionalLight(vec3 position, vec3 normal, vec3 cameraDirection,
                           int lightIndex) {
  // Get material properties.
  vec4 ambientMaterialColor = material.ambient;
  vec4 diffuseMaterialColor = texture2D(texture, textureCoordinates);
  vec4 specularMaterialColor = material.specular;
  float shininess = material.shininess;

  // Get light properties.
  LightInfo light = lights[lightIndex];
  vec4 ambientLightColor = light.ambient;
  vec4 diffuseLightColor = light.diffuse;
  vec4 specularLightColor = light.specular;

  vec4 fragmentAmbientColor = ambientMaterialColor * ambientLightColor;
  vec4 finalColor = fragmentAmbientColor;

  vec3 lightDirection = -1.0f * normalize(light.position.xyz);
  vec3 halfDirection = normalize(lightDirection + cameraDirection);

  float cosTeta = max(dot(normal, lightDirection), 0.0);
  float cosH = max(dot(normal, halfDirection), 0.0);

  vec4 diffuseColor = diffuseMaterialColor * diffuseLightColor * cosTeta;
  vec4 specularColor =
      (cosTeta > 0.0)
          ? specularMaterialColor * specularLightColor * pow(cosH, shininess)
          : vec4(0.0, 0.0, 0.0, 1.0);

  finalColor = diffuseColor + specularColor;
  return finalColor;
}

// -----------------------------------------------------------------------------
vec4 shadePositionalLight(vec3 position, vec3 normal, vec3 cameraDirection,
                          int lightIndex) {
  // Get material properties.
  vec4 ambientMaterialColor = material.ambient;
  //vec4 diffuseMaterialColor = material.diffuse;
  vec4 diffuseMaterialColor = texture2D(texture, textureCoordinates);
  vec4 specularMaterialColor = material.specular;
  float shininess = material.shininess;

  // Get light properties.
  LightInfo light = lights[lightIndex];
  vec3 lightPosition = light.position.xyz;
  vec4 ambientLightColor = light.ambient;
  vec4 diffuseLightColor = light.diffuse;
  vec4 specularLightColor = light.specular;
  float constantAttenuation = light.constantAttenuation;
  float linearAttenuation = light.linearAttenuation;
  float quadraticAttenuation = light.quadraticAttenuation;
  float spotExponent = light.spotExponent;
  float spotCosCutOff = light.spotCosCutOff;
  float spotCutOff = light.spotCutOff;
  vec3 spotDirection = light.spotDirection;

  vec4 fragmentAmbientColor = ambientMaterialColor * ambientLightColor;
  vec4 finalColor = fragmentAmbientColor;

  vec3 lightDirection = normalize(lightPosition - position);
  float cosTeta = dot(normal, lightDirection);

  if (cosTeta > 0.0f) {
    // Half vector.
    vec3 halfDirection = normalize(lightDirection + cameraDirection);
    float cosH = max(dot(normal, halfDirection), 0.0);

    vec4 diffuseColor = diffuseMaterialColor * diffuseLightColor * cosTeta;
    vec4 specularColor =
        specularMaterialColor * specularLightColor * pow(cosH, shininess);

    // Get distance of point from light.
    float lightDistance = distance(position, lightPosition);

    // Compute attenuation.
    float attenuation =
        1.0f / (constantAttenuation + (linearAttenuation * lightDistance) +
                (quadraticAttenuation * lightDistance * lightDistance));

    if (spotCutOff < DEFAULT_SPOT_CUTOFF) {
      float spotCosine = dot(normalize(spotDirection), -1.0f * lightDirection);
      float spotAttenuation = (spotCosine > spotCosCutOff) ?
                                  // Inside the cone.
                                  pow(spotCosine, spotExponent) :
                                  // Outside the cone.
                                  0.0f;
      attenuation *= spotAttenuation;
    }

    finalColor += attenuation * (diffuseColor + specularColor);
  }

  return finalColor;
}

// -----------------------------------------------------------------------------
void main() {
  vec3 normalizedNormal = (2 * int(gl_FrontFacing) - 1) * normalize(normal);
// This is equivalent to: 
// if(gl_FrontFacing == false) -> normalizedNormal = -normalizedNormal; 

  vec3 cameraDirection = normalize(-1.0 * position);

  vec4 finalFragmentColor = shadeAmbientColor();

  for (int lightIndex = 0; lightIndex < lightsNumber; ++lightIndex) {
    if ( int((1 << lightIndex) & lightMask) == 0) {
      continue;
    } 

    LightInfo light = lights[lightIndex];

    // Directional light.
    if (light.position.w == 0.0f) {
      finalFragmentColor += shadeDirectionalLight(position, normalizedNormal,
                                                  cameraDirection, lightIndex);
    }
    // Positional light.
    else {
      finalFragmentColor += shadePositionalLight(position, normalizedNormal,
                                                 cameraDirection, lightIndex);
    }
  }

  outputColor = finalFragmentColor;
}
