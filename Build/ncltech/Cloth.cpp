#include "Cloth.h"
#include "PhysicsEngine.h"


Cloth::Cloth(GLint dim, GLfloat len) 
	: Mesh(), 
	m_dimension(dim), m_length(len), m_tileSize(len / (dim - 1)),
	m_numPoints(dim * dim)
{
	
	m_acceleration = new Vector3[m_numPoints];
	m_currentPosition = new Vector3[m_numPoints];
	m_oldPosition = new Vector3[m_numPoints];

	for (int i = 0; i < m_numPoints; ++i) {
		m_acceleration[i] = Vector3(0.0f);
		m_currentPosition[i] = Vector3(0.0f);
		m_oldPosition[i] = Vector3(0.0f);
	}

	BuildMesh();
	BufferData();
}


Cloth::~Cloth()
{
	SAFE_DELETE(m_acceleration);
	SAFE_DELETE(m_currentPosition);
	SAFE_DELETE(m_oldPosition);
}



void Cloth::BuildMesh()
{
	// Shift coordinates by an offset to keep centred at origin
	GLfloat xOffset = m_length / 2.0f;
	GLfloat zOffset = m_length / 2.0f;

	numVertices = m_dimension * m_dimension;
	type = GL_TRIANGLES;

	vertices = new Vector3[numVertices];

	//textureCoords = new Vector2[numVertices]; Can live without texture coordinates for now


	// x increases with columns, y is up, z increases with rows
	int index = 0;
	for (int row = 0; row != m_dimension; ++row)
		for (int col = 0; col != m_dimension; ++col)
		{
			Vector3 pos = Vector3(col * m_tileSize - xOffset, 1.0f, row * m_tileSize - zOffset);
			vertices[index] = pos;
			m_currentPosition[index] = pos;
			m_oldPosition[index++] = pos;
			//m_clothVertices.push_back(glm::vec3(col * m_tileSize - xOffset, 1.0f, row * m_tileSize - zOffset));
		}



	// Setup indices for quads
	index = 0;
	//numIndices = 6 * m_dimension * m_dimension - 12 * m_dimension - 1;
	numIndices = 5766;// Unexpected number : Hardcode for now
	indices = new unsigned int[numIndices];

	for (GLint i = 0; i < m_dimension * m_dimension - m_dimension; ++i)
	{
		if ((i + 1) % m_dimension) // Not for bottom row
		{
			// Triangle 1
			indices[index++] = i;
			indices[index++] = i + 1;
			indices[index++] = i + m_dimension;
			//m_clothVertexIndices.push_back(i);
			//m_clothVertexIndices.push_back(i + 1);
			//m_clothVertexIndices.push_back(i + m_dimension);
		}


		if (i % m_dimension) // Not for top row
		{
			// Triangle 2
			indices[index++] = i;
			indices[index++] = i + m_dimension - 1;
			indices[index++] = i + m_dimension;
			//m_clothVertexIndices.push_back(i);
			//m_clothVertexIndices.push_back(i + m_dimension - 1);
			//m_clothVertexIndices.push_back(i + m_dimension);
		}

	}


}

void Cloth::Update(float dt) {
	AccumulateForces(dt);
	Integrate(dt);
	Rebuffer();
}


void Cloth::Draw() {
	if (m_wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	Mesh::Draw();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Cloth::AccumulateForces(float dt) {
	Vector3 gravity = GRAVITY;
	for (int i = 0; i < m_numPoints; ++i) {
		m_acceleration[i] = gravity;
	}
}


void Cloth::Integrate(float dt) {
	// Using Verlet should move to PhysicsNode really but doesn't sit so elegantly in the framework
	for (int i = 0; i < m_numPoints; ++i)
	{
		Vector3 temp = m_currentPosition[i]; // Need to store old value
		m_currentPosition[i] += (m_currentPosition[i] - m_oldPosition[i] + 
			m_acceleration[i] * dt * dt) * m_energyLossFactor;

		m_oldPosition[i] = temp;

	}
}

void Cloth::Rebuffer() {
	glBindBuffer(GL_ARRAY_BUFFER, bufferObject[VERTEX_BUFFER]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_numPoints * sizeof(Vector3), m_currentPosition);
}