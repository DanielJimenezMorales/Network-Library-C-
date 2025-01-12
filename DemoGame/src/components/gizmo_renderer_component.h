#pragma once

class Gizmo;

struct GizmoRendererComponent
{
		GizmoRendererComponent( Gizmo* gizmo )
		    : gizmo( gizmo )
		{
		}

		Gizmo* gizmo;
};
