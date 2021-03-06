/******************************************************************************
* Copyright 2018 NVIDIA Corporation. All rights reserved.
*****************************************************************************/

// This file contains the implementations and the vtable of the texture access functions.
// 包含了用户自定的纹理查找函数的实现和指针的文件
#pragma once
#ifndef TEXTURE_SUPPORT_H
#define TEXTURE_SUPPORT_H

#define USE_SMOOTHERSTEP_FILTER

#include <mi/mdl_sdk.h>

typedef mi::neuraylib::Texture_handler_base Texture_handler_base;
typedef mi::neuraylib::Tex_wrap_mode Tex_wrap_mode;

// Custom structure representing an MDL texture
// 定义Texture结构体表现一个MDL材质
struct mdlTexture
{
	mdlTexture(mi::base::Handle<const mi::neuraylib::ICanvas> c)//传入一个canvas
		: canvas(c)
		, ncomp(4)
	{
		// for now, we only support floating point rgba
		if (strcmp(canvas->get_type(), "Color") == 0)return;

		mi::base::Handle < const mi::neuraylib::ITile> tile(canvas->get_tile(0, 0));

		size.x = canvas->get_resolution_x();
		size.y = canvas->get_resolution_y();
		size.z = canvas->get_layers_size();

		data = static_cast<const mi::Float32*> (tile->get_data());
	}

	mi::base::Handle<const mi::neuraylib::ICanvas> canvas;

	mi::Float32 const       *data;  // texture data for fast access

	mi::Uint32_3_struct     size;   // size of the texture

	mi::Uint32              ncomp;  // components per pixel

};

// The texture handler structure required by the MDL SDK with custom additional fields.
// handler结构体需要继承自Texture_handler_base
struct Texture_handler : Texture_handler_base {
	// additional data for the texture access functions can be provided here
	// 额外的纹理查找函数数据在这里提供
	size_t         num_textures;       // the number of textures used by the material
									   // (without the invalid texture)
	mdlTexture const *textures;           // the textures used by the material
									   // (without the invalid texture)
};

//===================================以下为辅助函数========================================

// Stores a float4 in a float[4] array.
inline static void store_result4(float res[4], const mi::Float32_4_struct &v)
{
    res[0] = v.x;
    res[1] = v.y;
    res[2] = v.z;
    res[3] = v.w;
}

// Stores a float in all elements of a float[4] array.
static inline void store_result4(float res[4], const float v)
{
    res[0] = res[1] = res[2] = res[3] = v;
}

// Stores the given float values in a float[4] array.
static inline void store_result4(
    float res[4], const float v0, const float v1, const float v2, const float v3)
{
    res[0] = v0;
    res[1] = v1;
    res[2] = v2;
    res[3] = v3;
}

// Stores a float4 in a float[3] array, ignoring v.w.
static inline void store_result3(float res[3], const mi::Float32_4_struct &v)
{
    res[0] = v.x;
    res[1] = v.y;
    res[2] = v.z;
}

// Stores a float in all elements of a float[3] array.
static inline void store_result3(float res[3], const float v)
{
    res[0] = res[1] = res[2] = v;
}

// Stores the given float values in a float[3] array.
static inline void store_result3(float res[3], const float v0, const float v1, const float v2)
{
    res[0] = v0;
    res[1] = v1;
    res[2] = v2;
}

static inline mi::Float32 u2f_rn(const mi::Uint32 u) {
    return (mi::Float32) u;
}

static inline mi::Uint32 f2u_rz(const mi::Float32 f) {
    return (mi::Uint32) f;
}

static inline mi::Sint64 f2ll_rz(const mi::Float32 f) {
    return (mi::Sint64) f;
}

static inline mi::Sint64 f2ll_rd(const mi::Float32 f) {
    return (mi::Sint64) mi::math::floor(f);
}
    
