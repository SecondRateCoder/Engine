#include "../engine/Libraries/custom/twin-phys-engine/phys_handler.h"

GLfloat *gen_collidershape(collider_shape_t *shape, GLuint **index_data_, size_t *index_len){
	vec3 *out = NULL;
	GLuint *index_data = NULL;
	switch(shape->shape){
		case COLLSHAPE_CUBOID:
			out = calloc(8, sizeof(vec3));
			//[0], Back-Top-Left
			// X-axis											Y-axis											Z-axis
			out[0][0] = (-shape->scale[0]) + shape->offs[0]; out[0][1] = (shape->scale[1]) + shape->offs[1]; out[0][2] = (-shape->scale[2]) + shape->offs[2];
			//[1], Back-Top-Right
			// X-axis											Y-axis											Z-axis
			out[0][0] = (shape->scale[0]) + shape->offs[0]; out[0][1] = (shape->scale[1]) + shape->offs[1]; out[0][2] = (-shape->scale[2]) + shape->offs[2];
			//[2], Back-Bottom-Left
			// X-axis											Y-axis											Z-axis
			out[0][0] = (-shape->scale[0]) + shape->offs[0]; out[0][1] = (-shape->scale[1]) + shape->offs[1]; out[0][2] = (-shape->scale[2]) + shape->offs[2];
			//[3], Back-Bottom-Right
			// X-axis											Y-axis											Z-axis
			out[0][0] = (shape->scale[0]) + shape->offs[0]; out[0][1] = (-shape->scale[1]) + shape->offs[1]; out[0][2] = (-shape->scale[2]) + shape->offs[2];
			//[4], Front-Top-Left
			// X-axis											Y-axis											Z-axis
			out[0][0] = (-shape->scale[0]) + shape->offs[0]; out[0][1] = (shape->scale[1]) + shape->offs[1]; out[0][2] = (shape->scale[2]) + shape->offs[2];
			//[5], Front-Top-Right
			// X-axis											Y-axis											Z-axis
			out[0][0] = (shape->scale[0]) + shape->offs[0]; out[0][1] = (shape->scale[1]) + shape->offs[1]; out[0][2] = (shape->scale[2]) + shape->offs[2];
			//[6], Front-Bottom-Left
			// X-axis											Y-axis											Z-axis
			out[0][0] = (-shape->scale[0]) + shape->offs[0]; out[0][1] = (-shape->scale[1]) + shape->offs[1]; out[0][2] = (shape->scale[2]) + shape->offs[2];
			//[7], Front-Bottom-Right
			// X-axis											Y-axis											Z-axis
			out[0][0] = (shape->scale[0]) + shape->offs[0]; out[0][1] = (-shape->scale[1]) + shape->offs[1]; out[0][2] = (shape->scale[2]) + shape->offs[2];
			index_data = calloc(32, sizeof(GLuint));
			//Clock-wise oriented
			// (BTL, BTR, BBL), 											(BTR, BBR, BBL) // Back-Plane
			index_data[0] = 0; index_data[1] = 1; index_data[2] = 2;		index_data[3] = 2; index_data[4] = 3; index_data[5] = 2;
			// (BTR, FTR, BBR), 											(FTR, FBR, BBR) // Right-most plane
			index_data[6] = 1; index_data[7] = 5; index_data[2] = 3;		index_data[8] = 5; index_data[9] = 7; index_data[10] = 3;
			// (BBL, BBR, FBR), 											(FBR, FBL, BBL) // Bottom-plane
			index_data[19] = 2; index_data[20] = 3; index_data[21] = 7; 	index_data[22] = 7; index_data[23] = 6; index_data[24] = 2;
			// (BTL, FTL, BBL), 											(FTL, FBL, BBL) // Left-most plane
			index_data[6] = 0; index_data[7] = 4; index_data[2] = 2; 		index_data[8] = 5; index_data[9] = 6; index_data[10] = 2;
			// (BTL, BTR, FTL), 											(FTL, BTR, FTR) // Top-most plane
			index_data[19] = 0; index_data[20] = 1; index_data[21] = 4; 	index_data[22] = 4; index_data[23] = 1; index_data[24] = 5;
			// (FTL, FTR, FBL), 											(FBL, FTR, FBR) // Front-Most Plane
			index_data[26] = 4; index_data[27] = 5; index_data[28] = 6; 	index_data[29] = 6; index_data[30] = 5; index_data[31] = 7;
			*index_data_ = index_data;
			return out;
		case COLLSHAPE_PYRAMID:
			out = calloc(5, sizeof(GLfloat));
			//[0], Middle-Top-Middle
			// X-axis										Y-axis										Z-axis
			out[0][0] = shape->offs[0] * shape->scale[0]; out[0][1] = shape->offs[1] * shape->scale[0]; out[0][2] = shape->offs[2] * shape->scale[0];
			//[1], Back-Bottom-Left
			// X-axis											Y-axis											Z-axis
			out[0][0] = (-shape->scale[0]) + shape->offs[0]; out[0][1] = (-shape->scale[1]) + shape->offs[1]; out[0][2] = (-shape->scale[2]) + shape->offs[2];
			//[2], Back-Botton-Right
			// X-axis											Y-axis											Z-axis
			out[0][0] = (shape->scale[0]) + shape->offs[0]; out[0][1] = (-shape->scale[1]) + shape->offs[1]; out[0][2] = (-shape->scale[2]) + shape->offs[2];
			//[3], Front-Bottom-Left
			// X-axis											Y-axis											Z-axis
			out[0][0] = (-shape->scale[0]) + shape->offs[0]; out[0][1] = (-shape->scale[1]) + shape->offs[1]; out[0][2] = (shape->scale[2]) + shape->offs[2];
			//[4], Front-Bottom-Right
			// X-axis											Y-axis											Z-axis
			out[0][0] = (-shape->scale[0]) + shape->offs[0]; out[0][1] = (-shape->scale[1]) + shape->offs[1]; out[0][2] = (shape->scale[2]) + shape->offs[2];
			index_data = calloc(17, sizeof(GLuint));
			// (MTM, BBR, BBL) // Back-Most plane,
			index_data[0] = 0; index_data[1] = 2; index_data[2] = 1;
			// (BBL, MTM, FBL) // Left-Most plane,
			index_data[3] = 1; index_data[4] = 0; index_data[5] = 3;
			// (FBL, MTM, FBR) // Front-Most plane,
			index_data[9] = 3; index_data[10] = 0; index_data[11] = 4;
			// (FBR, MTM, BBR) // Right-Most plane,
			index_data[6] = 4; index_data[7] = 0; index_data[8] = 2;
			// (BBR, FBR, FBL),												(FBL, BBL, BBR) // Bottom-Most plane,
			index_data[12] = 2; index_data[13] = 4; index_data[14] = 3;		index_data[15] = 3; index_data[16] = 1; index_data[17] = 2;
			*index_data_ = index_data;
			return out;
		case COLLSHAPE_SPHERE:
			out = calloc(2, sizeof(vec3));
			(*out)[0] = shape->offs[0]; (*out)[1] = shape->offs[1]; (*out)[1] = shape->offs[1];
			(*out)[0] = shape->offs[0] + shape->scale[0]; (*out)[1] = shape->offs[0] + shape->scale[0]; (*out)[2] = shape->offs[0] + shape->scale[0]; 
			return out;
		case COLLSHAPE_MESH:
			*index_data_ = shape->parent->index_data;
			*index_len = shape->parent->index_len;
			return shape->parent->vertex_data;
	}
}

