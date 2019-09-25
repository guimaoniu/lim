#include <lim/base/execute_task.h>
#include <lim/base/time_utils.h>
#include <assert.h>
#include <algorithm>
#include <chrono>

namespace lim {
	#define EXECUTE_TASK_SLEEP_TIME 10
	ExecuteTimer::ExecuteTimer(ExecuteThread &execute_thread, TimeoutCallback callback):
		execute_thread_(execute_thread), callback_(callback) {

		timeout_unit_ = std::make_tuple(0, this, false);
	}
			
	ExecuteTimer::~ExecuteTimer() {
		Cancel();
	}

	void ExecuteTimer::Start(int milli_sceonds) {
		std::get<0>(timeout_unit_) = CurrentMilliTime() + milli_sceonds;
		execute_thread_.AddToMaxHeap(&timeout_unit_);
	}
	
	void ExecuteTimer::Cancel() {
		execute_thread_.RemoveFromMaxHeap(&timeout_unit_);
	}
	
	ExecuteTask::ExecuteTask(ExecuteThread &execute_thread): 
		execute_thread_(execute_thread), execute_events_(ExecuteEvent::NONE_EVENT) {
			
		deque_unit_ = std::make_tuple(this, false);
  }

	ExecuteTask::~ExecuteTask() {
		execute_thread_.RemoveFromQueue(&deque_unit_);
	}

	//�¼���������
	void ExecuteTask::Signal(int execute_events) {
		std::lock_guard<std::mutex> guard(mutex_);
		execute_events_ |= execute_events;
		execute_thread_.AddToQueue(&deque_unit_);
	}

	//��ȡ�¼���Ϣ
	int ExecuteTask::GetEvents() {
		std::lock_guard<std::mutex> guard(mutex_);
		int execute_events = execute_events_;
		execute_events_ = 0;
		return execute_events;
  }

	//��ʼ��
	bool ExecuteTask::Initialize() {
		return true;
	}

	//Kill�¼�������
	bool ExecuteTask::HandleKillEvent() {
		return false;
	}

	//��ʼ���¼�������
	bool ExecuteTask::HandleInitEvent() {
		return true;
	}

	//���¼�������
	bool ExecuteTask::HandleReadEvent() {
		return true;
	}

	//д�¼�������
	bool ExecuteTask::HandleWriteEvent() {
		return true;
	}

	//�Զ����¼�������
	bool ExecuteTask::HandleUserEvent(int user_events) {
		return true;
	}
	
	bool ExecuteTask::Run() {
		int events = GetEvents();
		if (events & ExecuteEvent::KILL_EVENT) {
			return HandleKillEvent();
		}

		if ((events & ExecuteEvent::INIT_EVENT) && !HandleInitEvent()) {
			return false;
		}
		
		if ((events & ExecuteEvent::READ_EVENT) && !HandleReadEvent()) {
			return false;
		}

		if ((events & ExecuteEvent::WRITE_EVENT) && !HandleWriteEvent()) {
			return false;
		}

		if ((events >= ExecuteEvent::USER_EVENT) && !HandleUserEvent(events)) {
			return false;
		}
		return true;
	}

	bool MaxHeapUnitCompare(const MaxHeapUnit *a, const MaxHeapUnit *b) {
		return std::get<0>(*a) > std::get<0>(*b);
	}
	
	ExecuteThread::ExecuteThread() : is_running_(true) {
		thread_ = std::thread(&ExecuteThread::Run, this);
		std::make_heap(timeout_heap_.begin(), timeout_heap_.end(), MaxHeapUnitCompare);
	}
	
	ExecuteThread::~ExecuteThread() {
		is_running_ = false;
		condvar_.notify_one();
		thread_.join();
	}

