#ifndef SphereSelectionTool_H
#define SphereSelectionTool_H

#include "OpenGL.h"
#include "Vec3.h"
#include <vector>

void glVertex(Vec3 const & p) {
    glVertex3f(p[0] , p[1] , p[2]);
}
void glNormal(Vec3 const & p) {
    glNormal3f(p[0] , p[1] , p[2]);
}

struct SphereSelectionTool {
    Vec3 center;
    float radius;
    bool isActive;
    bool isAdding;

    SphereSelectionTool() : radius(0.1f), isActive(false) {}

    void initSphere(float x, float y, float z) {
        center[0] = x;
        center[1] = y;
        center[2] = z;
        radius = 0.1f;
    }

    void setCenter(Vec3 center) {
        this->center = center;
    }

    void setRadius(float radius) {
        this->radius = radius;
    }

    bool contains(Vec3 point) {
        float distance = (point - center).length();
        return distance <= radius;
    }

    void draw() {

        if(!this->isActive) return;

        int slices = 20, stacks = 20;
        int Nb = slices*stacks +2;
        std::vector<Vec3> points(Nb);

        float sinP , cosP , sinT , cosT , Phi , Theta;
        points[0] = Vec3(0,0,1);
        points[Nb-1] = Vec3(0,0,-1);

        for(int i=1; i<=stacks; i++)
        {
            Phi = 90 - (float)(i*180)/(float)(stacks+1);
            sinP = sinf(Phi*3.14159265/180);
            cosP = cosf(Phi*3.14159265/180);

            for(int j=1; j<=slices; j++)
            {
                Theta = (float)(j*360)/(float)(slices);
                sinT = sinf(Theta*3.14159265/180);
                cosT = cosf(Theta*3.14159265/180);

                points[ j + (i-1)*slices ] = Vec3(cosT*cosP,sinT*cosP,sinP);
            }
        }

        int k1,k2;
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glBegin(GL_TRIANGLES);
        for(int i=1; i<=slices; i++)
        {
            k1 = i;
            k2 = (i%slices+1);
            glNormal(points[0]);
            glVertex((this->center + this->radius*points[0]));
            glNormal(points[k1]);
            glVertex((this->center + this->radius*points[k1]));
            glNormal(points[k2]);
            glVertex((this->center + this->radius*points[k2]));

            k1 = (stacks-1)*slices+i;
            k2 = (stacks-1)*slices+(i%slices+1);
            glNormal(points[k1]);
            glVertex((this->center + this->radius*points[k1]));
            glNormal(points[Nb-1]);
            glVertex((this->center + this->radius*points[Nb-1]));
            glNormal(points[k2]);
            glVertex((this->center + this->radius*points[k2]));
        }
        glEnd();

        glBegin(GL_QUADS);
        for(int j=1; j<stacks; j++)
        {
            for(int i=1; i<=slices; i++)
            {
                k1 = i + (j-1)*slices;
                k2 = (i%slices+1) + (j-1)*slices;
                glNormal(points[k2]);
                glVertex((this->center + this->radius*points[k2]));
                glNormal(points[k1]);
                glVertex((this->center + this->radius*points[k1]));

                k1 = i + (j)*slices;
                k2 = (i%slices+1) + (j)*slices;
                glNormal(points[k1]);
                glVertex((this->center + this->radius*points[k1]));
                glNormal(points[k2]);
                glVertex((this->center + this->radius*points[k2]));
            }
        }
        glEnd();
    }
};

#endif
