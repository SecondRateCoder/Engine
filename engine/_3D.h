#pragma once

#include "../engine/Public.h"
#include "../engine/graphics/graphics.h"

#ifndef _3D_H
#define _3D_H

typedef struct mesh{
    /// @brief The float mesh_data that makes up a Mesh object.
    GLfloat *mesh_data;
    /// @brief The length of this mesh's mesh_data.
    size_t data_len;

    /// @brief The order that Vertices in the Mesh are drawn in.
    GLuint *vertex_index;
    /// @brief The length of @ref mesh_index_order.
    size_t index_len;


    /// @brief The number of @ref GLfloat values that make up a Vertex in a Mesh's mesh_data. Hence "stride"
    uint8_t vertex_stride;
    /// @brief The number of @ref GLfloat values that make up a Vertex's Color mesh_data in a Mesh's mesh_data.
    uint8_t color_stride;
    /// @brief The number of @ref GLfloat values that make up a Vertex's Texture per-pixel stride in a Mesh's mesh_data.
    uint8_t dpi_stride;
    /// @brief The Layout index in a shader corresponding to this mesh's Position vertex data.
    uint32_t pos_layoutindex, 
    /// @brief The Layout index in a shader corresponding to this mesh's vertex Color data.
    color_layoutindex, 
    /// @brief The Layout index in a shader corresponding to this mesh's vertex Texture data.
    ///! Texture co-ordinates are local to a texture (Image) as well as being normalised.
    local_texcoordinates_layoutindex;
}mesh;

#define mesh_t mesh

void mesh_attrlink(bufferobj_t *buffer, uint32_t pos_layout,  uint32_t color_layout,  uint32_t texture_layout, mesh_t *mesh);

#endif