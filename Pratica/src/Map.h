//
// Quake map viewer
//
// Author: Johan Gardhage <johan.gardhage@gmail.com>
//
#ifndef _MAP_H_
#define _MAP_H_

#include <stdlib.h> // free
#include <cstdint>

#define BSP_VERSION		29
#define MAXLIGHTMAPS	4
#define ANIM_TEX_FRAMES	32



// BSP entries
struct dentry_t
{
	int offset;		// Offset to entry, in bytes, from start of file (converted from long!)
	int size;		// Size of entry in file, in bytes (converted from long!)
};

// The BSP file header
struct dheader_t
{
	int version;			// Model version (converted from long!)
	dentry_t entities;		// List of Entities
	dentry_t planes;		// Map Planes
	dentry_t miptex;		// Wall Textures
	dentry_t vertices;		// Map Vertices
	dentry_t visilist;		// Leaves Visibility lists
	dentry_t nodes;			// BSP Nodes
	dentry_t texinfo;		// Texture info
	dentry_t surfaces;		// Map Surfaces
	dentry_t lightmaps;		// Wall Light Maps
	dentry_t boundnodes;	// BSP to bound Hulls
	dentry_t leaves;		// Hull BSP Leaves
	dentry_t lstsurf;		// List of Surfaces
	dentry_t edges;			// Original surface Edges
	dentry_t lstedges;		// List of surfaces Edges
	dentry_t hulls;			// List of Hulls
};

typedef float vec2_t[2];	// S,T coordinates of the vertex
typedef float vec3_t[3];	// X,Y,Z coordinates of the vertex
typedef short svec3_t[3];	// X,Y,Z coordinates of the vertex in shorts

struct boundbox_t
{
	vec3_t min;		// Minimum values of X,Y,Z
	vec3_t max;		// Maximum values of X,Y,Z
};

struct sboundbox_t
{
	svec3_t min;	// Minimum values of X,Y,Z in shorts
	svec3_t max;	// Maximum values of X,Y,Z in shorts
};

struct edge_t
{
	unsigned short startVertex;	// Id of the start vertex in the vertex array
	unsigned short endVertex;	// Id of the end vertex in the vertex array
};

// Mip texture list header
struct mipheader_t
{
	int numtex;		// Number of textures in mip texture list (converted from long!)
	int offset[1];	// [numtex] Offsets to each of the individual textures from the begining of mipheader_t (converted from long!)
};

// Mip Texture
struct miptex_t
{
	char name[16];			// Name of the texture
	unsigned int width;		// Width of the texture, must be a multiple of 8 (converted from long!)
	unsigned int height;	// Height of the texture, must be a multiple of 8 (converted from long!)
	unsigned int offset1;	// -> byte texture[width   * height] (converted from long!)
	unsigned int offset2;	// -> byte texture[width/2 * height/2] (converted from long!)
	unsigned int offset4;	// -> byte texture[width/4 * height/4] (converted from long!)
	unsigned int offset8;	// -> byte texture[width/8 * height/8] (converted from long!)
};

struct plane_t
{
	vec3_t normal;	// Vector orthogonal to plane (Nx,Ny,Nz) with Nx2+Ny2+Nz2 = 1
	float dist;		// Offset to plane, along the normal vector
	int type;		// Plane type (converted from long!)
};

struct texinfo_t
{
	vec3_t snrm;	// S projection
	float soff;		// S offset
	vec3_t tnrm;	// T projection
	float toff;		// T offset
	int texid;		// Id to the mipmap texture (converted from long!)
	int flag;		// Flag for sky/water texture (converted from long!)
};

struct surface_t
{
	unsigned short planenum;				// The plane in which the surface lies
	unsigned short side;					// 0 if in front of the plane, 1 if behind the plane
	int firstedge;							// First edge in the list of edges (converted from long!)
	unsigned short numedge;					// Number of edges in the list of edges
	unsigned short texinfoid;				// Index to the texture info which contains an index to the mipmap texture
	unsigned char lightstyle[MAXLIGHTMAPS];	// Type of light or 0xFF for no light map
	int lightmap;							// Offset to the lightmap or -1 (converted from long!)
};

struct bsphull_t
{
	boundbox_t box;		// The bounding box of the Hull
	vec3_t origin;		// Always zero
	int headnode[4];	// Heads of BSP hulls, headnode[0] is main hull (index to the first bsp tree node) (converted from long!)
	int visleaves;		// Number of visible BSP leaves (converted from long!)
	int firstsurf;		// Id of surfaces (converted from long!)
	int numsurf;		// Number of surfaces (converted from long!)
};

struct bspnode_t
{
	int planenum;				// Id of the plane that splits the node (intersecting plane), must be in [0,numplanes) (converted from long!)
	short front;				// Index to (front >= 0) ? front child node : leaf[~front]
	short back;					// Index to (back >= 0) ? back child node : leaf[~back]
	sboundbox_t sbox;			// Bounding box
	unsigned short firstsurf;	// First surface
	unsigned short numsurf;		// Number of surfaces
};

struct bspleaf_t
{
	int type;						// Special type of leaf (converted from long!)
	int vislist;					// Beginning of visibility lists, must be -1 or in [0,numvislist) (converted from long!)
	sboundbox_t sbox;				// Bounding box
	short firstsurf;				// Id to the first surface in the list of surfaces
	short numsurf;					// Number of entries in the list of surfaces
	unsigned char ambientsnd[4];	// Ambient sound volumes, ff = max
};

