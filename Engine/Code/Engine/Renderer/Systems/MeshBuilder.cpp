#include "MeshBuilder.hpp"
#include "Engine/Renderer/RenderableComponents/Mesh.hpp"
#include "Engine\Math\MathUtils.hpp"
#include <stdio.h> //fopen
#include "../Images/Sprites/Sprite.hpp"
#include "../../TankGame/Code/Game/Main/GameCommon.hpp"
#include "../Images/Fonts/BitmapFont.hpp"

MeshBuilder::MeshBuilder()
{
	m_bounds = AABB3(Vector3::ZERO, Vector3::ZERO);
}

void MeshBuilder::Begin(PrimitiveType theType, bool useIndices)
{
	m_draw.primitiveType = theType;
	m_draw.usingIndices = useIndices;

	if (useIndices) {
		m_draw.startIndex = (uint) m_indices.size(); 
	} else {
		m_draw.startIndex = (uint) m_vertices.size();
	}
}

void MeshBuilder::End()
{
	uint end_idx;

	if (m_draw.usingIndices) {
		end_idx = (uint) m_indices.size(); 
	} else {
		end_idx = (uint) m_vertices.size(); 
	}

	m_draw.elemCount = end_idx - m_draw.startIndex; 
}

void MeshBuilder::SetColor(Rgba const & c)
{
	m_stamp.m_color = c;
}

void MeshBuilder::SetUV(Vector2 const & uv)
{
	m_stamp.m_uvTexCoords = uv;
}

void MeshBuilder::SetUV(float x, float y)
{
	m_stamp.m_uvTexCoords = Vector2(x,y);
}

void MeshBuilder::SetNormal(const Vector3 & theNormal)
{
	m_stamp.m_normal = theNormal;
}

void MeshBuilder::SetTangents(const Vector3 & theTangents)
{
	m_stamp.m_tangent = theTangents;
}

void MeshBuilder::SetBitangents(const Vector3 & theBitangents)
{
	m_stamp.m_biTangent = theBitangents;
}

void MeshBuilder::SetNormalAndTangents(const Vector3& theNormal)
{
	// This only works for sphere, do not use for 
	
	m_stamp.m_normal = theNormal;

	// Creates and sets the stamp
	GenerateArbitraryTangents(&m_stamp.m_tangent, &m_stamp.m_biTangent, theNormal);
}

uint MeshBuilder::PushVertex(Vector3 position)
{
	m_stamp.m_position = position;
	m_vertices.push_back(m_stamp);

	// store off the bounds
	m_bounds.GrowToContain(position);

	return (uint) m_vertices.size() -1;
}

void MeshBuilder::AddFace(uint a, uint b, uint c)
{
	m_indices.push_back(a);
	m_indices.push_back(b);
	m_indices.push_back(c);
}

void MeshBuilder::AddQuad(uint a, uint b, uint c, uint d)
{
	m_indices.push_back(a);
	m_indices.push_back(b);
	m_indices.push_back(c);

	m_indices.push_back(a);
	m_indices.push_back(c);
	m_indices.push_back(d);
}


//////////////////////////////////////////////////////////////////////////
// MAKERS

void MeshBuilder::AddPlane(const Vector3& center, const Vector3& dimensions, Rgba color)
{

	//////////////////////////////////////////////////////////////////////////

	Begin(PRIMITIVE_TRIANGLES, true); // true means you also need to push indices

										 // this is assuming all the sides are the same color
	SetColor(color);


	//////////////////////////////////////////////////////////////////////////

	SetUV(0,0);
	SetNormal(-Vector3::FORWARD);
	SetTangents(Vector3::RIGHT);
	SetBitangents(Vector3::UP);

	uint idx = PushVertex(Vector3((center.x - dimensions.x), (center.y - dimensions.y),(center.z - dimensions.z)));

	SetUV(1,0);
	PushVertex(Vector3((center.x + dimensions.x), (center.y - dimensions.y), (center.z - dimensions.z)));

	SetUV(0,1);
	PushVertex(Vector3((center.x - dimensions.x), (center.y + dimensions.y),(center.z - dimensions.z)));

	SetUV(1,1);
	PushVertex(Vector3((center.x + dimensions.x), (center.y + dimensions.y),(center.z - dimensions.z)));

	AddFace(idx + 0, idx + 1, idx + 2);
	AddFace(idx + 2, idx + 1, idx + 3);

	//////////////////////////////////////////////////////////////////////////

	End();

	//return mb.CreateMesh();
}

