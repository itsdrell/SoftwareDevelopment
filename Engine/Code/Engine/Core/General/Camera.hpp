#pragma once
#include "Engine/Math/Vectors/Vector3.hpp"
#include "Engine/Math/Matrices/Matrix44.hpp"
#include "Engine/Renderer/Pipeline/FrameBuffer.hpp"
#include "Transform.hpp"
#include "../../Math/Geometry/Frustrum.hpp"
#include "Engine/Math/Ray.hpp"

//////////////////////////////////////////////////////////////////////////
class Texture;


//////////////////////////////////////////////////////////////////////////
class Camera
{
public:
	
	// Default to identity, but allows us to create a camera with a specific matrix if wanted
	Camera(Matrix44 cameraMatrix = Matrix44(), Matrix44 view = Matrix44(), Matrix44 proj = Matrix44());
	~Camera();

	void CreateSkyBox(std::string imagePath);
	void RenderSkyBox() const;

	// Set + Get
	void SetColorTarget( Texture *color_target ) { m_output.SetColorTarget(color_target); }
	void SetDepthStencilTarget( Texture *depth_target ) { m_output.SetDepthStencilTarget(depth_target); }
	Texture* GetColorTarget() const { return m_output.m_color_target; }
	int GetFramebufferID() {return m_output.m_ID;}
	Vector3 GetForward();
	Vector3 GetRight();
	Vector3 GetUp();
	AABB2 GetOrthoBounds() const;

	// model setters
	void LookAt( Vector3 position, Vector3 target, Vector3 up = Vector3::UP);  // this is what he had..? Matrix44::UP ); 

	// projection settings
	void SetProjection( Matrix44 proj ); 
	void SetProjectionOrtho( float width, float height, float Near, float Far ); 
	void SetProjectionOrthoByAspect(float size, float Near = -10.f, float Far = 100.f);
	void SetPerspective(float degrees, float aspect, float nearZ, float farZ);


	// math
	// returns a pixel coordinate this world
	// coordinate would go to.  (0, 0) being the 
	// top-left pixel
	Vector2 WorldToScreenCoordinate( Vector3 world_pos ); 
	Vector3 ScreenToWorldCoordinate( Vector2 pixel, float depthFromCamera );
	Ray3 ScreenToPickRay( Vector2 pixel ); 

	Frustrum GetFrustrum();


	void RenderDebugOrtho() const;


public:
	Transform			transform;
	
	// default all to identity
	Matrix44			m_cameraMatrix;  // where is the camera?
	Matrix44			m_viewMatrix;    // inverse of camera (used for shader)
	Matrix44			m_projMatrix;    // projection

	FrameBuffer			m_output;


	// Sky box options
	bool				m_hasSkyBox;
	TextureCube*		m_skyBoxTexture;
	Mesh*				m_skyMesh;

	// debug stuff
	Vector2				m_orthoSize;
}; 