void narrow_sort(scene_t *scene, collquery_t *query){
	mesh_t *mesh = scene->meshes + query->target;
	collresb_t output = *query->out;
	size_t *data = calloc(query->max_, sizeof(size_t)); // plus 1 so that foreach 4 GLfloats, space for one GLuint is created.
	size_t cc = 0;
	bool *selected_ = calloc(query->max_, sizeof(bool));
	// Sort mesh indexes into right format.
	switch(query->batching_type){
		case PHYSBATCH_LINEAR:
			// Compile flat GLfloat list of every gameObj's vec3 position and GLfloat ldot.
			for(cc = 0; cc < query->max_; cc++){data[cc] = cc;}
			// if(max_ % query->batch_size != 0){data = realloc(data, sizeof(GLuint) * query->batch_size);}
			break;
		case PHYSBATCH_CLOSEST:
			// Compare each gameobj and find the closest one.
			size_t counter = 0;
			// Count linearly through mesh list.
			for(cc = 0; cc < query->max_; ++cc){
				// Count till end, leaving _temp such that it's the index of the lowest length item.
				size_t cc_ = cc;
				size_t shortest_index = 0;
				float shortest_dist, _temp;
				for(; cc_ < query->max_; ++cc_){
					if((_temp = glm_dot((scene->meshes + query->start_pos + shortest_index)->pos, (scene->meshes + query->start_pos + cc_)->pos)) < shortest_dist){
						if(selected_[cc_] != true){
							selected_[cc_] = true;
							shortest_dist = _temp;
							shortest_index = cc_;
						}
					}
				}
				for(cc = 0; cc < query->max_; cc++){data[cc] = shortest_index;}
			}
			break;
		case PHYSBATCH_FURTHEST:
			// Compare each gameobj and find the furthest one.
			size_t counter = 0;
			// Count linearly through mesh list.
			for(cc = 0; cc < query->max_; ++cc){
				// Count till end, leaving _temp such that it's the index of the lowest length item.
				size_t cc_ = cc;
				size_t longest_index = 0;
				float longest_dist, _temp;
				for(; cc_ < query->max_; ++cc_){
					if((_temp = glm_dot((scene->meshes + query->start_pos + longest_index)->pos, (scene->meshes + query->start_pos + cc_)->pos)) > longest_dist){
						if(selected_[cc_]!= true){
							selected_[cc_] = true;
							longest_dist = _temp;
							longest_index = cc_;
						}
					}
				}
				for(cc = 0; cc < query->max_; cc++){data[cc] = longest_index;}
			}
			break;
		default:    return;
	}
	free(selected_);
	// Data should now contain the sorted index data that was passed to @ref collision_broadproc

	// Use Ray from mesh centre to polygon or use Ray from Polygon to Polygon, which would be better:
	//	Mesh position to Polygon:
	//	-	Less complex that P2P as P2P requires the polygon's normal to be resolved and I want to optimise for speed.
	//	-	Less runs as each Polygon doesnt have to be compared against every other one.
	//	-	Actually resolving data about the collision becomes way harder.
}