mi::Uint32 texremap(
    mi::Uint32 tex_size, Tex_wrap_mode wrap_mode, const mi::Sint32 crop_offset, float texf)
{
    mi::Sint32 texi = 0;
    const mi::Sint64 texil = f2ll_rz(texf);
    const mi::Sint64 texsizel = mi::Sint64(tex_size);

    if (mi::Uint64(f2ll_rd(texf)) >= mi::Uint64(tex_size)) {

        // Wrap or clamp
        if (wrap_mode == Tex_wrap_mode::TEX_WRAP_CLAMP || wrap_mode == Tex_wrap_mode::TEX_WRAP_CLIP)
            texi = (int) std::min(std::max(texil, 0ll), (texsizel - 1ll));
        // Repeat
        else {
            
            texi = mi::Sint32(texil % texsizel);
            
            const mi::Sint32 s = mi::math::sign_bit(texf);
            const mi::Sint64 d = texil / (mi::Sint64) tex_size;
            const mi::Sint32 a = 
                (mi::Sint32) (wrap_mode == Tex_wrap_mode::TEX_WRAP_MIRRORED_REPEAT) &
                ((mi::Sint32) d^s) & 1;
            const bool alternate = (a != 0);
            if (alternate)   // Flip negative tex
                texi = -texi;
            if (s != a) // "Otherwise" pad negative tex back to positive
                texi += (mi::Sint32) tex_size - 1;
        }
    }
    else texi = (int) texil;

    // Crop
    texi += crop_offset;
    return mi::Uint32(texi);
}

void tex_lookup2D(
    mi::Float32 res[4], 
    mdlTexture tex, 
    const mi::Float32 uv[2], 
    mi::neuraylib::Tex_wrap_mode wrap_u,
    mi::neuraylib::Tex_wrap_mode wrap_v,
    const mi::Float32            crop_u[2],
    const mi::Float32            crop_v[2]) {

    const mi::Float32 crop_w = crop_u[1] - crop_u[0];
    const mi::Float32 crop_h = crop_v[1] - crop_v[0];

    const mi::Sint32_2 crop_offset(
        f2u_rz(u2f_rn(tex.size.x-1) * crop_u[0]),
        f2u_rz(u2f_rn(tex.size.y-1) * crop_v[0]));

    const mi::Uint32_2 crop_texres(
        std::max(f2u_rz(u2f_rn(tex.size.x) * crop_w), 1u),
        std::max(f2u_rz(u2f_rn(tex.size.y) * crop_h), 1u));
    
    const float U = uv[0] * crop_texres.x - 0.5f;
    const float V = uv[1] * crop_texres.y - 0.5f;

    const mi::Uint32 U0 = texremap(crop_texres.x, wrap_u, crop_offset[0], U);
    const mi::Uint32 U1 = texremap(crop_texres.x, wrap_u, crop_offset[0], U+1.0f);
    const mi::Uint32 V0 = texremap(crop_texres.y, wrap_v, crop_offset[1], V);
    const mi::Uint32 V1 = texremap(crop_texres.y, wrap_v, crop_offset[1], V+1.0f);

    const mi::Uint32 i00 = (tex.size.x * V0 + U0) * tex.ncomp;
    const mi::Uint32 i01 = (tex.size.x * V0 + U1) * tex.ncomp;
    const mi::Uint32 i10 = (tex.size.x * V1 + U0) * tex.ncomp;
    const mi::Uint32 i11 = (tex.size.x * V1 + U1) * tex.ncomp;

    mi::Float32 ufrac = U - mi::math::floor(U);
    mi::Float32 vfrac = V - mi::math::floor(V);

    ufrac *= ufrac*ufrac*(ufrac*(ufrac*6.0f - 15.0f) + 10.0f); // smoother step
    vfrac *= vfrac*vfrac*(vfrac*(vfrac*6.0f - 15.0f) + 10.0f);

    const  mi::Float32_4 c1 = mi::math::lerp(
        mi::Float32_4(tex.data[i00 + 0], tex.data[i00 + 1], tex.data[i00 + 2], tex.data[i00 + 3]),
        mi::Float32_4(tex.data[i01 + 0], tex.data[i01 + 1], tex.data[i01 + 2], tex.data[i01 + 3]),
        ufrac);

    const  mi::Float32_4 c2 = mi::math::lerp(
        mi::Float32_4(tex.data[i10 + 0], tex.data[i10 + 1], tex.data[i10 + 2], tex.data[i10 + 3]),
        mi::Float32_4(tex.data[i11 + 0], tex.data[i11 + 1], tex.data[i11 + 2], tex.data[i11 + 3]),
        ufrac);

    store_result4(res, mi::math::lerp(c1, c2, vfrac));
}


