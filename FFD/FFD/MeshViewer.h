class HE_edge;
class HE_vert{
public:
	HE_vert()
	{
		normal[0]=0.0;
		normal[1]=0.0;
		normal[2]=0.0;
		edge =NULL;
	}
public:
	GLdouble x;
	GLdouble y;
	GLdouble z;
	int v_index;
	float normal[3];
	HE_edge *edge;	

};
class HE_face{
public:
	HE_face()
	{
		edge = NULL;
	}
public:
    int v1,v2,v3;
	int f_index;
	HE_edge* edge;
};
class HE_edge{
public:
	HE_edge()
	{ 
		pair = NULL; 
		face = NULL;
		next = NULL;
		vert_start = NULL;
		vert_end = NULL;
	}
public:
	 HE_vert* vert_start;   // vertex at the start of the half-edge
	 HE_vert* vert_end;   // vertex at the end of the half-edge
	 HE_edge* pair;   // oppositely oriented adjacent half-edge 
     HE_face* face;   // face the half-edge borders
	 HE_edge* next;   // next half-edge around the face
	 int e_index;
};
//For mapping the edges.
struct HE_line{    
	unsigned int vstart; 
	unsigned int vend; 
	bool operator<(const HE_line& other)const
	{
		if(vstart<other.vstart)
		{
			return TRUE;
		}
		else if(vstart == other.vstart)
		{
			return vend<other.vend;
		}
		return false;
	}
	HE_line(){}; 
	HE_line( const HE_line &v){vstart=v.vstart;vend=v.vend;}
	HE_line(unsigned int v1,unsigned int v2){vstart=v1;vend=v2;};
};
struct Normal_vector{
	float normal[3];
};