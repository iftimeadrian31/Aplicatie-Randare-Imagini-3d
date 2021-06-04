#version 400
out vec4 fragColor;

in vec3 normal;
in vec3 pos;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 culoare;
uniform vec3 ambient;
uniform vec3 specular;

vec3 lighting(vec3 objectColor, vec3 pos, vec3 normal, vec3 lightPos, vec3 viewPos,
				vec3 ambient, vec3 lightColor, vec3 specular, float specPower)
{
	vec3 L=normalize(lightPos-pos);
	vec3 N=normalize(normal);
	vec3 V=normalize(viewPos-pos);
	vec3 R=reflect(-L,N);


	float diffCoef = max(0,dot(L,N));
	float specCoef = pow(max(0,dot(R,V)),specPower);



	vec3 ambientColor = ambient * lightColor;
	vec3 diffuseColor = diffCoef * lightColor;
	vec3 specularColor = specCoef * specular * lightColor;
	vec3 col = ( ambientColor + diffuseColor + specularColor ) * objectColor; 

	return clamp(col, 0, 1);
}


void main() 
{

	vec3 objectColor = culoare;
	vec3 lightColor = vec3(1.0, 1.0, 1.0);
	vec3 ambient = vec3(0.1);
	vec3 specular = vec3(0.7);
	float specPower = 32;
	
	vec3 color = lighting(objectColor, pos, normal, lightPos, viewPos, ambient, lightColor, specular, specPower);
		
	fragColor = vec4(color, 1.0);
}
