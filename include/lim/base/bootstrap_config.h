#ifndef LIM_BOOTSTRAP_CONFIG_H
#define LIM_BOOTSTRAP_CONFIG_H
#include <lim/base/event_loop.h>
#include <lim/base/execute_task.h>

namespace lim {
  enum class LoggerLevel { LOG_DEBUG, LOG_INFO, LOG_WARN, LOG_ERROR };
  /**
  * ��־�ص���������
  * @param [in] LoggerLevel ��־�ȼ�
  * @param [in] string ��־����
  */
  typedef std::function<void(LoggerLevel, const std::string&)> LoggerCallback;
  
  class BootstrapConfig {
  public:
    BootstrapConfig(EventLoopGroup &event_loop_group, ExecuteThreadGroup &execute_thread_group):
      event_loop_group_(event_loop_group), execute_thread_group_(execute_thread_group),
      server_event_loop_(event_loop_group.Next()), server_execute_thread_(execute_thread_group.Next()),
      max_buffer_size_(1024*1024*4), logger_callback_(NULL), timeout_millisec_(60*1000) {
    }

    BootstrapConfig(EventLoopGroup &event_loop_group,
                  ExecuteThreadGroup &execute_thread_group,
                  EventLoop &server_event_loop,
                  ExecuteThread &server_execute_thread) :
      event_loop_group_(event_loop_group), execute_thread_group_(execute_thread_group),
      server_event_loop_(server_event_loop), server_execute_thread_(server_execute_thread),
      max_buffer_size_(1024 * 1024 * 4), logger_callback_(NULL), timeout_millisec_(60 * 1000) {

    }

    virtual ~BootstrapConfig() = default;

    //��ȡһ�����õ��¼�������(����connect/accept����)
    EventLoop &NextEventLoop() { return event_loop_group_.Next(); }
    //��ȡһ�����õĹ����߳�(����connect/accept����)
    ExecuteThread &NextExecuteThread() { return execute_thread_group_.Next(); }

    //��ȡ�¼�������(����listen����)
    EventLoop &ServerEventLoop() { return server_event_loop_; }
    //��ȡ�����߳�(����listen����)
    ExecuteThread &ServerExecuteThread() { return server_execute_thread_; }

    //������־�ص�����
    void SetLoggerCallback(LoggerCallback callback) { logger_callback_ = callback; }
    //��ȡ��־�ص�����
    LoggerCallback GetLoggerCallback() { return logger_callback_; }

    //���������ջ����С
    void SetMaxBufferSize(int max_buffer_size) { max_buffer_size_ = max_buffer_size; }
    //��ȡ�����ջ����С
    int GetMaxBufferSize() { return max_buffer_size_; }

    //���ó�ʱʱ��(����)
    void SetTimeout(int timeout_millisec) { timeout_millisec_ = timeout_millisec; }
    //��ȡ��ʱʱ��(����)
    int GetTimeout() { return timeout_millisec_; }

	protected:
    int max_buffer_size_; /***�����ջ����С***/
    int timeout_millisec_; /***��ʱʱ��(����),-1Ϊ�����ó�ʱ***/
    LoggerCallback logger_callback_; /***��־�ص�����***/
		
    EventLoopGroup &event_loop_group_; /***�¼�����������(����connect/accept����)***/
    ExecuteThreadGroup &execute_thread_group_; /***�����̳߳�(����connect/accept����)***/
		
    EventLoop &server_event_loop_; /***�¼�������(����listen����)***/
    ExecuteThread &server_execute_thread_; /***�����߳�(����listen����)***/  
  };
}
#endif
