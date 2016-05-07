// The main ray tracer.

#pragma warning (disable: 4786)

#include "RayTracer.h"
#include "scene/light.h"
#include "scene/material.h"
#include "scene/ray.h"

#include "parser/Tokenizer.h"
#include "parser/Parser.h"

#include "ui/TraceUI.h"
#include <cmath>
#include <algorithm>

extern TraceUI* traceUI;

#include <iostream>
#include <fstream>
#include <stdlib.h>

using namespace std;

// Use this variable to decide if you want to print out
// debugging messages.  Gets set in the "trace single ray" mode
// in TraceGLWindow, for example.
bool debugMode = false;
// bool DISTRIBUTION_TRACING = true;
// int SuperSampleX = 0;

// Trace a top-level ray through pixel(i,j), i.e. normalized window coordinates (x,y),
// through the projection plane, and out into the scene.  All we do is
// enter the main ray-tracing method, getting things started by plugging
// in an initial ray weight of (0.0,0.0,0.0) and an initial recursion depth of 0.

std::pair<Vec3d,Vec3d> RayTracer::trace(double x, double y)
{
  // Clear out the ray cache in the scene for debugging purposes,
  if (TraceUI::m_debug) scene->intersectCache.clear();
  ray r(Vec3d(0,0,0), Vec3d(0,0,0), ray::VISIBILITY);
  scene->getCamera().rayThrough(x,y,r);
  std::pair<Vec3d, Vec3d> mPair = traceRay(r, traceUI->getDepth()); 
  Vec3d mColor = mPair.first;
  Vec3d mPhoton = mPair.second;
  // Vec3d ret = traceRay(r, traceUI->getDepth());
  mColor.clamp();
  mPhoton.clamp();
  return std::make_pair(mColor, mPhoton);
}

void RayTracer::firePhotons(int numPhotons, Vec3d mFlux)
{

  for ( vector<Light*>::const_iterator litr = scene->beginLights(); 
  		litr != scene->endLights(); ++litr )
  {
  	  Light* pLight  = *litr;

  	  if(!pLight->hasPhotonsAbility()) 
    	continue;

	  for (vector<Geometry*>::const_iterator giter = scene->beginObjectsBB(); giter != scene->endObjectsBB(); giter++)
	  {
	  	for(int i = 0; i < numPhotons; i++ ){
	  		BoundingBox* pBox = new BoundingBox((*giter)->getBoundingBox().getMin(), (*giter)->getBoundingBox().getMax());
		  	std::tuple<Vec3d,Vec3d> thing = pLight->firePhoton(pBox);
		  	photon r(std::get<0>(thing), std::get<1>(thing), mFlux, ray::VISIBILITY);
		  	tracePhoton(r, traceUI->getDepth());
	  	}
	  }
  }

  //Clear out the ray cache in the scene for debugging purposes,
  // for ( vector<Light*>::const_iterator litr = scene->beginLights(); 
  //      litr != scene->endLights(); ++litr )
  // {

  //   Light* pLight  = *litr;

  //   if(!pLight->hasPhotonsAbility()) 
  //   	continue;
  // 	std::cout << "Firing Photons" << std::endl;
  // 	for(int i = 0; i < numPhotons; i++ ){
		// std::tuple<Vec3d,Vec3d> thing = pLight->firePhoton();
		// //std::tuple<Vec3d,Vec3d> thing = pLight->firePhoton(scene->bounds());
		// photon r(std::get<0>(thing), std::get<1>(thing), mFlux, ray::VISIBILITY);
		// // std::cout << std::get<0>(thing) << " " << std::get<1>(thing) << std::endl;
		// tracePhoton(r, traceUI->getDepth());
  // 	}

  // }
  std::cout << "Number of Photon bins: " << scene->mSpatialHash.size() << std::endl;
  return;
}

