#pragma once
#include "Gizmo.h"

class CircleGizmo : public Gizmo
{
	public:
		CircleGizmo( float32 radius )
		    : Gizmo( 0, 255, 0, 255 )
		    , _radius( radius )
		{
		}

	protected:
		void RenderConcrete( const CameraComponent& camera, const TransformComponent& transform,
		                     SDL_Renderer* renderer ) const override;

	private:
		const float32 _radius;
};
