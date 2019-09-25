#ifndef LIM_EXECUTE_TASK_H
#define LIM_EXECUTE_TASK_H
#include <deque>
#include <vector>
#include <mutex>
#include <thread> 
#include <condition_variable>

namespace lim {
	namespace ExecuteEvent { 
		const int NONE_EVENT = 0;
		const int INIT_EVENT = 1;
		const int READ_EVENT = 2;
		const int WRITE_EVENT = 4;
		const int KILL_EVENT = 8;
		const int USER_EVENT = 16;
	};
	
	class ExecuteTask;
	class ExecuteThread;
	class ExecuteTimer;
	using MaxHeapUnit = std::tuple<int64_t, ExecuteTimer*, bool>; //std::tuple<timestamp, ExecuteTimer, is_in_heap>
	using TimeoutCallback = std::function<void()>;
	//��ʱ��(�ײ�ͨ������ʵ��)
	class ExecuteTimer {
	public:
		/**
		*��ʱ�����캯��
		* @param execute_thread ��ʱ��ִ���߳�
		* @param callback ��ʱ���ص�����
		*/
		ExecuteTimer(ExecuteThread &execute_thread, TimeoutCallback callback);
		virtual ~ExecuteTimer();

		void Start(int milli_sceonds = 0);
		void Cancel();
		
	private:
		ExecuteTimer(const ExecuteTimer& other) = delete;
		ExecuteTimer &operator=(const ExecuteTimer& other) = delete;

	private:
		TimeoutCallback callback_;
		MaxHeapUnit timeout_unit_;
		ExecuteThread &execute_thread_;
		friend class ExecuteThread;
	};
	
	using DeQueueUnit = std::tuple<ExecuteTask*, bool>; //std::tuple<ExecuteTask, is_in_queue>
	//��ִ������,�����������������������̹߳���(���������󲻴����¼��ͳ�ʱ,�п�������ڴ�й©)
	class ExecuteTask {
	public:
		/**
		*��ִ�������캯��
		* @param execute_thread ִ���߳�
		*/
		ExecuteTask(ExecuteThread &execute_thread);
		virtual ~ExecuteTask();
		
	private:
		ExecuteTask(const ExecuteTask& other) = delete;
		ExecuteTask &operator=(const ExecuteTask& other) = delete;

	public:
		//�¼���������
		void Signal(int execute_events);
		//��ȡ�¼���Ϣ
		int GetEvents();
		//��ȡִ���߳�
		ExecuteThread &GetExecuteThread() { return execute_thread_; }

		//��ʼ��
		virtual bool Initialize();

	private:
		//Kill�¼�������
		virtual bool HandleKillEvent();
		//��ʼ���¼�������
		virtual bool HandleInitEvent();
		//���¼�������
		virtual bool HandleReadEvent();
		//д�¼�������
		virtual bool HandleWriteEvent();
		//�Զ����¼�������
		virtual bool HandleUserEvent(int user_events);
	
	protected:
		virtual bool Run();

	private:
		std::mutex mutex_;
		int execute_events_;  /***��ǰ�������¼�***/
		DeQueueUnit deque_unit_; /***�¼�������е�Ԫ***/
		ExecuteThread &execute_thread_; /***�󶨵�ִ���߳�***/
		friend class ExecuteThread;
  };
	
	class ExecuteThread {
	public:
		ExecuteThread();
		virtual ~ExecuteThread();

	private:
		ExecuteThread(const ExecuteThread& other) = delete;
		ExecuteThread &operator=(const ExecuteThread& other) = delete;
		
	protected:
		virtual void Run();

		void AddToQueue(DeQueueUnit *deque_unit);
		void RemoveFromQueue(DeQueueUnit *deque_unit);

		void AddToMaxHeap(MaxHeapUnit *heap_unit);
		void RemoveFromMaxHeap(MaxHeapUnit *heap_unit);

	protected:
		bool is_running_;
		std::thread thread_;
		std::mutex mutex_;
		std::condition_variable condvar_;
    
		std::vector<MaxHeapUnit*> timeout_heap_; /***��ʱ���б�(����)***/
		std::deque<DeQueueUnit*> task_que_; /***�����б�***/
		friend class ExecuteTimer;
		friend class ExecuteTask;
  };

	class ExecuteThreadGroup {
	public:
		ExecuteThreadGroup(int execute_thread_num = 0);
		virtual ~ExecuteThreadGroup();

	private:
		ExecuteThreadGroup(const ExecuteThreadGroup& other) = delete;
		ExecuteThreadGroup &operator=(const ExecuteThreadGroup& other) = delete;

	public:
		ExecuteThread &Next();
		
	protected:
		std::mutex mutex_;
		int execute_thread_num_;
		int execute_thread_index_;
		ExecuteThread **execute_threads_;
	};
}
#endif