// Primitive descriptor
struct primdesc_t
{
	vec2_t t;	// Texture coordinate
	vec3_t v;	// Vertex coordinate
};

class Map
{
private:
	char *bsp;
	dheader_t *header;

	int LoadFile(const char *filename, void **bufferptr);
	bool LoadPalette(const char *filename);
	bool LoadBSP(const char *filename);

public:
	unsigned int palette[256];

	Map() {
		bsp = NULL;
	}
	~Map() {
		if (bsp) free (bsp);
	}

	bool Initialize(const char *bspFilename, const char *paletteFilename);

	//
	// BSP helper methods
	//

	// Get number of edges for surface
	int getNumEdges(int surfaceId) { return getSurface(surfaceId)->numedge; }

	// Get number of surfaces
	int getNumSurfaces() { return header->surfaces.size / sizeof (getSurfaces()[0]); }

	// Get number of textures
	int getNumTextures() { return ((mipheader_t *) getMipHeader())->numtex; }

	// Get number of surface lists
	int getNumSurfaceLists() { return header->lstsurf.size / sizeof (getSurfaceLists()[0]); }

	// Get number of leaves
	int getNumLeaves() { return getHull(0)->visleaves; }

	// Get array of vertices
	vec3_t *getVertices() { return (vec3_t *) &bsp[header->vertices.offset]; }

	// Get vertex
	vec3_t *getVertex(int id) { return &getVertices()[id]; }

	// Get array of edges, contains the index to the start and end vertices in the pV
	edge_t *getEdges() { return (edge_t *) &bsp[header->edges.offset]; }

	// Get array of edges, contains the index to the start and end vertices in the pV
	edge_t *getEdge(int id) { return &getEdges()[id]; }

	// Get array of edge lists, contains an index to an edge for a specific edge number
	// used when we got a surface and wants to get an edge index from an edge number
	// (the surface stores the edge number, not the index)
	int *getEdgeLists() { return (int *) &bsp[header->lstedges.offset]; }

	// Get index to an edge for a specific edge number
	// used when we got a surface and wants to get an edge index from an edge number
	// (the surface stores the edge number, not the index)
	int getEdgeList(int id) { return getEdgeLists()[id]; }

	// Get array of planes
	plane_t *getPlanes() { return (plane_t *) &bsp[header->planes.offset]; }

	// Get plane
	plane_t *getPlane(int planeId) { return &getPlanes()[planeId]; }

	// Get array of surfaces, contains an index to the pEL (the first edge number) and pTI
	surface_t *getSurfaces() { return (surface_t *) &bsp[header->surfaces.offset]; }

	// Get the surface
	surface_t *getSurface(int id) { return &getSurfaces()[id]; }

	// Get array of surface lists, contains an index to a surface for a specific surface number
	// used when we got a leaf and wants to get a surface index from a surface number
	// (the leaf stores the surface number, not the index)
	unsigned short *getSurfaceLists() { return (unsigned short *) &bsp[header->lstsurf.offset]; }

	// Get surface list
	unsigned short getSurfaceList(int surfaceListId) { return getSurfaceLists()[surfaceListId]; }

	// Get array of hulls (0 is the main rendering hull)
	// used to get an index to the start node in the bsp tree
	// used to get the total number of bsp leaves
	bsphull_t *getHulls() { return (bsphull_t *) &bsp[header->hulls.offset]; }

	// Get hull
	bsphull_t *getHull(int hullId) { return &getHulls()[hullId]; }

	// Get array of nodes, contains an index to the plane which intersects the node
	// contains an index to a right and a left node or to a leaf
	// used when traversing the bsp tree to find the leaf containing visible surfaces
	bspnode_t *getNodes() { return (bspnode_t *) &bsp[header->nodes.offset]; }

	// Get node
	bspnode_t *getNode(int nodeId) { return &getNodes()[nodeId]; }

	// Get start node
	bspnode_t *getStartNode() { return &getNodes()[getHulls()[0].headnode[0]]; }

	// Get array of leaves, contains an index to the visibility list
	// contains a number to the first surface and the number of surfaces in the leaf
	// used when we should sort out the surfaces that are visible
	bspleaf_t *getLeaves() { return (bspleaf_t *) &bsp[header->leaves.offset]; }

	// Get leaf
	bspleaf_t *getLeaf(int leafId) { return &getLeaves()[leafId]; }

	// Get array of visibility lists, used to determine which other leaves are visible from a given bsp leaf
	unsigned char *getVisibilityLists() { return (unsigned char *)(&bsp[header->visilist.offset]); }

	// Get visibility list
	unsigned char *getVisibilityList(int visibilityListId) { return &getVisibilityLists()[visibilityListId]; }

	// Get mip header
	unsigned char *getMipHeader() { return (unsigned char *) &(bsp[header->miptex.offset]); }

	// Get mip texture
	miptex_t *getMipTexture(int id) { return (miptex_t *)(getMipHeader() + ((mipheader_t *)getMipHeader())->offset[id]); }

	// Get raw texture
	unsigned char *getRawTexture(int id) { return (unsigned char *)(getMipHeader() + ((mipheader_t *)getMipHeader())->offset[id] + getMipTexture(id)->offset1); }

	// Get array of texture info for every surface, contains an index to pMipTex
	texinfo_t *getTextureInfo() { return (texinfo_t *) &bsp[header->texinfo.offset]; }

	// Get texture info of a surface
	texinfo_t *getTextureInfo(int id) { return &getTextureInfo()[getSurface(id)->texinfoid]; }
};

#endif