void MeshBuilder::AddPlane(const Vector3& center, const Vector3& dimensions, Vector3 right /*= Vector3::RIGHT*/, Vector3 up /*= Vector3::UP*/, Rgba color /*= Rgba::WHITE*/)
{
	//////////////////////////////////////////////////////////////////////////

	Begin(PRIMITIVE_TRIANGLES, true); // true means you also need to push indices

									  // this is assuming all the sides are the same color
	SetColor(color);


	Vector3 r = right * .5f;
	Vector3 u = up * .5f;

	//////////////////////////////////////////////////////////////////////////

	SetUV(0,0);
	SetNormal(-Vector3::FORWARD);
	SetTangents(Vector3::RIGHT);
	SetBitangents(Vector3::UP);

	//uint idx = PushVertex(Vector3((center.x - dimensions.x), (center.y - dimensions.y),(center.z - dimensions.z)) + -r  -u);
	uint idx = PushVertex(center + (-r * dimensions.x) + (-u * dimensions.y));

	SetUV(1,0);
	//PushVertex(Vector3((center.x + dimensions.x), (center.y - dimensions.y), (center.z - dimensions.z)) + r + -u);
	PushVertex(center + (r * dimensions.x) + (-u * dimensions.y));
	
	SetUV(0,1);
	//PushVertex(Vector3((center.x - dimensions.x), (center.y + dimensions.y),(center.z - dimensions.z)) + -r + u);
	PushVertex(center + (-r * dimensions.x) + (u * dimensions.y));
	
	SetUV(1,1);
	//PushVertex(Vector3((center.x + dimensions.x), (center.y + dimensions.y),(center.z - dimensions.z)) + r + u);
	PushVertex(center + (r * dimensions.x) + (u * dimensions.y));
	
	AddFace(idx + 0, idx + 1, idx + 2);
	AddFace(idx + 2, idx + 1, idx + 3);

	//////////////////////////////////////////////////////////////////////////

	End();
}

