#pragma once
#include "NullTextureLoader.h"

class ITextureLoader;
class IGizmoQueryStorage;

class ServiceLocator
{
	public:
		ServiceLocator()
		    : _textureLoader( nullptr )
		    , _nullTextureLoader()
		{
		}

		~ServiceLocator() {}

		static ServiceLocator& CreateInstance();
		static ServiceLocator& GetInstance();
		static void DestroyInstance();

		ITextureLoader& GetTextureLoader();
		void RegisterTextureLoader( ITextureLoader* textureLoader );
		void UnregisterTextureLoader();

		void UnregisterAll();

	private:
		static ServiceLocator* _instance;

		ITextureLoader* _textureLoader;
		NullTextureLoader _nullTextureLoader;
};