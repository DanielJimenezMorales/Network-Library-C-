#pragma once
#include "ecs/i_simple_system.h"

#include "SDL_image.h"

class GizmoRendererSystem : public ECS::ISimpleSystem
{
	public:
		GizmoRendererSystem( SDL_Renderer* renderer );

		void Execute( ECS::EntityContainer& entity_container, float32 elapsed_time ) override;

		void AllocateGizmoRendererComponentIfHasCollider( GameEntity& entity );
		void DeallocateGizmoRendererComponentIfHasCollider( GameEntity& entity );

	private:
		SDL_Renderer* _renderer;
};
