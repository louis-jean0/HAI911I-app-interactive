#include "mesh.h"

// Constructeur
Mesh::Mesh() : m_vertexBuffer(QOpenGLBuffer::VertexBuffer), m_indexBuffer(QOpenGLBuffer::IndexBuffer)
{

}

// Destructeur
Mesh::~Mesh()
{
    m_vertexBuffer.destroy();
    m_indexBuffer.destroy();
    m_vao.destroy();
}

// Méthode pour construire un cube avec vertices et indices
void Mesh::buildCube() {
    // Clear previous data
    m_data.clear();
    m_indexes.clear();

    // Define the normals for each face
    QVector3D normals[] = {
        QVector3D( 0.0f,  0.0f,  1.0f), // Front face
        QVector3D( 0.0f,  0.0f, -1.0f), // Back face
        QVector3D(-1.0f,  0.0f,  0.0f), // Left face
        QVector3D( 1.0f,  0.0f,  0.0f), // Right face
        QVector3D( 0.0f,  1.0f,  0.0f), // Top face
        QVector3D( 0.0f, -1.0f,  0.0f)  // Bottom face
    };

    // Define the positions for each face (4 vertices per face)
    QVector<QVector3D> positions[] = {
        // Front face (z = 0.5f)
        { QVector3D(-0.5f, -0.5f,  0.5f), // Bottom-left
          QVector3D( 0.5f, -0.5f,  0.5f), // Bottom-right
          QVector3D( 0.5f,  0.5f,  0.5f), // Top-right
          QVector3D(-0.5f,  0.5f,  0.5f)  // Top-left
        },
        // Back face (z = -0.5f)
        { QVector3D(-0.5f, -0.5f, -0.5f),
          QVector3D(-0.5f,  0.5f, -0.5f),
          QVector3D( 0.5f,  0.5f, -0.5f),
          QVector3D( 0.5f, -0.5f, -0.5f)
        },
        // Left face (x = -0.5f)
        { QVector3D(-0.5f, -0.5f, -0.5f),
          QVector3D(-0.5f, -0.5f,  0.5f),
          QVector3D(-0.5f,  0.5f,  0.5f),
          QVector3D(-0.5f,  0.5f, -0.5f)
        },
        // Right face (x = 0.5f)
        { QVector3D(0.5f, -0.5f, -0.5f),
          QVector3D(0.5f,  0.5f, -0.5f),
          QVector3D(0.5f,  0.5f,  0.5f),
          QVector3D(0.5f, -0.5f,  0.5f)
        },
        // Top face (y = 0.5f)
        { QVector3D(-0.5f, 0.5f, -0.5f),
          QVector3D(-0.5f, 0.5f,  0.5f),
          QVector3D( 0.5f, 0.5f,  0.5f),
          QVector3D( 0.5f, 0.5f, -0.5f)
        },
        // Bottom face (y = -0.5f)
        { QVector3D(-0.5f, -0.5f, -0.5f),
          QVector3D( 0.5f, -0.5f, -0.5f),
          QVector3D( 0.5f, -0.5f,  0.5f),
          QVector3D(-0.5f, -0.5f,  0.5f)
        }
    };

    // Build the vertex data and indices
    for (int face = 0; face < 6; ++face) {
        QVector3D normal = normals[face];
        QVector<QVector3D> &facePositions = positions[face];

        // Add vertices for the current face
        for (const QVector3D &pos : facePositions) {
            // Position
            m_data.append(pos.x());
            m_data.append(pos.y());
            m_data.append(pos.z());
            m_data.append(1.0f); // w component

            // Normal
            m_data.append(normal.x());
            m_data.append(normal.y());
            m_data.append(normal.z());
        }

        // Calculate indices for two triangles of the face
        int startIndex = face * 4; // 4 vertices per face
        m_indexes.append(startIndex);
        m_indexes.append(startIndex + 1);
        m_indexes.append(startIndex + 2);

        m_indexes.append(startIndex);
        m_indexes.append(startIndex + 2);
        m_indexes.append(startIndex + 3);
    }
}

void Mesh::loadMeshFromFile(std::string fileName) {
        std::ifstream myfile;
        myfile.open(fileName.c_str());
        if (!myfile.is_open())
        {
            std::cout << fileName << " cannot be opened" << std::endl;
            return;
        }

        std::string magic_s;
        int iLine = 0;

        myfile >> magic_s;
        iLine++;

        if( magic_s != "OFF" )
        {
            std::cout << magic_s << " != OFF :   We handle ONLY *.off files." << std::endl;
            myfile.close();
            exit(1);
        }

        int n_vertices , n_faces , dummy_int;
        myfile >> n_vertices >> n_faces >> dummy_int;
        iLine++;

        m_data.clear();

        for( int v = 0 ; v < n_vertices ; ++v )
        {
            float x , y , z ;
            myfile >> x >> y >> z ;

            // Positions
            m_data.append(x);
            m_data.append(y);
            m_data.append(z);
            m_data.append(1.0f);

            // Normales (affectées de la position car la flemme de calculer les normales pour chaque sommet)
            m_data.append(x);
            m_data.append(y);
            m_data.append(z);
            iLine++;
        }

        m_indexes.clear();

        std::vector<QVector3D> normals;

        for( int f = 0 ; f < n_faces ; ++f )
        {
            int n_vertices_on_face;
            myfile >> n_vertices_on_face;

            if( n_vertices_on_face == 3 )
            {
                unsigned int _v1 , _v2 , _v3;
                myfile >> _v1 >> _v2 >> _v3;

                m_indexes.append(_v1);
                m_indexes.append(_v2);
                m_indexes.append(_v3);
            }
            else if (n_vertices_on_face == 4) {
                unsigned int _v1, _v2, _v3, _v4;
                myfile >> _v1 >> _v2 >> _v3 >> _v4;

                m_indexes.append(_v1);
                m_indexes.append(_v2);
                m_indexes.append(_v3);
                m_indexes.append(_v1);
                m_indexes.append(_v3);
                m_indexes.append(_v4);
            }
            else
            {
                std::cout << "We handle ONLY *.off files with 3 or 4 vertices per face (here = " << n_vertices_on_face << " on line " << iLine << ")" << std::endl;
                myfile.close();
                exit(1);
            }
            iLine++;
        }
}

void Mesh::initializeBuffers(QOpenGLShaderProgram *program) {
    if(!m_vao.isCreated()) m_vao.create();
    m_vao.bind();

    m_vertexBuffer.create();
    m_vertexBuffer.bind();
    m_vertexBuffer.allocate(m_data.constData(), m_data.size() * sizeof(GLfloat));

    program->enableAttributeArray(0);
    program->setAttributeBuffer(0, GL_FLOAT, 0, 3, 7 * sizeof(GLfloat));
    program->enableAttributeArray(1);
    program->setAttributeBuffer(1, GL_FLOAT, 4 * sizeof(GLfloat), 3, 7 * sizeof(GLfloat));

    m_indexBuffer.create();
    m_indexBuffer.bind();
    m_indexBuffer.allocate(m_indexes.constData(), m_indexes.size() * sizeof(GLuint));

    m_vao.release();
}

void Mesh::render() {
    m_vao.bind();
    glDrawElements(GL_TRIANGLES, m_indexes.size(), GL_UNSIGNED_INT, 0);
    m_vao.release();
}
