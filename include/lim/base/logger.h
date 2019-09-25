#ifndef _LIM_LOGGER_
#define _LIM_LOGGER_
#include <string>
#include <mutex>

namespace lim {
	class Logger {
	public:
		enum { LOG_LEVEL_DEBUG = 1, LOG_LEVEL_INFO = 2, LOG_LEVEL_WARN = 4, LOG_LEVEL_ERROR = 8 };
		~Logger();

	private:
		Logger();
		Logger(const Logger &other) = delete;
		Logger &operator=(const Logger &other) = delete;

	public:
		//��ȡ��־����
		static Logger *GetLogger(const std::string &log_name);
		/**
		*�����־
		* @param file �����ļ�����
		* @param line �����ļ��к�
		* @param level ��־�ȼ�
		* @param format ��־�����ʽ
		*/
		void Trace(const std::string &file, int line, int level, const char *format, ...);

		//������־�ļ������ʽ��������־�ȼ����
		void SetLogFileLevel(int log_file_level);
		//���ÿ���̨�����ʽ��������־�ȼ����
		void SetLogConsoleLevel(int log_console_level);
		//������־�ļ�·��
		bool SetLogFilePath(const std::string &log_path);

  private:
		//��ʼ��
		bool Initialize(const std::string &log_name);
		//������־�ļ�
		void RollFile(const std::string day_time);
		//������ļ���С������־�ļ�
		void LogRollOver();
  private:
		std::mutex mutex_;
		FILE * log_file_fd_; /***��־�ļ����***/
		std::string log_path_; /***��־�ļ�·��***/
		std::string log_name_; /***��־�ļ�����***/
		
		int log_file_level_; /***�ļ������־�ȼ�***/
		int log_console_level_; /***����̨�����־�ȼ�***/
		
		int log_rotate_max_file_size_; /***������־�ļ�����ֽ���***/
		int log_rotate_max_file_num_; /***ÿ����־�ļ���������***/
		std::string last_day_time_; /***��һ������***/
	};

#define		TRACE_DEBUG(logger, ...)		logger->Trace(__FILE__, __LINE__, Logger::LOG_LEVEL_DEBUG, __VA_ARGS__)
#define		TRACE_INFO(logger, ...)		logger->Trace(__FILE__, __LINE__, Logger::LOG_LEVEL_INFO, __VA_ARGS__)
#define		TRACE_WARN(logger, ...)	logger->Trace(__FILE__, __LINE__, Logger::LOG_LEVEL_WARN, __VA_ARGS__)
#define		TRACE_ERROR(logger, ...)	logger->Trace(__FILE__, __LINE__, Logger::LOG_LEVEL_ERROR, __VA_ARGS__)
}

#endif

