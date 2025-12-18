#include "http_utils.h"
#include "curl/curl.h"
#include <sstream>
#include <iostream>
#include <fstream>
#include <cstring>

// 全局初始化标记
static bool curl_initialized = false;

// 实现类
class HttpClient::Impl {
public:
    CURL* curl;
    curl_slist* headers;

    WriteCallback write_callback;
    ReadCallback read_callback;
    ProgressCallback progress_callback;
    HeaderCallback header_callback;

    struct ResponseData {
        std::string body;
        std::string header_data;
        std::map<std::string, std::string> headers;

        void clear() {
            body.clear();
            header_data.clear();
            headers.clear();
        }
    } response_data;

    Impl() : curl(NULL), headers(NULL) {
        curl = curl_easy_init();
    }

    ~Impl() {
        if (headers) {
            curl_slist_free_all(headers);
        }
        if (curl) {
            curl_easy_cleanup(curl);
        }
    }

    static size_t write_callback_wrapper(char* ptr, size_t size, size_t nmemb, void* userdata) {
        Impl* impl = static_cast<Impl*>(userdata);
        size_t total_size = size * nmemb;

        if (impl->write_callback) {
            return impl->write_callback(ptr, size, nmemb);
        } else {
            impl->response_data.body.append(ptr, total_size);
            return total_size;
        }
    }

    static size_t read_callback_wrapper(char* buffer, size_t size, size_t nitems, void* userdata) {
        Impl* impl = static_cast<Impl*>(userdata);

        if (impl->read_callback) {
            return impl->read_callback(buffer, size, nitems);
        }
        return 0;
    }

    static int progress_callback_wrapper(void* clientp,
                                         double dltotal, double dlnow,
                                         double ultotal, double ulnow) {
        Impl* impl = static_cast<Impl*>(clientp);

        if (impl->progress_callback) {
            return impl->progress_callback(dltotal, dlnow, ultotal, ulnow) ? 0 : 1;
        }
        return 0;
    }

    static size_t header_callback_wrapper(char* buffer, size_t size, size_t nitems, void* userdata) {
        Impl* impl = static_cast<Impl*>(userdata);
        size_t total_size = size * nitems;

        if (impl->header_callback) {
            return impl->header_callback(buffer, size, nitems);
        } else {
            std::string header(buffer, total_size);
            impl->response_data.header_data.append(header);

            size_t colon_pos = header.find(':');
            if (colon_pos != std::string::npos) {
                std::string key = header.substr(0, colon_pos);
                std::string value = header.substr(colon_pos + 1);

                // 去除空白字符
                key.erase(0, key.find_first_not_of(" \t\r\n"));
                key.erase(key.find_last_not_of(" \t\r\n") + 1);
                value.erase(0, value.find_first_not_of(" \t\r\n"));
                value.erase(value.find_last_not_of(" \t\r\n") + 1);

                if (!key.empty()) {
                    impl->response_data.headers[key] = value;
                }
            }

            return total_size;
        }
    }

    Response perform_request(const std::string& url,
                             const Config& config,
                             const std::string& body = "",
                             bool is_post = false) {
        Response response;
        response_data.clear();

        if (!curl) {
            response.error = "cURL handle not initialized";
            return response;
        }

        // 重置cURL选项
        curl_easy_reset(curl);

        // 设置URL
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        // 设置HTTP方法
        if (is_post && !body.empty()) {
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
            curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, body.size());
        } else if (is_post) {
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
        }

