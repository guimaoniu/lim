#ifndef LIM_BYTE_BUFFER_H
#define LIM_BYTE_BUFFER_H
#include <string>
#include <atomic>

namespace lim {
  //�����ü������ֽڻ�����(���̰߳�ȫ)
  class ByteBuffer {
  public:
    ByteBuffer(int max_buffer_size = -1);
    ByteBuffer(int init_buffer_size, int max_buffer_size);
    ByteBuffer(const ByteBuffer& other);
    ByteBuffer &operator=(const ByteBuffer& other);
    virtual ~ByteBuffer();

  public:
    //��ջ�����
    void Clear();
    //��ȡ��������С
    int Capacity();
    //�ַ����л�����������
    std::string ToString();

    //��ȡ�������ɶ��ֽ���
    int ReadableBytes();
    //��ȡ��������д�ֽ���
    int WritableBytes();

    //skipָ���ɶ��ֽ���
    int SkipBytes(int size);
    /**
    *��ȡָ����С�ֽ���
    * @param bytes ��������
    * @param size ����������С
    * @return ����ʵ�ʶ������ֽ���
    */
    int ReadBytes(char *bytes, int size);
    /**
    *��ȡָ����С�ֽ���
    * @param other ��������
    * @param size ����������С,-1��ʾ����������
    * @return ����ʵ�ʶ������ֽ���
    */
    int ReadBytes(ByteBuffer &other, int size = -1);
		
    uint8_t ReadUInt8();
    void WriteUInt8(uint8_t value);
		
    uint16_t ReadUInt16();
    void WriteUInt16(uint16_t value);
		
    uint32_t ReadUInt32();
    void WriteUInt32(uint32_t value);
		
    uint64_t ReadUInt64();
    void WriteUInt64(uint64_t value);
		
    /**
    *дָ����С�ֽ���
    * @param bytes д������
    * @param size д��������С
    * @return ����ʵ��д����ֽ���
    */
    int WriteBytes(const char *bytes, int size);
    /**
    *дָ����С�ֽ���
    * @param other д������
    * @param size д��������С,-1��ʾд��������
    * @return ����ʵ��д����ֽ���
    */
    int WriteBytes(ByteBuffer &other, int size = -1);
    /**
    *��ָ��λ��д��ָ����С�ֽ���
    * @param bytes д������
    * @param bytes д������
    * @param size д��������С
    * @return �ɹ�true,ʧ��false
    */
    bool InsertWriteBytes(int position, const char *bytes, int size);

    /**
    *��ȡһ������
    * @param line ���ص�������
    * @param delim �ָ��ַ���
    * @return �ɹ�true,ʧ��false
    */
    bool GetLine(std::string &line, const std::string &delim);
		
  private:
    /**
    *�������ֽڴ�������
    * @param size ��Ҫ��ȡ���ֽ�����size <= sizeof(uint64_t)
    * @return ���������ֽڴ�������
    */
    uint64_t ReadValue(int size);
    /**
    *д�����ֽڴ�������
    * @param value ��Ҫд����ֵ
    * @param size valueֵ�Ĵ�С��size <= sizeof(uint64_t)
    */
    void WriteValue(uint64_t value, int size);

    //���뻺����
    bool AllocateMemory(int memory_size);
		
  private:
    char *buffer_; /***������***/
    int *buffer_read_index_; /***��ƫ��ָ��***/
    int *buffer_write_index_; /***дƫ��ָ��***/
    int *buffer_size_; /***��ǰ��������Сָ��***/
    int *max_buffer_size_; /***��󻺴�����Сָ��***/
    std::atomic<int> *reference_count_; /***���ü���ָ��***/
  };
}
#endif

