#include <sstream>
#include <cassert>

#include "ReplicationObjectRegistry.h"
#include "Logger.h"

namespace NetLib
{
	NetworkEntityFactory* NetLib::ReplicationObjectRegistry::GetNetworkEntityFactory(uint32_t entityType)
	{
		NetworkEntityFactory* networkEntityFactory = nullptr;
		auto it = _entityFactories.find(entityType);
		if (it != _entityFactories.end())
		{
			networkEntityFactory = it->second;
		}

		return networkEntityFactory;
	}

	bool ReplicationObjectRegistry::IsEntityFactoryRegistered(uint32_t entityType) const
	{
		return _entityFactories.find(entityType) != _entityFactories.cend();
	}

	bool NetLib::ReplicationObjectRegistry::RegisterNetworkEntityFactory(NetworkEntityFactory* entityFactory, uint32_t entityType)
	{
		if (IsEntityFactoryRegistered(entityType))
		{
			std::stringstream ss;
			ss << "The Entity Factory of entity type " << static_cast<int>(entityType) << " is already registered. Skipping...";
			Common::LOG_WARNING(ss.str());
			return false;
		}

		_entityFactories[entityType] = entityFactory;
		return true;
	}

	INetworkEntity* ReplicationObjectRegistry::CreateObjectOfType(uint32_t entityType)
	{
		NetworkEntityFactory* networkEntityFactory = GetNetworkEntityFactory(entityType);
		assert(networkEntityFactory != nullptr);
		INetworkEntity& networkEntity = networkEntityFactory->Create();

		return &networkEntity;
	}

	void ReplicationObjectRegistry::DestroyObjectOfType(INetworkEntity& networkEntityToDestroy)
	{
		NetworkEntityFactory* networkEntityFactory = GetNetworkEntityFactory(networkEntityToDestroy.GetEntityType());
		assert(networkEntityFactory != nullptr);
		networkEntityFactory->Destroy(networkEntityToDestroy);
	}
}
