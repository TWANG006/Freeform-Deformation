#ifndef halfedge_h
#define halfedge_h

#include <cstdlib>

using namespace std;

// Halfedge structures
struct HE_edge;
struct HE_vert;
struct HE_face;
struct HE_line;

struct HE_edge
{
	int edgeID;
	HE_vert* vertS;	// Vertex at the start of halfedge
	HE_vert* vertE;	// Vertex at the end of halfedge
	HE_edge* pair;	// Oppositely oriented halfedge
	HE_face* face;	// Incident face
	HE_edge* next;	// Next halfedge around the face
};

struct HE_vert
{
	int vertID;			// Vertex ID
	float x, y, z;		// Vertex coordinates
	float nx, ny, nz;	// Vertex normals
	bool selected;		// Selected for FFD
	HE_edge* edge;		// Halfedge emanating from vertex
};

struct HE_face
{
	int faceID;			// Face ID
	int v1, v2, v3;		// Vertex IDs
	float nx, ny, nz;	// Face normals
	HE_edge* edge;		// Halfedge bordering the face
};

struct HE_line
{
	unsigned int startVert, endVert;
	bool operator < (const HE_line &other) const
	{
		if(startVert < other.startVert)
			return true;
		else if(startVert == other.startVert)
			return endVert < other.endVert;
		else
			return false;
	}
	HE_line(){};
	HE_line(const HE_line &vert) {
		startVert = vert.startVert;
		endVert = vert.endVert;
	}
	HE_line(unsigned int v1, unsigned int v2) {
		startVert = v1;
		endVert = v2;
	}
};

#endif