void MeshBuilder::AddCube(const Vector3& center, const Vector3& dimensions, Rgba color)
{

	//////////////////////////////////////////////////////////////////////////
	
	Begin(PRIMITIVE_TRIANGLES, true); // true means you also need to push indices

	// this is assuming all the sides are the same color
	SetColor(color);


	//////////////////////////////////////////////////////////////////////////
	SetNormal(-Vector3::RIGHT);
	SetTangents(-Vector3::FORWARD);
	SetBitangents(Vector3::UP);
	
	SetUV(0,0);
	uint idx = PushVertex(Vector3((center.x - dimensions.x),(center.y - dimensions.y),(center.z + dimensions.z)));

	SetUV(1,0);
	PushVertex(Vector3((center.x - dimensions.x),(center.y - dimensions.y),(center.z - dimensions.z)));

	SetUV(0,1);
	PushVertex(Vector3((center.x - dimensions.x),(center.y + dimensions.y),(center.z + dimensions.z)));

	SetUV(1,1);
	PushVertex(Vector3((center.x - dimensions.x),(center.y + dimensions.y),(center.z - dimensions.z)));

	AddFace(idx + 0, idx + 1, idx + 2);
	AddFace(idx + 2, idx + 1, idx + 3);

	//////////////////////////////////////////////////////////////////////////
	SetNormal(-Vector3::FORWARD);
	SetTangents(Vector3::RIGHT);
	SetBitangents(Vector3::UP);

	SetUV(0,0);
	idx = PushVertex(Vector3((center.x - dimensions.x), (center.y - dimensions.y),(center.z - dimensions.z)));

	SetUV(1,0);
	PushVertex(Vector3((center.x + dimensions.x), (center.y - dimensions.y), (center.z - dimensions.z)));

	SetUV(0,1);
	PushVertex(Vector3((center.x - dimensions.x), (center.y + dimensions.y),(center.z - dimensions.z)));

	SetUV(1,1);
	PushVertex(Vector3((center.x + dimensions.x), (center.y + dimensions.y),(center.z - dimensions.z)));

	AddFace(idx + 0, idx + 1, idx + 2);
	AddFace(idx + 2, idx + 1, idx + 3);

	//////////////////////////////////////////////////////////////////////////
	SetNormal(Vector3::UP);
	SetTangents(Vector3::RIGHT);
	SetBitangents(Vector3::FORWARD);
	
	SetUV(0,0);
	idx = PushVertex(Vector3((center.x - dimensions.x),(center.y + dimensions.y),(center.z - dimensions.z)));

	SetUV(1,0);
	PushVertex(Vector3((center.x + dimensions.x),(center.y + dimensions.y),(center.z - dimensions.z)));

	SetUV(0,1);
	PushVertex(Vector3((center.x - dimensions.x),(center.y + dimensions.y),(center.z + dimensions.z)));

	SetUV(1,1);
	PushVertex(Vector3((center.x + dimensions.x),(center.y + dimensions.y),(center.z + dimensions.z)));

	AddFace(idx + 0, idx + 1, idx + 2);
	AddFace(idx + 2, idx + 1, idx + 3);

	//////////////////////////////////////////////////////////////////////////
	SetNormal(Vector3::FORWARD);
	SetTangents(-Vector3::RIGHT);
	SetBitangents(Vector3::UP);

	SetUV(0,0);
	idx = PushVertex(Vector3((center.x + dimensions.x),(center.y - dimensions.y), (center.z + dimensions.z)));

	SetUV(1,0);
	PushVertex(Vector3((center.x - dimensions.x),(center.y - dimensions.y), (center.z + dimensions.z)));

	SetUV(0,1);
	PushVertex(Vector3((center.x + dimensions.x),(center.y + dimensions.y), (center.z + dimensions.z)));

	SetUV(1,1);
	PushVertex(Vector3((center.x - dimensions.x),(center.y + dimensions.y), (center.z + dimensions.z)));

	AddFace(idx + 0, idx + 1, idx + 2);
	AddFace(idx + 2, idx + 1, idx + 3);

	//////////////////////////////////////////////////////////////////////////
	SetNormal(Vector3::RIGHT);
	SetTangents(Vector3::FORWARD);
	SetBitangents(Vector3::UP);
	
	SetUV(0,0);
	idx = PushVertex(Vector3( (center.x + dimensions.x),(center.y - dimensions.y),(center.z - dimensions.z)));

	SetUV(1,0);
	PushVertex(Vector3( (center.x + dimensions.x),(center.y - dimensions.y),(center.z + dimensions.z)));

	SetUV(0,1);
	PushVertex(Vector3( (center.x + dimensions.x),(center.y + dimensions.y),(center.z - dimensions.z)));

	SetUV(1,1);
	PushVertex(Vector3( (center.x + dimensions.x),(center.y + dimensions.y),(center.z + dimensions.z)));

	AddFace(idx + 0, idx + 1, idx + 2);
	AddFace(idx + 2, idx + 1, idx + 3);

	//////////////////////////////////////////////////////////////////////////
	SetNormal(-Vector3::UP);
	SetTangents(Vector3::RIGHT);
	SetBitangents(-Vector3::FORWARD);
	
	SetUV(0,0);
	idx = PushVertex(Vector3((center.x - dimensions.x), (center.y - dimensions.y),(center.z + dimensions.z)));

	SetUV(1,0);
	PushVertex(Vector3((center.x + dimensions.x), (center.y - dimensions.y),(center.z + dimensions.z)));

	SetUV(0,1);
	PushVertex(Vector3((center.x - dimensions.x), (center.y - dimensions.y),(center.z - dimensions.z)));

	SetUV(1,1);
	PushVertex(Vector3((center.x + dimensions.x), (center.y - dimensions.y),(center.z - dimensions.z)));

	AddFace(idx + 0, idx + 1, idx + 2);
	AddFace(idx + 2, idx + 1, idx + 3);


	//////////////////////////////////////////////////////////////////////////
	End();

	//return mb.CreateMeshPCU();
}

