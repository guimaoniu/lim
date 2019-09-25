#ifndef LIM_HTTP_RESPONSE_SESSION_H
#define LIM_HTTP_RESPONSE_SESSION_H
#include <lim/http/http_base_decoder.h>
#include <lim/base/connected_channel_session.h>

namespace lim {
	class HttpResponseDecoder: public HttpBaseDecoder {
	public:
		/**
		*Http��Ӧ������
		* @param max_first_line_size ����������󳤶�
		* @param max_header_size header��󳤶�
		* @param max_content_size content��󳤶�
		*/
		HttpResponseDecoder(int max_first_line_size, int max_header_size, int max_content_size);
		virtual ~HttpResponseDecoder();

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
		HttpResponse *http_response_;
	};
	
	class HttpResponseSession : public ConnectedChannelSession {
	public:
		HttpResponseSession(SocketChannel &channel, BootstrapConfig &config);
		virtual ~HttpResponseSession() = default;
		
		/**
		*����http������(�Զ�����User-Agent��Hostͷ��Ϣ)
		* @param response http��Ӧ����
		* @param callback ���ͽ�����Ļص�����
		* @return ʧ�ܷ���false, �ɹ�����true
		*/
		bool WriteHttpRequest(HttpRequest &request, WriteCompleteCallback callback = NULL);
		/**
		*����http body������
		* @param content http body������
		* @param callback ���ͽ�����Ļص�����
		* @return ʧ�ܷ���false, �ɹ�����true
		*/
		bool WriteHttpConent(HttpContent &content, WriteCompleteCallback callback = NULL);
	private:
		//�������Ľ�����
		virtual MessageDecoder *CreateDecoder();
		//�����ı��Ĵ�����
		virtual bool HandleMessage(Message &message) = 0;
	};

	class HttpFullResponseDecoder: public HttpResponseDecoder {
	public:
		/**
		*Http full��Ӧ������
		* @param max_first_line_size ����������󳤶�
		* @param max_header_size header��󳤶�
		* @param max_content_size content��󳤶�
		*/
		HttpFullResponseDecoder(int max_first_line_size, int max_header_size, int max_content_size);
		virtual ~HttpFullResponseDecoder() = default;

	protected:
		virtual bool CreatHttpMessage(const std::string &http_first_line);
		//http message������
		virtual bool DoHttpMessage(HandleMessageCallback &message_callback);
		//http content������
		virtual bool DoHttpContent(HttpContent &content, HandleMessageCallback &message_callback);
	};
	
	class HttpFullResponseSession : public HttpResponseSession {
	public:
		HttpFullResponseSession(SocketChannel &channel, BootstrapConfig &config);
		virtual ~HttpFullResponseSession() = default;

	private:
		//�������Ľ�����
		virtual MessageDecoder *CreateDecoder();
	};
}
#endif
