#pragma once

#include "../engine/Public.h"

typedef struct tex_format{
	/// @brief The Dimensions or shape of the Texture Image.
	GLenum target,
		/// @brief The format used to store pixel data
		pixel_format,
		/// @brief The type used for each pixel's channels
		pixel_type
	;
	/// @brief  @brief Mip-map level.
	GLint level,
	/// @brief The format & type used to store Pixels GPU-side.
		internalFormat,
	/// @brief For 3D textures or array layers
		depth;
}tex_format;
#define texformat_t tex_format

typedef struct texture_image{
	GLint width, height, color_channels;
	char *path;
	GLuint ID;
	uint8_t unit;
	unsigned char* img;
	float border[4];
	texformat_t format;
}texture_image;
#define image_t texture_image

typedef enum BUFFER_OPTIONS{
	//For single buffer objects.
	BUFFER_OPTIONS_CLEAR = 0,
	//For single buffer objects.
	BUFFER_OPTIONS_CLEAR_VBO = 1,
	//For single buffer objects.
	BUFFER_OPTIONS_CLEAR_EBO = 2,
	//For single buffer objects.
	BUFFER_OPTIONS_FREE = 3,

	//For multiple buffer objects.
	BUFFER_OPTIONS_FREE_ALL = 4,
	//For multiple buffer objects.
	BUFFER_OPTIONS_CLEAR_VAO = 5,
	//For multiple buffer objects.
	BUFFER_OPTIONS_COLLECT_VAO = 6,
	//For multiple buffer objects.
	BUFFER_OPTIONS_COLLECT_VBO_MULTIPLE = 7,
	//For multiple buffer objects.
	BUFFER_OPTIONS_COLLECT_EBO_MULTIPLE = 8,
}BUFFER_OPTIONS;

typedef struct buffer_object{
	/*
	[0]: Is VAO set-up?
	[1]: Is VBO set-up?
	[2]: Is EBO set-up?
	*/
	bool buffer_[3];

	/// @brief The Vertex Array Object for the Window, it contains Multiple States about the Window's OpenGL state.
	/// It describes the VBO and EBO.
	/// @remark VAO is used to store the state of the OpenGL pipeline.
	GLuint VAO;

	/// @brief An array of VBO GLuint IDs, for accessing each VBO element.
	/// As in, the Literal GLfloat vertices to be drawn.
	/// Accepts whole Model's, in the format of Vertex arrays.
	/// @remark VBO is used to store the vertex data for the OpenGL pipeline.
	GLuint VBO;
	/// @brief An array of EBO GLuint IDs, for accessing each one.
	/// @remark The EBO is used to store the index data for the OpenGL pipeline, 
	/// if there is only one the same EBO will be applied to each VBO item.
	/// If EBO_curr isn't equal to VBO_curr, this buffer_oject is considered invalid.
	/// If it is invalid and there are no other valid buffer_objects then the first EBO item will be used.
	/// If there are no EBOs then a default will be attributed.
	GLuint EBO;
	GLenum format;
}buffer_object;
#define bufferobj_t buffer_object

typedef struct _mesh{
	size_t ID;
	vec3 pos, rot;
	GLfloat ldot;
    /// @brief The float vertex_data that makes up a _mesh object.
    GLfloat *vertex_data;
    /// @brief The length of this _mesh's vertex_data.
    size_t data_len;
    
    /// @brief The order that Vertices in the _mesh are drawn in.
    GLuint *index_data;
    /// @brief The length of @ref mesh_index_order.
    size_t index_len;
    bufferobj_t *buffer;
    
    image_t *texture;

	/// @brief The offset for the position elements in vertex_data.
	uint8_t pos_offset;
	/// @brief The offset for the color elements in vertex_data.
	uint8_t color_offset;
	/// @brief The offset for the UV elements in vertex_data.
	uint8_t uv_offset;
    
    /// @brief The number of @ref GLfloat values that make up a Vertex in a _mesh's vertex_data. Hence "stride"
    uint8_t vertex_stride;
    /// @brief The number of @ref GLfloat values that make up a Vertex's Color vertex_data in a _mesh's vertex_data.
    uint8_t color_stride;
    /// @brief The number of @ref GLfloat values that make up a Vertex's Texture per-pixel stride in a _mesh's vertex_data.
    uint8_t uv_stride;
    /// @brief The Layout index in a shader corresponding to this _mesh's Position vertex data.
    uint8_t pos_layoutindex, 
    /// @brief The Layout index in a shader corresponding to this _mesh's vertex Color data.
    color_layoutindex, 
    /// @brief The Layout index in a shader corresponding to this _mesh's vertex Texture data.
    ///! Texture co-ordinates are local to a texture (Image) as well as being normalised.
    local_texcoordinates_layoutindex;
}_mesh;
#define mesh_t _mesh

typedef struct Color4 {
	// @brief a: 1.0f := Solid, 0.0f := Transparent.
	float a, r, g, b;
} Color4;
#define argb_t Color4

typedef struct cam_type{
	vec3 pos,
		 rot,
		 up;
	GLint width,
		  height,
		  speed,
		  sensitivity;
	float FOV, near, far;
}cam_type;
#define cam_t cam_type