void MeshBuilder::AddSkyBox()
{
	
	Vector3 center = Vector3::ZERO;
	//Vector3 dimensions = Vector3(10.f,10.f,10.f);
	Vector3 dimensions = Vector3::ONE;
	//////////////////////////////////////////////////////////////////////////

	Begin(PRIMITIVE_TRIANGLES, true); // true means you also need to push indices

									  // this is assuming all the sides are the same color
	SetColor(Rgba::WHITE);

	
	float top = (2.f/3.f);
	float bottom = (1.f/3.f);
	
	//////////////////////////////////////////////////////////////////////////
 	SetUV((1.f/4.f),bottom);
	SetNormal(-Vector3::RIGHT);
	uint idx = PushVertex(Vector3((center.x - dimensions.x),(center.y - dimensions.y),(center.z + dimensions.z)));

	SetUV(0.f,bottom);
	PushVertex(Vector3((center.x - dimensions.x),(center.y - dimensions.y),(center.z - dimensions.z)));

	SetUV((1.f/4.f),top);
	PushVertex(Vector3((center.x - dimensions.x),(center.y + dimensions.y),(center.z + dimensions.z)));

	SetUV(0.f,top);
	PushVertex(Vector3((center.x - dimensions.x),(center.y + dimensions.y),(center.z - dimensions.z)));

	AddFace(idx + 0, idx + 1, idx + 2);
	AddFace(idx + 2, idx + 1, idx + 3);

	//////////////////////////////////////////////////////////////////////////
	SetUV(1.f,bottom);
	SetNormal(-Vector3::FORWARD);
	idx = PushVertex(Vector3((center.x - dimensions.x), (center.y - dimensions.y),(center.z - dimensions.z)));

	SetUV((3.f/4.f),bottom);
	PushVertex(Vector3((center.x + dimensions.x), (center.y - dimensions.y), (center.z - dimensions.z)));

	SetUV(1.f,top);
	PushVertex(Vector3((center.x - dimensions.x), (center.y + dimensions.y),(center.z - dimensions.z)));

	SetUV((3.f/4.f),top);
	PushVertex(Vector3((center.x + dimensions.x), (center.y + dimensions.y),(center.z - dimensions.z)));

	AddFace(idx + 0, idx + 1, idx + 2);
	AddFace(idx + 2, idx + 1, idx + 3);

	//////////////////////////////////////////////////////////////////////////
	SetUV((1.f/4.f),1.f);
	SetNormal(Vector3::UP);
	idx = PushVertex(Vector3((center.x - dimensions.x),(center.y + dimensions.y),(center.z - dimensions.z)));

	SetUV((2.f/4.f),1.0f);
	PushVertex(Vector3((center.x + dimensions.x),(center.y + dimensions.y),(center.z - dimensions.z)));

	SetUV((1.f/4.f),top);
	PushVertex(Vector3((center.x - dimensions.x),(center.y + dimensions.y),(center.z + dimensions.z)));

	SetUV((2.f/4.f),top);
	PushVertex(Vector3((center.x + dimensions.x),(center.y + dimensions.y),(center.z + dimensions.z)));

	AddFace(idx + 0, idx + 1, idx + 2);
	AddFace(idx + 2, idx + 1, idx + 3);

	//////////////////////////////////////////////////////////////////////////
	SetUV((2.f/4.f),bottom);
	SetNormal(Vector3::FORWARD);
	idx = PushVertex(Vector3((center.x + dimensions.x),(center.y - dimensions.y), (center.z + dimensions.z)));

	SetUV((1.f/4.f),bottom);
	PushVertex(Vector3((center.x - dimensions.x),(center.y - dimensions.y), (center.z + dimensions.z)));

	SetUV((2.f/4.f),top);
	PushVertex(Vector3((center.x + dimensions.x),(center.y + dimensions.y), (center.z + dimensions.z)));

	SetUV((1.f/4.f),top);
	PushVertex(Vector3((center.x - dimensions.x),(center.y + dimensions.y), (center.z + dimensions.z)));

	AddFace(idx + 0, idx + 1, idx + 2);
	AddFace(idx + 2, idx + 1, idx + 3);

	//////////////////////////////////////////////////////////////////////////
	SetUV((3.f/4.f),bottom);
	SetNormal(Vector3::RIGHT);
	idx = PushVertex(Vector3( (center.x + dimensions.x),(center.y - dimensions.y),(center.z - dimensions.z)));

	SetUV((2.f/4.f),bottom);
	PushVertex(Vector3( (center.x + dimensions.x),(center.y - dimensions.y),(center.z + dimensions.z)));

	SetUV((3.f/4.f),top);
	PushVertex(Vector3( (center.x + dimensions.x),(center.y + dimensions.y),(center.z - dimensions.z)));

	SetUV((2.f/4.f),top);
	PushVertex(Vector3( (center.x + dimensions.x),(center.y + dimensions.y),(center.z + dimensions.z)));

	AddFace(idx + 0, idx + 1, idx + 2);
	AddFace(idx + 2, idx + 1, idx + 3);

	//////////////////////////////////////////////////////////////////////////
	SetUV((1.f/4.f),bottom);
	SetNormal(-Vector3::UP);
	idx = PushVertex(Vector3((center.x - dimensions.x), (center.y - dimensions.y),(center.z + dimensions.z)));

	SetUV((2.f/4.f),bottom);
	PushVertex(Vector3((center.x + dimensions.x), (center.y - dimensions.y),(center.z + dimensions.z)));

	SetUV((1.f/4.f),.0f);
	PushVertex(Vector3((center.x - dimensions.x), (center.y - dimensions.y),(center.z - dimensions.z)));

	SetUV((2.f/4.f),0.f);
	PushVertex(Vector3((center.x + dimensions.x), (center.y - dimensions.y),(center.z - dimensions.z)));

	AddFace(idx + 0, idx + 1, idx + 2);
	AddFace(idx + 2, idx + 1, idx + 3);


	//////////////////////////////////////////////////////////////////////////
	End();
}

