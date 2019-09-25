#ifndef LIM_WEBSOCKET_REQUEST_SESSION_H
#define LIM_WEBSOCKET_REQUEST_SESSION_H
#include <lim/websocket/websocket_frame_decoder.h>
#include <lim/http/http_request_session.h>

namespace lim {
	typedef std::function<bool(WebSocketFrame&)> WebSocketFrameHandle;
	class WebSocketRequestSession: public HttpFullRequestSession {
	protected:
		enum { HTTP_HANDSHAKE, WEBSOCKET_FRAME };
	public:
		WebSocketRequestSession(SocketChannel &channel, BootstrapConfig &config);
		virtual ~WebSocketRequestSession() = default;

		/**
		*����websocket frame����
		* @param websocket frame��Ӧ����
		* @param callback ���ͽ�����Ļص�����
		* @return ʧ�ܷ���false, �ɹ�����true
		*/
		bool WriteWebSocketFrame(WebSocketFrame &frame, WriteCompleteCallback callback = NULL);
		
	protected:
		/**
		*ע�ᴦ��·��
		* @param path ·��
		* @param handle �ص�����
		*/
		bool RegistHandleRouter(const std::string &path, WebSocketFrameHandle &handle);

	private:
		//�������Ľ�����
		virtual MessageDecoder *CreateDecoder();
		//�����ı��Ĵ�����
		virtual bool HandleMessage(Message &message);
		//���ִ���
		bool Handshake(HttpFullRequest& request);

	protected:
		int current_state_; /***��ǰ״̬***/
		//std::map<path, handle>
		std::map<std::string, WebSocketFrameHandle> handle_routers_;
		WebSocketFrameHandle current_handle_;
	};
	
	class WebSocketFullRequestSession: public WebSocketRequestSession {
	public:
		WebSocketFullRequestSession(SocketChannel &channel, BootstrapConfig &config);
		virtual ~WebSocketFullRequestSession() = default;

	protected:
		//�������Ľ�����
		virtual MessageDecoder *CreateDecoder();
	};
}
#endif