        // 设置超时
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, config.timeout);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, config.timeout);

        // 重定向
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, config.follow_redirects ? 1L : 0L);
        curl_easy_setopt(curl, CURLOPT_MAXREDIRS, config.max_redirects);

        // SSL验证
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, config.verify_ssl ? 1L : 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, config.verify_ssl ? 2L : 0L);

        // CA证书
        if (!config.ca_bundle.empty()) {
            curl_easy_setopt(curl, CURLOPT_CAINFO, config.ca_bundle.c_str());
        }

        // 客户端证书
        if (!config.ssl_cert.empty()) {
            curl_easy_setopt(curl, CURLOPT_SSLCERT, config.ssl_cert.c_str());
        }
        if (!config.ssl_key.empty()) {
            curl_easy_setopt(curl, CURLOPT_SSLKEY, config.ssl_key.c_str());
        }
        if (!config.ssl_key_passwd.empty()) {
            curl_easy_setopt(curl, CURLOPT_KEYPASSWD, config.ssl_key_passwd.c_str());
        }

        // 代理
        if (!config.proxy.empty()) {
            curl_easy_setopt(curl, CURLOPT_PROXY, config.proxy.c_str());
        }
        if (!config.proxy_userpwd.empty()) {
            curl_easy_setopt(curl, CURLOPT_PROXYUSERPWD, config.proxy_userpwd.c_str());
        }

        // User-Agent
        if (!config.user_agent.empty()) {
            curl_easy_setopt(curl, CURLOPT_USERAGENT, config.user_agent.c_str());
        }

        // 详细输出
        curl_easy_setopt(curl, CURLOPT_VERBOSE, config.verbose ? 1L : 0L);

        // 设置回调函数
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback_wrapper);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);

        curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback_wrapper);
        curl_easy_setopt(curl, CURLOPT_READDATA, this);

        curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, progress_callback_wrapper);
        curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, this);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);

        curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_callback_wrapper);
        curl_easy_setopt(curl, CURLOPT_HEADERDATA, this);

        // 设置自定义头部
        if (headers) {
            curl_slist_free_all(headers);
            headers = NULL;
        }

        for (size_t i = 0; i < config.headers.size(); ++i) {
            headers = curl_slist_append(headers, config.headers[i].c_str());
        }

        if (headers) {
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        }

        // 执行请求
        CURLcode res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            response.error = curl_easy_strerror(res);
        } else {
            // 获取响应信息
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.status_code);
            curl_easy_getinfo(curl, CURLINFO_HTTP_VERSION, &response.http_version);
            curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &response.total_time);
            curl_easy_getinfo(curl, CURLINFO_CONNECT_TIME, &response.connect_time);

            response.body = response_data.body;
            response.headers = response_data.headers;
        }

        return response;
    }
};

// HttpClient实现
HttpClient::HttpClient() {
    impl_ = new Impl();
}

HttpClient::~HttpClient() {
    delete impl_;
}

std::string HttpClient::Response::get_header(const std::string& key) const {
    std::map<std::string, std::string>::const_iterator it = headers.find(key);
    if (it != headers.end()) {
        return it->second;
    }

    // 尝试不区分大小写查找
    for (std::map<std::string, std::string>::const_iterator it2 = headers.begin();
         it2 != headers.end(); ++it2) {
        if (strcasecmp(it2->first.c_str(), key.c_str()) == 0) {
            return it2->second;
        }
    }

    return "";
}

bool HttpClient::init() {
    if (!curl_initialized) {
        CURLcode res = curl_global_init(CURL_GLOBAL_ALL);
        if (res == CURLE_OK) {
            curl_initialized = true;
        }
    }
    return curl_initialized;
}

void HttpClient::cleanup() {
    if (curl_initialized) {
        curl_global_cleanup();
        curl_initialized = false;
    }
}

HttpClient::Response HttpClient::request(Method method,
                                         const std::string& url,
                                         const std::string& body,
                                         const Config& config) {
    if (method == HTTP_GET || method == HTTP_HEAD || method == HTTP_DELETE) {
        return impl_->perform_request(url, config, "", false);
    } else {
        return impl_->perform_request(url, config, body, true);
    }
}

HttpClient::Response HttpClient::get(const std::string& url,
                                     const std::map<std::string, std::string>& params,
                                     const Config& config) {
    std::string full_url = url;
    if (!params.empty()) {
        std::string query = build_query_string(params);
        if (url.find('?') == std::string::npos) {
            full_url += "?" + query;
        } else {
            full_url += "&" + query;
        }
    }
    return impl_->perform_request(full_url, config, "", false);
}

HttpClient::Response HttpClient::post(const std::string& url,
                                      const std::string& data,
                                      const Config& config) {
    Config post_config = config;
    post_config.headers.push_back("Content-Type: application/octet-stream");
    return impl_->perform_request(url, post_config, data, true);
}

HttpClient::Response HttpClient::post_form(const std::string& url,
                                           const std::map<std::string, std::string>& form_data,
                                           const Config& config) {
    std::string form_body = build_query_string(form_data);

    Config form_config = config;
    form_config.headers.push_back("Content-Type: application/x-www-form-urlencoded");

    return impl_->perform_request(url, form_config, form_body, true);
}

