#include "sprite_renderer_system.h"

#include "GameEntity.hpp"
#include "Texture.h"
#include "Vec2f.h"
#include "Logger.h"

#include "ecs/entity_container.h"

#include "components/transform_component.h"
#include "components/sprite_renderer_component.h"
#include "components/camera_component.h"

#include <cassert>

SpriteRendererSystem::SpriteRendererSystem( SDL_Renderer* renderer )
    : _renderer( renderer )
{
	assert( _renderer != nullptr );
}

void SpriteRendererSystem::Execute( ECS::EntityContainer& entity_container, float32 elapsed_time )
{
	const GameEntity cameraEntity = entity_container.GetFirstEntityOfType< CameraComponent >();
	const CameraComponent& cameraComponent = cameraEntity.GetComponent< CameraComponent >();

	std::vector< GameEntity > entities =
	    entity_container.GetEntitiesOfBothTypes< SpriteRendererComponent, TransformComponent >();
	for ( auto it = entities.begin(); it != entities.end(); ++it )
	{
		// auto [spriteRenderer, transform] = view.get<SpriteRendererComponent, TransformComponent>(entity);
		const SpriteRendererComponent& spriteRenderer = it->GetComponent< SpriteRendererComponent >();
		const TransformComponent& transform = it->GetComponent< TransformComponent >();

		Texture* texture = spriteRenderer.texture;
		Vec2f screenPosition = cameraComponent.ConvertFromWorldPositionToScreenPosition( transform.GetPosition() );

		SDL_Rect destRect;
		destRect.x = static_cast< int >( screenPosition.X() - ( texture->GetDimensions().w / 2.f ) );
		destRect.y = static_cast< int >( screenPosition.Y() - ( texture->GetDimensions().h / 2.f ) );
		destRect.w = static_cast< int >( texture->GetDimensions().w * transform.GetScale().X() );
		destRect.h = static_cast< int >( texture->GetDimensions().h * transform.GetScale().Y() );

		// SDL_RenderCopy(renderer, texture->GetRaw(), &texture->GetDimensions(), &destRect);
		SDL_RenderCopyEx( _renderer, texture->GetRaw(), &texture->GetDimensions(), &destRect,
		                  transform.GetRotationAngle(), nullptr, SDL_RendererFlip::SDL_FLIP_NONE );
	}
}
