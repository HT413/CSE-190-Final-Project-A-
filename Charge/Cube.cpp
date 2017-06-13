#include "Cube.h"



Cube::Cube()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	glBindVertexArray(0);
}


Cube::~Cube()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

void Cube::draw(GLuint shaderProgram) {
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glUniform3f(glGetUniformLocation(shaderProgram, "color"), color.x, color.y, color.z);
	mat4 fullMat = translation * rotation * scale(mat4(1.f), vec3(.2f, .2f, .2f));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &(fullMat[0][0]));

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}