#include <lim/base/sslcontext.h>

namespace lim {
  SSLContext::SSLContext(bool is_client): is_verify_host_name_(false) {
    is_client_ = is_client;
		if (is_client)
			context_ = SSL_CTX_new(SSLv23_client_method());
		else
			context_ = SSL_CTX_new(SSLv23_server_method());
	}

  SSLContext::~SSLContext() {
    SSL_CTX_free(context_);
	}

	/**
	*SSL������ʼ��
	* @return ʧ�ܷ���false, �ɹ�����true
	*/
	bool SSLContext::InitEnviroment() {
		SSL_library_init(); 
    SSL_load_error_strings();
		OpenSSL_add_ssl_algorithms();
		return true;
	}

	/**
	*SSL��������
	* @return ʧ�ܷ���false, �ɹ�����true
	*/
	bool SSLContext::FreeEnviroment() {
		return true;
	}
	
	/**
	*����CA֤��
	* @param ca_file ����һ������ PEM ��ʽ��֤����ļ���·��(����)
	* @param ca_path һ������ PEM ��ʽ�ļ���·���������ļ�������ʹ���ض��ĸ�ʽ(��Ϊ��)
	* @return ʧ�ܷ���false, �ɹ�����true
	*/
	bool SSLContext::LoadCALocations(const std::string &ca_file, const std::string &ca_path) {
		const char *char_ca_file = (ca_file.empty() ? NULL : ca_file.c_str());
		const char *char_ca_path = (ca_path.empty() ? NULL : ca_path.c_str());
		if (!SSL_CTX_load_verify_locations(context_, char_ca_file, char_ca_path))
			return false;
		else 
			return true;
	}

	/**
	*��������֤�飬���ڷ��͸��Զ�
	* @param certificate_file PEM ��ʽ����֤���ļ�
	* @return ʧ�ܷ���false, �ɹ�����true
	*/
	bool SSLContext::LoadCertificateFile(const std::string &certificate_file) {
		if (SSL_CTX_use_certificate_file(context_, certificate_file.c_str(), SSL_FILETYPE_PEM) <= 0)
			return false;
		else
			return true;
	}

	/**
	*����˽Կ�ļ�
	* @param private_key_file PEM ��ʽ˽Կ�ļ�
	* @return ʧ�ܷ���false, �ɹ�����true
	*/
	bool SSLContext::LoadPrivateKeyFile(const std::string &private_key_file) {
		if (SSL_CTX_use_PrivateKey_file(context_, private_key_file.c_str(), SSL_FILETYPE_PEM) <= 0)
			return false;
		else
			return true;
	}

	/**
	*��֤�����ص�˽Կ��֤���Ƿ���ƥ��
	* @return ʧ�ܷ���false, �ɹ�����true
	*/
	bool SSLContext::CheckPrivateKey() {
		if (!SSL_CTX_check_private_key(context_))
			return false;
		else
			return true;
	}

	/**
	*�����Ƿ���֤�Զ�
	* @param is_verify_peer, �Ƿ���֤�Զ�
	*/
	void SSLContext::SetVerifyPeer(int is_verify_peer) {
		if (is_verify_peer) {
			if (is_client_) {
				SSL_CTX_set_verify(context_, SSL_VERIFY_PEER, NULL);
			} else {
				SSL_CTX_set_verify(context_, SSL_VERIFY_PEER|SSL_VERIFY_FAIL_IF_NO_PEER_CERT, NULL);
			}
		} else {
			SSL_CTX_set_verify(context_, SSL_VERIFY_NONE, NULL);
		}
	}

  /**
  *�Ƿ�У��HostName��ֻ������Clientģʽ��
  * @param is_verify_host_name �Ƿ�У��HostName
  */
  void SSLContext::SetVerifyHostName(bool is_verify_host_name) {
    if (is_client_) {
      is_verify_host_name_ = is_verify_host_name;
    }
  }

  bool SSLContext::IsVerifyHostName() {
    if (is_client_) {
      return is_verify_host_name_;
    } else {
      return false;
    }
  }
}

