#include "MessageUtils.h"
#include "MessageFactory.h"
#include "Buffer.h"

void MessageUtils::ReadMessage(Buffer& buffer, Message** message)
{
	MessageFactory* messageFactory = MessageFactory::GetInstance();
	if (messageFactory == nullptr)
	{
		return;
	}

	MessageType type = static_cast<MessageType>(buffer.ReadByte());

	switch (type)
	{
	case MessageType::ConnectionRequest:
		*message = messageFactory->GetMessage(MessageType::ConnectionRequest);
		break;
	case MessageType::ConnectionAccepted:
		*message = messageFactory->GetMessage(MessageType::ConnectionAccepted);
		break;
	case MessageType::ConnectionDenied:
		*message = messageFactory->GetMessage(MessageType::ConnectionDenied);
		break;
	case MessageType::ConnectionChallenge:
		*message = messageFactory->GetMessage(MessageType::ConnectionChallenge);
		break;
	case MessageType::ConnectionChallengeResponse:
		*message = messageFactory->GetMessage(MessageType::ConnectionChallengeResponse);
		break;
	case MessageType::Disconnection:
		*message = messageFactory->GetMessage(MessageType::Disconnection);
		break;
	}

	if (*message == nullptr)
	{
		return;
	}

	(*message)->Read(buffer);
}