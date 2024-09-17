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
void Mesh::buildCube()
{
    // Définir les 8 sommets du cube (unité, centré à l'origine)
    QVector<QVector3D> vertices = {
        QVector3D(-0.5f, -0.5f, -0.5f), // 0
        QVector3D( 0.5f, -0.5f, -0.5f), // 1
        QVector3D( 0.5f,  0.5f, -0.5f), // 2
        QVector3D(-0.5f,  0.5f, -0.5f), // 3
        QVector3D(-0.5f, -0.5f,  0.5f), // 4
        QVector3D( 0.5f, -0.5f,  0.5f), // 5
        QVector3D( 0.5f,  0.5f,  0.5f), // 6
        QVector3D(-0.5f,  0.5f,  0.5f)  // 7
    };

    // Ajouter les vertices au buffer
    for (const QVector3D &v : vertices) {
        m_data.append(v.x());
        m_data.append(v.y());
        m_data.append(v.z());
        m_data.append(1.0f);

        m_data.append(0.0f);
        m_data.append(0.0f);
        m_data.append(1.0f);
    }

    // Définir les 12 triangles (36 indices pour les triangles)
    m_indexes = {
        0, 1, 2, 2, 3, 0, // Face avant
        4, 5, 6, 6, 7, 4, // Face arrière
        0, 4, 7, 7, 3, 0, // Face gauche
        1, 5, 6, 6, 2, 1, // Face droite
        3, 2, 6, 6, 7, 3, // Face haut
        0, 1, 5, 5, 4, 0  // Face bas
    };
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
            m_data.append(x);
            m_data.append(y);
            m_data.append(z);
            m_data.append(1.0f);

            m_data.append(0.0f);
            m_data.append(0.0f);
            m_data.append(1.0f);

            iLine++;
        }

        m_indexes.clear();

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

void Mesh::render(QOpenGLShaderProgram *program) {
    Q_UNUSED(program);
    m_vao.bind();
    glDrawElements(GL_TRIANGLES, m_indexes.size(), GL_UNSIGNED_INT, 0);
    m_vao.release();
}
