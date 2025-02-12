#pragma once
#include <queue>
#include <unordered_map>
#include <memory>

#include "communication/message.h"

namespace NetLib
{
	class MessageFactory
	{
	public:
		static void CreateInstance(uint32 size);

		/// <summary>
		/// Get unique instance. Before calling to this method, be sure to call CreateInstance(uint32 size) or you will get an error.
		/// </summary>
		/// <returns></returns>
		static MessageFactory& GetInstance();

		std::unique_ptr<Message> LendMessage(MessageType messageType);
		void ReleaseMessage(std::unique_ptr<Message> message);

		static void DeleteInstance();

	private:
		MessageFactory(uint32 size);
		MessageFactory(const MessageFactory&) = delete;

		MessageFactory& operator=(const MessageFactory&) = delete;

		void InitializePools();
		void InitializePool(std::queue<std::unique_ptr<Message>>& pool, MessageType messageType);
		std::queue<std::unique_ptr<Message>>* GetPoolFromType(MessageType messageType);
		std::unique_ptr<Message> CreateMessage(MessageType messageType);
		void ReleasePool(std::queue<std::unique_ptr<Message>>& pool);

		~MessageFactory();

		static MessageFactory* _instance;

		bool _isInitialized;
		uint32 _initialSize;

		std::unordered_map<MessageType, std::queue<std::unique_ptr<Message>>> _messagePools;
	};
}
