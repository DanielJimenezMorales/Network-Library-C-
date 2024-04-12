#include <sstream>
#include <memory>

#include "Client.h"
#include "Message.h"
#include "Logger.h"
#include "NetworkPacket.h"
#include "MessageFactory.h"
#include "RemotePeer.h"
#include "TimeClock.h"

namespace NetLib
{
	Client::Client(float serverMaxInactivityTimeout) : Peer(PeerType::ClientMode, 1, 1024, 1024),
		_serverAddress("127.0.0.1", 54000),
		inGameMessageID(0),
		_timeSinceLastTimeRequest(0.0f),
		_numberOfInitialTimeRequestBurstLeft(NUMBER_OF_INITIAL_TIME_REQUESTS_BURST),
		_currentState(ClientState::CS_Disconnected)
	{
	}

	Client::~Client()
	{
	}

	bool Client::StartConcrete()
	{
		sockaddr_in addressInfo;
		addressInfo.sin_family = AF_INET;
		addressInfo.sin_port = 0; //This is zero so the system picks up a random port number

		char ip[] = "127.0.0.1";
		int iResult = inet_pton(addressInfo.sin_family, ip, &addressInfo.sin_addr);
		if (iResult == -1)
		{
			std::stringstream ss;
			ss << "Error at converting IP string into address. Error code: " << WSAGetLastError();
			Common::LOG_ERROR(ss.str());
		}
		else if (iResult == 0)
		{
			std::stringstream ss;
			ss << "The IP string: " << ip << " is not valid";
			Common::LOG_ERROR(ss.str());
		}

		Address address = Address(addressInfo);
		BindSocket(address);

		_currentState = ClientState::CS_SendingConnectionRequest;

		uint64_t clientSalt = GenerateClientSaltNumber();
		AddRemotePeer(_serverAddress, 0, clientSalt, 0);

		SubscribeToOnRemotePeerDisconnect(std::bind(&NetLib::Client::OnServerDisconnect, this));

		Common::LOG_INFO("Client started succesfully!");

		return true;
	}

	uint64_t Client::GenerateClientSaltNumber()
	{
		//TODO Change this for a better generator. rand is not generating a full 64bit integer since its maximum is roughly 32767. I have tried to use mt19937_64 but I think I get a conflict with winsocks and std::uniform_int_distribution
		srand(time(NULL));
		return rand();
	}

	void Client::ProcessMessageFromPeer(const Message& message, RemotePeer& remotePeer)
	{
		MessageType messageType = message.GetHeader().type;

		switch (messageType)
		{
		case MessageType::ConnectionChallenge:
			if (_currentState == ClientState::CS_SendingConnectionRequest || _currentState == ClientState::CS_SendingConnectionChallengeResponse)
			{
				const ConnectionChallengeMessage& connectionChallengeMessage = static_cast<const ConnectionChallengeMessage&>(message);
				ProcessConnectionChallenge(connectionChallengeMessage, remotePeer);
			}
			break;
		case MessageType::ConnectionAccepted:
			if (_currentState == ClientState::CS_SendingConnectionChallengeResponse)
			{
				const ConnectionAcceptedMessage& connectionAcceptedMessage = static_cast<const ConnectionAcceptedMessage&>(message);
				ProcessConnectionRequestAccepted(connectionAcceptedMessage, remotePeer);
			}
			break;
		case MessageType::ConnectionDenied:
			if (_currentState == ClientState::CS_SendingConnectionChallengeResponse || _currentState == ClientState::CS_SendingConnectionRequest)
			{
				const ConnectionDeniedMessage& connectionDeniedMessage = static_cast<const ConnectionDeniedMessage&>(message);
				ProcessConnectionRequestDenied(connectionDeniedMessage);
			}
			break;
		case MessageType::Disconnection:
			if (_currentState == ClientState::CS_Connected)
			{
				const DisconnectionMessage& disconnectionMessage = static_cast<const DisconnectionMessage&>(message);
				ProcessDisconnection(disconnectionMessage, remotePeer);
			}
			break;
		case MessageType::TimeResponse:
			if (_currentState == ClientState::CS_Connected)
			{
				const TimeResponseMessage& timeResponseMessage = static_cast<const TimeResponseMessage&>(message);
				ProcessTimeResponse(timeResponseMessage);
			}
			break;
		case MessageType::InGameResponse:
			if (_currentState == ClientState::CS_Connected)
			{
				const InGameResponseMessage& inGameResponseMessage = static_cast<const InGameResponseMessage&>(message);
				ProcessInGameResponse(inGameResponseMessage);
			}
			break;
		default:
			Common::LOG_WARNING("Invalid Message type, ignoring it...");
			break;
		}
	}