	void ExecuteThread::Run() {
		while (is_running_) {
			{
				std::unique_lock<std::mutex> lock(mutex_);
				condvar_.wait(lock, [this] {
					return (this->timeout_heap_.size() > 0 || this->task_que_.size() > 0 || this->is_running_ == false);
				});
			}

			//1.���ȴ����¼���Ϣ
			ExecuteTask *execute_task = NULL;
			{
				//1.1.�Ӷ����л�ȡ�¼���Ԫ
				std::unique_lock<std::mutex> guard(mutex_);
				if (task_que_.size() > 0) {
					execute_task = std::get<0>(*task_que_.front());
				}
			}
		
			if (execute_task != NULL) {
				//1.2.�Ƴ��¼���Ԫ��ִ��,Run����falseʱ���ٶ���
				RemoveFromQueue(&execute_task->deque_unit_);
				if (!execute_task->Run()) {
					delete execute_task;
				}
			}

			//2.ִ�ж�ʱ��
			ExecuteTimer *execute_timer = NULL;
			{
				//2.1.�������л�ȡ��ʱ�Ķ�ʱ��
				std::unique_lock<std::mutex> guard(mutex_);
				if (timeout_heap_.size() > 0 && std::get<0>(*(timeout_heap_[0])) <= CurrentMilliTime()) {
					execute_timer = std::get<1>(*timeout_heap_[0]);
				}
			}

			//2.2.�Ƴ���ʱ����ִ�г�ʱ�ص�����
			if (execute_timer != NULL) {
				RemoveFromMaxHeap(&execute_timer->timeout_unit_);
				if (execute_timer->callback_ != NULL) {
					execute_timer->callback_();
				}
			}

			//3.��������߳�û���¼���Ҫ����,����sleep�ͷ�CPUռ��
			if (execute_task == NULL) {
				std::this_thread::sleep_for(std::chrono::milliseconds(EXECUTE_TASK_SLEEP_TIME));
			}
		}

		//4.�߳��˳�ʱ���ٶ����е�����, �̲߳�����ʱ��������
		auto iter = task_que_.begin();
		while (iter != task_que_.end()) {
			ExecuteTask *execute_task = std::get<0>(**iter);
			task_que_.erase(iter);
				
			delete execute_task;
			iter = task_que_.begin();
		}
	}
	
	void ExecuteThread::AddToQueue(DeQueueUnit *deque_unit) {
		std::unique_lock<std::mutex> guard(mutex_);
		if (std::get<1>(*deque_unit) == false) { //if the unit is not in the queue
			task_que_.push_back(deque_unit);
			std::get<1>(*deque_unit) = true;
			condvar_.notify_one();
		}
	}
	
	void ExecuteThread::RemoveFromQueue(DeQueueUnit *deque_unit) {
		std::unique_lock<std::mutex> guard(mutex_);
		if (std::get<1>(*deque_unit) == false) //if the unit is not in the queue
			return;
	
		for (auto iter = task_que_.begin(); iter != task_que_.end(); iter++) {
			if (*iter == deque_unit) {
				task_que_.erase(iter);
				std::get<1>(*deque_unit) = false;
				break;
			}
		}
	}
	
	void ExecuteThread::AddToMaxHeap(MaxHeapUnit *heap_unit) {
		std::unique_lock<std::mutex> guard(mutex_);
		if (std::get<2>(*heap_unit) == true) { //if the unit is in the heap
			for (auto iter = timeout_heap_.begin(); iter != timeout_heap_.end(); iter++) {
				if (*iter == heap_unit) {
					timeout_heap_.erase(iter);
					std::get<2>(*heap_unit) = false;
					//std::make_heap(timeout_heap_.begin(), timeout_heap_.end(), MaxHeapUnitCompare);
					break;
				}
			}
		}
	
		timeout_heap_.push_back(heap_unit);
		std::get<2>(*heap_unit) = true;
		push_heap(timeout_heap_.begin(), timeout_heap_.end(), MaxHeapUnitCompare);
		condvar_.notify_one();
	}
	
	void ExecuteThread::RemoveFromMaxHeap(MaxHeapUnit *heap_unit) {
		std::unique_lock<std::mutex> guard(mutex_);
		if (std::get<2>(*heap_unit) == false) //if the unit is not in the heap
			return;
	
		for (auto iter = timeout_heap_.begin(); iter != timeout_heap_.end(); iter++) {
			if (*iter == heap_unit) {
				timeout_heap_.erase(iter);
				std::get<2>(*heap_unit) = false;
				std::make_heap(timeout_heap_.begin(), timeout_heap_.end(), MaxHeapUnitCompare);
				break;
			}
		}
	}
		
	ExecuteThreadGroup::ExecuteThreadGroup(int execute_thread_num): 
		execute_thread_num_(execute_thread_num), execute_thread_index_(0) {
		if (execute_thread_num_ <= 0) {
      execute_thread_num_ = 2*std::thread::hardware_concurrency();
    }
		
		execute_threads_ = new ExecuteThread*[execute_thread_num_];
    assert(execute_threads_);

    for (int i = 0; i < execute_thread_num_; i++) {
      execute_threads_[i] = new ExecuteThread();
      assert(execute_threads_[i]);
    }
	}

	ExecuteThreadGroup::~ExecuteThreadGroup() {
		for (int i = 0; i < execute_thread_num_; i++) {
			delete execute_threads_[i];
			execute_threads_[i] = NULL;
    }
    delete[]execute_threads_;
	}

	ExecuteThread &ExecuteThreadGroup::Next() {
		std::lock_guard<std::mutex> guard(mutex_);
		int index = execute_thread_index_;
		execute_thread_index_ = (execute_thread_index_ + 1) % execute_thread_num_;
		return (*execute_threads_[index]);
	}
}