Vec3d RayTracer::tracePixelBlock(const pixelBlock pxB){
	for(int j = pxB.yMin; j < pxB.yMax; j++){
		for(int i = pxB.xMin; i < pxB.xMax; i++){
			Vec3d tmp = tracePixel(i, j);
		}
	}
}
Vec3d RayTracer::tracePixel(int i, int j)
{
	Vec3d col(0,0,0);
	Vec3d pCol(0,0,0);

	if( ! sceneLoaded() ) return col;

	//NOTE top left is (px + 0.0, py + 0.0)
	// bottom right is (px + 1.0, py + 1.0)
	int SuperSampleX = traceUI->getSuperSampling();

	double x = double(i)/double(buffer_width);
	double y = double(j)/double(buffer_height);

	unsigned char *pixel = buffer + ( i + j * buffer_width ) * 3;
	unsigned char *pixelP = bufferP + ( i + j * buffer_width ) * 3;

	if(SuperSampleX > 0){
	double subPx = 1.0/((double) SuperSampleX);
	std::vector<Vec3d> Samples;
	std::vector<Vec3d> PSamples;

	double xx, yy;
	for(int ix = 0; ix < SuperSampleX; ix++){
		for(int iy = 0; iy < SuperSampleX; iy++){
			if(isDistributionRayTracing()){
				double xJitter = 0.0;
				double yJitter = 0.0;

				xJitter = (double(rand() % 10000))/10000.0;
				yJitter = (double(rand() % 10000))/10000.0;
				xx = x + subPx*(double(ix) + xJitter)/double(buffer_width);
				yy = y + subPx*(double(iy) + yJitter)/double(buffer_height);
				// std::cout << xx << " " << yy << std::endl;
			}else{
				xx = x + subPx*(double(ix))/double(buffer_width);
				yy = y + subPx*(double(iy))/double(buffer_height);

			}
			std::pair<Vec3d, Vec3d> mPair = trace(xx, yy);
			Samples.push_back(mPair.first);
			PSamples.push_back(mPair.second);
		
		}		
	}

	for(auto& sample: Samples){
		col = col + sample;
	}
	col = col / (double(SuperSampleX*SuperSampleX));

	for(auto& sample: PSamples){
		pCol = pCol + sample;
	}
	pCol = pCol / (double(SuperSampleX*SuperSampleX));
}
else{
	std::pair<Vec3d, Vec3d> mPair = trace(x, y);
	col = mPair.first;
	pCol = mPair.second;
}

	pixel[0] = (int)( 255.0 * col[0]);
	pixel[1] = (int)( 255.0 * col[1]);
	pixel[2] = (int)( 255.0 * col[2]);

	pixelP[0] = (int)( 255.0 * pCol[0]);
	pixelP[1] = (int)( 255.0 * pCol[1]);
	pixelP[2] = (int)( 255.0 * pCol[2]);

	return col;
}

// void RayTracer::traceBlock()
inline double clamp(double minval, double maxval, double mVal){
	if (mVal < minval) mVal = minval;
	else if(mVal > maxval) mVal = maxval;
	return mVal;
}

// void RayTracer::interpolateBetween(int x1, int y1, int x2, int y2 )
// {

