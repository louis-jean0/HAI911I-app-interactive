#include "Mesh.h"
#include <iostream>
#include <fstream>
#include <cmath>

void Mesh::loadOFF (const std::string & filename) {
    std::ifstream in (filename.c_str ());
    if (!in)
        exit (EXIT_FAILURE);
    std::string offString;
    unsigned int sizeV, sizeT, tmp;
    in >> offString >> sizeV >> sizeT >> tmp;
    V.resize (sizeV);
    T.resize (sizeT);
    for (unsigned int i = 0; i < sizeV; i++)
        in >> V[i].p;
    int s;
    for (unsigned int i = 0; i < sizeT; i++) {
        in >> s;
        for (unsigned int j = 0; j < 3; j++)
            in >> T[i].v[j];
    }
    in.close ();
    recomputeNormals ();
}

void Mesh::recomputeNormals () {
    for (unsigned int i = 0; i < V.size (); i++)
        V[i].n = Vec3 (0.0, 0.0, 0.0);
    for (unsigned int i = 0; i < T.size (); i++) {
        Vec3 e01 = V[T[i].v[1]].p -  V[T[i].v[0]].p;
        Vec3 e02 = V[T[i].v[2]].p -  V[T[i].v[0]].p;
        Vec3 n = Vec3::cross (e01, e02);
        n.normalize ();
        for (unsigned int j = 0; j < 3; j++)
            V[T[i].v[j]].n += n;
    }
    for (unsigned int i = 0; i < V.size (); i++)
        V[i].n.normalize ();
}

void Mesh::compute_skinning_weights( Skeleton & skeleton ) {
    //---------------------------------------------------//
    //---------------------------------------------------//
    // code to change :

    // Indications:
    // you should compute weights for each vertex w.r.t. the skeleton bones
    // so each vertex will have B weights (B = number of bones)
    // these weights shoud be stored in vertex.w:


    //---------------------------------------------------//
    //---------------------------------------------------//
    //---------------------------------------------------//

    int nbVertices = V.size();
    int nbBones = skeleton.bones.size();

    for(int i = 0; i < nbVertices; ++i) {
        MeshVertex &vertex = V[i];
        Vec3 p = vertex.p;
        double sum = 0.0;
        for(int j = 0; j < nbBones; ++j) {
            Bone &bone = skeleton.bones[j];
            Vec3 p0 = skeleton.articulations[bone.joints[0]].p;
            Vec3 p1 = skeleton.articulations[bone.joints[1]].p;

            Vec3 p0p = p - p0;
            Vec3 p0p1 = p1 - p0; 
            float t = Vec3::dot(p0p, p0p1) / Vec3::dot(p0p1, p0p1);
            float d;
            if(t < 0) {
                d = p0p.length();
            }
            else if(t > 1) {
                d = (p - p1).length();
            }
            else {
                Vec3 proj = p0 + t * p0p1;
                Vec3 pproj = proj - p;
                d = pproj.length();
            }
            double weight = pow(1/d, 5);
            sum += weight;
            vertex.w.push_back(weight);
        }
        for(int j = 0; j < nbBones; ++j) {
            vertex.w[j] /= sum;
        }
    }
}

void Mesh::draw( int displayedBone ) const {

    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
    glBegin (GL_TRIANGLES);
    for (unsigned int i = 0; i < T.size (); i++)
        for (unsigned int j = 0; j < 3; j++) {
            const MeshVertex & v = V[T[i].v[j]];
            if( displayedBone >= 0 && displayedBone < v.w.size() ) {
                Vec3 rgb = HSVtoRGB( v.w[displayedBone], 0.8,0.8 );
                glColor3f( rgb[0], rgb[1], rgb[2] );
            } else glColor3f( 0.6, 0.6, 0.6 );
            glNormal3f (v.n[0], v.n[1], v.n[2]);
            glVertex3f (v.p[0], v.p[1], v.p[2]);
        }

    glEnd ();
}

