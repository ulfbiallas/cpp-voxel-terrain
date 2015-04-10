#ifndef SHADERCODE_H
#define SHADERCODE_H

#define ShaderCode(version, code)  "#version " #version "\n" #code



static const char* vertexShaderSrc = ShaderCode(140,
	varying vec4 position;
	varying vec3 normal;
	varying vec3 transformedNormal;

	void main() {
		position = gl_Vertex;
		normal = gl_Normal;
		gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
		transformedNormal = normalize(gl_NormalMatrix * gl_Normal);
	}
);



static const char* fragmentShaderSrc = ShaderCode(140,
	uniform sampler2D texture0;
	uniform sampler2D texture1;
	varying vec4 position;
	varying vec3 normal;
	varying vec3 transformedNormal;

	vec4 triplanarMapping(sampler2D texture, vec4 position, vec3 normal) {
		vec3 n = abs(normal);
		vec4 colorX = texture2D(texture, position.yz);
		vec4 colorY = texture2D(texture, position.xz);
		vec4 colorZ = texture2D(texture, position.xy);
		return colorX * n.x + colorY * n.y + colorZ * n.z;
	}

	void main(void) {
		vec4 diffuse;
		vec4 colorGrass = triplanarMapping(texture0, position, normal);
		vec4 colorRock = triplanarMapping(texture1, position, normal);

		if(normal.y > 0.85) {
			diffuse = colorGrass;
		} else if (normal.y >= 0.7 && normal.y <= 0.85){
			float y = normal.y - 0.7;
			y /= 0.15;
			diffuse = colorGrass * y + colorRock * (1.0-y);
		} else {
			diffuse = colorRock;
		}
		
		int numLightSources = 1;
		vec4 finalColor = diffuse * 0.2;

		for(int i = 0; i < numLightSources; i++) {
			vec3 lightDir = normalize(vec3(gl_LightSource[i].position));
			float  NdotL = max(dot(transformedNormal, lightDir), 0.0);
			vec4 diffuseC = diffuse * gl_LightSource[i].diffuse * NdotL;
			finalColor += diffuseC;
		}	  

		gl_FragColor = finalColor;
	}
);



#endif