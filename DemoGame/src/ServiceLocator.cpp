#include "ServiceLocator.h"
#include <cassert>

ServiceLocator& ServiceLocator::CreateInstance()
{
	assert( _instance == nullptr );
	_instance = new ServiceLocator();
	return *_instance;
}

ServiceLocator& ServiceLocator::GetInstance()
{
	assert( _instance != nullptr );
	return *_instance;
}

void ServiceLocator::DestroyInstance()
{
	assert( _instance != nullptr );
	delete _instance;
	_instance = nullptr;
}

ITextureLoader& ServiceLocator::GetTextureLoader()
{
	if ( _textureLoader == nullptr )
	{
		return _nullTextureLoader;
	}

	return *_textureLoader;
}

void ServiceLocator::RegisterTextureLoader( ITextureLoader* textureLoader )
{
	assert( _textureLoader == nullptr );
	_textureLoader = textureLoader;
}

void ServiceLocator::UnregisterTextureLoader()
{
	assert( _textureLoader != nullptr );
	delete _textureLoader;
	_textureLoader = nullptr;
}

void ServiceLocator::UnregisterAll()
{
	UnregisterTextureLoader();
}

ServiceLocator* ServiceLocator::_instance = nullptr;
