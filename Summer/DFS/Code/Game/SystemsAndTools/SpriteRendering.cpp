#include "SpriteRendering.hpp"
#include "Scene2D.hpp"
#include "Engine\Renderer\Systems\DebugRenderSystem.hpp"
#include "Engine\Renderer\Systems\MeshBuilder.hpp"
#include "Engine\Renderer\Images\Sprites\Sprite.hpp"

void SpriteRendering::Render(Scene2D* scene) const
{

	for(uint cameraIndex = 0; cameraIndex < scene->m_cameras.size(); cameraIndex++)
	{
		RenderSceneForCamera(scene->m_cameras.at(cameraIndex), scene);
	}

}

void SpriteRendering::RenderSceneForCamera(Camera* cam, Scene2D* scene) const
{
	Renderer* r = Renderer::GetInstance();

	r->SetCamera(cam);

	//////////////////////////////////////////////////////////////////////////
	// Creating Draw Calls
	std::vector<DrawCall2D> drawCalls;

	//=============================================================
	// One of the things you can do here is batch the draw calls by
	// stuff that have the same sprite or texture
	TODO("Batch Sprites");

	MeshBuilder mb;

	for(uint renderableIndex = 0; renderableIndex < scene->m_renderables.size(); renderableIndex++)
	{
		 
		Renderable2D* currentRenderable = scene->m_renderables.at(renderableIndex);


		DrawCall2D dc;

		// Set the variables for draw call
		dc.m_material = currentRenderable->GetMaterial();
		dc.m_model = currentRenderable->GetModelMatrix();
		dc.m_sort = currentRenderable->GetLayer();

		//---------------------------------------------------------
		// Checking if the renderable has a mesh, otherwise make one
		if(currentRenderable->m_usesMesh == true)
		{
			dc.m_mesh = currentRenderable->GetMesh();
		}
		else
		{
			// We can sort by sprites later if we want
			mb.AddFromSprite( currentRenderable->GetPosition().xy(), *currentRenderable->GetSprite());
			dc.m_mesh = mb.CreateMesh<Vertex3D_PCU>();
		}
		

		drawCalls.push_back(dc);

	}

	//////////////////////////////////////////////////////////////////////////
	// Sort the draw calls by sort order
	// had to do it this way cause const..?
	//SortDrawsBySortOrder(&drawCalls, *cam);

	//////////////////////////////////////////////////////////////////////////
	// Do the Draw
	for(uint drawIndex = 0; drawIndex < drawCalls.size(); drawIndex++)
	{
		DrawCall2D current = drawCalls.at(drawIndex);

		r->BindMaterial(current.m_material);
		r->SetUniform("MODEL", current.m_model);

		// optimize here as well
		//r->DrawSprite( current.m_model.GetPosition(), *current.m_sprite);
		r->DrawMesh(current.m_mesh);
	}


	DebugRenderSet3DCamera(cam);
	DebugRenderUpdateAndRender();

}
