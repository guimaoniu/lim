#ifndef LIM_WEBSOCKET_FRAME_DECODER_H
#define LIM_WEBSOCKET_FRAME_DECODER_H
#include <lim/base/message_decoder.h>
#include <lim/websocket/websocket_frame_message.h>

namespace lim {
	class WebSocketHandshakeError: public MessageError {
	public:
		WebSocketHandshakeError(const std::string &error_message);
		virtual ~WebSocketHandshakeError() = default;
	};
	class WebSocketMessageError: public MessageError {
	public:
		WebSocketMessageError(const std::string &error_message);
		virtual ~WebSocketMessageError() = default;
	};
	class WebSocketFrameDecoder: public MessageDecoder {
	private:
		enum { READING_FIRST, READING_SECOND, READING_SIZE, MASKING_KEY, PAYLOAD, CORRUPT };
	public:
		WebSocketFrameDecoder(int max_payload_size, bool expected_frame_masked_flag);
		virtual ~WebSocketFrameDecoder() = default;

		virtual void Reset();
		/**
		*���뺯��
		* @param buffer ���ݻ�����
		* @param message_callback ������message�ص�����
		* @param error_callback ������Ϣ�ص�����
		* @param is_socket_closed socket�����Ƿ��ѶϿ�
		*/
		virtual bool Decode(ByteBuffer &buffer, 
                      HandleMessageCallback &callback, 
                      HandleErrorCallback &error_callback, 
                      bool is_socket_closed);

  protected:
    WebSocketFrame *CreateWebSocketFream(ByteBuffer &buffer);
    WebSocketFrame *CloneWebSocketFream(WebSocketFrame &frame);
		//websocket frame������
    virtual bool DoContent(WebSocketFrame &frame, HandleMessageCallback &message_callback, HandleErrorCallback &error_callback);
		
	protected:
		int current_state_; /***��ǰ״̬***/
		int max_payload_size_; /***websocket������󳤶�***/
		bool expected_frame_masked_flag_; /***������Ϣ�Ƿ����***/

		bool frame_final_flag_; /***������Ϣ�Ƿ����***/
		int frame_rsv_; /***��չλ***/
		int frame_opcode_; /***��չλ***/
		bool frame_masked_; /***��Ϣ����***/

		char frame_mask_[4]; /***������Ϣ***/
    int64_t frame_payload_length_; /***��Ϣ����***/
	};
	
	class WebSocketFullFrameDecoder : public WebSocketFrameDecoder {
	public:
		WebSocketFullFrameDecoder(int max_payload_size, bool expected_frame_masked_flag);
		virtual ~WebSocketFullFrameDecoder();

	protected:
		//websocket frame������
		virtual bool DoContent(WebSocketFrame &frame, HandleMessageCallback &message_callback, HandleErrorCallback &error_callback);
  
	private:
		WebSocketFrame *full_frames_[16];
	};
}
#endif
