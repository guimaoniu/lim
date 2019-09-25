#ifndef LIM_WEBSOCKET_FRAME_MESSAGE_H
#define LIM_WEBSOCKET_FRAME_MESSAGE_H
#include <map>
#include <lim/base/byte_buffer.h>
#include <lim/base/message_decoder.h>

namespace lim {
	class WebSocketFrame: public Message {
	public:
		WebSocketFrame(int frame_opcode, bool frame_final_flag, int frame_rsv);
		virtual ~WebSocketFrame() = default;

		int &FrameOpCode() { return frame_opcode_; }
		bool &FrameFinalFlag() { return frame_final_flag_; }
		int &FrameRsv() { return frame_rsv_; }
		bool &FrameMasked() { return frame_masked_; }
		ByteBuffer &FrameContent() { return frame_content_; }
		virtual int ToBytes(ByteBuffer &buffer);

	private:
		bool frame_final_flag_; /***������Ϣ�Ƿ����***/
		int frame_rsv_; /***��չλ***/
		int frame_opcode_; /***��չλ***/
		bool frame_masked_; /***��Ϣ����***/
		ByteBuffer frame_content_; /***��Ϣ����***/
  };

	class ContinuationWebSocketFrame: public WebSocketFrame {
	public:
		ContinuationWebSocketFrame(bool frame_final_flag, int frame_rsv);
		virtual ~ContinuationWebSocketFrame() = default;
	};

	class TextWebSocketFrame: public WebSocketFrame {
	public:
		TextWebSocketFrame(bool frame_final_flag, int frame_rsv);
		virtual ~TextWebSocketFrame() = default;
	};

	class BinaryWebSocketFrame: public WebSocketFrame {
	public:
		BinaryWebSocketFrame(bool frame_final_flag, int frame_rsv);
		virtual ~BinaryWebSocketFrame() = default;
	};

	class CloseWebSocketFrame: public WebSocketFrame {
	public:
		CloseWebSocketFrame(bool frame_final_flag, int frame_rsv);
		virtual ~CloseWebSocketFrame() = default;
	};

	class PingWebSocketFrame: public WebSocketFrame {
	public:
		PingWebSocketFrame(bool frame_final_flag, int frame_rsv);
		virtual ~PingWebSocketFrame() = default;
	};
	
	class PongWebSocketFrame: public WebSocketFrame {
	public:
		PongWebSocketFrame(bool frame_final_flag, int frame_rsv);
		virtual ~PongWebSocketFrame() = default;
	};
}
#endif