	void Client::ProcessMessageFromUnknownPeer(const Message& message, const Address& address)
	{
		Common::LOG_WARNING("Client does not process messages from unknown peers. Ignoring it...");
	}

	void Client::TickConcrete(float elapsedTime)
	{
		if (_currentState == ClientState::CS_SendingConnectionRequest || _currentState == ClientState::CS_SendingConnectionChallengeResponse)
		{
			RemotePeer* remotePeer = _remotePeersHandler.GetRemotePeerFromAddress(_serverAddress);
			if (remotePeer == nullptr)
			{
				std::stringstream ss;
				ss << "Can't create new Connection Request Message because there is no remote peer corresponding to IP: " << _serverAddress.GetIP();
				Common::LOG_ERROR(ss.str());
				return;
			}

			CreateConnectionRequestMessage(*remotePeer);
		}

		if (_currentState == ClientState::CS_Connected)
		{
			UpdateTimeRequestsElapsedTime(elapsedTime);

			RemotePeer* remotePeer = _remotePeersHandler.GetRemotePeerFromAddress(_serverAddress);
			if (remotePeer == nullptr)
			{
				std::stringstream ss;
				ss << "There is no Remote peer corresponding to IP: " << _serverAddress.GetIP();
				Common::LOG_ERROR(ss.str());
				return;
			}
			CreateInGameMessage(*remotePeer);
		}
	}

	bool Client::StopConcrete()
	{
		_currentState = ClientState::CS_Disconnected;
		return true;
	}

	void Client::ProcessConnectionChallenge(const ConnectionChallengeMessage& message, RemotePeer& remotePeer)
	{
		Common::LOG_INFO("Challenge packet received from server");

		uint64_t clientSalt = message.clientSalt;
		uint64_t serverSalt = message.serverSalt;
		if (remotePeer.GetClientSalt() != clientSalt)
		{
			Common::LOG_WARNING("The generated salt number does not match the server's challenge client salt number. Aborting operation");
			return;
		}

		remotePeer.SetServerSalt(serverSalt);

		_currentState = ClientState::CS_SendingConnectionChallengeResponse;

		CreateConnectionChallengeResponse(remotePeer);

		Common::LOG_INFO("Sending challenge response packet to server...");
	}

	void Client::ProcessConnectionRequestAccepted(const ConnectionAcceptedMessage& message, RemotePeer& remotePeer)
	{
		uint64_t remoteDataPrefix = message.prefix;
		if (remoteDataPrefix != remotePeer.GetDataPrefix())
		{
			Common::LOG_WARNING("Packet prefix does not match. Skipping packet...");
			return;
		}

		ConnectRemotePeer(remotePeer);

		_clientIndex = message.clientIndexAssigned;
		_currentState = ClientState::CS_Connected;

		Common::LOG_INFO("Connection accepted!");
		ExecuteOnLocalPeerConnect();
	}

	void Client::ProcessConnectionRequestDenied(const ConnectionDeniedMessage& message)
	{
		Common::LOG_INFO("Processing connection denied");
		ConnectionFailedReasonType reason = static_cast<ConnectionFailedReasonType>(message.reason);

		RequestStop(false, reason);
	}