void MeshBuilder::AddUVSphere(const Vector3& position, float radius, uint wedges, uint slices, Rgba color)
{

	//////////////////////////////////////////////////////////////////////////

	Begin(PRIMITIVE_TRIANGLES, true); // true means you also need to push indices

									 // this is assuming all the sides are the same color
	SetColor(color);

	//////////////////////////////////////////////////////////////////////////

	for (uint slice_idx = 0; slice_idx <= slices; ++slice_idx ) 
	{
		
		float v = (float)slice_idx / ((float)slices);
		//float azimuth = RangeMapFloat( v, 0, 1, -PI/2.0f, PI/2.0f ); 
		float azimuth = RangeMapFloat( v, 0, 1, -90, 90 ); 

		for (uint wedge_idx = 0; wedge_idx <= wedges; ++wedge_idx ) 
		{
			
			float u = (float)wedge_idx / ((float)wedges); 
			//float rot = 2.0f * PI * u; 
			float rot = 360 * u; 

			SetUV( u , v ); 
			Vector3 pos = position + PolarToCartesian( radius, rot, azimuth ); // I think this is polar to cartesian. Forseth just had "Polar" 
			
			Vector3 norm = pos - position;
			SetNormalAndTangents(norm.Normalize());
			
			PushVertex( pos ); 
		}
	}

	for (uint slice_idx = 0; slice_idx < slices; ++slice_idx ) //y
	{
		for (uint wedge_idx = 0; wedge_idx < wedges; ++wedge_idx ) //x
		{
			
			uint bl_idx = ((wedges + 1) * slice_idx) + wedge_idx; 
			uint tl_idx = bl_idx + wedges + 1; //bl_idx + wedges; 
			uint br_idx = bl_idx + 1; 
			uint tr_idx = tl_idx + 1; 

			AddQuad( bl_idx, br_idx, tr_idx, tl_idx ); 
		}
	}

	End();

	//return mb.CreateMeshPCU();
}

void MeshBuilder::AddQuad(const Vector3& position, AABB2& bounds)
{
	
	//////////////////////////////////////////////////////////////////////////

	Begin(PRIMITIVE_TRIANGLES, true); // true means you also need to push indices

									 // this is assuming all the sides are the same color
	SetColor(Rgba::WHITE);

	//////////////////////////////////////////////////////////////////////////

	SetUV(0,0);
	uint idx = PushVertex(Vector3((position.x - bounds.mins.x), (position.y - bounds.mins.y), (position.z)));

	SetUV(1,0);
	PushVertex(Vector3((position.x + bounds.maxs.x), (position.y - bounds.mins.y), (position.z)));

	SetUV(0,1);
	PushVertex(Vector3((position.x - bounds.mins.x), (position.y + bounds.maxs.y), (position.z)));

	SetUV(1,1);
	PushVertex(Vector3((position.x + bounds.maxs.x), (position.y + bounds.maxs.y), (position.z)));

	AddFace(idx + 0, idx + 1, idx + 2);
	AddFace(idx + 2, idx + 1, idx + 3);


	//////////////////////////////////////////////////////////////////////////
	
	End();

}

