#include "window.h"

/// @brief Initialise an Un-Resizable GLFW Window.
/// @param name The Name of the Window.
/// @param w The Width of the Window.
/// @param h The Height of the Window.
/// @return An initialised Pointer to the Window.
win_t *win_init(char *name, poll_do *polld, poll_kill *pollk, uint32_t w, uint32_t h){
	//Initialise GLFW.
	glfwInit();
	//Enable Depth testing, So Triangles behind other Triangles are not drawn.
	glEnable(GL_DEPTH_TEST);
	// glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	//Not Resizable.
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	//Using Legacy, OpenGL 3.3 for my 9800 GT GPU.
	glfwWindowHint(GLFW_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//malloc on heap.
	win_t *win = malloc(sizeof(win_t));
	if(polld != NULL){win->polld = polld;}else{win->polld = polld_default;}
	if(pollk != NULL){win->pollk = pollk;}else{win->pollk = pollk_default;}
	win->name = name;
	win->w =w;
	win->h =h;
	win->window =glfwCreateWindow(w, h, name, NULL, NULL);
	//Handle errors.
	if(win->window == NULL){
		//If nothing was created, free the window and it's attributes.
		printf("Window %s failed to Initialise\nError!", name);
		free(name);
		glfwTerminate();
		return NULL;
	}else{
		// Window successfully created.
		printf("Window %s successfully created\n", name);
		glfwMakeContextCurrent(win->window);
		gladLoadGL();
	}
	return win;
}

/// @brief Poll the GLFW window's events.
/// @param win The Window to be Polled.
/// @remark If the Window is set to close, It will Kill it after calling a set poll_kill function.
void win_poll(win_t *win){
	size_t cc =0;
	while(!win_shouldclose(win)){
		//Flood color with Orange.
		win_flood(win, (argb_t){0.5f, 0.5f, 0, 1});
		//Ensure that win's Shader's are Handled.
		SHADERBLOCK_HANDLE(win->shaders, true);
		//Explicitly define the Shader to be Used.
		glUseProgram(win->shaders->shaderProgram);
		win->polld(win, cc);
		//Bind win's VAO for Drawing.
		glBindVertexArray(win->buffers[win->buffer_curr].VAO);
		//Draw Triangles with GL_TRIANGLE Primitive.
		glDrawElements(GL_TRIANGLES, win->vert_count, GL_UNSIGNED_INT, 0);
		// glDrawElements(GL);
		glfwSwapBuffers(win->window);
		glfwPollEvents();
		++cc;
	}
	win->pollk(win);
	win_kill(win);
}

/// @brief Check if the specified Window was set to be closed.
/// @param win The Window to be checked.
/// @return Was the User trying to close the Window.
bool win_shouldclose(win_t *win){return glfwWindowShouldClose(win->window);}

/// @brief Terminate and free the specified Window Pointer.
/// @param win The Window to be terminated.
void win_kill(win_t *win){
	//Delete buffers.
	GLuint *VAOs, *VBOs, *EBOs;
	size_t VBO_len, EBO_len;
	//Coalesce the VAOs, VBOs, and EBOs.
	for(size_t cc =0; cc < win->buffer_len; ++cc){
		VAOs[cc] = win->buffers[cc].VAO;
		const size_t len = (win->buffers[cc].VBO_len > win->buffers[cc].EBO_len) ? win->buffers[cc].VBO_len : win->buffers[cc].EBO_len;
		for(size_t cc_ =0; cc_ < len; ++cc_){
			if(cc_ < win->buffers[cc].VBO_len){VBOs[cc+cc_] = win->buffers[cc].VBO;}
			if(cc_ < win->buffers[cc].EBO_len){EBOs[cc+cc_] = win->buffers[cc].EBO;}
		}
	}
	buffer_bufferdo(win->buffers, win->buffer_len, BUFFER_OPTIONS_FREE_ALL);
	glDeleteProgram(win->shaders->shaderProgram);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glfwDestroyWindow(win->window);
	GLuint *TEXTURES_ID = (GLuint *)malloc(win->textures_len);
	for(size_t cc =0; cc < win->textures_len; ++cc){TEXTURES_ID[cc] = win->textures[cc].ID;}
	glDeleteTextures(win->textures_len, TEXTURES_ID);
	free(TEXTURES_ID);
	glfwTerminate();
	free(win->name);
	free(win->polld);
	free(win->pollk);
	free(win->textures);
	free(win->shaders);
	free(win);
}


/// @brief Link the win's VAO with the VBO.
/// @param win The Window containing the VAO and VBO.
/// @param layout ...
/// @param component_num ...
/// @param type ...
/// @param stride ...
/// @param offset ...
void win_attrblink(window *win, GLuint layout, GLuint component_num, GLenum type, GLsizeiptr stride, const void *offset){
	glBindBuffer(GL_ARRAY_BUFFER, win->buffers[win->buffer_curr].VBO);
	glVertexAttribPointer(layout, component_num, type, GL_FALSE, stride, offset);
	glEnableVertexAttribArray(layout);
}

void polld_default(win_t *win, size_t cycles){return;}
void pollk_default(win_t *win){return;}