	void Client::ProcessDisconnection(const DisconnectionMessage& message, RemotePeer& remotePeer)
	{
		uint64_t dataPrefix = message.prefix;
		if (dataPrefix != remotePeer.GetDataPrefix())
		{
			Common::LOG_WARNING("Packet prefix does not match. Skipping message...");
			return;
		}

		std::stringstream ss;
		ss << "Disconnection message received from server with reason code equal to " << (int)message.reason << ". Disconnecting...";
		Common::LOG_INFO(ss.str());
		
		StartDisconnectingRemotePeer(remotePeer.GetClientIndex(), false, ConnectionFailedReasonType::CFR_UNKNOWN);
	}

	void Client::ProcessTimeResponse(const TimeResponseMessage& message)
	{
		Common::LOG_INFO("PROCESSING TIME RESPONSE");

		//Add new RTT to buffer
		TimeClock& timeClock = TimeClock::GetInstance();
		unsigned int rtt = timeClock.GetLocalTimeMilliseconds() - message.remoteTime;
		_timeRequestRTTs.push_back(rtt);

		if (_timeRequestRTTs.size() == TIME_REQUEST_RTT_BUFFER_SIZE + 1)
		{
			_timeRequestRTTs.pop_front();
		}

		//Get RTT to adjust server's clock elapsed time
		unsigned int meanRTT = 0;
		if (_timeRequestRTTs.size() == TIME_REQUEST_RTT_BUFFER_SIZE)
		{
			//Sort RTTs and remove the smallest and biggest values (They are considered outliers!)
			std::list<unsigned int> sortedTimeRequestRTTs = _timeRequestRTTs;
			sortedTimeRequestRTTs.sort();

			//Remove potential outliers
			for (unsigned int i = 0; i < NUMBER_OF_RTTS_CONSIDERED_OUTLIERS_PER_SIDE; ++i)
			{
				sortedTimeRequestRTTs.pop_back();
				sortedTimeRequestRTTs.pop_front();
			}

			std::list<unsigned int>::const_iterator cit = sortedTimeRequestRTTs.cbegin();
			for (; cit != sortedTimeRequestRTTs.cend(); ++cit)
			{
				meanRTT += *cit;
			}

			const unsigned int NUMBER_OF_VALID_RTT_TO_AVERAGE = TIME_REQUEST_RTT_BUFFER_SIZE - (2 * NUMBER_OF_RTTS_CONSIDERED_OUTLIERS_PER_SIDE);

			meanRTT /= NUMBER_OF_VALID_RTT_TO_AVERAGE;
		}
		else
		{
			meanRTT = rtt;
		}

		//Calculate server clock delta time
		unsigned int serverClockElapsedTimeMilliseconds = message.serverTime - message.remoteTime - (meanRTT / 2);
		double serverClockElapsedTimeSeconds = static_cast<double>(serverClockElapsedTimeMilliseconds) / 1000;
		timeClock.SetServerClockTimeDelta(serverClockElapsedTimeSeconds);

		std::stringstream ss;
		ss << "SERVER TIME UPDATED. Local time: " << timeClock.GetLocalTimeSeconds() << "s, Server time: " << timeClock.GetServerTimeSeconds() << "s";
		Common::LOG_INFO(ss.str());
	}

	void Client::ProcessInGameResponse(const InGameResponseMessage& message)
	{
		std::stringstream ss;
		ss << "In game response ID = " << message.data;
		Common::LOG_INFO(ss.str());
	}

	void Client::CreateConnectionRequestMessage(RemotePeer& remotePeer)
	{
		//Get a connection request message
		MessageFactory& messageFactory = MessageFactory::GetInstance();
		std::unique_ptr<Message> message = messageFactory.LendMessage(MessageType::ConnectionRequest);

		if (message == nullptr)
		{
			Common::LOG_ERROR("Can't create new Connection Request Message because the MessageFactory has returned a null message");
			return;
		}

		std::unique_ptr<ConnectionRequestMessage> connectionRequestMessage(static_cast<ConnectionRequestMessage*>(message.release()));

		//Set connection request fields
		connectionRequestMessage->clientSalt = remotePeer.GetClientSalt();

		//Store message in server's pending connection in order to send it
		remotePeer.AddMessage(std::move(connectionRequestMessage));

		Common::LOG_INFO("Connection request created.");
	}

