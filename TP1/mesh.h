#ifndef MESH_H
#define MESH_H

#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QVector>
#include <QVector3D>
#include <fstream>
#include <string>
#include <iostream>

class Mesh
{
public:
    Mesh();
    ~Mesh();

    void buildCube();
    void loadMeshFromFile(std::string fileName);
    void initializeBuffers(QOpenGLShaderProgram* program);
    void render(QOpenGLShaderProgram* program);

private:
    QVector<GLfloat> m_data;
    QVector<GLuint> m_indexes;
    QOpenGLBuffer m_vertexBuffer;
    QOpenGLBuffer m_indexBuffer;
    QOpenGLVertexArrayObject m_vao;
};

#endif // MESH_H
