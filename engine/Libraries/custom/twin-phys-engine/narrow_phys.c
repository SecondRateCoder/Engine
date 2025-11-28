#include "../engine/Libraries/custom/twin-physics-engine/phys_handler.h"

void narrow_proc(scene_t *scene, collquery_t *query){
	mesh_t *mesh = scene->meshes + query->target;
	collres_t output = *query->buffer;
	for(size_t cc =0; cc < query->max_; ++cc, output = query->buffer[cc]){
		GLuint *data = calloc(query->max_, sizeof(GLuint)); // plus 1 so that foreach 4 GLfloats, space for one GLuint is created.
		size_t cc = 0;
		bool selected_[query->max_] = {0};
		switch(temp->batching_type){
			case PHYSBATCH_LINEAR:
				// Compile flat GLfloat list of every gameObj's vec3 position and GLfloat ldot.
				for(cc = 0; cc < query->max_; cc++){data[cc] = cc;}
				// if(max_ % temp->batch_size != 0){data = realloc(data, sizeof(GLuint) * temp->batch_size);}
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
						if((_temp = glm_dot((scene->meshes + temp->start_pos + shortest_index)->pos, (scene->meshes + temp->start_pos + cc_)->pos)) < shortest_dist){
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
					bool selected[max_] = {0};
					for(; cc_ < query->max_; ++cc_){
						if((_temp = glm_dot((scene->meshes + temp->start_pos + longest_index)->pos, (scene->meshes + temp->start_pos + cc_)->pos)) > longest_dist){
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
	}
}