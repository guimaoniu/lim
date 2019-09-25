#include <lim/base/logger.h>
#include <lim/base/time_utils.h>
#ifdef _WIN32
#include <Windows.h>
#include <io.h>
#include <direct.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <sys/stat.h>
#include <dirent.h>
#endif
#include <stdarg.h>
#include <sstream>
#include <algorithm>
#include <vector>

#define PRINTF(...)	printf(__VA_ARGS__)
namespace lim {
	static const std::string kLogLevels[] = {"DEBUG", "INFO", "WARN", "ERROR"};
	//��ȡ��־����
	Logger *Logger::GetLogger(const std::string &log_name) {
		Logger *logger = new Logger();
		if (!logger->Initialize(log_name))
			return NULL;
		else
			return logger;
	}

	Logger::Logger() {
		log_file_level_ = LOG_LEVEL_DEBUG | LOG_LEVEL_INFO | LOG_LEVEL_WARN | LOG_LEVEL_ERROR;
		log_console_level_ = LOG_LEVEL_DEBUG | LOG_LEVEL_INFO | LOG_LEVEL_WARN | LOG_LEVEL_ERROR;

		log_path_ = "./";
		log_file_fd_ = NULL;

		log_rotate_max_file_size_ = 10 * 1024 * 1024; //Ĭ�ϵ����ļ����10MB
		log_rotate_max_file_num_ = 10; //Ĭ��ÿ����ౣ��10����־�ļ�
		
		last_day_time_ = GetCurrentTimeString("%04d%02d%02d");
	}

	Logger::~Logger() {
		if (log_file_fd_ != NULL) {
			fclose(log_file_fd_);
		}
	}

	//��ʼ��
	bool Logger::Initialize(const std::string &log_name) {	
		log_name_ = log_name;
		std::string file_name = log_path_ + "/" + log_name_ + ".log";
		log_file_fd_ = fopen(file_name.c_str(), "a+");
		if (log_file_fd_ == NULL) {
			return false;
		}

		return true;
	}

	//������־�ļ������ʽ��������־�ȼ����
	void Logger::SetLogConsoleLevel(int log_console_level) {
		std::lock_guard<std::mutex> guard(mutex_);
		log_console_level_ = log_console_level;
	}

	//���ÿ���̨�����ʽ��������־�ȼ����
	void Logger::SetLogFileLevel(int log_file_level) {
		std::lock_guard<std::mutex> guard(mutex_);
		log_file_level_ = log_file_level;
	}

	//������־�ļ�·��
	bool Logger::SetLogFilePath(const std::string &log_path) {
		std::lock_guard<std::mutex> guard(mutex_);
		if (log_file_fd_ != NULL) {
			fclose(log_file_fd_);
		}

		log_path_ = log_path;
		std::string file_name = log_path_ + "/" + log_name_ + ".log";
		log_file_fd_ = fopen(file_name.c_str(), "a+");
		if (log_file_fd_ == NULL) {
			return false;
		}
		return true;
  }

	//������־�ļ�
	void Logger::RollFile(const std::string day_time) {
		fclose(log_file_fd_);
		//remove expired old files
		for (int i = log_rotate_max_file_num_ - 1; ; i++) {
			std::stringstream ss;
			ss << log_path_ << "/" << log_name_ << "_" << day_time << ".log" << "." << i;
			if (remove(ss.str().c_str()) != 0) {
				break;
			}
		}

		//rename each existing file to the consequent one
		for (int i = log_rotate_max_file_num_ - 1; i >= 1; i--) {
			std::stringstream last;
			last << log_path_ << "/" << log_name_ << "_" << day_time << ".log" << "." << i;

			std::stringstream previous;
			previous << log_path_ << "/" << log_name_ << "_" << day_time << ".log" << "." << i - 1;

			rename(previous.str().c_str(), last.str().c_str());
		}

		std::string file_name = log_path_ + "/" + log_name_ + ".log";
		std::string last_name = log_path_ + "/" + log_name_ + "_" + day_time + ".log.0";
		rename(file_name.c_str(), last_name.c_str());

		log_file_fd_ = fopen(file_name.c_str(), "a+");
	}

	//������ļ���С������־�ļ�
	void Logger::LogRollOver() {
		std::lock_guard<std::mutex> guard(mutex_);
		std::string now_day_time = GetCurrentTimeString("%04d%02d%02d");
		if (ftell(log_file_fd_) >= log_rotate_max_file_size_ && log_rotate_max_file_num_ > 0) {
			RollFile(now_day_time);
		} else if (last_day_time_ != now_day_time) {
			RollFile(last_day_time_);
			last_day_time_ = now_day_time;
		}
	}

	/**
	*�����־
	* @param file �����ļ�����
	* @param file �����ļ��к�
	* @param file ��־�ȼ�
	* @param file ��־�����ʽ
	*/
	void Logger::Trace(const std::string &file, int line, int level, const char *format, ...) {
		int log_console_level, log_file_level;
		{
			std::lock_guard<std::mutex> guard(mutex_);
			log_console_level = log_console_level_;
			log_file_level = log_file_level_;
		}
   
		std::stringstream ss;
		ss << "[" << GetCurrentTimeString() << "] "
			 << "[" << kLogLevels[(int)log2(level)] << "] "
       << "[" << file << ":" << line << "] "
       << format << "\n";

    va_list maker;
    va_start(maker, format);
		if (log_console_level&level) {
#ifdef _WIN32
			HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);

			//�������̨������ɫ
			CONSOLE_SCREEN_BUFFER_INFO csbi_info;
			GetConsoleScreenBufferInfo(handle, &csbi_info);
			WORD old_color_attrs = csbi_info.wAttributes;
			
			//����־�ȼ�������ɫ
			if (level == LOG_LEVEL_INFO) {
				SetConsoleTextAttribute(handle, FOREGROUND_BLUE);
			} else if (level == LOG_LEVEL_WARN) {
				SetConsoleTextAttribute(handle, FOREGROUND_RED|FOREGROUND_GREEN);
			} else if (level == LOG_LEVEL_ERROR) {
				SetConsoleTextAttribute(handle, FOREGROUND_RED);
			}
			vprintf(ss.str().c_str(), maker);		

			//�ָ�����̨������ɫ
			SetConsoleTextAttribute(handle, old_color_attrs);
#else
			//����־�ȼ�������ɫ
			std::string color = "\033[0;m";
			if (level == LOG_LEVEL_INFO) {
				color = "\033[0;34m"; //blue
			} else if (level == LOG_LEVEL_WARN) {
				color = "\033[0;33m"; //yellow
			} else if (level == LOG_LEVEL_ERROR) {
				color = "\033[0;31m"; //red
			}
      
			std::string log = color + ss.str().c_str() + "\033[0;m";
			vprintf(log.c_str(), maker);
#endif
		}

		if ((log_file_level&level) && log_file_fd_ != NULL) {
			{
				std::lock_guard<std::mutex> guard(mutex_);
				vfprintf(log_file_fd_, ss.str().c_str(), maker);
				fflush(log_file_fd_);
			}
			LogRollOver();
		}
		va_end(maker);
	}
}
