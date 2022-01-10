#version 450 core

in vec2 fragTexCoord;

out vec4 color;

uniform sampler2D tex;

uniform float currentTime;
uniform float zoomRate;
uniform float border;
uniform float smoothRate;
uniform float fadeEdge;

void main(void)
{
    vec2 signs = min(vec2(1.0, 1.0), sign(fragTexCoord - 0.5) * 2.0 + 1.0);
	vec2 midCoords = abs(fragTexCoord - 0.5);

	//midCoords = (midCoords * 2.0) * (0.5 - mod(vec2(currentTime, currentTime) * zoomRate, border));
	

	
	//midCoords *= pow(2.0, mod(currentTime * zoomRate, 2.3)); // 0.4 border only
	//midCoords *= pow(2.0, currentTime * zoomRate);
	midCoords = midCoords * pow(2.0, mod(currentTime * zoomRate, 2.3) + 1.0); // 0.4 border only
	//midCoords = midCoords * pow(2.0, mix(1.0, 4.9245776533, mod(currentTime * zoomRate, 1.0)));
	//midCoords *= pow(10.0, mod(currentTime * zoomRate, pow(4.8, 0.1)));
	
	//midCoords *= pow(2.0, currentTime * zoomRate);
	
	//midCoords *= currentTime;

	float temp = 1.0 - border * 2.0;
    vec2 i = vec2(temp, temp);
    vec2 xVal = log(1.0 - 2 * (0.5 - midCoords)) / log(i);

	float minVal = min(xVal.x, xVal.y);
    xVal = vec2(minVal, minVal);

    vec4 color1 = texture(tex, midCoords / pow(i, floor(xVal)) + 0.5 * signs);
    vec4 color2 = texture(tex, midCoords / pow(i, floor(xVal - 1)) + 0.5 * signs);

	vec4 color3 = mix(color1, color2, clamp((smoothRate - fract(xVal.x)), 0.0, 1.0)); // Smooth transitional edges
	color3.w = clamp(1.0 - fadeEdge * (max(0.0, length(fragTexCoord - 0.5) - 0.4) * 10), 0.0, 1.0); // Smooth outter edge
	color = color3;
}