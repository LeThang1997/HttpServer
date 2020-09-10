#include <iostream>
#include <fstream>
#include "json.h"
#include "HttpServer.hpp"
#include <ctime>




HttpServer::HttpServer()
{
    this->base = NULL;
    this->server = NULL;
}

HttpServer::~HttpServer()
{
    if (this->server != NULL)
    {
        evhttp_free(this->server);
    }
    if (this->base != NULL)
    {
        event_base_free(this->base);
    }
}

void HttpServer::loop() {
    if (this->base == NULL)
    {
        this->base = event_base_new();
    }
    else if (this->server == NULL)
    {
        this->server = evhttp_new(this->base);
        evhttp_set_allowed_methods(this->server, EVHTTP_REQ_GET | EVHTTP_REQ_POST | EVHTTP_REQ_PUT | EVHTTP_REQ_DELETE | EVHTTP_REQ_OPTIONS);
        evhttp_set_gencb(this->server, callback, this);

        if (evhttp_bind_socket(this->server, "0.0.0.0", PORT_NUMBER) != 0)
        {
            std::cerr << "Failed to bind to 0.0.0.0:" << PORT_NUMBER << std::endl;
        }
        else
        {
            std::cout << "HTTP Server Listening on 0.0.0.0:" << PORT_NUMBER << std::endl;
        }
    }
    else
    {
        event_base_loop(this->base, EVLOOP_NONBLOCK);
    }
}

bool equal(const char *src, const char *dst)
{
    return strcmp(src, dst) == 0;
}

bool startWith(const char **src, const char *dst)
{
    const char *str = *src;
    size_t len = strlen(dst);
    if (strncmp(str, dst, len) == 0)
    {
        *src = str + len;
        return true;
    }
    else
    {
        return false;
    }
}

bool nextInt(const char **src, int *out)
{
    int number = 0;
    bool found = false;
    const char *str = *src;
    while (*str)
    {
        char c = *str;
        if ((c >= '0') && (c <= '9'))
        {
            found = true;
            number = number * 10 + c - '0';
        }
        else if (c == '/')
        {
            break;
        }
        else
        {
            return false;
        }
        str++;
    }

    if (found)
    {
        *src = str + 1;
        *out = number;
        return true;
    }
    else
    {
        return false;
    }
}

void HttpServer::callback(struct evhttp_request *request, void *param)
{
     HttpServer *server = (HttpServer *)param;
    const char *urisz = evhttp_request_get_uri(request);
    struct evhttp_uri *uri = evhttp_uri_parse(urisz);
    if (uri != NULL) {
        const char *path = evhttp_uri_get_path(uri);
        std::cout << path << std::endl;
        std::cout << readHeader(request) << std::endl;
        if (startWith(&path, "/anpr")) 
        {
            if (readHeader(request) == "POST") 
            {
                std::string content = readContent(request);
                auto j = json::parse(content);
                std::cout << "Data:" << content << std::endl; 
                json fb ;
                fb["status"] = "success";
                sendJson(request, 200,fb ); 
            }               
        }
        if (startWith(&path, "/frame")) 
        {
            if (readHeader(request) == "POST") 
            {
                cv::Mat image = readImage(request);
                if (!image.empty()) {
                    std::cout << "Post image succeed\n"; 
                    cv::imwrite("image.jpg", image);
                    json fb ;
                    fb["status"] = "success";
                    sendJson(request, 200,fb ); 
                }
            }
                
        }
    }
           
        //end
        evhttp_uri_free(uri);

}

const std::string HttpServer::readContent(struct evhttp_request *request)
{
    std::string content;
    struct evbuffer *buffer;
    buffer = evhttp_request_get_input_buffer(request);
    while (evbuffer_get_length(buffer) > 0)
    {
        int n;
        char cbuf[1024];
        n = evbuffer_remove(buffer, cbuf, sizeof(cbuf));
        if (n > 0)
        {
            content += std::string(cbuf, n);
        }
    }
    return content;
}

const std::string HttpServer::readHeader(struct evhttp_request *request)
{
    std::string cmdType;
    switch (evhttp_request_get_command(request)) {
    case EVHTTP_REQ_GET: cmdType = "GET"; break;
    case EVHTTP_REQ_POST: cmdType = "POST"; break;
    case EVHTTP_REQ_HEAD: cmdType = "HEAD"; break;
    case EVHTTP_REQ_PUT: cmdType = "PUT"; break;
    case EVHTTP_REQ_DELETE: cmdType = "DELETE"; break;
    case EVHTTP_REQ_OPTIONS: cmdType = "OPTIONS"; break;
    case EVHTTP_REQ_TRACE: cmdType = "TRACE"; break;
    case EVHTTP_REQ_CONNECT: cmdType = "CONNECT"; break;
    case EVHTTP_REQ_PATCH: cmdType = "PATCH"; break;
    default: cmdType = "unknown"; break;
    }
    return cmdType;
}

void HttpServer::sendError(struct evhttp_request *request, int status, const std::string &message)
{
    json j;
    j["error"] = true;
    j["reason"] = message;
    sendJson(request, status, j);
}

void HttpServer::sendJson(struct evhttp_request *request, int status, const json &j)
{
    struct evbuffer *buffer;
    std::string content;
    std::string header;
    buffer = evbuffer_new();
    content = j.dump();
    evbuffer_add(buffer, content.data(), content.size());

    evhttp_add_header(request->output_headers, "Content-Type", "application/json");
    evhttp_add_header(request->output_headers, "Content-Length", std::to_string(content.size()).c_str());
    evhttp_send_reply(request, status, NULL, buffer);
    evbuffer_free(buffer);
}

void HttpServer::sendText(struct evhttp_request *request, int status, const std::string &text)
{

    struct evbuffer *buffer;
    std::string header;
    buffer = evbuffer_new();
    evbuffer_add(buffer, text.data(), text.size());

    evhttp_add_header(request->output_headers, "Content-Type", "text/plain; charset=us-ascii");
    evhttp_add_header(request->output_headers, "Content-Length", std::to_string(text.size()).c_str());
    evhttp_send_reply(request, status, NULL, buffer);
    evbuffer_free(buffer);
}

void HttpServer::sendImage(struct evhttp_request *request, int status, const cv::Mat& frame)
{
    struct evbuffer *buffer;
    std::vector<uchar> databuffer;
    cv::imencode(".jpg", frame, databuffer);
    buffer = evbuffer_new();
    evbuffer_add(buffer, databuffer.data(), databuffer.size());

    evhttp_add_header(request->output_headers,"Content-Type", "image/jpeg");
    evhttp_add_header(request->output_headers, "Content-Length", std::to_string(databuffer.size()).c_str());
    evhttp_send_reply(request, status, NULL, buffer);
    evbuffer_free(buffer);
}

const cv::Mat HttpServer::readImage(struct evhttp_request* request) {
    struct evbuffer* buffer;
    buffer = evhttp_request_get_input_buffer(request);
    size_t size = evbuffer_get_length(buffer);
    if (size > 0) {
        uint8_t* data = (uint8_t*)malloc(size);
        if (data != NULL) {
            int n = evbuffer_remove(buffer, data, size);
            if (n > 0) {
                // decode here
                cv::Mat raw(1, n, CV_8UC1, data);
                cv::Mat image = cv::imdecode(raw, cv::IMREAD_COLOR);
                return image;
            }
            free(data);
        }
    }
    return cv::Mat();
}