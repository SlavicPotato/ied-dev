
cbuffer Parameters : register(b0)
{
	float4 DiffuseColor : packoffset(c0);
	float3 EmissiveColor : packoffset(c1);
	float3 SpecularColor : packoffset(c2);
	float  SpecularPower : packoffset(c2.w);

	float3 LightDirection[3] : packoffset(c3);
	float3 LightDiffuseColor[3] : packoffset(c6);
	float3 LightSpecularColor[3] : packoffset(c9);

	float3 EyePosition : packoffset(c12);

	float3 FogColor : packoffset(c13);
	float4 FogVector : packoffset(c14);

	float4x4 World : packoffset(c15);
	float3x3 WorldInverseTranspose : packoffset(c19);
	float4x4 WorldViewProj : packoffset(c22);
};

#include "Structures.fxh"
#include "Common.fxh"
#include "Lighting.fxh"
#include "Utilities.fxh"

// Vertex shader: vertex color.
VSOutput VSBasicVc(VSInputVc vin)
{
	VSOutput vout;

	CommonVSOutput cout = ComputeCommonVSOutput(vin.Position);
	SetCommonVSOutputParams;

	vout.Diffuse *= vin.Color;

	return vout;
}

// Vertex shader: vertex color, no fog.
VSOutputNoFog VSBasicVcNoFog(VSInputVc vin)
{
	VSOutputNoFog vout;

	CommonVSOutput cout = ComputeCommonVSOutput(vin.Position);
	SetCommonVSOutputParamsNoFog;

	vout.Diffuse *= vin.Color;

	return vout;
}

// Vertex shader: vertex lighting + vertex color.
VSOutput VSBasicVertexLightingVc(VSInputNmVc vin)
{
	VSOutput vout;

	CommonVSOutput cout = ComputeCommonVSOutputWithLighting(vin.Position, vin.Normal, 3);
	SetCommonVSOutputParams;

	vout.Diffuse *= vin.Color;

	return vout;
}

// Pixel shader: basic.
float4 PSBasic(PSInput pin) : SV_Target0
{
	float4 color = pin.Diffuse;

	ApplyFog(color, pin.Specular.w);

	return color;
}

// Pixel shader: no fog.
float4 PSBasicNoFog(PSInputNoFog pin) : SV_Target0
{
	return pin.Diffuse;
}

// Pixel shader: vertex lighting.
float4 PSBasicVertexLighting(PSInput pin) : SV_Target0
{
    float4 color = pin.Diffuse;

    AddSpecular(color, pin.Specular.rgb);
    ApplyFog(color, pin.Specular.w);

    return color;
}
