#include "UI_Bar.h"
#include "Texture.h"

const GLfloat texCoords[12] = {0.f, 1.f, 1.f, 1.f, 1.f, 0.f,
							   0.f, 1.f, 1.f, 0.f, 0.f, 0.f};

UI_Bar::UI_Bar(bool s)
{
	glGenVertexArrays(1, &VAO);
	glGenVertexArrays(2, rectVAO);
	glGenBuffers(2, VBO);
	glGenBuffers(2, rectVBO);
	
	isSelfUI = s;
	GLfloat uiRect[12] = {-1.f, s? -1.f : .92f, 1.f, s? -1.f : .92f, 1.f, s? -.86f : 1.f,
		                  -1.f, s? -1.f : .92f, 1.f, s? -.86f : 1.f, -1.f, s? -.86f : 1.f};

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uiRect), uiRect, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);

	glBindVertexArray(0);

	GLfloat hpRect[12] = {0.f, s? -.93f : .92f, 1.f, s? -.93f : .92f, 1.f, s? -.87f : 1.f,
		                  0.f, s? -.93f : .92f, 1.f, s? -.87f : 1.f, 0.f, s? -.87f : 1.f};

	glBindVertexArray(rectVAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, rectVBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(hpRect), hpRect, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
	glBindVertexArray(0);

	if(s){
		GLfloat nrgRect[12] ={0.f, -.99f, 1.f, -.99f , 1.f, -.93f,
			                  0.f, -.99f, 1.f, -.93f, 0.f, -.93f};

		glBindVertexArray(rectVAO[1]);
		glBindBuffer(GL_ARRAY_BUFFER, rectVBO[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(nrgRect), nrgRect, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
		glBindVertexArray(0);
	}

	glGenTextures(1, &texID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texID);
	int width, height;
	unsigned char* image;
	image = loadPPM(s? "textures/P2_UI.ppm" : "textures/P2_HP.ppm", width, height);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	delete(image);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

UI_Bar::~UI_Bar()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteVertexArrays(2, rectVAO);
	glDeleteBuffers(2, VBO);
	glDeleteBuffers(2, rectVBO);
	glDeleteTextures(1, &texID);
}

void UI_Bar::fetchUniforms(GLuint uiShader, GLuint rectShader){
	glUseProgram(uiShader);
	uSampler = glGetUniformLocation(uiShader, "tex");
	glUseProgram(rectShader);
	uColor = glGetUniformLocation(rectShader, "color");
}

void UI_Bar::draw(GLuint uiShader, GLuint rectShader){
	glUseProgram(uiShader);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texID);
	glUniform1i(uSampler, 0);

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	glUseProgram(rectShader);
	glUniform3f(uColor, .95f, .2f, .4f);
	glBindVertexArray(rectVAO[0]);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	if(isSelfUI){
		glUniform3f(uColor, .1f, .4f, .95f);
		glBindVertexArray(rectVAO[1]);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}
}