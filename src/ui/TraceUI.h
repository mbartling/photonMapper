//
// rayUI.h
//
// The header file for the UI part
//

#ifndef __rayUI_h__
#define __rayUI_h__

// who the hell cares if my identifiers are longer than 255 characters:
#pragma warning(disable : 4786)

#include <string>

using std::string;

class RayTracer;

class TraceUI {
public:
	TraceUI() : m_nDepth(0), m_nSize(512), m_displayDebuggingInfo(false),
                    m_shadows(true), m_smoothshade(true), raytracer(0),
                    m_nFilterWidth(1), m_numThreads(0), superSampleX(0),
                    DRT(false)
                    {}

	virtual int	run() = 0;
	virtual ~TraceUI(){}
	// Send an alert to the user in some manner
	virtual void alert(const string& msg) = 0;

	// setters
	virtual void setRayTracer( RayTracer* r ) { raytracer = r; }
	void setCubeMap(bool b) { m_gotCubeMap = b; }
	void useCubeMap(bool b) { m_usingCubeMap = b; }

	// accessors:
	int	getSize() const { return m_nSize; }
	int	getDepth() const { return m_nDepth; }
	int		getFilterWidth() const { return m_nFilterWidth; }

	bool	shadowSw() const { return m_shadows; }
	bool	smShadSw() const { return m_smoothshade; }
	void 	setSmoothShade(bool val){ m_smoothshade = val;}

	double getThresh() const { return (double)m_thresh * 0.001;}
	int getBlockSize() const { return m_blkSize;}
	void setNumThreads(int numThreads){ m_numThreads = numThreads;}
	int getNumThreads(void) const {return m_numThreads;}
	int getSuperSampling(void) const {return superSampleX;}
	void setSuperSampling(int x){superSampleX = x;}
	bool isDistributionRayTracing(void){return DRT;}

	static bool m_debug;

protected:
	RayTracer*	raytracer;

	int	m_nSize;	// Size of the traced image
	int	m_nDepth;	// Max depth of recursion

	int m_blkSize; //default 4, min 2, max 64
	int m_thresh; //x0.001 :default 0, max 1000
	// Determines whether or not to show debugging information
	// for individual rays.  Disabled by default for efficiency
	// reasons.
	int superSampleX;
	bool m_displayDebuggingInfo;
	bool m_shadows;  // compute shadows?
	bool m_smoothshade;  // turn on/off smoothshading?
	bool		m_usingCubeMap;  // render with cubemap
	bool		m_gotCubeMap;  // cubemap defined
	int m_nFilterWidth;  // width of cubemap filter
	int m_numThreads;
	bool DRT;
};

#endif