HttpClient::Response HttpClient::put(const std::string& url,
                                     const std::string& data,
                                     const Config& config) {
    // 对于PUT请求，需要设置自定义请求方法
    curl_easy_setopt(impl_->curl, CURLOPT_CUSTOMREQUEST, "PUT");
    curl_easy_setopt(impl_->curl, CURLOPT_POSTFIELDS, data.c_str());
    curl_easy_setopt(impl_->curl, CURLOPT_POSTFIELDSIZE, data.size());

    return impl_->perform_request(url, config, data, false);
}

HttpClient::Response HttpClient::delete_(const std::string& url,
                                         const Config& config) {
    curl_easy_setopt(impl_->curl, CURLOPT_CUSTOMREQUEST, "DELETE");
    return impl_->perform_request(url, config, "", false);
}

bool HttpClient::download(const std::string& url,
                          const std::string& local_path,
                          const Config& config) {
    std::ofstream file(local_path.c_str(), std::ios::binary);
    if (!file.is_open()) {
        return false;
    }

    // 设置写回调为写入文件
    WriteCallback old_callback = impl_->write_callback;
    impl_->write_callback = [&file](const char* data, size_t size, size_t nmemb) -> size_t {
        file.write(data, size * nmemb);
        return size * nmemb;
    };

    Response response = impl_->perform_request(url, config, "", false);

    // 恢复原来的回调
    impl_->write_callback = old_callback;

    return response.ok();
}

HttpClient::Response HttpClient::upload(const std::string& url,
                                        const std::string& file_path,
                                        const std::string& field_name,
                                        const Config& config) {
    std::ifstream file(file_path.c_str(), std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        Response response;
        response.error = "Cannot open file: " + file_path;
        return response;
    }

    std::streamsize file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(file_size);
    if (!file.read(&buffer[0], file_size)) {
        Response response;
        response.error = "Cannot read file: " + file_path;
        return response;
    }

    // 构建multipart/form-data
    std::string boundary = "----WebKitFormBoundary7MA4YWxkTrZu0gW";
    std::string body;

    body += "--" + boundary + "\r\n";
    body += "Content-Disposition: form-data; name=\"" + field_name + "\"; filename=\"" + file_path + "\"\r\n";
    body += "Content-Type: application/octet-stream\r\n\r\n";
    body.append(&buffer[0], buffer.size());
    body += "\r\n";
    body += "--" + boundary + "--\r\n";

    Config upload_config = config;
    upload_config.headers.push_back("Content-Type: multipart/form-data; boundary=" + boundary);

    return impl_->perform_request(url, upload_config, body, true);
}

void HttpClient::set_write_callback(WriteCallback callback) {
    impl_->write_callback = callback;
}

void HttpClient::set_read_callback(ReadCallback callback) {
    impl_->read_callback = callback;
}

void HttpClient::set_progress_callback(ProgressCallback callback) {
    impl_->progress_callback = callback;
}

void HttpClient::set_header_callback(HeaderCallback callback) {
    impl_->header_callback = callback;
}

std::string HttpClient::url_encode(const std::string& str) {
    CURL* curl = curl_easy_init();
    if (!curl) return str;

    char* encoded = curl_easy_escape(curl, str.c_str(), str.length());
    std::string result = encoded ? encoded : str;

    curl_free(encoded);
    curl_easy_cleanup(curl);

    return result;
}

std::string HttpClient::url_decode(const std::string& str) {
    CURL* curl = curl_easy_init();
    if (!curl) return str;

    int outlen = 0;
    char* decoded = curl_easy_unescape(curl, str.c_str(), str.length(), &outlen);
    std::string result = decoded ? std::string(decoded, outlen) : str;

    curl_free(decoded);
    curl_easy_cleanup(curl);

    return result;
}

std::string HttpClient::build_query_string(const std::map<std::string, std::string>& params) {
    std::string query;
    for (std::map<std::string, std::string>::const_iterator it = params.begin();
         it != params.end(); ++it) {
        if (!query.empty()) {
            query += "&";
        }
        query += url_encode(it->first) + "=" + url_encode(it->second);
    }
    return query;
}

std::string HttpClient::method_to_string(Method method) {
    switch (method) {
    case HTTP_GET: return "GET";
    case HTTP_POST: return "POST";
    case HTTP_PUT: return "PUT";
    case HTTP_DELETE: return "DELETE";
    case HTTP_HEAD: return "HEAD";
    case HTTP_PATCH: return "PATCH";
    default: return "UNKNOWN";
    }
}