// }
// Do recursive ray tracing!  You'll want to insert a lot of code here
// (or places called from here) to handle reflection, refraction, etc etc.
// Returns (ColorC, PhotonMask) Pair
std::pair<Vec3d,Vec3d> RayTracer::traceRay(ray& r, int depth)
{
	isect i;
	Vec3d colorC;
	Vec3d photonC;

	// std::default_random_engine generator;
  // std::normal_distribution<double> distribution(0.0,0.01);
	if(scene->intersect(r, i)) {
		// YOUR CODE HERE
		Vec3d q = r.at(i.t);

		// An intersection occurred!  We've got work to do.  For now,
		// this code gets the material for the surface that was intersected,
		// and asks that material to provide a color for the ray.  

		// This is a great place to insert code for recursive ray tracing.
		// Instead of just returning the result of shade(), add some
		// more steps: add in the contributions from reflected and refracted
		// rays.
	  const Material& m = i.getMaterial();	  
	  colorC = m.shade(scene, r, i);
	  if(depth <= 0) return std::make_pair(colorC, photonC); //Vec3d(0.0, 0.0, 0.0); //Finish recursion
	  if(m.Refl()){
	  	// std::cout<< "HERE"<< std::endl;

			Vec3d Rdir = -2.0*(r.getDirection()*i.N)*i.N + r.getDirection();
			Rdir.normalize();
			if(isDistributionRayTracing()){
				// std::cout << "I AM HERE Too" << std::endl;
				Vec3d V = scene->getCamera().getEye() - r.at(i.t) ;
		    // V = -V;
    		V.normalize();

				Vec3d perterbR = CosWeightedRandomHemiDir2(Rdir);
				double tmpP = Rdir*V;

    		tmpP =  pow(max(0.0, tmpP), m.shininess(i));

				// Rdir = Rdir + pow(Rdir * perterbR ,m.shininess(i)) * perterbR;
				Rdir = Rdir + tmpP*perterbR;
				Rdir.normalize();
				// std::cout << Rdir << std::endl;
			} 
			ray R(q, Rdir, ray::REFLECTION);
			std::pair<Vec3d, Vec3d> mPair = traceRay(R, depth - 1);
			colorC += m.kr(i) % mPair.first;
			photonC += mPair.second;
	  	// colorC += m.kr(i)%traceRay(R, depth - 1);
	  }

	  // Now handle the Transmission (Refraction)
	  if(m.Trans()){
	  	// double eta;
	  	// double etai = 1.0;
	  	// double etat = m.index(i);
	  	// // if(etat < 1.0) etat = 1.0;
	  	// Vec3d rd = -r.getDirection();
	  	// double cos_i = clamp(-1, 1, i.N*rd);
	  	// // double cos_i = i.N*rd;
	  	// Vec3d n = i.N;
	  	// if(cos_i < 0){cos_i = -cos_i;}
	  	// else {std::swap(etai, etat); n = -i.N;}
	  	
	  	// eta = etai/etat;
	  	
	  	// double TIR = 1 - eta*eta*(1 - cos_i*cos_i);
	  	// if(TIR >= 0.0){ //No Total internal reflection
	  	// // if(true){ //No Total internal reflection
	  	// 	double cos_t = sqrt(TIR);
	  	// 	Vec3d Tdir = (eta*cos_i - cos_t)*n - eta*rd;
		  // 	Tdir.normalize();
		  // 	if(isDistributionRayTracing() && m.index(i) > 1.0){
				// 	// std::cout << "I AM HERE Too" << std::endl;	
				// 	Vec3d perterbT = CosWeightedRandomHemiDir2(Tdir);
				// 	Tdir = 1.25*pow(m.index(i), sqrt(m.shininess(i)))*Tdir + perterbT;
				// 	Tdir.normalize();
				// // std::cout << Rdir << std::endl;
				// } 
				// ray T(q, Tdir, ray::REFRACTION);
				// // std::cout << m.kt(i) << std::endl;
				// Vec3d kt = m.kt(i);
		  // 	colorC += kt%traceRay(T, depth - 1);

	  	// }


	  	Vec3d n = i.N;
	  	Vec3d rd = r.getDirection();
	  	Vec3d rcos = n*(-rd*n);
	  	Vec3d rsin = rcos + rd;
	  	double etai = 1.0;
	  	double etat = m.index(i);
	  	Vec3d tcos, tsin;
	  	double eta;
	  	if(rd*n < 0){
	  		eta = etai/etat;
	  		n = -n;
	  	} else{
	  		eta = etat/etai;
	  	}
	  	tsin = eta*rsin;
	  	double TIR = 1 - tsin*tsin;
	  	if(TIR >= 0){
		  	tcos = n*sqrt(TIR);
		  	Vec3d Tdir = tcos + tsin;
		  	Tdir.normalize();
		  	if(isDistributionRayTracing() && m.index(i) > 1.0){
					// std::cout << "I AM HERE Too" << std::endl;	
					Vec3d perterbT = CosWeightedRandomHemiDir2(Tdir);
					Tdir = 1.25*pow(m.index(i), sqrt(m.shininess(i)))*Tdir + perterbT;
					Tdir.normalize();
				// std::cout << Rdir << std::endl;
				} 
  
				ray T(q, Tdir, ray::REFRACTION);
				std::pair<Vec3d, Vec3d> mPair = traceRay(T, depth - 1);
			  colorC += m.kt(i)% mPair.first;
			  photonC += mPair.second;

	  	}

	  }
	  // colorC = Vec3d(0.0, 0.0, 0.0); //See just the spatial hash map

	  if(scene->mSpatialHash.count(q)) {
	  	// std::cout << "FLUX: " << scene->mSpatialHash[q].flux << std::endl;
	  	// colorC += colorC % scene->mSpatialHash[q].flux; // Flux will be an additive multiple of the color
	  	
	  	photonC =  0.5*(colorC % scene->mSpatialHash[q].flux + scene->mSpatialHash[q].flux); // Flux will be an additive multiple of the color
	  	//photonC = scene->mSpatialHash[q].flux;

	  	//colorC = scene->mSpatialHash[q].flux; // Flux will be an additive multiple of the color
		}
	} else {
		// No intersection.  This ray travels to infinity, so we color
		// it according to the background color, which in this (simple) case
		// is just black.
		if(haveCubeMap()){
			colorC = environment->intersect(r);
			// colorC = Vec3d(0.0, 0.0, 0.0);

		}else
			colorC = Vec3d(0.0, 0.0, 0.0);
	}
	return std::make_pair(colorC, photonC);
}

