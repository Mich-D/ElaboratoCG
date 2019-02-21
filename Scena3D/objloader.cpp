
#include <vector>
#include <stdio.h>
#include <string>
#include <cstring>

#include <glm/glm.hpp>

#include "objloader.hpp"


bool loadOBJ(
	const char * path, 
	std::vector<glm::vec3> & out_vertices, 
	std::vector<glm::vec2> & out_uvs,
	std::vector<glm::vec3> & out_normals
){
	printf("Loading OBJ file %s...\n", path);

	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> temp_vertices; 
	std::vector<glm::vec3> temp_vertices_1;
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;


	FILE * file = fopen(path, "r");
	if( file == NULL ){
		printf("Impossible to open the file ! Are you in the right path ? See Tutorial 1 for details\n");
		getchar();
		return false;
	}

	while( 1 ){

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		// else : parse lineHeader
		
		if ( strcmp( lineHeader, "v" ) == 0 ){
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
			temp_vertices.push_back(vertex);
		}
		/*else if ( strcmp( lineHeader, "vt" ) == 0 ){
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y );
			uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
			temp_uvs.push_back(uv);
		}*/
	else if ( strcmp( lineHeader, "vn" ) == 0 ){
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
			temp_normals.push_back(normal);
	}
	else if (strcmp(lineHeader, "f") == 0) {
		std::string vertex1, vertex2, vertex3;
		unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];

		fscanf(file, "%s", lineHeader);
		/* caso in cui si ha  v0//n0, v1//n1 v2//n1   %d//%d, */
		if (strstr(lineHeader, "//")) {
			/* v//n */
			sscanf(lineHeader, "%d//%d ", &vertexIndex[0], &normalIndex[0]);
			fscanf(file, "%d//%d ", &vertexIndex[1], &normalIndex[1]);
			fscanf(file, "%d//%d ", &vertexIndex[2], &normalIndex[2]);
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
/*			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);*/
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);

		}
		else {
			/* v */
			sscanf(lineHeader, "%d", &vertexIndex[0]);
			fscanf(file, "%d", &vertexIndex[1]);
			fscanf(file, "%d", &vertexIndex[2]);
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
		}
		

	}
		else{
			// Probably a comment, eat up the rest of the line
			char commenti[1000];
			fgets(commenti, 1000, file);
		}

	}

	
	// For each vertex of each triangle
	for( unsigned int i=0; i<vertexIndices.size(); i++ ){

		// Get the indices of its attributes
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int normalIndex = normalIndices[i];
		
		// Get the attributes thanks to the index
//		glm::vec3 vertex = temp_vertices[ vertexIndex-1 ];
		int dim_vertice = vertexIndex - 1;
		int dim_normal = normalIndex - 1;
		if (vertexIndex == 0)
			dim_vertice = 0;

		if (normalIndex == 0)
			dim_normal = 0;

		glm::vec3 vertex = temp_vertices[dim_vertice];
		glm::vec3 normal = temp_normals[dim_normal];
		
		// Put the attributes in buffers
		out_vertices.push_back(vertex);
		
		out_normals .push_back(normal);
	
	}
	fclose(file);

	temp_vertices_1 = out_vertices;
	//out_vertices= normalizzaObj(temp_vertices_1);

	return true;
}


std::vector<glm::vec3> normalizzaObj(std::vector<glm::vec3> vertices)
{
	int i;

	float xmax = 0, ymax = 0, zmax = 0, xmin = 0, ymin = 0, zmin = 0;
	for (i = 0; i < vertices.size(); i++)
	{
		if (vertices[i].x >= xmax)
			xmax = vertices[i].x;
	}
	for (i = 0; i <vertices.size(); i++)
	{
		if (vertices[i].y >= ymax)
			ymax = vertices[i].y;
	}
	for (i = 0; i < vertices.size(); i++)
	{
		if (vertices[i].z >= zmax)
			zmax = vertices[i].z;
	}
	printf("xmax %f \n", xmax);
	printf("ymax %f \n", ymax);
	printf("zmax %f \n", zmax);

	/*	for (i = 0; i < vertices.size(); i++)
	printf("Vertice[%d], X= %f Y=%f Z=%f \n", i, vertices[i].x, vertices[i].y, vertices[i].z);*/

	
	for (i = 0; i < vertices.size(); i++)
	{
		if (vertices[i].x <= xmin)
			xmin = vertices[i].x;
	}
	for (i = 0; i < vertices.size(); i++)
	{
		if (vertices[i].y <= ymin)
			ymin = vertices[i].y;
	}
	for (i = 0; i <vertices.size(); i++)
	{
		if (vertices[i].z <= zmin)
			zmin = vertices[i].z;
	}
	printf("xmin %f \n", xmin);
	printf("ymin %f \n", ymin);
	printf("zmin %f \n", zmin);

	for (i = 0; i < vertices.size(); i++)
		vertices[i].x = (vertices[i].x - xmin) / (xmax - xmin);

	for (i = 0; i < vertices.size(); i++)
		vertices[i].y = (vertices[i].y - ymin) / (ymax - ymin);

	for (i = 0; i < vertices.size(); i++)
		vertices[i].z = (vertices[i].z - zmin) / (zmax - zmin);

return vertices;

}