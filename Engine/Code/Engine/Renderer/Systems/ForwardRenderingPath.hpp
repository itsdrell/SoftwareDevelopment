#pragma once
#include "Engine\Core\General/EngineCommon.hpp"
#include "Engine\Math\Matrices/Matrix44.hpp"



//////////////////////////////////////////////////////////////////////////
class Scene;
class Camera;
class Material;
class Mesh;
class Texture;
class Sampler;
class Light;

//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
struct DrawCall
{
public:
	DrawCall() {}

	Matrix44	m_model;
	Mesh*		m_mesh;
	Material*	m_material;

	uint m_lightIndices[MAX_LIGHTS];
	uint m_lightCount;

	int m_sort;
};
bool CompareSortOrder(DrawCall a, DrawCall b);

//////////////////////////////////////////////////////////////////////////
class ForwardRenderingPath
{
public:
	ForwardRenderingPath();

	void Render( Scene* scene ) const;

	void RenderSceneForCamera( Camera* cam, Scene* scene ) const;
	void RenderSceneWithShadow( Light* theLight, Scene* scene) const;
	void RenderSkyBox( Scene* scene ) const;
	void RenderFog() const;

	void SortDrawsBySortOrder(std::vector<DrawCall>* dc, Camera& currentCam) const;
	void EnableLightsForDrawCall(DrawCall dc, Scene* scene ) const;


	
public:
	Camera*			m_shadowCamera;
	Texture*		m_shadowDepthTarget;
	Texture*		m_shadowColorTarget;

	Sampler*		m_shadowSampler;
};