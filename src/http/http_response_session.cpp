#include <lim/http/http_response_session.h>
#include <lim/http/http_bootstrap_config.h>
#include <lim/base/string_utils.h>
#include <assert.h>
#include <string.h>
#include <sstream>

namespace lim {	
	/**
	*Http��Ӧ������
	* @param max_first_line_size ����������󳤶�
	* @param max_header_size header��󳤶�
	* @param max_content_size content��󳤶�
	*/
	HttpResponseDecoder::HttpResponseDecoder(int max_first_line_size, int max_header_size, int max_content_size): 
		http_response_(NULL), HttpBaseDecoder(max_first_line_size, max_header_size, max_content_size) {

	}

	HttpResponseDecoder::~HttpResponseDecoder() {
		delete http_response_;
	}

	void HttpResponseDecoder::Reset() {
		HttpBaseDecoder::Reset();
		delete http_response_;
		http_response_ = NULL;
	}
	
	bool HttpResponseDecoder::IsChunked() {
		return http_response_->Headers().IsChunked();
	}
	
	int64_t HttpResponseDecoder::GetContentLength() {
		int64_t length = http_response_->Headers().GetContentLength(-1);
		if (length != -1) {
			return length;
		}

		std::string connection;
		if (http_response_->Headers().GetHeaderValue("Connection", connection) && !strcasecmp(connection.c_str(), "Close")) {
			return -1;
		} else {
			return 0;
		}
	}

	bool HttpResponseDecoder::CreatHttpMessage(const std::string &http_first_line) {
		const int HTTP_RESPONSE_LINE_FIELD_COUNT = 3;
		std::vector<std::string> fields = split(http_first_line, " ");
		if (fields.size() != HTTP_RESPONSE_LINE_FIELD_COUNT) {
			return false;
		}

		if (strcasecmp(fields[0].c_str(), "HTTP/1.0") != 0 && strcasecmp(fields[0].c_str(), "HTTP/1.1") != 0) {
			return false;
		}

		http_response_ = new HttpResponse(atoi(fields[1].c_str()), trim(fields[2]), trim(fields[0]));
		return true;
	}

	void HttpResponseDecoder::SetHeaderValue(const std::string &header_name, const std::string &header_value) {
		http_response_->Headers().SetHeaderValue(header_name, header_value);
	}

	//http message������
	bool HttpResponseDecoder::DoHttpMessage(HandleMessageCallback &message_callback) {
		return message_callback(*http_response_);
	}

	//http content������
	bool HttpResponseDecoder::DoHttpContent(HttpContent &content, HandleMessageCallback &message_callback) {
		return message_callback(content);
	}
	
	HttpResponseSession::HttpResponseSession(SocketChannel &channel, BootstrapConfig &config):
		ConnectedChannelSession(channel, config) {
		
	}

	/**
	*����http������(�Զ�����User-Agent��Hostͷ��Ϣ)
	* @param response http��Ӧ����
	* @param callback ���ͽ�����Ļص�����
	* @return ʧ�ܷ���false, �ɹ�����true
	*/
	bool HttpResponseSession::WriteHttpRequest(HttpRequest &request, WriteCompleteCallback callback) {
		HttpBootstrapConfig &config = (HttpBootstrapConfig&)config_;
		request.Headers().SetHeaderValue("User-Agent", config.GetUserAgent());
		if (channel_.GetRemoteHostName() != "") {
      std::stringstream ss;
      ss << channel_.GetRemoteHostName() << ":" << channel_.GetRemoteHostPort();
			request.Headers().SetHeaderValue("Host", ss.str());
		}
		return WriteMessage(request, callback);
	}

	/**
	*����http body������
	* @param content http body������
	* @param callback ���ͽ�����Ļص�����
	* @return ʧ�ܷ���false, �ɹ�����true
	*/
	bool HttpResponseSession::WriteHttpConent(HttpContent &content, WriteCompleteCallback callback) {
		return WriteMessage(content, callback);
	}
		
	//�������Ľ�����
	MessageDecoder *HttpResponseSession::CreateDecoder() {
		HttpBootstrapConfig &config = (HttpBootstrapConfig &)config_;
		int max_first_line_size = config.GetMaxFirstLineSize();
		int max_header_size = config.GetMaxHeaderSize();
		int max_content_size = config.GetMaxContentSize();
    return (new HttpResponseDecoder(max_first_line_size, max_header_size, max_content_size));
	}

	HttpFullResponseDecoder::HttpFullResponseDecoder(int max_first_line_size, int max_header_size, int max_content_size): 
		HttpResponseDecoder(max_first_line_size, max_header_size, max_content_size) {

	}

	bool HttpFullResponseDecoder::CreatHttpMessage(const std::string &http_first_line) {
		const int HTTP_RESPONSE_LINE_FIELD_COUNT = 3;
		std::vector<std::string> fields = split(http_first_line, " ");
		if (fields.size() < HTTP_RESPONSE_LINE_FIELD_COUNT) {
			return false;
		}

		if (strcasecmp(fields[0].c_str(), "HTTP/1.0") != 0 && strcasecmp(fields[0].c_str(), "HTTP/1.1") != 0) {
			return false;
		}
		
    std::string reason_phrase;
    for (int i = 2; i < (int)fields.size(); i++) {
      if (i == 2)
        reason_phrase += fields[i];
      else
        reason_phrase += " " + fields[i];
    }

		http_response_ = new HttpFullResponse(atoi(fields[1].c_str()), trim(reason_phrase), trim(fields[0]));
		return true;
	}

	//http message������
	bool HttpFullResponseDecoder::DoHttpMessage(HandleMessageCallback &message_callback) {
		return true;
	}

	//http content������
	bool HttpFullResponseDecoder::DoHttpContent(HttpContent &content, HandleMessageCallback &message_callback) {
		HttpFullResponse *response = (HttpFullResponse *)http_response_;
		content.ToBytes(response->Content().Content());
		if (content.IsLast()) 
			return message_callback(*response);
		else
			return true;
	}
	
	HttpFullResponseSession::HttpFullResponseSession(SocketChannel &channel, BootstrapConfig &config):
		HttpResponseSession(channel, config) {
		
	}

	//�������Ľ�����
	MessageDecoder *HttpFullResponseSession::CreateDecoder() {
		HttpBootstrapConfig &config = (HttpBootstrapConfig &)config_;
		int max_first_line_size = config.GetMaxFirstLineSize();
		int max_header_size = config.GetMaxHeaderSize();
		int max_content_size = config.GetMaxContentSize();
		return (new HttpFullResponseDecoder(max_first_line_size, max_header_size, max_content_size));
	}
}
