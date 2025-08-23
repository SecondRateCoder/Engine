#pragma once

// #include "../engine/Public.h"
#include "../engine/graphics/graphics.h"

#ifndef _3D_H
#define _3D_H

typedef struct _mesh{
    /// @brief The float mesh_data that makes up a _mesh object.
    GLfloat *mesh_data;
    /// @brief The length of this _mesh's mesh_data.
    size_t data_len;

    /// @brief The order that Vertices in the _mesh are drawn in.
    GLuint *vertex_index;
    /// @brief The length of @ref mesh_index_order.
    size_t index_len;

    size_t num_textures;
    image_t *textures;

    /// @brief The number of @ref GLfloat values that make up a Vertex in a _mesh's mesh_data. Hence "stride"
    uint8_t vertex_stride;
    /// @brief The number of @ref GLfloat values that make up a Vertex's Color mesh_data in a _mesh's mesh_data.
    uint8_t color_stride;
    /// @brief The number of @ref GLfloat values that make up a Vertex's Texture per-pixel stride in a _mesh's mesh_data.
    uint8_t dpi_stride;
    /// @brief The Layout index in a shader corresponding to this _mesh's Position vertex data.
    uint32_t pos_layoutindex, 
    /// @brief The Layout index in a shader corresponding to this _mesh's vertex Color data.
    color_layoutindex, 
    /// @brief The Layout index in a shader corresponding to this _mesh's vertex Texture data.
    ///! Texture co-ordinates are local to a texture (Image) as well as being normalised.
    local_texcoordinates_layoutindex;
}_mesh;
#define mesh_t _mesh
#endif
/*
#pragma once

#ifndef _3D_H
#define _3D_H

#include "../engine/graphics/graphics.h"  // for image_t, bufferobj_t
#include "../engine/Public.h"             // for mesh_addtexture, mesh_attrlink, stb, OpenGL types

typedef struct _mesh {
    GLfloat  *mesh_data;
    size_t    data_len;

    GLuint   *vertex_index;
    size_t    index_len;

    size_t    num_textures;
    image_t  *textures;

    uint8_t   vertex_stride;
    uint8_t   color_stride;
    uint8_t   dpi_stride;

    uint32_t  pos_layoutindex;
    uint32_t  color_layoutindex;
    uint32_t  local_texcoordinates_layoutindex;
} _mesh;

#define mesh_t _mesh

*/