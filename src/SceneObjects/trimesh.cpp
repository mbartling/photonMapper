#include <cmath>
#include <float.h>
#include <algorithm>
#include <assert.h>
#include "trimesh.h"
#include "../ui/TraceUI.h"
extern TraceUI* traceUI;

using namespace std;
#define MYMETHOD 0

const int kDDepth = 15;
Trimesh::~Trimesh()
{
	for( Materials::iterator i = materials.begin(); i != materials.end(); ++i )
		delete *i;
}

// must add vertices, normals, and materials IN ORDER
void Trimesh::addVertex( const Vec3d &v )
{
    vertices.push_back( v );
}

void Trimesh::addMaterial( Material *m )
{
    materials.push_back( m );
}

void Trimesh::addNormal( const Vec3d &n )
{
    normals.push_back( n );
}

// Returns false if the vertices a,b,c don't all exist
bool Trimesh::addFace( int a, int b, int c )
{
    int vcnt = vertices.size();

    if( a >= vcnt || b >= vcnt || c >= vcnt ) return false;

    TrimeshFace *newFace = new TrimeshFace( scene, new Material(*this->material), this, a, b, c );
    newFace->setTransform(this->transform);
    if (!newFace->degen) faces.push_back( newFace );


    // Don't add faces to the scene's object list so we can cull by bounding box
    scene->add(newFace);
    return true;
}

char* Trimesh::doubleCheck()
// Check to make sure that if we have per-vertex materials or normals
// they are the right number.
{
    if( !materials.empty() && materials.size() != vertices.size() )
        return "Bad Trimesh: Wrong number of materials.";
    if( !normals.empty() && normals.size() != vertices.size() )
        return "Bad Trimesh: Wrong number of normals.";

    return 0;
}
bool Trimesh::hasNormals(){
    if( !normals.empty() && normals.size() == vertices.size() ){
        return true;
    }
    else return false;
}
bool Trimesh::hasVertexMaterials(){
    if( !materials.empty() && materials.size() == vertices.size() ){
        return true;
    }
    else return false;
}
void Trimesh::buildKdTree(void){
    BoundingBox localBounds = ComputeLocalBoundingBox();

    kdtree.buildTree(faces, 25, localBounds);
    std::cout << "Done Building Trimesh Tree" << std::endl;
}

bool Trimesh::intersectLocal(ray& r, isect& i) const
{
	double tmin = 0.0;
	double tmax = 0.0;
	typedef Faces::const_iterator iter;
	bool have_one = false;
	// for( iter j = faces.begin(); j != faces.end(); ++j )
	//   {
	//     isect cur;
	//     if( (*j)->intersectLocal( r, cur ) )
	//       {
	// 	if( !have_one || (cur.t < i.t) )
	// 	  {
	// 	    i = cur;
	// 	    have_one = true;
	// 	  }
	//       }
	//   }
    have_one = kdtree.intersectLocal(r, i);

	if( !have_one ) i.setT(1000.0);
	return have_one;
}

// bool TrimeshFace::intersect(ray& r, isect& i) const {
//   return intersectLocal(r, i);
// }

