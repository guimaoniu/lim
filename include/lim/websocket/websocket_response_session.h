#ifndef LIM_WEBSOCKET_RESPONSE_SESSION_H
#define LIM_WEBSOCKET_RESPONSE_SESSION_H
#include <lim/websocket/websocket_frame_decoder.h>
#include <lim/http/http_response_session.h>

namespace lim {
	class WebSocketResponseSession: public HttpFullResponseSession {
	protected:
		enum { HTTP_HANDSHAKE, WEBSOCKET_FRAME };
	public:
    WebSocketResponseSession(SocketChannel &channel, BootstrapConfig &config);
		virtual ~WebSocketResponseSession() = default;

		/**
		*����websocket frame����
		* @param websocket frame��Ӧ����
		* @param callback ���ͽ�����Ļص�����
		* @return ʧ�ܷ���false, �ɹ�����true
		*/
		bool WriteWebSocketFrame(WebSocketFrame &frame, WriteCompleteCallback callback = NULL);
			
	protected:
		//�������ֱ���
		void DoHandshake(const std::string &path, std::map< std::string, std::string> *header = NULL);
		
	private:
		//�������Ľ�����
		virtual MessageDecoder *CreateDecoder();
		//�����ı��Ĵ�����
		virtual bool HandleMessage(Message &message);
		//���ִ���
		bool Handshake(HttpFullResponse& response);

		//���ֳɹ���
		virtual void HandleHandshaked();
		//websocket frame���Ĵ���
		virtual bool HandleWebSocketFrame(WebSocketFrame&) = 0;

	protected:
		int current_state_; /***��ǰ״̬***/
		std::string expected_accept_key_; /***���ֱ��ĺ�������Sec-Websocket-Acceptֵ***/
	};
	
	class WebSocketFullResponseSession: public WebSocketResponseSession {
	public:
    WebSocketFullResponseSession(SocketChannel &channel, BootstrapConfig &config);
		virtual ~WebSocketFullResponseSession() = default;

	protected:
		//�������Ľ�����
		virtual MessageDecoder *CreateDecoder();
	};
}
#endif