	void Client::CreateConnectionChallengeResponse(RemotePeer& remotePeer)
	{
		//Get a connection challenge message
		MessageFactory& messageFactory = MessageFactory::GetInstance();
		std::unique_ptr<Message> message = messageFactory.LendMessage(MessageType::ConnectionChallengeResponse);
		if (message == nullptr)
		{
			Common::LOG_ERROR("Can't create new Connection Challenge Response Message because the MessageFactory has returned a null message");
			return;
		}

		std::unique_ptr<ConnectionChallengeResponseMessage> connectionChallengeResponseMessage(static_cast<ConnectionChallengeResponseMessage*>(message.release()));
		
		//Set connection challenge fields
		connectionChallengeResponseMessage->prefix = remotePeer.GetDataPrefix();

		//Store message in server's pending connection in order to send it
		remotePeer.AddMessage(std::move(connectionChallengeResponseMessage));
	}

	void Client::CreateTimeRequestMessage(RemotePeer& remotePeer)
	{
		Common::LOG_INFO("TIME REQUEST CREATED");
		MessageFactory& messageFactory = MessageFactory::GetInstance();
		std::unique_ptr<Message> lendMessage(messageFactory.LendMessage(MessageType::TimeRequest));

		std::unique_ptr<TimeRequestMessage> timeRequestMessage(static_cast<TimeRequestMessage*>(lendMessage.release()));

		timeRequestMessage->SetOrdered(true);
		TimeClock& timeClock = TimeClock::GetInstance();
		timeRequestMessage->remoteTime = timeClock.GetLocalTimeMilliseconds();

		remotePeer.AddMessage(std::move(timeRequestMessage));
	}

	void Client::CreateInGameMessage(RemotePeer& remotePeer)
	{
		MessageFactory& messageFactory = MessageFactory::GetInstance();
		std::unique_ptr<Message> message = messageFactory.LendMessage(MessageType::InGame);
		if (message == nullptr)
		{
			Common::LOG_ERROR("Can't create new Connection Challenge Response Message because the MessageFactory has returned a null message");
			return;
		}

		std::unique_ptr<InGameMessage> inGameMessage(static_cast<InGameMessage*>(message.release()));
		inGameMessage->SetOrdered(true);
		inGameMessage->data = inGameMessageID;
		inGameMessageID++;
		remotePeer.AddMessage(std::move(inGameMessage));
	}

	void Client::UpdateTimeRequestsElapsedTime(float elapsedTime)
	{
		if (_numberOfInitialTimeRequestBurstLeft > 0)
		{
			RemotePeer* remotePeer = _remotePeersHandler.GetRemotePeerFromAddress(_serverAddress);
			if (remotePeer == nullptr)
			{
				std::stringstream ss;
				ss << "There is no Remote peer corresponding to IP: " << _serverAddress.GetIP();
				Common::LOG_ERROR(ss.str());
				return;
			}

			--_numberOfInitialTimeRequestBurstLeft;
			CreateTimeRequestMessage(*remotePeer);
			return;
		}

		_timeSinceLastTimeRequest += elapsedTime;
		if (_timeSinceLastTimeRequest >= TIME_REQUESTS_FREQUENCY_SECONDS)
		{
			RemotePeer* remotePeer = _remotePeersHandler.GetRemotePeerFromAddress(_serverAddress);
			if (remotePeer == nullptr)
			{
				std::stringstream ss;
				ss << "There is no Remote peer corresponding to IP: " << _serverAddress.GetIP();
				Common::LOG_ERROR(ss.str());
				return;
			}

			_timeSinceLastTimeRequest = 0;
			CreateTimeRequestMessage(*remotePeer);
		}
	}

	void Client::OnServerDisconnect()
	{
		Common::LOG_INFO("ON SERVER DISCONNECT");
		Stop();
	}
}