RayTracer::RayTracer()
	: scene(0), buffer(0), buffer_width(256), buffer_height(256), m_bBufferReady(false), environment(nullptr), bufferP(0)
{}

RayTracer::~RayTracer()
{
	delete scene;
	delete environment;
	delete [] buffer;
	delete [] bufferP;
}

void RayTracer::getBuffer( unsigned char *&buf, unsigned char *&bufP, int &w, int &h )
{
	buf = buffer;
	bufP = bufferP;
	w = buffer_width;
	h = buffer_height;
}

double RayTracer::aspectRatio()
{
	return sceneLoaded() ? scene->getCamera().getAspectRatio() : 1;
}

bool RayTracer::loadScene( char* fn ) {
	ifstream ifs( fn );
	if( !ifs ) {
		string msg( "Error: couldn't read scene file " );
		msg.append( fn );
		traceUI->alert( msg );
		return false;
	}
	
	// Strip off filename, leaving only the path:
	string path( fn );
	if( path.find_last_of( "\\/" ) == string::npos ) path = ".";
	else path = path.substr(0, path.find_last_of( "\\/" ));

	// Call this with 'true' for debug output from the tokenizer
	Tokenizer tokenizer( ifs, false );
    Parser parser( tokenizer, path );
	try {
		delete scene;
		scene = 0;
		scene = parser.parseScene();
	} 
	catch( SyntaxErrorException& pe ) {
		traceUI->alert( pe.formattedMessage() );
		return false;
	}
	catch( ParserException& pe ) {
		string msg( "Parser: fatal exception " );
		msg.append( pe.message() );
		traceUI->alert( msg );
		return false;
	}
	catch( TextureMapException e ) {
		string msg( "Texture mapping exception: " );
		msg.append( e.message() );
		traceUI->alert( msg );
		return false;
	}

	if( !sceneLoaded() ) return false;

	//This is where I will build the KD tree
	scene->buildKdTree();
	std::cout << "Done Building KD Tree" << std::endl;
	return true;
}

void RayTracer::traceSetup(int w, int h)
{
	if (buffer_width != w || buffer_height != h)
	{
		buffer_width = w;
		buffer_height = h;
		bufferSize = buffer_width * buffer_height * 3;
		delete[] buffer;
		delete[] bufferP;

		buffer = new unsigned char[bufferSize];
		bufferP = new unsigned char[bufferSize];
	}
	memset(buffer, 0, w*h*3);
	memset(bufferP, 0, w*h*3);
	m_bBufferReady = true;
}

