#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include "tiny_obj_loader.h"

#include <iostream>
#include <vector>
#include <fstream>

using namespace std;

int main(int argc, char** argv)
{
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  
  ofstream mFile;
  mFile.open(argv[2]);

  std::string err;
  bool ret = tinyobj::LoadObj(shapes, materials, err, argv[1]);
  
  if (!err.empty()) { // `err` may contain warning message.
    std::cerr << err << std::endl;
  }
  
  if (!ret) {
    exit(1);
  }

  mFile << "SBT-raytracer 1.0\ncamera {\n  position = (10,10,1.25);\n  viewdir = (-1,-0.5,-0.25);\n  aspectratio = 1.777;\n  updir = (0,1,0);\n}\n\npoint_light {\n  position = (1.0, 1.75, 1.1);\n  color = (.5, .5, .5);\n  constant_attenuation_coeff= 0.25;\n  linear_attenuation_coeff = 0.003372407;\n  quadratic_attenuation_coeff = 0.000045492;\n  }\npoint_light {\n  position = (1.0, 8.75, -1.1);\n  color = (0.282, 0.239, 0.545);\n  constant_attenuation_coeff= 0.25;\n  linear_attenuation_coeff = 0.003372407;\n  quadratic_attenuation_coeff = 0.000045492;\n  }\n";
  //Else successful now add the objects to the scene
  for(int i = 0; i < shapes.size(); i++){
    //Will only support one material per object
    // Get Materials
    mFile << "polymesh {\n name=\"" << shapes[i].name <<"\";\n";
    if(shapes[i].name.find("glass") != std::string::npos)
      mFile << " material={\n  diffuse=( 0.01,0.01,0.01);\n  ambient=( 0.5,0.5,0.5);\n  specular=( 0.9,0.9,0.9);\n  reflective=( 0.2, 0.2, 0.2);\n  emissive=( 0,0,0);\n  shininess=125.0;\n  transmissive=( 0.7,0.7,0.7 );\n  index = 1.5;\n};\n";
    else
      mFile << " material={\n  diffuse=( 0.345880, 0.309265, 0.240385);\n  ambient=( 0,0,0);\n  specular=( 0.167958, 0.192770, 0.273011);\n  reflective=( 0, 0, 0);\n  emissive=( 0,0,0);\n  shininess=35.294118;\n  transmissive=( 0,0,0 );\n};\n";

    mFile << " points=(\n";
    for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++) {
    //First Add Vertices
      mFile << "  ( " << shapes[i].mesh.positions[3*v+0] <<", "<< shapes[i].mesh.positions[3*v+1] <<", "<< shapes[i].mesh.positions[3*v+2] << " )";
      if(v < shapes[i].mesh.positions.size()/3 - 1) mFile << ",";
      mFile << "\n";
    }
    mFile << " );\n";
    mFile << " faces=(\n";

    for (size_t f = 0; f < shapes[i].mesh.indices.size() / 3; f++) {
    //Then Add Faces
      mFile << "  ( " << shapes[i].mesh.indices[3*f+0] << ", "<< shapes[i].mesh.indices[3*f+1]<< ", "<< shapes[i].mesh.indices[3*f+2]<< " )";
      if(f < shapes[i].mesh.indices.size()/3 - 1) mFile << ",";
      mFile << "\n";
    }
    mFile << " );\n";
    mFile << " gennormals;\n}\n";

  }

  mFile.close();
}