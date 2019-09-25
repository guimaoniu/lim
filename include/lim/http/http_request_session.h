#ifndef LIM_HTTP_REQUEST_SESSION_H
#define LIM_HTTP_REQUEST_SESSION_H
#include <lim/http/http_base_decoder.h>
#include <lim/base/connected_channel_session.h>

namespace lim {
	class HttpRequestDecoder: public HttpBaseDecoder {
	public:
		/**
		*Http���������
		* @param max_first_line_size ����������󳤶�
		* @param max_header_size header��󳤶�
		* @param max_content_size content��󳤶�
		*/
		HttpRequestDecoder(int max_first_line_size, int max_header_size, int max_content_size);
		virtual ~HttpRequestDecoder();

		virtual void Reset();

	protected:
		virtual bool IsChunked();
		virtual int64_t GetContentLength();
		virtual bool CreatHttpMessage(const std::string &http_first_line);
		virtual void SetHeaderValue(const std::string &header_name, const std::string &header_value);
		//http message������
		virtual bool DoHttpMessage(HandleMessageCallback &message_callback);
		//http content������
		virtual bool DoHttpContent(HttpContent &content, HandleMessageCallback &message_callback);

	protected:
		HttpRequest *http_request_;
	};

	typedef std::function<bool(Message&)> HttpRequstHandle;
	class HttpRequestSession: public ConnectedChannelSession {
	public:
		HttpRequestSession(SocketChannel &channel, BootstrapConfig &config);
		virtual ~HttpRequestSession() = default;

		/**
		*����http��Ӧ����(�Զ�����Serverͷ��Ϣ)
		* @param response http��Ӧ����
		* @param callback ���ͽ�����Ļص�����
		* @return ʧ�ܷ���false, �ɹ�����true
		*/
		bool WriteHttpResponse(HttpResponse &response, WriteCompleteCallback callback = NULL);

		/**
		*����http body��Ӧ����
		* @param content http body��Ӧ����
		* @param callback ���ͽ�����Ļص�����
		* @return ʧ�ܷ���false, �ɹ�����true
		*/
		bool WriteHttpConent(HttpContent &content, WriteCompleteCallback callback = NULL);
	protected:
		/**
		*ע�ᴦ��·��
		* @param method ��������
		* @param path ·��
		* @param handle �ص�����
		*/
		bool RegistHandleRouter(const std::string &method, const std::string &path, HttpRequstHandle &handle);

	private:
		//�������Ľ�����
		virtual MessageDecoder *CreateDecoder();
		//�����ı��Ĵ�����
		virtual bool HandleMessage(Message &message);

	protected:
		//std::map<path, std::pair<method, handle>>
		std::map<std::string, std::pair<std::string, HttpRequstHandle>> handle_routers_;
		HttpRequstHandle current_handle_;
	};

	class HttpFullRequestDecoder: public HttpRequestDecoder {
	public:
		/**
		*Http full���������
		* @param max_first_line_size ����������󳤶�
		* @param max_header_size header��󳤶�
		* @param max_content_size content��󳤶�
		*/
		HttpFullRequestDecoder(int max_first_line_size, int max_header_size, int max_content_size);
		virtual ~HttpFullRequestDecoder() = default;

	protected:
		virtual bool CreatHttpMessage(const std::string &http_first_line);
		//http message������
		virtual bool DoHttpMessage(HandleMessageCallback &message_callback);
		//http content������
		virtual bool DoHttpContent(HttpContent &content, HandleMessageCallback &message_callback);
	};

	class HttpFullRequestSession : public HttpRequestSession {
	public:
		HttpFullRequestSession(SocketChannel &channel, BootstrapConfig &config);
		virtual ~HttpFullRequestSession() = default;

	private:	
		//�������Ľ�����
		virtual MessageDecoder *CreateDecoder();
	};
}
#endif