// Intersect ray r with the triangle abc.  If it hits returns true,
// and put the parameter in t and the barycentric coordinates of the
// intersection in u (alpha) and v (beta).
bool TrimeshFace::intersectLocal(ray& r, isect& i) const
{

    const Vec3d& a = parent->vertices[ids[0]];
    const Vec3d& b = parent->vertices[ids[1]];
    const Vec3d& c = parent->vertices[ids[2]];

    // YOUR CODE HERE
    // RAY_EPSILON defined in Ray.h
    /*
      -DxAO = AOxD
      AOx-D = -(-DxAO)
      |-D AB AC| = -D*(ABxAC) = -D*normal = 1. 1x
      |AO AB AC| = AO*(ABxAC) = AO*normal = 1. 
      |-D AO AC| = -D*(AOxAC) = 1. 1x || AC*(-DxAO) = AC*M = 1. 1x
      |-D AB AO| = -D*(ABxAO) = 1. 1x || (AOx-D)*AB = (DxAO)*AB = -M*AB = 1.
    */
    double mDet;
    double mDetInv;
    double alpha;
    double beta;
    double t;
    Vec3d rDir = r.getDirection();
    //Moller-Trombore approach is a change of coordinates into a local uv space
    // local to the triangle
    Vec3d AB = b - a;
    Vec3d AC = c - a;

    #if MYMETHOD
        mDet     = -rDir*normal*mNorm;
        Vec3d AO;
        Vec3d M;
        // Ray and triangle approximately parallel
        if(fabs(mDet) < RAY_EPSILON ) return false;
        
        AO  = r.getPosition() - a;
        M =  AO ^ rDir;
        mDetInv = 1/mDet;
        alpha = (M * AC) * mDetInv;
        if(alpha < 0 || alpha > 1) return false;
    
        beta = -M * AB * mDetInv;
        if(beta < 0 || alpha + beta > 1) return false;
    
        t = AO * normal * mDetInv;

    #else
        // if (normal * -r.getDirection() < 0) return false;
        Vec3d P = rDir ^ AC;
        mDet = AB * P;
        if(fabs(mDet) < RAY_EPSILON ) return false;
    
        mDetInv = 1/mDet;
        Vec3d T = r.getPosition() - a;
        alpha = T * P * mDetInv;    
        if(alpha < 0 || alpha > 1) return false;
    
        Vec3d Q = T ^ AB;
        beta = rDir * Q * mDetInv;
        if(beta < 0 || alpha + beta > 1) return false;
        t = AC * Q * mDetInv;

    #endif
    if(fabs(t) < RAY_EPSILON) return false; // Jaysus this sucked
    i.setBary(1 - (alpha + beta), alpha, beta);
    i.setT(t);
    

    // std::cout << traceUI->smShadSw() << std::endl; 
    // if(traceUI->smShadSw() && !parent->doubleCheck()){
    if(traceUI->smShadSw() && parent->hasNormals()){

    // if(traceUI->smShadSw()){
        // std::cout<<"Smooth Shading" << std::endl;
        Vec3d aN = parent->normals[ids[0]];
        Vec3d bN = parent->normals[ids[1]];
        Vec3d cN = parent->normals[ids[2]];
        i.N = (1 - (alpha + beta))*aN + \
         alpha*bN + \
         beta*cN;

    }   
    else{
        i.N = normal;
        // i.N = ((AB) ^ (c_coords - a_coords));

    }
    i.N.normalize();

    i.obj = this;
    if(!parent->materials.empty() && parent->hasVertexMaterials()){
        Material aM;
        aM += (1 - (alpha + beta))*(*parent->materials[ids[0]]); 
        aM += alpha*(*parent->materials[ids[1]]); 
        aM += beta*(*parent->materials[ids[2]]); 
        // std::cout << "HAHAHAH" << std::endl;
        i.setMaterial(aM);
    } else{
        i.setMaterial(this->getMaterial());
    }

    return true;

    // return false;
}

void Trimesh::generateNormals()
// Once you've loaded all the verts and faces, we can generate per
// vertex normals by averaging the normals of the neighboring faces.
{
    int cnt = vertices.size();
    normals.resize( cnt );
    int *numFaces = new int[ cnt ]; // the number of faces assoc. with each vertex
    memset( numFaces, 0, sizeof(int)*cnt );
    
    for( Faces::iterator fi = faces.begin(); fi != faces.end(); ++fi )
    {
		Vec3d faceNormal = (**fi).getNormal();
        
        for( int i = 0; i < 3; ++i )
        {
            normals[(**fi)[i]] += faceNormal;
            ++numFaces[(**fi)[i]];
        }
    }

    for( int i = 0; i < cnt; ++i )
    {
        if( numFaces[i] )
            normals[i]  /= numFaces[i];
    }

    delete [] numFaces;
    vertNorms = true;
}