void MeshBuilder::AddMeshFromObjFile(std::string path)
{
	// reference (cause im bad at reading files) http://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/
	

	Begin(PRIMITIVE_TRIANGLES, true); // true means you also need to push indices

    // this is assuming all the sides are the same color
	SetColor(Rgba::WHITE);


	//////////////////////////////////////////////////////////////////////////
	std::vector<uint>	 vertexIndices, uvIndices, normalIndices;
	std::vector<Vector3> verts;
	std::vector<Vector2> uvs;
	std::vector<Vector3> normals;

	//////////////////////////////////////////////////////////////////////////
	FILE* file;
	fopen_s(&file, path.c_str(), "r");
	
	if( file == NULL )
		ERROR_AND_DIE("Impossible to open the file: " + path + "\n");

	//////////////////////////////////////////////////////////////////////////
	while( 1 )
	{

		char lineHeader[500];
		
		// read the first word of the line
		int res = fscanf_s(file, "%s", lineHeader, 3); // 3 might be bad but it works for now
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		//////////////////////////////////////////////////////////////////////////
		if ( strcmp( lineHeader, "v" ) == 0 )
		{
			Vector3 vertex;
			fscanf_s(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
			verts.push_back(vertex);
		}
		else if ( strcmp( lineHeader, "vt" ) == 0 )
		{
			Vector2 uv;
			fscanf_s(file, "%f %f\n", &uv.x, &uv.y );
			uvs.push_back(uv);
		}
		else if ( strcmp( lineHeader, "vn" ) == 0 )
		{
			Vector3 normal;
			fscanf_s(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
			normals.push_back(normal);
		}
		else if ( strcmp( lineHeader, "f" ) == 0 )
		{
			unsigned int vertexIndex[4], uvIndex[4], normalIndex[4];
			
			int matches = fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d \n", 
				&vertexIndex[0], &uvIndex[0], &normalIndex[0], 
				&vertexIndex[1], &uvIndex[1], &normalIndex[1], 
				&vertexIndex[2], &uvIndex[2], &normalIndex[2],
				&vertexIndex[3], &uvIndex[3], &normalIndex[3]);
			
			if (matches == 9)
			{
// 				vertexIndices.push_back(vertexIndex[0]);
// 				vertexIndices.push_back(vertexIndex[1]);
// 				vertexIndices.push_back(vertexIndex[2]);
// 
// 				uvIndices.push_back(uvIndex[0]);
// 				uvIndices.push_back(uvIndex[1]);
// 				uvIndices.push_back(uvIndex[2]);
// 
// 				normalIndices.push_back(normalIndex[0]);
// 				normalIndices.push_back(normalIndex[1]);
// 				normalIndices.push_back(normalIndex[2]);
				SetUV(uvs.at(uvIndex[0] -1));
				SetNormalAndTangents(normals.at(normalIndex[0]-1));
				uint idx = PushVertex((verts.at(vertexIndex[0]-1)));

				SetUV(uvs.at(uvIndex[1]-1));
				SetNormalAndTangents(normals.at(normalIndex[1]-1));
				PushVertex((verts.at(vertexIndex[1]-1)));
				
				SetUV(uvs.at(uvIndex[2]-1));
				SetNormalAndTangents(normals.at(normalIndex[2]-1));
				PushVertex((verts.at(vertexIndex[2]-1)));


				AddFace(idx + 0, idx + 1, idx + 2);

			}
			else
			{
				SetUV(uvs.at(uvIndex[0]-1));
				SetNormalAndTangents(normals.at(normalIndex[0]-1));
				uint idx = PushVertex((verts.at(vertexIndex[0]-1)));
				
				SetUV(uvs.at(uvIndex[1]-1));
				SetNormalAndTangents(normals.at(normalIndex[1]-1));
				PushVertex((verts.at(vertexIndex[1]-1)));

				SetUV(uvs.at(uvIndex[2]-1));
				SetNormalAndTangents(normals.at(normalIndex[2]-1));
				PushVertex((verts.at(vertexIndex[2]-1)));

				SetUV(uvs.at(uvIndex[3]-1));
				SetNormalAndTangents(normals.at(normalIndex[3]-1));
				PushVertex((verts.at(vertexIndex[3]-1)));

				AddQuad(idx + 0, idx + 1, idx + 2, idx + 3);

			}

		}
	}

	//////////////////////////////////////////////////////////////////////////
	End();

	//return mb.CreateMeshLIT();
}

void MeshBuilder::AddDeformedSphere(const Vector3& position, float radius, float offsetRange, uint wedges, uint slices, Rgba color /*= Rgba::WHITE*/)
{
	//////////////////////////////////////////////////////////////////////////

	Begin(PRIMITIVE_TRIANGLES, true); // true means you also need to push indices

									  // this is assuming all the sides are the same color
	SetColor(color);

	//////////////////////////////////////////////////////////////////////////

	for (uint slice_idx = 0; slice_idx <= slices; ++slice_idx ) 
	{

		float v = (float)slice_idx / ((float)slices);
		//float azimuth = RangeMapFloat( v, 0, 1, -PI/2.0f, PI/2.0f ); 
		float azimuth = RangeMapFloat( v, 0, 1, -90, 90 ); 

		for (uint wedge_idx = 0; wedge_idx <= wedges; ++wedge_idx ) 
		{

			float u = (float)wedge_idx / ((float)wedges); 
			//float rot = 2.0f * PI * u; 
			float rot = 360 * u; 

			SetUV( u , v ); 
			Vector3 pos = position + PolarToCartesian( GetRandomFloat(radius - offsetRange, radius + offsetRange), rot, azimuth ); // I think this is polar to cartesian. Forseth just had "Polar" 

			Vector3 norm = pos - position;
			SetNormalAndTangents(norm.Normalize());

			PushVertex( pos ); 
		}
	}

	for (uint slice_idx = 0; slice_idx < slices; ++slice_idx ) //y
	{
		for (uint wedge_idx = 0; wedge_idx < wedges; ++wedge_idx ) //x
		{

			uint bl_idx = ((wedges + 1) * slice_idx) + wedge_idx; 
			uint tl_idx = bl_idx + wedges + 1; //bl_idx + wedges; 
			uint br_idx = bl_idx + 1; 
			uint tr_idx = tl_idx + 1; 

			AddQuad( bl_idx, br_idx, tr_idx, tl_idx ); 
		}
	}

	End();
}

void MeshBuilder::AddPlaneFromFourPoints(const Vector3& bl, const Vector3& br, const Vector3& tr, const Vector3& tl)
{
	//////////////////////////////////////////////////////////////////////////

	Begin(PRIMITIVE_TRIANGLES, true); // true means you also need to push indices

									  // this is assuming all the sides are the same color
	SetColor(Rgba::WHITE);

	//////////////////////////////////////////////////////////////////////////

	SetUV(0,0);
	uint idx = PushVertex(bl);

	SetUV(1,0);
	PushVertex(br);

	SetUV(1,1);
	PushVertex(tr);

	SetUV(0,1);
	PushVertex(tl);

	
	AddFace(idx + 0, idx + 1, idx + 2);
	AddFace(idx + 2, idx + 3, idx);


	//////////////////////////////////////////////////////////////////////////

	End();

	//return mb.CreateMeshPCU();
}

void MeshBuilder::AddFromSprite(const Vector2& pos, const Sprite& theSprite)
{
	//---------------------------------------------------------

	Begin(PRIMITIVE_TRIANGLES, true); // true means you also need to push indices

									  // this is assuming all the sides are the same color
	SetColor(Rgba::WHITE);

	//---------------------------------------------------------
	AABB2 uvs = theSprite.m_uv;
	
	// this may be wrong
	Vector2 dims = theSprite.m_dimensions;
	AABB2 quad = AABB2(pos - (dims * .5f), pos + (dims * .5f));
	//quad = AABB2(-1.f, -1.f, 1.f, 1.f);
	
	//---------------------------------------------------------
	SetUV(uvs.mins);
	uint idx = PushVertex(Vector3(quad.mins, 0.f));

	SetUV(uvs.maxs.x, uvs.mins.y);
	PushVertex(Vector3(quad.maxs.x, quad.mins.y, 0.f));

	SetUV(uvs.mins.x, uvs.maxs.y);
	PushVertex(Vector3(quad.mins.x, quad.maxs.y, 0.f));

	SetUV(uvs.maxs);
	PushVertex(Vector3(quad.maxs, 0.f));

	AddFace(idx + 0, idx + 1, idx + 3);
	AddFace(idx + 0, idx + 3, idx + 2);


	//---------------------------------------------------------

	End();
}

void MeshBuilder::Add3DBounds(const AABB3& theBounds)
{
	//////////////////////////////////////////////////////////////////////////

	Begin(PRIMITIVE_TRIANGLES, true); // true means you also need to push indices

									  // this is assuming all the sides are the same color
	SetColor(Rgba::WHITE);

	//////////////////////////////////////////////////////////////////////////

	std::vector<Vector3> points = theBounds.GetCornerPoints();


//--------------------------------------------------------------------------
	//	Points
	//		   6--------------7
	//		  /|             /|
	//		 / |            / |
	//		2--+-----------3  |
	//		|  |           |  |
	//		|  |     c     |  |
	//		|  |           |  |
	//		|  4-----------+--5
	//		| /            | /
	//		|/             |/
	//		0--------------1
	//
	// 0 being the mins, 7 the maxs
	//--------------------------------------------------------------------------
	AddPlaneFromFourPoints( points.at(0), points.at(1), points.at(3), points.at(4)); // front
	AddPlaneFromFourPoints( points.at(0), points.at(1), points.at(5), points.at(4)); // bottom
	AddPlaneFromFourPoints( points.at(4), points.at(0), points.at(2), points.at(6)); // left
	AddPlaneFromFourPoints( points.at(1), points.at(5), points.at(7), points.at(3)); // right
	AddPlaneFromFourPoints( points.at(5), points.at(4), points.at(6), points.at(7)); // back
	AddPlaneFromFourPoints( points.at(2), points.at(3), points.at(7), points.at(6)); // top


	//////////////////////////////////////////////////////////////////////////

	End();

	//return mb.CreateMeshPCU();
}

void MeshBuilder::Add2DPlane(AABB2 bounds, Rgba color /*= Rgba::WHITE*/)
{
	//////////////////////////////////////////////////////////////////////////

	Begin(PRIMITIVE_TRIANGLES, true); // true means you also need to push indices

									  // this is assuming all the sides are the same color
	SetColor(color);

	//////////////////////////////////////////////////////////////////////////

	SetUV(0,0);
	uint idx = PushVertex(Vector3(bounds.mins.x, bounds.mins.y, .01f));

	SetUV(1,0);
	PushVertex(Vector3(bounds.maxs.x, bounds.mins.y, .01f));

	SetUV(0,1);
	PushVertex(Vector3(bounds.mins.x, bounds.maxs.y, .01f));

	SetUV(1,1);
	PushVertex(Vector3(bounds.maxs.x, bounds.maxs.y, .01f));

	AddFace(idx + 0, idx + 1, idx + 2);
	AddFace(idx + 2, idx + 1, idx + 3);


	//////////////////////////////////////////////////////////////////////////

	End();
}

void MeshBuilder::Add2DPlane(AABB2 & bounds, AABB2 & uvs, Rgba color)
{
	//////////////////////////////////////////////////////////////////////////

	Begin(PRIMITIVE_TRIANGLES, true); // true means you also need to push indices

									  // this is assuming all the sides are the same color
	SetColor(color);

	//////////////////////////////////////////////////////////////////////////

	SetUV(uvs.mins);
	uint idx = PushVertex(Vector3(bounds.mins.x, bounds.mins.y, .01f));

	SetUV(Vector2(uvs.maxs.x, uvs.mins.y));
	PushVertex(Vector3(bounds.maxs.x, bounds.mins.y, .01f));

	SetUV(Vector2(uvs.mins.x, uvs.maxs.y));
	PushVertex(Vector3(bounds.mins.x, bounds.maxs.y, .01f));

	SetUV(uvs.maxs);
	PushVertex(Vector3(bounds.maxs.x, bounds.maxs.y, .01f));

	AddFace(idx + 0, idx + 1, idx + 2);
	AddFace(idx + 2, idx + 1, idx + 3);


	//////////////////////////////////////////////////////////////////////////

	End();
}

void MeshBuilder::Add2DText(Vector2 startPos, std::string text,  float cellHeight, float aspectScale, Rgba color, BitmapFont * font)
{
	int length = (int) text.size();
	Vector2 startPoint = startPos;

	// Use a default font
	if(font == nullptr)
		font = g_theRenderer->m_defaultFont;

	// Draw
	for(int i = 0; i < length; i++)
	{
		// Get Current Letter
		char currentLetter = text.at(i);

		// calculate cell width
		float cellWidth = font->GetGlyphAspect() * cellHeight * aspectScale;

		AABB2 posBox = AABB2(startPoint,Vector2(startPoint.x + cellWidth,startPoint.y + cellHeight));
		AABB2 uvBox = AABB2(font->GetUVsForGlyph(currentLetter));

		Add2DPlane(posBox, uvBox , color);

		startPoint.x += cellWidth;
	}
}

