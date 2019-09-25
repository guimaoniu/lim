#include <lim/base/time_utils.h>
#include <chrono>
#include <string>
#include <time.h>
#include <sys/timeb.h>

namespace lim {
  int64_t CurrentMilliTime() {
		auto tp = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
    auto tmp = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch());
    return tmp.count();
  }

	std::string GetCurrentTimeString(const char* format) {
		int64_t millisec = CurrentMilliTime();
		return TimeToString(millisec, format);
	}
	
	std::string TimeToString(uint64_t millisec, const char *format) {
		struct tm tm_time;
		struct timeb tb_time;
		char time_string[128] = {0};

		ftime(&tb_time);
		tb_time.time = millisec /1000;
		tb_time.millitm = millisec %1000;
	
		const int kArrayDaysPerMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

		//��ǰʱ�����ʱ��
		if (tb_time.dstflag == 0) {
			tb_time.time -= tb_time.timezone*60;
		}
	
		//ȡ��ʱ��
		tm_time.tm_sec = tb_time.time%60;
	
		//ȡ����ʱ��
		tm_time.tm_min = (tb_time.time/60)%60;

		//ȡ��ȥ���ٸ�Сʱ
		int total_hours = (int)(tb_time.time/3600);

		//ȡ��ȥ���ٸ�����
		int four_year_count = total_hours /((365*4+1) * 24L);
	
		//�������
		tm_time.tm_year = (four_year_count << 2)+70;
		
		//������ʣ�µ�Сʱ��
		int left_hours = total_hours %((365*4+1) * 24L);

		//ȡһ���ڵĵڼ���(1970-1-1��4)
		tm_time.tm_wday = (total_hours/24 + 4) % 7;
	
  
		//У������Ӱ�����ݣ�����һ����ʣ�µ�Сʱ��
		for (;;) {
			//һ���Сʱ��
			int total_hours_per_year = 365 * 24;

			//�ж�����
			if ((tm_time.tm_year & 3) == 0) {
				//�����꣬һ�����24Сʱ����һ��
				total_hours_per_year += 24;
			}

			if (left_hours < total_hours_per_year) {
				break;
			}
		
			tm_time.tm_year++;
			left_hours -= total_hours_per_year;
		}

		//ȡСʱʱ��
		tm_time.tm_hour = left_hours%24;
	
		//һ����ʣ�µ�����
		int left_days_of_one_year = left_hours/24;

		//�ٶ�Ϊ����
		left_days_of_one_year++;

		//ȡһ��ĵڼ���(�ٶ�Ϊ������)
		tm_time.tm_yday = left_days_of_one_year % 365;
	
		//У��������������·ݣ�����
		if ((tm_time.tm_year & 3) == 0)	{
			tm_time.tm_yday --;
		
			if (left_days_of_one_year > 60) {
				left_days_of_one_year--;
			} else {
				if (left_days_of_one_year == 60) {
					//��������
					tm_time.tm_mon = 1;
					tm_time.tm_mday = 29;

					if (format == NULL) {
						sprintf(time_string, "%04d-%02d-%02d %02d:%02d:%02d.%03d", 
							tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday, tm_time.tm_hour, 
							tm_time.tm_min, tm_time.tm_sec, tb_time.millitm);
 					} else {
						sprintf(time_string, format, 
							tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday, tm_time.tm_hour, 
							tm_time.tm_min, tm_time.tm_sec, tb_time.millitm);
 					}
					return time_string;
				}
			}
		}

		//��������
		for (tm_time.tm_mon = 0; kArrayDaysPerMonth[tm_time.tm_mon] < left_days_of_one_year; tm_time.tm_mon++) {
			left_days_of_one_year -= kArrayDaysPerMonth[tm_time.tm_mon];
		}
		tm_time.tm_mday = left_days_of_one_year;
	
		if (format == NULL) {
			sprintf(time_string, "%04d-%02d-%02d %02d:%02d:%02d.%03d", 
				tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday, tm_time.tm_hour, 
				tm_time.tm_min, tm_time.tm_sec, tb_time.millitm);
 		} else {
			sprintf(time_string, format, 
				tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday, tm_time.tm_hour, 
				tm_time.tm_min, tm_time.tm_sec, tb_time.millitm);
 		}
		return time_string;
	}
}