void Mesh::drawTransformedMesh( SkeletonTransformation & transfo ) const {


    std::vector< Vec3 > new_positions( V.size() );
    std::vector< Vec3 > new_normals( V.size() );

    //---------------------------------------------------//
    //---------------------------------------------------//
    // code to change :
    for( unsigned int i = 0 ; i < V.size() ; ++i ) {
        Vec3 p = V[i].p;
        Vec3 n = V[i].n;
        int nbBones = V[i].w.size();

        // Indications:
        // you should use the skinning weights to blend the transformations of the vertex position by the bones.
        Vec3 sum_position(0.0,0.0,0.0);
        Mat3 sum_rotation;
        Vec3 sum_normal(0.0,0.0,0.0);
        for(int j = 0; j < nbBones; ++j) {
            double weight = V[i].w[j];
            sum_position += weight * (transfo.bone_transformations[j].world_space_rotation * p + transfo.bone_transformations[j].world_space_translation);
            sum_rotation += weight * transfo.bone_transformations[j].world_space_rotation;
        }
        new_positions[i] = sum_position;
        sum_normal = Mat3::inverse(sum_rotation).getTranspose() * n;
        sum_normal.normalize();
        new_normals[i] = sum_normal;
    }
    //---------------------------------------------------//
    //---------------------------------------------------//
    //---------------------------------------------------//

    glEnable(GL_LIGHTING);
    glBegin (GL_TRIANGLES);
    for (unsigned int i = 0; i < T.size (); i++)
        for (unsigned int j = 0; j < 3; j++) {
            const MeshVertex & v = V[T[i].v[j]];
            Vec3 p = new_positions[ T[i].v[j] ];
            Vec3 n = new_normals[ T[i].v[j] ];
            glColor3f( 0.6, 0.6, 0.6 );
            glNormal3f (n[0], n[1], n[2]);
            glVertex3f (p[0], p[1], p[2]);
        }
    glEnd ();
}

/*! \brief Convert HSV to RGB color space

  Converts a given set of HSV values `h', `s', `v' into RGB
  coordinates. The output RGB values are in the range [0, 1], and
  the input HSV values are in the ranges h = [0, 360], and s, v =
  [0, 1], respectively.

  \param fH Hue component, used as input, range: [0, 1]
  \param fS Hue component, used as input, range: [0, 1]
  \param fV Hue component, used as input, range: [0, 1]

  \param fR Red component, used as output, range: [0, 1]
  \param fG Green component, used as output, range: [0, 1]
  \param fB Blue component, used as output, range: [0, 1]

*/
Vec3 Mesh::HSVtoRGB( float fH, float fS, float fV) const {

    fH=(1.-fH)*0.65*360.;

    float fR, fG, fB;
    float fC = fV * fS; // Chroma
    float fHPrime = fmod(fH / 60.0, 6);
    float fX = fC * (1 - fabs(fmod(fHPrime, 2) - 1));
    float fM = fV - fC;

    if(0 <= fHPrime && fHPrime < 1) {
        fR = fC;
        fG = fX;
        fB = 0;
    } else if(1 <= fHPrime && fHPrime < 2) {
        fR = fX;
        fG = fC;
        fB = 0;
    } else if(2 <= fHPrime && fHPrime < 3) {
        fR = 0;
        fG = fC;
        fB = fX;
    } else if(3 <= fHPrime && fHPrime < 4) {
        fR = 0;
        fG = fX;
        fB = fC;
    } else if(4 <= fHPrime && fHPrime < 5) {
        fR = fX;
        fG = 0;
        fB = fC;
    } else if(5 <= fHPrime && fHPrime < 6) {
        fR = fC;
        fG = 0;
        fB = fX;
    } else {
        fR = 0;
        fG = 0;
        fB = 0;
    }

    fR += fM;
    fG += fM;
    fB += fM;
    return Vec3(fR,fG,fB);
}
