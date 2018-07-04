#include "Camera.hpp"
#include "EngineCommon.hpp"
#include "Engine\Renderer\Images/Textures/TextureCube.hpp"
#include "Engine\Renderer\Systems/MeshBuilder.hpp"
#include "../Platform/Window.hpp"
#include "../../Renderer/Systems/DebugRenderSystem.hpp"


Camera::Camera(Matrix44 cameraMatrix /*= Matrix44()*/, Matrix44 view /*= Matrix44()*/, Matrix44 proj /*= Matrix44()*/)
{
	m_cameraMatrix = cameraMatrix;
	m_viewMatrix = view;
	m_projMatrix = proj;

	m_output = FrameBuffer();
}

Camera::~Camera()
{
	if(m_skyBoxTexture != nullptr)
	{
		delete m_skyBoxTexture;
		m_skyBoxTexture = nullptr;

		delete m_skyMesh;
		m_skyMesh = nullptr;
	}
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
	// Screen to clip
	// we do -1 cause we are flipping the basis
	Vector2 ndc2D = Vector2::RangeMap(pixel, Vector2(0.f,0.f), Vector2(Window::GetInstance()->GetWidth(), Window::GetInstance()->GetHeight()), 
		Vector2(-1.f, 1.f), Vector2(1.f, -1.f));

	// vec3 clip
	Vector3 ndc = Vector3(ndc2D, depthFromCamera);
	
	// Clip to view 
	Matrix44 iproj = Invert(m_projMatrix);
	Vector4 viewPos = iproj.TransformHomogeneous(Vector4(ndc, 1.0f));
	
	// View to world
	Matrix44 iWorld = Invert(m_viewMatrix);
	Vector4 homoegeneousWorld = iWorld.TransformHomogeneous(viewPos);

	// Divide out the W
	Vector3 worldPos = homoegeneousWorld.xyz() / homoegeneousWorld.w;
	
	
	return worldPos;

}

Ray3 Camera::ScreenToPickRay(Vector2 pixel)
{

	Vector3 start = ScreenToWorldCoordinate(pixel,-1.0f);
	Vector3 end = ScreenToWorldCoordinate(pixel,1.0f);

	Vector3 direction = end - start;

	return Ray3(start, direction);

}

Frustrum Camera::GetFrustrum()
{
	Matrix44 mat = m_projMatrix;
	mat.Append(m_viewMatrix); // may need to be swapped
	return Frustrum::FromMatrix( mat ); 
}
