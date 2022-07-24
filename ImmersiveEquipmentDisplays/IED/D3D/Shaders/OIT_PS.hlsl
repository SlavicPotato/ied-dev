//-----------------------------------------------------------------------------
// File: OITPS.hlsl
//
// Desc: Pixel shaders used in the Order Independent Transparency sample.
// 
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License (MIT).
//-----------------------------------------------------------------------------
//TODO: Use structured buffers
RWTexture2D<uint> fragmentCount     : register( u1 );
RWBuffer<float>   deepBufferDepth   : register( u2 );
RWBuffer<uint4>   deepBufferColor   : register( u3 );
RWBuffer<uint>    prefixSum         : register( u4 );

#include "Structures.fxh"

cbuffer CB : register( b0 )
{
    uint g_nFrameWidth      : packoffset( c0.x );
    uint g_nFrameHeight     : packoffset( c0.y );
    uint g_nReserved0       : packoffset( c0.z );
    uint g_nReserved1       : packoffset( c0.w );
}

void FragmentCountPS(VSOutput input)
{
    // Increments need to be done atomically
	InterlockedAdd(fragmentCount[input.PositionPS.xy], 1);
}

void FillDeepBufferPS(VSOutput input)
{
	uint x = input.PositionPS.x;
	uint y = input.PositionPS.y;

    // Atomically allocate space in the deep buffer
    uint fc;
	InterlockedAdd(fragmentCount[input.PositionPS.xy], 1, fc);

    uint nPrefixSumPos = y*g_nFrameWidth + x;
    uint nDeepBufferPos;
    if( nPrefixSumPos == 0 )
        nDeepBufferPos = fc;
    else
        nDeepBufferPos = prefixSum[nPrefixSumPos-1] + fc;

    // Store fragment data into the allocated space
	deepBufferDepth[nDeepBufferPos] = input.PositionPS.z;
	deepBufferColor[nDeepBufferPos] = clamp(input.Diffuse, 0, 1) * 255;
}