// void RayTracer::addCubeFace(int faceNum, std::string fileName){
	
// }

void RayTracer::tracePhoton(photon& r, int depth)
{
	isect i;
	Vec3d colorC;
	Vec3d fluxDecreased;
	// std::default_random_engine generator;
  // std::normal_distribution<double> distribution(0.0,0.01);
	if(scene->intersect(r, i)) {
		// YOUR CODE HERE
		Vec3d q = r.at(i.t);
		double mRand = (double)rand()/(double)RAND_MAX;
		// An intersection occurred!  We've got work to do.  For now,
		// this code gets the material for the surface that was intersected,
		// and asks that material to provide a color for the ray.  

		// This is a great place to insert code for recursive ray tracing.
		// Instead of just returning the result of shade(), add some
		// more steps: add in the contributions from reflected and refracted
		// rays.
	  const Material& m = i.getMaterial();
	  
	  fluxDecreased = r.flux;// * abs(i.N * r.getDirection()); // Need to update this
	  // std::cout << "HERE" << std::endl;
	  if(!m.Trans() && !m.Refl()){
		  if(scene->mSpatialHash.count(q)) scene->mSpatialHash[q] += r;
		  else scene->mSpatialHash[q] = r;
	  }

	  // if(!m.Refl() && !m.Trans()){
	  // 	return;
	  // }	  
	  // colorC = m.shade(scene, r, i); //Does this make sense?
	  if(depth <= 0) return ; //Vec3d(0.0, 0.0, 0.0); //Finish recursion
	  if(m.Refl()){
	  	// std::cout<< "HERE"<< std::endl;
	  	
	  	// Russian Roulette
	  	if(mRand > m.kr(i)[0]){
	  		if(scene->mSpatialHash.count(q)) scene->mSpatialHash[q] += r;
			  else scene->mSpatialHash[q] = r;

	  		return;
	  	}

			Vec3d Rdir = -2.0*(r.getDirection()*i.N)*i.N + r.getDirection();
			Rdir.normalize();
			photon R(q, Rdir, fluxDecreased, ray::REFLECTION);
	  	// colorC += m.kr(i)%tracePhoton(R, depth - 1);
	  	tracePhoton(R, depth - 1);
	  }

	  // Now handle the Transmission (Refraction)
	  mRand = (double)rand()/(double)RAND_MAX;
	  if(m.Trans()){
	  	//cout << "yes" << endl;
	  	//cout << "m.kt(i)[0] = " << m.kt(i)[0] << endl;
	    // Russian Roulette
	  	if(mRand > m.kt(i)[0]){
	  		if(scene->mSpatialHash.count(q)) scene->mSpatialHash[q] += r;
			  else scene->mSpatialHash[q] = r;

	  		return;

	  	}

	  	Vec3d n = i.N;
	  	Vec3d rd = r.getDirection();
	  	Vec3d rcos = n*(-rd*n);
	  	Vec3d rsin = rcos + rd;
	  	double etai = 1.0;
	  	double etat = m.index(i);
	  	Vec3d tcos, tsin;
	  	double eta;
	  	if(rd*n < 0){
	  		eta = etai/etat;
	  		n = -n;
	  	} else{
	  		eta = etat/etai;
	  	}
	  	tsin = eta*rsin;
	  	double TIR = 1 - tsin*tsin;
	  	if(TIR >= 0){
		  	tcos = n*sqrt(TIR);
		  	Vec3d Tdir = tcos + tsin;
		  	Tdir.normalize();  
			  photon T(q, Tdir, fluxDecreased, ray::REFRACTION);
			  tracePhoton(T, depth - 1);
			  // colorC += m.kt(i)%tracePhoton(T, depth - 1);
	  	}

	  }
	} else {
		// No intersection.  This ray travels to infinity, so we color
		// it according to the background color, which in this (simple) case
		// is just black.
	
		// if(haveCubeMap()){
		// 	colorC = environment->intersect(r);
		// }else
		// 	colorC = Vec3d(0.0, 0.0, 0.0);
	}
	// return colorC;
	return;
}