//==================================以下为纹理查找函数=====================================
// For simplicity, the texture access functions are only implemented for float4
// 为了简单，只实现了float4相关函数

/// Implementation of \c tex::lookup_float4() for a texture_2d texture.
void tex_lookup_float4_2d(
    mi::Float32                  result[4],
    const mi::neuraylib::Texture_handler_base *self_base,
    mi::Uint32                   texture_idx,
    const mi::Float32            coord[2],
    mi::neuraylib::Tex_wrap_mode wrap_u,
    mi::neuraylib::Tex_wrap_mode wrap_v,
    const mi::Float32            crop_u[2],
    const mi::Float32            crop_v[2])
{
    Texture_handler const *self = static_cast<Texture_handler const *>(self_base);

    if (texture_idx == 0 || texture_idx - 1 >= self->num_textures) {
        // invalid texture returns zero
        store_result4(result, 0.0f);
        return;
    }

    mdlTexture const &tex = self->textures[texture_idx - 1];
    tex_lookup2D(result, tex, coord, wrap_u, wrap_v, crop_u, crop_v);
}

/// Implementation of \c tex::lookup_float3() for a texture_2d texture.
void tex_lookup_float3_2d(
    mi::Float32                  result[3],
    const mi::neuraylib::Texture_handler_base *self_base,
    mi::Uint32                   texture_idx,
    const mi::Float32            coord[2],
    mi::neuraylib::Tex_wrap_mode wrap_u,
    mi::neuraylib::Tex_wrap_mode wrap_v,
    const mi::Float32            crop_u[2],
    const mi::Float32            crop_v[2])
{
    Texture_handler const *self = static_cast<Texture_handler const *>(self_base);

    if (texture_idx == 0 || texture_idx - 1 >= self->num_textures) {
        // invalid texture returns zero
        store_result3(result, 0.0f);
        return;
    }

    mi::Float32 c[4];
    tex_lookup_float4_2d(c, self, texture_idx, coord, wrap_u, wrap_v, crop_u, crop_v);

    result[0] = c[0];
    result[1] = c[1];
    result[2] = c[2];
}

/// Implementation of \c tex::texel_float4() for a texture_2d texture.
void tex_texel_float4_2d(
    mi::Float32                  result[4],
    const mi::neuraylib::Texture_handler_base *self_base,
    mi::Uint32                   texture_idx,
    const mi::Sint32             coord[2],
    const mi::Sint32             uv_tile[2])
{
    Texture_handler const *self = static_cast<Texture_handler const *>(self_base);

    if (texture_idx == 0 || texture_idx - 1 >= self->num_textures) {
        // invalid texture returns zero
        store_result3(result, 0.0f);
        return;
    }

    mdlTexture const &tex = self->textures[texture_idx - 1];

    const mi::Uint32 idx = (tex.size.x * coord[1] + coord[0]) * tex.ncomp;

    store_result4(result,
        mi::Float32_4(tex.data[idx + 0], tex.data[idx + 1], tex.data[idx + 2], tex.data[idx + 3]));
}

