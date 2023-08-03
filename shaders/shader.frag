#version 460 core

out vec4 FragColor;

in vec3 meshColor;
in vec3 pos;
in vec3 norm;

uniform bool wireframe;
uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform float shininess;

void main() {
  // Calcola il vettore che punta verso la luce
  vec3 lightDirection = normalize(lightPosition - pos);

  // Calcola la componente diffusa dell'illuminazione
  float diffuseStrength = max(dot(norm, lightDirection), 0.0);
  vec3 diffuse = diffuseStrength * lightColor * meshColor;

  // Calcola la componente speculare dell'illuminazione
  vec3 viewDirection = normalize(-pos); // Direzione di vista dalla telecamera
  vec3 reflectDirection = reflect(-lightDirection, norm);
  float specularStrength = pow(max(dot(viewDirection, reflectDirection), 0.0), shininess);
  vec3 specular = specularStrength * lightColor;

  // Calcola l'illuminazione ambiente
  vec3 ambient = 0.1 * lightColor; // Luce ambiente debolmente illuminata

  // Calcola il colore finale del frammento sommando tutte le componenti dell'illuminazione
  vec3 finalColor = ambient + diffuse + specular;

  // Se è in modalità wireframe, usa un colore di wireframe, altrimenti usa il colore calcolato
    FragColor = wireframe ? vec4(0.0f, 0.0f, 0.0f, 0.2f) : vec4(finalColor, 1.0);
}
