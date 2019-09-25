#ifndef LIM_SERVER_CHANNEL_SESSION_H
#define LIM_SERVER_CHANNEL_SESSION_H
#include <lim/base/socket_channel.h>
#include <lim/base/bootstrap_config.h>

namespace lim {
	template<typename T>
	class ServerChannelSession: public ExecuteTask {
	public:
		ServerChannelSession(SocketChannel &channel, BootstrapConfig &config):
			channel_(channel), config_(config),
			event_loop_(config.ServerEventLoop()), ExecuteTask(config.ServerExecuteThread()) {
		}
				
		virtual ~ServerChannelSession() {
			event_loop_.RemoveChannel(channel_);
		}
		
	private:
		//��ʼ���¼�������
		virtual bool HandleInitEvent() {
			event_loop_.AddChannel(channel_, this);
			return true;
		}
		
		//���¼�������
		virtual bool HandleReadEvent() {
			std::vector<SocketChannel>socket_channels;
			channel_.Accept(socket_channels);
			for (size_t i = 0; i < socket_channels.size(); i++) {
				T *session = new T(socket_channels[i], config_);
				session->Signal(ExecuteEvent::INIT_EVENT);
			}
			return true;
		}

	private:
		SocketChannel channel_; /***socket���Ӷ���***/
		EventLoop &event_loop_; /***�¼�������***/
		BootstrapConfig &config_; /***��������(����Э�����)***/
  };
}
#endif
