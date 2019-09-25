#ifndef LIM_MESSAGE_CODER_H
#define LIM_MESSAGE_CODER_H
#include <lim/base/byte_buffer.h>
#include <functional>

namespace lim {
	class Message {
	public:
		Message() = default;
		virtual ~Message() = default;
		
	public:
		virtual int ToBytes(ByteBuffer &buffer) = 0;
	};
	class MessageError {
	public:
		MessageError(const std::string &error_message): error_message_(error_message) {
		}
		virtual ~MessageError() = default;
		
	public:
		std::string GetErrorMessage() { return error_message_; }

	protected:
		std::string error_message_;
	};
		
	typedef std::function<bool(Message&)> HandleMessageCallback;
	typedef std::function<void(MessageError&)> HandleErrorCallback;
	class MessageDecoder {
	public:
		MessageDecoder() = default;
		virtual ~MessageDecoder() = default;

		virtual void Reset() = 0;
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
                        bool is_socket_closed) = 0;
	};
}
#endif
