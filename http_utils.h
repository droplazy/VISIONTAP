#ifndef HTTP_UTILS_H
#define HTTP_UTILS_H

#include <string>
#include <map>
#include <vector>
#include <functional>

class HttpClient {
public:
    // HTTP方法
    enum Method {
        HTTP_GET,
        HTTP_POST,
        HTTP_PUT,
        HTTP_DELETE,
        HTTP_HEAD,
        HTTP_PATCH
    };

    // HTTP响应
    struct Response {
        int status_code;
        std::string body;
        std::string error;
        std::map<std::string, std::string> headers;
        long http_version;
        double total_time;
        double connect_time;

        Response() : status_code(0), http_version(0), total_time(0.0), connect_time(0.0) {}

        bool ok() const { return status_code >= 200 && status_code < 300; }
        std::string get_header(const std::string& key) const;
    };

    // 请求配置
    struct Config {
        int timeout;
        bool follow_redirects;
        int max_redirects;
        bool verify_ssl;
        bool verbose;
        std::string user_agent;
        std::string proxy;
        std::string proxy_userpwd;
        std::string ca_bundle;
        std::vector<std::string> headers;
        std::string ssl_cert;
        std::string ssl_key;
        std::string ssl_key_passwd;

        Config() :
            timeout(30),
            follow_redirects(true),
            max_redirects(10),
            verify_ssl(true),
            verbose(false),
            user_agent("HttpClient/1.0") {}
    };

    // 回调函数类型
    typedef std::function<size_t(const char*, size_t, size_t)> WriteCallback;
    typedef std::function<size_t(char*, size_t, size_t)> ReadCallback;
    typedef std::function<bool(double, double, double, double)> ProgressCallback;
    typedef std::function<size_t(const char*, size_t, size_t)> HeaderCallback;

    // 构造函数/析构函数
    HttpClient();
    ~HttpClient();

    // 基础请求方法
    Response request(Method method,
                     const std::string& url,
                     const std::string& body = "",
                     const Config& config = Config());

    // 便捷方法
    Response get(const std::string& url,
                 const std::map<std::string, std::string>& params = std::map<std::string, std::string>(),
                 const Config& config = Config());

    Response post(const std::string& url,
                  const std::string& data,
                  const Config& config = Config());

    Response post_form(const std::string& url,
                       const std::map<std::string, std::string>& form_data,
                       const Config& config = Config());

    Response put(const std::string& url,
                 const std::string& data,
                 const Config& config = Config());

    Response delete_(const std::string& url,
                     const Config& config = Config());

    // 文件操作
    bool download(const std::string& url,
                  const std::string& local_path,
                  const Config& config = Config());

    Response upload(const std::string& url,
                    const std::string& file_path,
                    const std::string& field_name = "file",
                    const Config& config = Config());

    // 设置回调函数
    void set_write_callback(WriteCallback callback);
    void set_read_callback(ReadCallback callback);
    void set_progress_callback(ProgressCallback callback);
    void set_header_callback(HeaderCallback callback);

    // 工具函数
    static std::string url_encode(const std::string& str);
    static std::string url_decode(const std::string& str);

    // 全局初始化/清理
    static bool init();
    static void cleanup();

private:
    // 前向声明
    class Impl;
    Impl* impl_;

    // 禁用拷贝
    HttpClient(const HttpClient&);
    HttpClient& operator=(const HttpClient&);

    // 辅助函数
    static std::string build_query_string(const std::map<std::string, std::string>& params);
    static std::string method_to_string(Method method);
};

#endif // HTTP_UTILS_H
