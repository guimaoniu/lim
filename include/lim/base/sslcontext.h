#ifndef LIM_SSL_CONTEXT_H
#define LIM_SSL_CONTEXT_H
#include <string>
#include <vector>
#include <stdint.h>
#include <openssl/ssl.h>

namespace lim {
	class SSLContext {
	public:
    SSLContext(bool is_client = false);
		virtual ~SSLContext();

		/**
		*SSL������ʼ��
		* @return ʧ�ܷ���false, �ɹ�����true
		*/
		static bool InitEnviroment();
		/**
		*SSL��������
		* @return ʧ�ܷ���false, �ɹ�����true
		*/
		static bool FreeEnviroment();
	
		/**
		*����CA֤��
		* @param ca_file ����һ������ PEM ��ʽ��֤����ļ���·��(����)
		* @param ca_path һ������ PEM ��ʽ�ļ���·���������ļ�������ʹ���ض��ĸ�ʽ(��Ϊ��)
		* @return ʧ�ܷ���false, �ɹ�����true
		*/
		bool LoadCALocations(const std::string &ca_file, const std::string &ca_path);
		/**
		*��������֤�飬���ڷ��͸��Զ�
		* @param certificate_file PEM ��ʽ����֤���ļ�
		* @return ʧ�ܷ���false, �ɹ�����true
		*/
		bool LoadCertificateFile(const std::string &certificate_file);
		/**
		*����˽Կ�ļ�
		* @param private_key_file PEM ��ʽ˽Կ�ļ�
		* @return ʧ�ܷ���false, �ɹ�����true
		*/
		bool LoadPrivateKeyFile(const std::string &private_key_file);
		/**
		*��֤�����ص�˽Կ��֤���Ƿ���ƥ��
		* @return ʧ�ܷ���false, �ɹ�����true
		*/
		bool CheckPrivateKey();

		/**
		*�����Ƿ���֤�Զ�
		* @param is_verify_peer, �Ƿ���֤�Զ�
		*/
		void SetVerifyPeer(int is_verify_peer);

    /**
    *�Ƿ�У��HostName��ֻ������Clientģʽ��
    * @param is_verify_host_name �Ƿ�У��HostName
    */
    void SetVerifyHostName(bool is_verify_host_name);
    bool IsVerifyHostName();

    bool IsClientContext() { return is_client_; };

	protected:
    bool is_client_;
		SSL_CTX	*context_;
    bool is_verify_host_name_;
    friend class SocketChannel;
	};
}
#endif