/// Implementation of \c tex::lookup_float4() for a texture_3d texture.
void tex_lookup_float4_3d(
    mi::Float32                  result[4],
    const mi::neuraylib::Texture_handler_base *self,
    mi::Uint32                   texture_idx,
    const mi::Float32            coord[3],
    mi::neuraylib::Tex_wrap_mode              wrap_u,
    mi::neuraylib::Tex_wrap_mode              wrap_v,
    mi::neuraylib::Tex_wrap_mode              wrap_w,
    const mi::Float32            crop_u[2],
    const mi::Float32            crop_v[2],
    const mi::Float32            crop_w[2])
{
    result[0] = 0.0f;
    result[1] = 0.0f;
    result[2] = 0.0f;
    result[3] = 1.0f;
}

/// Implementation of \c tex::lookup_float3() for a texture_3d texture.
void tex_lookup_float3_3d(
    mi::Float32                  result[3],
    const mi::neuraylib::Texture_handler_base *self,
    mi::Uint32                   texture_idx,
    const mi::Float32            coord[3],
    mi::neuraylib::Tex_wrap_mode              wrap_u,
    mi::neuraylib::Tex_wrap_mode              wrap_v,
    mi::neuraylib::Tex_wrap_mode              wrap_w,
    const mi::Float32            crop_u[2],
    const mi::Float32            crop_v[2],
    const mi::Float32            crop_w[2])
{
    result[0] = 0.0f;
    result[1] = 0.0f;
    result[2] = 0.0f;
}

/// Implementation of \c tex::texel_float4() for a texture_3d texture.
void tex_texel_float4_3d(
    mi::Float32                  result[4],
    const mi::neuraylib::Texture_handler_base *self,
    mi::Uint32                   texture_idx,
    const mi::Sint32              coord[3])
{
    result[0] = 0.0f;
    result[1] = 0.0f;
    result[2] = 0.0f;
    result[3] = 1.0f;
}

/// Implementation of \c tex::lookup_float4() for a texture_cube texture.
void tex_lookup_float4_cube(
    mi::Float32                  result[4],
    const mi::neuraylib::Texture_handler_base *self,
    mi::Uint32                   texture_idx,
    const mi::Float32            coord[3])
{
    result[0] = 0.0f;
    result[1] = 0.0f;
    result[2] = 0.0f;
    result[3] = 1.0f;
}

/// Implementation of \c tex::lookup_float3() for a texture_cube texture.
void tex_lookup_float3_cube(
    mi::Float32                  result[3],
    const mi::neuraylib::Texture_handler_base *self,
    mi::Uint32                   texture_idx,
    const mi::Float32            coord[3])
{
    result[0] = 0.0f;
    result[1] = 0.0f;
    result[2] = 0.0f;
}

/// Implementation of \c resolution_2d function needed by generated code,
/// which retrieves the width and height of the given texture.
void tex_resolution_2d(
    mi::Sint32                   result[2],
    const mi::neuraylib::Texture_handler_base *self_base,
    mi::Uint32                   texture_idx,
    const mi::Sint32             uv_tile[2])
{

    Texture_handler const *self = static_cast<Texture_handler const *>(self_base);

    if (texture_idx == 0 || texture_idx - 1 >= self->num_textures) {
        // invalid texture returns zero
        result[0] = 0;
        result[1] = 0;
        return;
    }
	mdlTexture const &tex = self->textures[texture_idx - 1];

    result[0] = tex.size.x;
    result[1] = tex.size.y;
}

//纹理查找函数指针列表
mi::neuraylib::Texture_handler_vtable tex_vtable = {
    tex_lookup_float4_2d,
    tex_lookup_float3_2d,
    tex_texel_float4_2d,
    tex_lookup_float4_3d,
    tex_lookup_float3_3d,
    tex_texel_float4_3d,
    tex_lookup_float4_cube,
    tex_lookup_float3_cube,
    tex_resolution_2d
};

#endif
