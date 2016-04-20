#include <iostream>
#include <time.h>
#include <stdarg.h>

#include <assert.h>
#include <vector>

#include "CommandLineUI.h"
#include "../fileio/bitmap.h"
#include "pixelblock.h"
#include "../RayTracer.h"
#include <thread>

using namespace std;

// The command line UI simply parses out all the arguments off
// the command line and stores them locally.
CommandLineUI::CommandLineUI( int argc, char* const* argv )
	: TraceUI()
{
	int i;
	int dummy;
	progName=argv[0];
	haveCubeMaps = false;
	char *ptr;
	// while( (i = getopt( argc, argv, "tr:w:h:" )) != EOF )
	while( (i = getopt( argc, argv, "r:w:h:s:t:L:T:D:F:R:B:x:dh" )) != EOF )
	{
		switch( i )
		{
			case 'r':
				m_nDepth = atoi( optarg );
				break;

			case 'w':
				m_nSize = atoi( optarg );
				break;
			case 's':
				dummy = atoi(optarg);
				m_smoothshade = (dummy == 0) ? false : true;
				break;
			case 't':
				m_numThreads = atoi(optarg);
				break;
			case 'x':
				superSampleX = atoi(optarg);
				break;
			case 'd':
				DRT = true;
				break;
			case 'L':
				haveCubeMaps = true;
				leftF = optarg;
				// ptr = optarg;
				// while(*ptr++){
				// 	leftF.push_back(*ptr);
				// }
				// leftF = ;
				// printf("%s\n", leftF);
				break;
			case 'T':
				haveCubeMaps = true;
				topF = optarg;
				break;
			case 'D':
				haveCubeMaps = true;
				downF = optarg;
				break;
			case 'F':
				haveCubeMaps = true;
				frontF = optarg;
				break;
			case 'R':
				haveCubeMaps = true;
				rightF = optarg;
				break;
			case 'B':
				haveCubeMaps = true;
				backF = optarg;
				break;
			case 'h':
				usage();
				exit(1);
				break;


			default:
			// Oops; unknown argument
			std::cerr << "Invalid argument: '" << i << "'." << std::endl;
			usage();
			exit(1);
		}
	}

	if( optind >= argc-1 )
	{
		std::cerr << "no input and/or output name." << std::endl;
		exit(1);
	}

	rayName = argv[optind];
	imgName = argv[optind+1];
}

void parallelThing(RayTracer* raytracer, std::vector<pixelBlock> pxBList){
	for(auto& pxB:  pxBList){
		raytracer->tracePixelBlock(pxB);
	}

}

int CommandLineUI::run()
{
	assert( raytracer != 0 );
	// setSmoothShade(true);
	// printf("%s\n", leftF);
	
	if(haveCubeMaps){
		raytracer->createCubeMap();
		if(topF)
		{			
			std::string bsT(topF);
			raytracer->environment->setTop(		bsT);		
		}		
		if(leftF) {
			std::string bsL(leftF);
			raytracer->environment->setLeft(	bsL);}		
		if(frontF){
			std::string bsF(frontF);

		 raytracer->environment->setFront(bsF);	}	
		if(rightF){
			std::string bsR(rightF);

		 raytracer->environment->setRight(bsR);	}	
		if(backF) {
			std::string bsB(backF);

			raytracer->environment->setBack(	bsB);}		
		if(downF) {
			std::string bsD(downF);

			raytracer->environment->setDown(	bsD);}		
	}
	raytracer->loadScene( rayName );
	raytracer->setDistributionRayTracing(DRT);
	// std::cout << raytracer->isDistributionRayTracing() << std::endl;
	if( raytracer->sceneLoaded() )
	{
		int width = m_nSize;
		int height = (int)(width / raytracer->aspectRatio() + 0.5);

		raytracer->traceSetup( width, height );
		std::vector<pixelBlock> pxBList =  getBlockList(width, height, 10, 10);

		int numThreads = (getNumThreads() > 0) ? getNumThreads() : std::thread::hardware_concurrency();
		// numThreads = 1;
		std::vector<std::vector<pixelBlock>> tPixelBlocks(numThreads);
		std::vector<std::thread> ThreadList;
		int i = 0;
		for(auto& pxB:  pxBList){
			i = i % numThreads;
			tPixelBlocks[i].push_back(pxB);
			i++;
		}

		clock_t start, end;
		start = clock();

		for(int i = 0; i < numThreads; i++){
			
			ThreadList.push_back(std::thread(parallelThing, raytracer, tPixelBlocks[i]));
		}
		for_each(ThreadList.begin(), ThreadList.end(), std::mem_fn(&std::thread::join) );
		// for(auto& pxB:  pxBList){
		// 	raytracer->tracePixelBlock(pxB);
		// }

		// for( int j = 0; j < height; ++j )
		// 	for( int i = 0; i < width; ++i )
		// 		raytracer->tracePixel(i,j);

		end=clock();

		// save image
		unsigned char* buf;

		raytracer->getBuffer(buf, width, height);

		if (buf)
			writeBMP(imgName, width, height, buf);

		double t=(double)(end-start)/CLOCKS_PER_SEC;
//		int totalRays = TraceUI::resetCount();
//		std::cout << "total time = " << t << " seconds, rays traced = " << totalRays << std::endl;
        return 0;
	}
	else
	{
		std::cerr << "Unable to load ray file '" << rayName << "'" << std::endl;
		return( 1 );
	}
}

void CommandLineUI::alert( const string& msg )
{
	std::cerr << msg << std::endl;
}

void CommandLineUI::usage()
{
	std::cerr << "usage: " << progName << " [options] [input.ray output.bmp]" << std::endl;
	std::cerr << "  -r <#>      set recursion level (default " << m_nDepth << ")" << std::endl; 
	std::cerr << "  -w <#>      set output image width (default " << m_nSize << ")" << std::endl;
	std::cerr << "  -s <(0,1)>  enable smooth shading (default " << m_smoothshade << ")" << std::endl;
	std::cerr << "  -t <#>      set number of threads (default " << m_numThreads << ")" << std::endl; 
	std::cerr << "  -x <#>      set Antialiasing supersampling (default " << superSampleX << ")" << std::endl; 
	std::cerr << "  -d          enable Distribution Ray Tracing (default " << DRT << ")" << std::endl; 
	std::cerr << "  -L <PATH>   Set Cube Map on Face: Left"  << std::endl; 
	std::cerr << "  -T <PATH>   Set Cube Map on Face: Top"   << std::endl; 
	std::cerr << "  -D <PATH>   Set Cube Map on Face: Down"  << std::endl; 
	std::cerr << "  -F <PATH>   Set Cube Map on Face: Front" << std::endl; 
	std::cerr << "  -R <PATH>   Set Cube Map on Face: Right" << std::endl; 
	std::cerr << "  -B <PATH>   Set Cube Map on Face: Back"  << std::endl; 

}
