#include "Circle2DGizmo.h"

#include "components/camera_component.h"
#include "components/transform_component.h"

void CircleGizmo::RenderConcrete( const CameraComponent& camera, const TransformComponent& transform,
                                  SDL_Renderer* renderer ) const
{
	const Vec2f position = transform.GetPosition();

	// Start at the top of the circle
	float32 x = _radius;
	float32 y = 0;

	// Decision parameter (initial value)
	float32 decisionOver2 = 1 - x;

	// We loop until x < y, covering one octant
	while ( y <= x )
	{
		// Draw the points in all 8 octants
		Vec2f renderPosition = camera.ConvertFromWorldPositionToScreenPosition( position + Vec2f( x, y ) );
		SDL_RenderDrawPoint( renderer, renderPosition.X(), renderPosition.Y() );
		renderPosition = camera.ConvertFromWorldPositionToScreenPosition( position + Vec2f( -x, y ) );
		SDL_RenderDrawPoint( renderer, renderPosition.X(), renderPosition.Y() );
		renderPosition = camera.ConvertFromWorldPositionToScreenPosition( position + Vec2f( x, -y ) );
		SDL_RenderDrawPoint( renderer, renderPosition.X(), renderPosition.Y() );
		renderPosition = camera.ConvertFromWorldPositionToScreenPosition( position + Vec2f( -x, -y ) );
		SDL_RenderDrawPoint( renderer, renderPosition.X(), renderPosition.Y() );
		renderPosition = camera.ConvertFromWorldPositionToScreenPosition( position + Vec2f( y, x ) );
		SDL_RenderDrawPoint( renderer, renderPosition.X(), renderPosition.Y() );
		renderPosition = camera.ConvertFromWorldPositionToScreenPosition( position + Vec2f( -y, x ) );
		SDL_RenderDrawPoint( renderer, renderPosition.X(), renderPosition.Y() );
		renderPosition = camera.ConvertFromWorldPositionToScreenPosition( position + Vec2f( y, -x ) );
		SDL_RenderDrawPoint( renderer, renderPosition.X(), renderPosition.Y() );
		renderPosition = camera.ConvertFromWorldPositionToScreenPosition( position + Vec2f( -y, -x ) );
		SDL_RenderDrawPoint( renderer, renderPosition.X(), renderPosition.Y() );

		// Move to the next pixel vertically
		y++;

		// Update the decision parameter based on the previous pixel's decision
		if ( decisionOver2 <= 0 )
		{
			// If the decision parameter is less than or equal to zero,
			// we move horizontally right (x stays the same, y increases)
			decisionOver2 += 2 * y + 1;
		}
		else
		{
			// Otherwise, we move diagonally down-right (x decreases, y increases)
			x--;
			decisionOver2 += 2 * ( y - x ) + 1;
		}
	}
}
