#include "Camera.hpp"
#include "EngineCommon.hpp"
#include "..\Renderer\TextureCube.hpp"
#include "..\Renderer\MeshBuilder.hpp"


Camera::Camera(Matrix44 cameraMatrix /*= Matrix44()*/, Matrix44 view /*= Matrix44()*/, Matrix44 proj /*= Matrix44()*/)
{
	m_cameraMatrix = cameraMatrix;
	m_viewMatrix = view;
	m_projMatrix = proj;

	m_output = FrameBuffer();
}

void Camera::CreateSkyBox(std::string imagePath)
{
	m_skyBoxTexture = new TextureCube();
	m_skyBoxTexture->make_from_image(imagePath.c_str());

	MeshBuilder mb;
	mb.AddCube(Vector3::ZERO, Vector3::ONE);
	m_skyMesh = mb.CreateMesh<Vertex3D_PCU>();

	m_hasSkyBox = true;
}

void Camera::RenderSkyBox() const
{
	Renderer* r = Renderer::GetInstance();
	
	r->ClearDepth(1.f);
	r->SetShader(Shader::CreateOrGetShader("Data/Shaders/skybox.shader"));
	r->SetUniform("MODEL", m_cameraMatrix);
								
	r->SetCurrentCubeMapTexture(m_skyBoxTexture, 0);
	r->DrawMesh(m_skyMesh);
	r->ClearDepth(1.f);
	r->SetShader();
	r->SetCurrentTexture();
}

Vector3 Camera::GetForward()
{
	return m_cameraMatrix.GetForward();
}

Vector3 Camera::GetRight()
{
	return m_cameraMatrix.GetRight();
}

Vector3 Camera::GetUp()
{
	return m_cameraMatrix.GetUp();
}

void Camera::LookAt(Vector3 position, Vector3 target, Vector3 up)
{
	// Look first creates the camera position
	m_cameraMatrix = Matrix44::LookAt(position,target,up);
	
	// Using the make view function its lookat + inverse in one call
	m_viewMatrix = Matrix44::MakeView(position, target, up);
}

void Camera::SetProjection(Matrix44 proj)
{
	m_projMatrix = proj;
}

void Camera::SetProjectionOrtho( float width, float height, float Near, float Far)
{
	//( -width / 2, +width / 2, -height / 2, +height / 2, near, far );
	m_projMatrix = Matrix44::MakeOrtho3D(Vector3(-width *.5f,-height *.5f,Near),Vector3(width * .5f,height *.5f,Far));
}

void Camera::SetPerspective(float degrees, float aspect, float nearZ, float farZ)
{
	m_projMatrix = Matrix44::PerspectiveProjection(degrees,aspect,nearZ,farZ);
}

Vector2 Camera::WorldToScreenCoordinate(Vector3 world_pos)
{
	Matrix44 vp = m_projMatrix;
	vp.Append(m_viewMatrix);

	UNIMPLEMENTED();
	
	
	return Vector2();

}

Vector3 Camera::ScreenToWorldCoordinate(Vector2 pixel, float depthFromCamera)
{
	UNIMPLEMENTED();
	// pseudo code
	//Vector2 ndc = RangeMap(pixel, Vector2(0.f,0.f), Vector2(resolution.x(), resolution.y()), Vector2(-1.f, 1), Vector2(-1, 1));
	//
	//Vector3 ndc = Vector3(ndc_xy, depthFromCamera);
	//
	//Vector4 homoegeneousWorld = Vector4(ndc, 1.0f) * m_projMatrix.Invert(); // this is supposed to be view projection
	//
	//
	//Vector3 worldPos = homoegeneousWorld.xyz() / homoegeneousWorld.w;
	//
	//return worldPos;
	depthFromCamera = 0.f;
	return Vector3();
}

Ray3 Camera::ScreenToPickRay(Vector2 pixel)
{

	Vector3 start = ScreenToWorldCoordinate(pixel,0);
	Vector3 end = ScreenToWorldCoordinate(pixel,1);

	Vector3 direction = end - start;

	return Ray3(start, direction);

}
