#include "MyMesh.h"
void MyMesh::Init(void)
{
	m_bBinded = false;
	m_uVertexCount = 0;

	m_VAO = 0;
	m_VBO = 0;

	m_pShaderMngr = ShaderManager::GetInstance();
}
void MyMesh::Release(void)
{
	m_pShaderMngr = nullptr;

	if (m_VBO > 0)
		glDeleteBuffers(1, &m_VBO);

	if (m_VAO > 0)
		glDeleteVertexArrays(1, &m_VAO);

	m_lVertex.clear();
	m_lVertexPos.clear();
	m_lVertexCol.clear();
}
MyMesh::MyMesh()
{
	Init();
}
MyMesh::~MyMesh() { Release(); }
MyMesh::MyMesh(MyMesh& other)
{
	m_bBinded = other.m_bBinded;

	m_pShaderMngr = other.m_pShaderMngr;

	m_uVertexCount = other.m_uVertexCount;

	m_VAO = other.m_VAO;
	m_VBO = other.m_VBO;
}
MyMesh& MyMesh::operator=(MyMesh& other)
{
	if (this != &other)
	{
		Release();
		Init();
		MyMesh temp(other);
		Swap(temp);
	}
	return *this;
}
void MyMesh::Swap(MyMesh& other)
{
	std::swap(m_bBinded, other.m_bBinded);
	std::swap(m_uVertexCount, other.m_uVertexCount);

	std::swap(m_VAO, other.m_VAO);
	std::swap(m_VBO, other.m_VBO);

	std::swap(m_lVertex, other.m_lVertex);
	std::swap(m_lVertexPos, other.m_lVertexPos);
	std::swap(m_lVertexCol, other.m_lVertexCol);

	std::swap(m_pShaderMngr, other.m_pShaderMngr);
}
void MyMesh::CompleteMesh(vector3 a_v3Color)
{
	uint uColorCount = m_lVertexCol.size();
	for (uint i = uColorCount; i < m_uVertexCount; ++i)
	{
		m_lVertexCol.push_back(a_v3Color);
	}
}
void MyMesh::AddVertexPosition(vector3 a_v3Input)
{
	m_lVertexPos.push_back(a_v3Input);
	m_uVertexCount = m_lVertexPos.size();
}
void MyMesh::AddVertexColor(vector3 a_v3Input)
{
	m_lVertexCol.push_back(a_v3Input);
}
void MyMesh::CompileOpenGL3X(void)
{
	if (m_bBinded)
		return;

	if (m_uVertexCount == 0)
		return;

	CompleteMesh();

	for (uint i = 0; i < m_uVertexCount; i++)
	{
		//Position
		m_lVertex.push_back(m_lVertexPos[i]);
		//Color
		m_lVertex.push_back(m_lVertexCol[i]);
	}
	glGenVertexArrays(1, &m_VAO);//Generate vertex array object
	glGenBuffers(1, &m_VBO);//Generate Vertex Buffered Object

	glBindVertexArray(m_VAO);//Bind the VAO
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);//Bind the VBO
	glBufferData(GL_ARRAY_BUFFER, m_uVertexCount * 2 * sizeof(vector3), &m_lVertex[0], GL_STATIC_DRAW);//Generate space for the VBO

	// Position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)0);

	// Color attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)(1 * sizeof(vector3)));

	m_bBinded = true;

	glBindVertexArray(0); // Unbind VAO
}
void MyMesh::Render(matrix4 a_mProjection, matrix4 a_mView, matrix4 a_mModel)
{
	// Use the buffer and shader
	GLuint nShader = m_pShaderMngr->GetShaderID("Basic");
	glUseProgram(nShader); 

	//Bind the VAO of this object
	glBindVertexArray(m_VAO);

	// Get the GPU variables by their name and hook them to CPU variables
	GLuint MVP = glGetUniformLocation(nShader, "MVP");
	GLuint wire = glGetUniformLocation(nShader, "wire");

	//Final Projection of the Camera
	matrix4 m4MVP = a_mProjection * a_mView * a_mModel;
	glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(m4MVP));
	
	//Solid
	glUniform3f(wire, -1.0f, -1.0f, -1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);  

	//Wire
	glUniform3f(wire, 1.0f, 0.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(-1.f, -1.f);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);
	glDisable(GL_POLYGON_OFFSET_LINE);

	glBindVertexArray(0);// Unbind VAO so it does not get in the way of other objects
}
void MyMesh::AddTri(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft)
{
	//C
	//| \
	//A--B
	//This will make the triangle A->B->C 
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);
}
void MyMesh::AddQuad(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft, vector3 a_vTopRight)
{
	//C--D
	//|  |
	//A--B
	//This will make the triangle A->B->C and then the triangle C->B->D
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);

	AddVertexPosition(a_vTopLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopRight);
}
void MyMesh::GenerateCube(float a_fSize, vector3 a_v3Color)
{
	if (a_fSize < 0.01f)
		a_fSize = 0.01f;

	Release();
	Init();

	float fValue = a_fSize * 0.5f;
	//3--2
	//|  |
	//0--1

	vector3 point0(-fValue,-fValue, fValue); //0
	vector3 point1( fValue,-fValue, fValue); //1
	vector3 point2( fValue, fValue, fValue); //2
	vector3 point3(-fValue, fValue, fValue); //3

	vector3 point4(-fValue,-fValue,-fValue); //4
	vector3 point5( fValue,-fValue,-fValue); //5
	vector3 point6( fValue, fValue,-fValue); //6
	vector3 point7(-fValue, fValue,-fValue); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCuboid(vector3 a_v3Dimensions, vector3 a_v3Color)
{
	Release();
	Init();

	vector3 v3Value = a_v3Dimensions * 0.5f;
	//3--2
	//|  |
	//0--1
	vector3 point0(-v3Value.x, -v3Value.y, v3Value.z); //0
	vector3 point1(v3Value.x, -v3Value.y, v3Value.z); //1
	vector3 point2(v3Value.x, v3Value.y, v3Value.z); //2
	vector3 point3(-v3Value.x, v3Value.y, v3Value.z); //3

	vector3 point4(-v3Value.x, -v3Value.y, -v3Value.z); //4
	vector3 point5(v3Value.x, -v3Value.y, -v3Value.z); //5
	vector3 point6(v3Value.x, v3Value.y, -v3Value.z); //6
	vector3 point7(-v3Value.x, v3Value.y, -v3Value.z); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCone(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Replace this with your code
	std::vector<vector3> topVertices;
	vector3 topCenter = vector3(0, 0, a_fHeight / 2);
	vector3 bottomCenter = vector3(0, 0, -a_fHeight / 2);
	float angle = 0;
	float angleChange = (2 * PI) / a_nSubdivisions;

	//create vertices of top and bottom circles
	for (int i = 0; i < a_nSubdivisions; i++) {
		vector3 topVertex = vector3(cos(angle) * a_fRadius, sin(angle) * a_fRadius, a_fHeight / 2.0f);
		angle += angleChange;
		topVertices.push_back(topVertex);
	}

	//create triangles of caps
	//each triangle is between center and two neighboring points
	for (int i = 0; i < a_nSubdivisions - 1; i++) {
		AddTri(topCenter, topVertices[i], topVertices[i + 1]);
	}

	//add the last triangle of each cap
	AddTri(topCenter, topVertices[a_nSubdivisions - 1], topVertices[0]);

	//add triangles to point
	for (int i = 0; i < a_nSubdivisions - 1; i++) {
		AddTri(bottomCenter, topVertices[i + 1], topVertices[i]);
	}

	//add last triangles on side
	AddTri(bottomCenter, topVertices[0], topVertices[a_nSubdivisions - 1]);
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCylinder(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Replace this with your code
	std::vector<vector3> topVertices;
	std::vector<vector3> bottomVertices;
	vector3 topCenter = vector3(0, 0, a_fHeight / 2);
	vector3 bottomCenter = vector3(0, 0, -a_fHeight / 2);
	float angle = 0;
	float angleChange = (2 * PI) / a_nSubdivisions;

	//create vertices of top and bottom circles
	for (int i = 0; i < a_nSubdivisions; i++) {
		vector3 topVertex = vector3(cos(angle) * a_fRadius, sin(angle) * a_fRadius, a_fHeight / 2.0f);
		vector3 bottomVertex = vector3(cos(angle) * a_fRadius, sin(angle) * a_fRadius, -a_fHeight / 2.0f);
		angle += angleChange;
		topVertices.push_back(topVertex);
		bottomVertices.push_back(bottomVertex);
	}

	//create triangles of caps
	//each triangle is between center and two neighboring points
	for (int i = 0; i < a_nSubdivisions - 1; i++) {
		AddTri(topCenter, topVertices[i], topVertices[i + 1]);
		AddTri(bottomCenter, bottomVertices[i + 1], bottomVertices[i]);
	}

	//add the last triangle of each cap
	AddTri(topCenter, topVertices[a_nSubdivisions - 1], topVertices[0]);
	AddTri(bottomCenter, bottomVertices[0], bottomVertices[a_nSubdivisions - 1]);

	//add triangles along sides
	for (int i = 0; i < a_nSubdivisions - 1; i++) {
		AddTri(topVertices[i], bottomVertices[i+1], topVertices[i + 1]);
		AddTri(topVertices[i], bottomVertices[i], bottomVertices[i + 1]);
	}

	//add last triangles on side
	AddTri(topVertices[a_nSubdivisions - 1], bottomVertices[0], topVertices[0]);
	AddTri(topVertices[a_nSubdivisions - 1], bottomVertices[a_nSubdivisions - 1], bottomVertices[0]);
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTube(float a_fOuterRadius, float a_fInnerRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Replace this with your code
	std::vector<vector3> topOuterVertices;
	std::vector<vector3> bottomOuterVertices;
	std::vector<vector3> topInnerVertices;
	std::vector<vector3> bottomInnerVertices;
	float angle = 0;
	float angleChange = (2 * PI) / a_nSubdivisions;

	//create vertices of top and bottom circles
	for (int i = 0; i < a_nSubdivisions; i++) {
		vector3 topOuterVertex = vector3(cos(angle) * a_fOuterRadius, sin(angle) * a_fOuterRadius, a_fHeight / 2.0f);
		vector3 bottomOuterVertex = vector3(cos(angle) * a_fOuterRadius, sin(angle) * a_fOuterRadius, -a_fHeight / 2.0f);
		vector3 topInnerVertex = vector3(cos(angle) * a_fInnerRadius, sin(angle) * a_fInnerRadius, a_fHeight / 2.0f);
		vector3 bottomInnerVertex = vector3(cos(angle) * a_fInnerRadius, sin(angle) * a_fInnerRadius, -a_fHeight / 2.0f);
		angle += angleChange;
		topOuterVertices.push_back(topOuterVertex);
		bottomOuterVertices.push_back(bottomOuterVertex);
		topInnerVertices.push_back(topInnerVertex);
		bottomInnerVertices.push_back(bottomInnerVertex);
	}

	//add triangles along outer sides
	for (int i = 0; i < a_nSubdivisions - 1; i++) {
		AddTri(topOuterVertices[i], bottomOuterVertices[i + 1], topOuterVertices[i + 1]);
		AddTri(topOuterVertices[i], bottomOuterVertices[i], bottomOuterVertices[i + 1]);
	}

	//add last triangles on outer side
	AddTri(topOuterVertices[a_nSubdivisions - 1], bottomOuterVertices[0], topOuterVertices[0]);
	AddTri(topOuterVertices[a_nSubdivisions - 1], bottomOuterVertices[a_nSubdivisions - 1], bottomOuterVertices[0]);

	//add triangles along inner sides
	for (int i = 0; i < a_nSubdivisions - 1; i++) {
		AddTri(topInnerVertices[i], topInnerVertices[i + 1], bottomInnerVertices[i + 1]);
		AddTri(topInnerVertices[i], bottomInnerVertices[i + 1], bottomInnerVertices[i]);
	}

	//add last triangles on inner side
	AddTri(topInnerVertices[a_nSubdivisions - 1], topInnerVertices[0], bottomInnerVertices[0]);
	AddTri(topInnerVertices[a_nSubdivisions - 1], bottomInnerVertices[0], bottomInnerVertices[a_nSubdivisions - 1]);


	//add triangles along top and bottom
	for (int i = 0; i < a_nSubdivisions - 1; i++) {
		AddTri(topOuterVertices[i], topOuterVertices[i + 1], topInnerVertices[i + 1]);
		AddTri(topOuterVertices[i], topInnerVertices[i + 1], topInnerVertices[i]);

		AddTri(bottomOuterVertices[i], bottomInnerVertices[i + 1], bottomOuterVertices[i + 1]);
		AddTri(bottomOuterVertices[i], bottomInnerVertices[i], bottomInnerVertices[i + 1]);
	}

	//add last top/bottom triangles
	AddTri(topOuterVertices[a_nSubdivisions - 1], topOuterVertices[0], topInnerVertices[0]);
	AddTri(topOuterVertices[a_nSubdivisions - 1], topInnerVertices[0], topInnerVertices[a_nSubdivisions - 1]);

	AddTri(bottomOuterVertices[a_nSubdivisions - 1], bottomInnerVertices[0], bottomOuterVertices[0]);
	AddTri(bottomOuterVertices[a_nSubdivisions - 1], bottomInnerVertices[a_nSubdivisions - 1], bottomInnerVertices[0]);

	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTorus(float a_fOuterRadius, float a_fInnerRadius, int a_nSubdivisionsA, int a_nSubdivisionsB, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_nSubdivisionsA < 3)
		a_nSubdivisionsA = 3;
	if (a_nSubdivisionsA > 360)
		a_nSubdivisionsA = 360;

	if (a_nSubdivisionsB < 3)
		a_nSubdivisionsB = 3;
	if (a_nSubdivisionsB > 360)
		a_nSubdivisionsB = 360;

	Release();
	Init();

	// Replace this with your code
	std::vector< std::vector<vector3>> vertices;
	std::vector<vector3> bottomVertices;
	float angleA = 0;
	float angleChangeA = (2 * PI) / a_nSubdivisionsA;
	float angleB = 0;
	float angleChangeB = (2 * PI) / a_nSubdivisionsB;

	//create vertices of top and bottom circles
	for (int i = 0; i < a_nSubdivisionsA; i++) {
		vertices.push_back(std::vector<vector3>());
		for (int j = 0; j < a_nSubdivisionsB; j++) {

			float zOffset = sin(angleB) * (a_fOuterRadius - a_fInnerRadius) / 2;
			//lerp between inner and outer radius based on cos of angleB
			float radialDistance = a_fInnerRadius + ((cos(angleB) + 1.0f)/ 2.0f) * (a_fOuterRadius - a_fInnerRadius);
			vector3 vertex = vector3(cos(angleA) * radialDistance, sin(angleA) * radialDistance, zOffset);

			vertices[i].push_back(vertex);
			angleB += angleChangeB;
		}
		angleA += angleChangeA;
	}

	for (int y = 0; y < a_nSubdivisionsA - 1; y++) {
		for (int i = 0; i < a_nSubdivisionsB - 1; i++) {
			AddTri(vertices[y][i], vertices[y + 1][i + 1], vertices[y][i + 1]);
			AddTri(vertices[y][i], vertices[y + 1][i], vertices[y + 1][i + 1]);
		}


		//add last triangles for each subring
		AddTri(vertices[y][a_nSubdivisionsA - 1], vertices[y + 1][0], vertices[y][0]);
		AddTri(vertices[y][a_nSubdivisionsA - 1], vertices[y + 1][a_nSubdivisionsA - 1], vertices[y + 1][0]);
	}

	//add last full ring
	for (int i = 0; i < a_nSubdivisionsB - 1; i++) {
		AddTri(vertices[a_nSubdivisionsA - 1][i], vertices[0][i + 1], vertices[a_nSubdivisionsA - 1][i + 1]);
		AddTri(vertices[a_nSubdivisionsA - 1][i], vertices[0][i], vertices[0][i + 1]);
	}

	AddTri(vertices[a_nSubdivisionsA - 1][a_nSubdivisionsB - 1], vertices[0][0], vertices[a_nSubdivisionsA - 1][0]);
	AddTri(vertices[a_nSubdivisionsA - 1][a_nSubdivisionsB - 1], vertices[0][a_nSubdivisionsB - 1], vertices[0][0]);

	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateSphere(float a_fRadius, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	//Sets minimum and maximum of subdivisions
	if (a_nSubdivisions < 1)
	{
		GenerateCube(a_fRadius * 2.0f, a_v3Color);
		return;
	}
	/*if (a_nSubdivisions > 6)
		a_nSubdivisions = 6;*/

	Release();
	Init();

	// Replace this with your code
	std::vector<std::vector<vector3>> vertices;
	vector3 topCenter = vector3(0, 0, a_fRadius);
	vector3 bottomCenter = vector3(0, 0, -a_fRadius);
	float angle = 0;
	float angleChange = (2 * PI) / a_nSubdivisions;
	float height = a_fRadius;
	float heightChange = 2 * a_fRadius / a_nSubdivisions;
	height -= heightChange / 2;
	//create vertices
	for (int i = 0; i < a_nSubdivisions; i++) {
		angle = 0;
		vertices.push_back(std::vector<vector3>());
		for (int j = 0; j < a_nSubdivisions; j++) {
			float widenessScale = std::sqrtf(a_fRadius * a_fRadius - height * height);
			vector3 vertex = vector3(cos(angle) * a_fRadius * widenessScale, sin(angle) * a_fRadius * widenessScale, height);
			angle += angleChange;
			vertices[i].push_back(vertex);
		}
		height -= heightChange;
	}

	//create triangles of caps
	//each triangle is between center and two neighboring points
	for (int i = 0; i < a_nSubdivisions - 1; i++) {
		AddTri(topCenter, vertices[0][i], vertices[0][i + 1]);
		AddTri(bottomCenter, vertices[a_nSubdivisions - 1][i + 1], vertices[a_nSubdivisions - 1][i]);
	}

	//add the last triangle of each cap
	AddTri(topCenter, vertices[0][a_nSubdivisions - 1], vertices[0][0]);
	AddTri(bottomCenter, vertices[a_nSubdivisions - 1][0], vertices[a_nSubdivisions - 1][a_nSubdivisions - 1]);

	//add triangles along sides
	for (int y = 0; y < a_nSubdivisions - 1; y++) {
		for (int i = 0; i < a_nSubdivisions - 1; i++) {
			AddTri(vertices[y][i], vertices[y + 1][i + 1], vertices[y][i + 1]);
			AddTri(vertices[y][i], vertices[y + 1][i], vertices[y + 1][i + 1]);
		}


		//add last triangles on side
		AddTri(vertices[y][a_nSubdivisions - 1], vertices[y + 1][0], vertices[y][0]);
		AddTri(vertices[y][a_nSubdivisions - 1], vertices[y + 1][a_nSubdivisions - 1], vertices[y + 1][0]);
	}

	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}