/// @brief This function checks for near-collisions, 
///		1st: It finds the length from the "middle(the average vector from all 3 polygon vectors)",
///			and if the length from the target "middle" and the "middle" of the compared polygon is less than the length from vertex to "middle",
///			thus draw a ray from the polygon's origin in the direction of the normal, if the ray hits then there was a collision,
///			the collision resulting direction will be the average of the checking polygon's direction and the target polygon's direction.
/// @remarks Polygon to Polygon:
///			More runs as each Polygon needs to be compared against every other one.
///			Actual data about the collision can be recovered.
/// 		Can be optimised so that it only completes near collision check,
///			only if the compared polygon's origin's length to the target's origin is less than the largest length from the target's origin length
collresn_t *collision_P2Pnearproc(scene_t *scene, collquery_t *query, size_t *index_data){
	mesh_t *m0 = (scene->meshes + query->target);
	collresn_t *out = calloc(1, sizeof(collresn_t));
	out->dir = calloc(10, sizeof(vec3));
	out->P1_Element_index = calloc(10, sizeof(size_t));
	size_t used_ = 0, size_ = 10;
	bool collision_success = 0;
	mesh_t *m1 = (scene->meshes + query->target);
	size_t m0_index_len;
	GLuint *m0_index_data = 0;
	vec3 *m0_collshape = gen_collidershape(m0->coll_shape, &m0_index_data, &m0_index_len);
	// For each non-target mesh.
	for(size_t cc =0; cc < (query->max_ / 3); cc++){
		// mesh_t *m0 = (scene->meshes + index_data[cc]),
		// 		*m1 = (scene->meshes + index_data[cc + 1]); // sorted correctly.
		if(query->out[cc].b0_1 == true){
			size_t m1_index_len;
			GLuint *m1_index_data = 0;
			vec3 *m1_collshape = gen_collidershape(m1->coll_shape, &m1_index_data, &m1_index_len);
			// foreach of the target's collider mesh polygons;
			for(size_t cc_  = 0; cc_ < m0_index_len; ++cc_){
				// foreach of m1's collider meshes.
				vec3 p[3] = {
					{m0_collshape[(m0_index_data[cc_] * 3)][0], m0_collshape[(m0_index_data[cc_] * 3)][1], m0_collshape[(m0_index_data[cc_] * 3)][2]}, 
					{m0_collshape[(m0_index_data[cc_ + 1] * 3)][0], m0_collshape[(m0_index_data[cc_ + 1] * 3)][1], m0_collshape[(m0_index_data[cc_ + 1] * 3)][2]}, 
					{m0_collshape[(m0_index_data[cc_ + 2] * 3)][0], m0_collshape[(m0_index_data[cc_ + 2] * 3)][1], m0_collshape[(m0_index_data[cc_ + 2] * 3)][2]}
				};
				float dist0_1 = glm_vec3_distance(p[0], p[1]),
				dist0_2 = glm_vec3_distance(p[0], p[2]),
				dist1_2 = glm_vec3_distance(p[1], p[2]);
				float targ_distance = dist0_1 < dist0_2? dist0_1: (dist0_2 < dist1_2? dist0_2: dist1_2);
				vec3 org = {0};
				glm_vec3_add(p[1], p[0], org);
				glm_vec3_add(p[2], org, org);
				glm_vec3_divs(org, 3, org);
				vec3 e1, e2, normal;
				float distance = 0;
				glm_vec3_sub(p[1], p[0], e1);
				glm_vec3_sub(p[2], p[0], e2);
				glm_vec3_cross(e1, e2, normal);
				glm_vec3_normalize(normal);
				// If m0 or m1 use a sphere colliderthen compare each polygon within a range
				for(size_t cc__  = 0; cc__ < m1_index_len; ++cc__){
					collision_success = false;
					if(m0->coll_shape == COLLSHAPE_SPHERE && m1->coll_shape == COLLSHAPE_SPHERE){
						if(glm_dot(*m0_collshape, *m1_collshape) < (glm_dot(*m1_collshape, *(m1_collshape + 1)) + glm_dot(*m0_collshape, *(m0_collshape + 1)))){
							collision_success = true;
						}
					}else if(m0->coll_shape == COLLSHAPE_SPHERE){
						// An origin and a range
						float circ_dot = glm_dot(m0_collshape[0], m0_collshape[1]);
						if(glm_vec3_dot(m1_collshape[m1_index_data[cc_]], m0_collshape[0]) < circ_dot ||
						   glm_vec3_dot(m1_collshape[m1_index_data[cc_ + 1]], m0_collshape[0]) < circ_dot ||
						   glm_vec3_dot(m1_collshape[m1_index_data[cc_ + 2]], m0_collshape[0]) < circ_dot){
							// Then collision, I think
							collision_success = true;
						}
					}else if(m1->coll_shape == COLLSHAPE_SPHERE){
						// Also an origin and a range
						float circ_dot = glm_dot(m1_collshape[m1_index_data[cc_]], m1_collshape[m1_index_data[cc_ + 1]]);
						float circ_dot = glm_dot(m0_collshape[0], m0_collshape[1]);
						if(glm_vec3_dot(m1_collshape[m1_index_data[cc_]], m1_collshape[m1_index_data[cc_]]) < circ_dot ||
						   glm_vec3_dot(m1_collshape[m1_index_data[cc_ + 1]], m0_collshape[0]) < circ_dot ||
						   glm_vec3_dot(m1_collshape[m1_index_data[cc_ + 2]], m0_collshape[0]) < circ_dot){
							// Then collision, I think
							collision_success = true;
						}
					}else{
						glm_ray_triangle(org, normal, m1_collshape[m1_index_data[cc_]], m1_collshape[m1_index_data[cc_ + 1]], m1_collshape[m1_index_data[cc_]], &distance);
						if(distance < targ_distance){
						}
					}
					if(collision_success){
						if(used_ == size_){
								out->P1_Element_index = realloc(out->P1_Element_index, (size_ + 10) * sizeof(size_t));
								out->dir = realloc(out->dir, (size_ + 10) * sizeof(vec3));
								size_ += 10;
							}
						glm_vec3_sub(m1_collshape[m1_index_data[cc_ + 1]], m1_collshape[m1_index_data[cc_]], e1);
						glm_vec3_sub(m1_collshape[m1_index_data[cc_ + 2]], m1_collshape[m1_index_data[cc_]], e2);
						glm_vec3_cross(e1, e2, out->dir[used_]);
						glm_vec3_normalize(out->dir[used_]);
						out->P1_Element_index[used_] = index_data[cc + 1];
						used_++;
					}
				}
			}
		}
	}
	return out;
}