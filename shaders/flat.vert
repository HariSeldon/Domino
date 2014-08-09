//#version 330

//flat varying out vec4 color;
varying out vec4 color;
uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;
uniform mat3 normalMatrix;

struct MaterialInfo {
  vec4 emission;
  vec4 ambient; 
  vec4 diffuse;
  vec4 specular;
  float shininess;
};
uniform MaterialInfo material;

struct LightInfo {
  vec4 position; // Light position in camera space.
  vec4 ambient;
  vec4 diffuse;
  vec4 specular;
};
uniform LightInfo light;

uniform vec4 ambientColor;

vec4 shade(vec3 position, vec3 normal, vec3 lightDirection,
           vec3 cameraDirection) {

  // Get material properties.
  vec4 emissionMaterialColor = material.emission;
  vec4 ambientMaterialColor = material.ambient;
  vec4 diffuseMaterialColor = material.diffuse;
  vec4 specularMaterialColor = material.specular;
  float shininess = material.shininess;

  // Get light properties.
  vec4 ambientLightColor = light.ambient;
  vec4 diffuseLightColor = light.diffuse;
  vec4 specularLightColor = light.specular;

  // Get model parameters.
  vec4 ambientModelColor = ambientColor;

  // Half vector.
  vec3 halfDirection = normalize(lightDirection + cameraDirection);

  float cosTeta = max(dot(normal, lightDirection), 0.0);
  float cosH = max(dot(normal, halfDirection), 0.0);

  vec4 ambientVertexColor = ambientMaterialColor * ambientLightColor + ambientModelColor;
  vec4 diffuseColor = diffuseMaterialColor * diffuseLightColor * cosTeta;
  vec4 specularColor =
      (cosTeta > 0.0)
          ? specularMaterialColor * specularLightColor * pow(cosH, shininess)
          : vec4(0.0, 0.0, 0.0, 1.0);

  vec4 finalColor =
      emissionMaterialColor + ambientVertexColor + diffuseColor + specularColor;
  return finalColor;
}

void main() {
  // Position.
  vec4 position = modelViewMatrix * gl_Vertex;
  vec3 worldPosition = position.xyz;
  gl_Position = projectionMatrix * position;

  // Normal.
  vec3 normal = normalize(normalMatrix * gl_Normal);

  // Light position: already in camera space.
  vec3 lightPosition = light.position.xyz; 
  vec3 lightDirection = normalize(lightPosition - worldPosition);

  // Camera position and direction.
  vec3 cameraDirection = normalize(-1.0 * worldPosition);

  // Set the final color;
  color = shade(worldPosition, normal, lightDirection, cameraDirection);
}
