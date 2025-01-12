#pragma once

class GameEntity;
struct BaseEntityConfiguration;

class IEntityFactory
{
	public:
		IEntityFactory() {}
		virtual ~IEntityFactory() {}

		virtual void Create( GameEntity& entity, const BaseEntityConfiguration* configuration ) = 0;
		virtual void Destroy( GameEntity& entity ) = 0;
};
