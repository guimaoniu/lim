#ifndef LIM_CONNECTED_CHANNEL_SESSION_H
#define LIM_CONNECTED_CHANNEL_SESSION_H
#include <lim/config.h>
#include <lim/base/socket_channel.h>
#include <lim/base/execute_task.h>
#include <lim/base/event_loop.h>
#include <lim/base/byte_buffer.h>
#include <lim/base/message_decoder.h>
#include <lim/base/bootstrap_config.h>

namespace lim {
	using WriteCompleteCallback = std::function<void()>;
	using WriteUnit = std::tuple<ByteBuffer, WriteCompleteCallback>;
	class ChannelClosedError : public MessageError {
	public:
		ChannelClosedError(const std::string &error_message): MessageError(error_message) {
		};
		virtual ~ChannelClosedError() = default;
	};
	class ReadBufferOverflowError : public MessageError {
	public:
		ReadBufferOverflowError(const std::string &error_message): MessageError(error_message) {
		};
		virtual ~ReadBufferOverflowError() = default;
	};
#ifdef ENABLE_OPENSSL
	class SSLHandshakeError : public MessageError {
	public:
		SSLHandshakeError(const std::string &error_message): MessageError(error_message) {
		};
		virtual ~SSLHandshakeError() = default;
	};
#endif
	class ConnectedChannelSession: public ExecuteTask {
	public:
		ConnectedChannelSession(SocketChannel &channel, BootstrapConfig &config);
		virtual ~ConnectedChannelSession();
		
		/**
		*�첽��������
		* @param buffer ���ͻ�����
		* @param callback ���ͽ�����Ļص�����
		* @return ʧ�ܷ���false, �ɹ�����true
		*/
		bool WriteData(ByteBuffer &buffer, WriteCompleteCallback callback = NULL);
		/**
		*�첽������Ϣ����
		* @param message ��Ϣ����
		* @param callback ���ͽ�����Ļص�����
		* @return ʧ�ܷ���false, �ɹ�����true
		*/
		bool WriteMessage(Message &message, WriteCompleteCallback callback = NULL);
		
	protected:
		//�������Ľ�����
		virtual MessageDecoder *CreateDecoder() = 0;
		//�����ı��Ĵ�����
		virtual bool HandleMessage(Message &message) = 0;

		//��ʼ���¼�������
		virtual bool HandleInitEvent();
		//���¼�������
		virtual bool HandleReadEvent();
		//д�¼�������
		virtual bool HandleWriteEvent();
		//������Ϣ������
		virtual void HandleMessageError(MessageError &error);
#ifdef ENABLE_OPENSSL
		//SSL���ֳɹ�������
		virtual bool HandleSSLHandshaked();
#endif	

	protected:
		SocketChannel channel_; /***socket���Ӷ���***/
		EventLoop &event_loop_; /***�¼�������***/
		ByteBuffer recv_buffer_; /***���ջ���***/
		
		BootstrapConfig &config_; /***��������(����Э�����)***/
		MessageDecoder *message_decoder_; /***���Ľ�����(����Э�����)***/
		
		int64_t last_read_timestamp_; /***���һ�ζ�ʱ���(����)***/
		int64_t last_write_timestamp_; /***���һ��дʱ���(����)***/
		ExecuteTimer *timeout_timer_; /***��д��ʱ��ʱ��(��ʱ�˳�)***/

#ifdef ENABLE_OPENSSL
		int read_waiton_flag_;
		int write_waiton_flag_;
#endif
		
		std::mutex mutex_;
		std::deque<WriteUnit> write_unit_que_;
  };
}
#endif
