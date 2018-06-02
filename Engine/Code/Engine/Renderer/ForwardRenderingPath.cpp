#include "ForwardRenderingPath.hpp"
#include "Engine/Core/Camera.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/Scene.hpp"
#include "Engine/Renderer/Mesh.hpp"
#include "Renderable.hpp"
#include "Engine/Renderer/Lights.hpp"
#include <algorithm>
#include "../Math/MathUtils.hpp"
#include "DebugRenderSystem.hpp"


//////////////////////////////////////////////////////////////////////////
bool CompareSortOrder(DrawCall a, DrawCall b)
{
	int aOrder = a.m_material->GetSortOrder();
	int bOrder = b.m_material->GetSortOrder();

	if(aOrder > bOrder)
		return true;

	// check for alpha and opaque

	return false;
}

bool CompareRenderQueue(DrawCall a, DrawCall b)
{
	// make sure we are in the same layer
	int aOrder = a.m_material->GetSortOrder();
	int bOrder = b.m_material->GetSortOrder();

	if(aOrder == bOrder)
		return false;

	// compare sort order
	RenderQueue aQueue = a.m_material->GetQueue();
	RenderQueue bQueue = b.m_material->GetQueue();

	if( aQueue < bQueue)
		return true;

	return false;
}


//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
void ForwardRenderingPath::Render(Scene* scene) const
{
	for(uint cameraIndex = 0; cameraIndex < scene->m_cameras.size(); cameraIndex++)
	{
		RenderSceneForCamera(scene->m_cameras.at(cameraIndex), scene);
	}


	// Render all the Debug Render stuff, this is assuming main camera is index 0
	DebugRenderSet3DCamera(scene->m_cameras.at(0));
	DebugRenderUpdateAndRender();
}

void ForwardRenderingPath::RenderSceneForCamera(Camera * cam, Scene * scene) const
{
	Renderer* r = Renderer::GetInstance();

	r->SetCamera(cam);

	//////////////////////////////////////////////////////////////////////////
	// Creating Draw Calls
	std::vector<DrawCall> drawCalls;

	for(uint renderableIndex = 0; renderableIndex < scene->m_renderables.size(); renderableIndex++)
	{
		uint lights[MAX_LIGHTS];
		uint lightCount = 0;

		Renderable* currentRenderable = scene->m_renderables.at(renderableIndex);

		// This is for if we have an empty mesh (particle emitter) we don't crash renderdoc
		if(currentRenderable->m_mesh->m_ibo.m_indexCount == 0)
			continue;

		if(currentRenderable->UsesLight())
		{
			lightCount = scene->GetMostContributingLights( lightCount, lights, currentRenderable->GetPosition());
		}
		

		DrawCall dc;
		
		// Set the variables for draw call
		dc.m_mesh = currentRenderable->GetMesh();
		dc.m_material = currentRenderable->GetMaterial();
		dc.m_model = currentRenderable->GetModelMatrix();
		dc.m_sort = currentRenderable->GetMaterial()->GetSortOrder();
		
		dc.m_lightCount = lightCount;
		for(uint lightIndex = 0; lightIndex < MAX_LIGHTS; lightIndex++)
		{
			dc.m_lightIndices[lightIndex] = lights[lightIndex];
		}

		drawCalls.push_back(dc);
		
	}
		
		//////////////////////////////////////////////////////////////////////////
		// Sort the draw calls by sort order
		// had to do it this way cause const..?
		SortDrawsBySortOrder(&drawCalls, *cam);

		//////////////////////////////////////////////////////////////////////////
		// Do the Draw
		for(uint drawIndex = 0; drawIndex < drawCalls.size(); drawIndex++)
		{
			DrawCall current = drawCalls.at(drawIndex);
			
			r->BindMaterial(current.m_material);
			r->SetUniform("MODEL", current.m_model);

			EnableLightsForDrawCall(current, scene);

			r->DrawMesh(current.m_mesh);
		}

	
}

void ForwardRenderingPath::SortDrawsBySortOrder(std::vector<DrawCall>* dc, Camera& currentCam) const
{
	//std::vector<DrawCall> result = dc;
	if(dc->size() == 0) return;
	
	// layer first
	std::sort(dc->begin(), dc->end(), CompareSortOrder);

	// queue sort
	std::sort(dc->begin(), dc->end(), CompareRenderQueue);

	// camera distance
	for(uint i = 0; i < dc->size() -1; i++)
	{
		DrawCall a = dc->at(i);
		DrawCall b = dc->at(i + 1);

		// if the second one is opaque it doesn;t matter
		if(b.m_material->GetQueue() == RENDER_QUEUE_OPAQUE)
			continue;
		
		// if they are both opaque doesn't matter
		if(a.m_material->GetQueue() == RENDER_QUEUE_OPAQUE && b.m_material->GetQueue() == RENDER_QUEUE_OPAQUE)
			continue;


		// if they are both alpha we gotta compare distances
		Vector3 aPos = a.m_model.GetPosition();
		Vector3 bPos = b.m_model.GetPosition();
		Vector3 cPos = currentCam.m_cameraMatrix.GetPosition();

		float aDistance = GetDistance( aPos, cPos );
		float bDistance = GetDistance( bPos, cPos );

		// do the swap
		if(aDistance < bDistance)
			std::swap(dc->at(i), dc->at(i + 1));
	}

}

void ForwardRenderingPath::EnableLightsForDrawCall(DrawCall dc, Scene* scene ) const
{
	uint size = dc.m_lightCount;
	Renderer* r = Renderer::GetInstance();

	for(uint lightIndex = 0; lightIndex < size; lightIndex++)
	{
		// We gotta let the light know what pos it is in the Light UBO, so we overwrite the index
		scene->m_lights[dc.m_lightIndices[lightIndex]]->m_index = lightIndex;
		
		// have the light draw and bind the value itself
		scene->m_lights[dc.m_lightIndices[lightIndex]]->Render();
	}

	r->BindLightUBOsToShader();

}
