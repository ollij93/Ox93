#pragma once

#ifndef OX93_SPECIFICATION_HASH_IDS_H__
#define OX93_SPECIFICATION_HASH_IDS_H__

#include "Types/Ox93_Hash.h"

///////////////////////////////////////////////
// P H Y S I C A L   O B J E C T   H A S H E S
///////////////////////////////////////////////
static const Ox93_Hash uOX93_SPEC_NAME_BLOCK = 0x03D4D46D;  // "Block"
static const Ox93_Hash uOX93_SPEC_NAME_PLAYER = 0x8EA38701; // "Player"

static const Ox93_Hash uOX93_SPEC_PARAM_MODEL   = 0x04710B09; // "Model"
static const Ox93_Hash uOX93_SPEC_PARAM_TEXTURE = 0x0EB66CFB; // "Texture"
static const Ox93_Hash uOX93_SPEC_PARAM_RADIUS  = 0x917358B2; // "Radius"
static const Ox93_Hash uOX93_SPEC_PARAM_OBJECT  = 0x8C658F5F; // "Object"

// Skybox...
static const Ox93_Hash uOX93_SPEC_NAME_SKYBOX = 0x93BE8D2A; // "Skybox"
static const Ox93_Hash uOX93_SPEC_PARAM_VERT_FACES = 0xEBEF07C0; // "VerticalFaces"
static const Ox93_Hash uOX93_SPEC_PARAM_HOR_FACES  = 0x02EB27D2; // "HorizontalFaces"

// Characters...
static const Ox93_Hash uOX93_SPEC_PARAM_HEIGHT = 0x809D7D47; // "Height"

///////////////////////////////////////////////
// T E X T U R E S   H A S H E S 
///////////////////////////////////////////////
static const Ox93_Hash uOX93_TEXTURE_HASH_DEFAULT = 0xB17D21AB; // "Textures/DEFAULT_TEXTURE.dds"
static const Ox93_Hash uOX93_TEXTURE_HASH_SKYBOX = 0xE3CC3CC6; // "Textures/Skybox.dds"

///////////////////////////////////////////////
// M O D E L   H A S H E S 
///////////////////////////////////////////////

static const Ox93_Hash uOX93_MODEL_HASH_CUBE = 0x255AF5A3; // "Models/CUBE.obj"
static const Ox93_Hash uOX93_MODEL_HASH_RANDOM = 0xCDB02EF5; // "Models/randomStuff.obj"


#endif // OX93_SPECIFICATION_HASH_IDS_H__
