#version 400 core

// Structure for matrices
uniform struct Matrices
{
	mat4 projMatrix;
	mat4 modelViewMatrix; 
	mat3 normalMatrix;
} matrices;

// Structure holding light information:  its position as well as ambient, diffuse, and specular colours
struct LightInfo
{
	vec4 position;
	vec3 La;
	vec3 Ld;
	vec3 Ls;
	vec3 direction;
	float exponent;
	float cutoff;
};

// Structure holding material information:  its ambient, diffuse, and specular colours, and shininess
struct MaterialInfo
{
	vec3 Ma;
	vec3 Md;
	vec3 Ms;
	float shininess;
};

in vec3 vertColour;			// Interpolated colour using colour calculated in the vertex shader
in vec2 vTexCoord;			// Interpolated texture coordinate using texture coordinate from the vertex shader

out vec4 vOutputColour;		// The output colour

uniform sampler2D sampler0;  // The texture sampler
uniform sampler2D sampler1;

uniform MaterialInfo material1;

uniform samplerCube CubeMapTex;
uniform bool bUseTexture;    // A flag indicating if texture-mapping should be applied
uniform bool bUseMultiTexture;
uniform bool renderSkybox;
uniform bool useToon;

in vec3 worldPosition;

in vec4 p;
in vec3 n;

uniform bool isNight;

uniform float t;

uniform LightInfo light[2];
uniform LightInfo light1;

float fIntensity;
float fDiffuseIntensity;
float fSpecularIntensity;


// Reducing the RGB values depending on the intensity amount
void toonColour()
{
	if(fIntensity < 0.5)
	{
		vOutputColour *= 0.25f;
	}
	else if(fIntensity >= 0.5f && fIntensity <= 0.75)
	{
		vOutputColour *= 0.5f;
	}
	else
	{
		vOutputColour *= 0.75f;
	}

}

// This function implements the Phong shading model
// The code is based on the OpenGL 4.0 Shading Language Cookbook, Chapter 2, pp. 62 - 63, with a few tweaks. 
// Please see Chapter 2 of the book for a detailed discussion.
vec3 PhongModel(vec4 eyePosition, vec3 eyeNorm)
{
	vec3 s = normalize(vec3(light1.position - eyePosition));
	vec3 v = normalize(-eyePosition.xyz);
	vec3 r = reflect(-s, eyeNorm);
	vec3 n = eyeNorm;
	vec3 ambient = light1.La * material1.Ma;
	fDiffuseIntensity = max(dot(s, n), 0.0f);
	vec3 diffuse = light1.Ld * material1.Md * fDiffuseIntensity;
	vec3 specular = vec3(0.0f);
	float eps = 0.000001f; // add eps to shininess below -- pow not defined if second argument is 0 (as described in GLSL documentation)
	if (fDiffuseIntensity > 0.0f)
	{
		fSpecularIntensity = pow(max(dot(r, v), 0.0f), material1.shininess + eps);
		specular = light1.Ls * material1.Ms * fSpecularIntensity;
	}
	

	return ambient + diffuse + specular;

}

// This function implements the Blinn Phong Spotlight model
vec3 BlinnPhongSpotlightModel(LightInfo light, vec4 p, vec3 n)
{
	vec3 s = normalize(vec3(light.position - p));
	float angle = acos(dot(-s, light.direction));
	float cutoff = radians(clamp(light.cutoff, 0.0, 90.0));
	vec3 ambient = light.La * material1.Ma;
	if (angle < cutoff) 
	{
		float spotFactor = pow(dot(-s, light.direction), light.exponent);
		vec3 v = normalize(-p.xyz);
		vec3 h = normalize(v + s);
		fDiffuseIntensity = max(dot(s, n), 0.0);
		vec3 diffuse = light.Ld * material1.Md * fDiffuseIntensity;
		vec3 specular = vec3(0.0);
		if (fDiffuseIntensity > 0.0)
		{
			fSpecularIntensity = pow(max(dot(h, n), 0.0), material1.shininess);
			specular = light.Ls * material1.Ms * fSpecularIntensity;
			return ambient + spotFactor * (diffuse + specular);
		}
	} 
	else
	return ambient;
}



void main()
{

	vec3 vColour;

	if (renderSkybox) {
		vOutputColour = texture(CubeMapTex, worldPosition);

	} else 
	{

		// Get the texel colour from the texture sampler

		vec4 vTexColour;
		
		// If multi-texture is used, multiply the textures
		if (bUseMultiTexture)
		{
			vec4 TexColour0 = texture(sampler0, vTexCoord);
			vec4 TexColour1 = texture(sampler1, vTexCoord);

			vTexColour = texture(sampler0, vTexCoord) * texture(sampler1, vTexCoord);
		}
		else
		{
			vTexColour = texture(sampler0, vTexCoord);
		}
		

		if (bUseTexture)
		{
			// Use the directional light if day mode
			if(isNight == false)
			{
				vColour = PhongModel(p, n);
			}
			// Else use the spotlights in night mode
			else
			{
				for(int i = 0; i < 2; i++)
				{
				// Add the colours if the objects are under both the spotlights
					vColour += BlinnPhongSpotlightModel(light[i], p, n);
				}
			}

			vOutputColour = vTexColour*vec4(vColour, 1.0f);	// Combine object colour and texture

			fIntensity = fDiffuseIntensity + fSpecularIntensity;

			// Apply toon shader if enabled
			if(useToon)
			toonColour();
			
		}
		else
			vOutputColour = vec4(vColour, 1.0f);	// Just use the colour instead
	}
	
	
}
