#ifndef LIM_HTTP_BASE_DECODER_H
#define LIM_HTTP_BASE_DECODER_H
#include <lim/http/http_message.h>

namespace lim {
	class HttpMessageError: public MessageError {
	public:
		HttpMessageError(const std::string &error_message): MessageError(error_message) {
		}
		virtual ~HttpMessageError() = default;
	};
		
	class HttpBaseDecoder: public MessageDecoder {
	private:
		enum { READ_INITIAL, READ_HEADER, READ_CHUNK_SIZE, READ_FIXED_LENGTH_CONTENT,
					 READ_VARIABLE_LENGTH_CONTENT, READ_CHUNKED_CONTENT, READ_CHUNK_DELIMITER,
					 READ_CHUNK_FOOTER };
	public:
		/**
		*Http����������
		* @param max_first_line_size ����������󳤶�
		* @param max_header_size header��󳤶�
		* @param max_content_size content��󳤶�
		*/
		HttpBaseDecoder(int max_first_line_size, int max_header_size, int max_content_size);
		virtual ~HttpBaseDecoder() = default;

		virtual void Reset();
		/**
		*���뺯��
		* @param buffer ���ݻ�����
		* @param message_callback ������message�ص�����
		* @param error_callback ������Ϣ�ص�����
		* @param is_socket_closed socket�����Ƿ��ѶϿ�
		*/
		virtual bool Decode(ByteBuffer &buffer, 
                      HandleMessageCallback &message_callback, 
                      HandleErrorCallback &error_callback,
                      bool is_socket_closed);
		
	protected:
		virtual bool IsChunked() = 0;
		virtual int64_t GetContentLength() = 0;
		virtual bool CreatHttpMessage(const std::string &http_first_line) = 0;
		virtual void SetHeaderValue(const std::string &header_name, const std::string &header_value) = 0;
		//http message������
		virtual bool DoHttpMessage(HandleMessageCallback &message_callback) = 0;
		//http content������
		virtual bool DoHttpContent(HttpContent &content, HandleMessageCallback &message_callback) = 0;

	private:
		int ReadContent(ByteBuffer &buffer, HttpContent &content);

	protected:
		int current_state_; /***��ǰ״̬***/
		int64_t chunk_size_; /***content/chunk��С***/
		HttpHeaders trailing_headers_; /***ֻ������chunk��ʽ***/

		int max_first_line_size_; /***����������󳤶�***/
		int max_header_size_; /***header��󳤶�***/
		int max_content_size_; /***content��󳤶�***/
		int bytes_for_current_state_; /***��ǰ״̬�Ѿ����յ��ֽ���***/
	};
}
#endif
