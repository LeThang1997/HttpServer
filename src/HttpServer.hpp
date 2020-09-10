

#ifndef HttpServer_hpp
#define HttpServer_hpp

#include <string>
#include <event2/listener.h>
#include <event2/bufferevent.h>
//#include <evhttp.h>
#include <event2/http.h>
#include <event2/http_struct.h>
#include <event2/http_compat.h>
#include <opencv2/opencv.hpp>
#include <event2/event_struct.h>
#include <event2/event.h>
#include <event2/event_compat.h>
#include <event2/buffer.h>
#include <event2/buffer_compat.h>
#include <event2/bufferevent_struct.h>
#include <event2/bufferevent_compat.h>
#include <event2/tag.h>
#include <event2/tag_compat.h>
#include <vector>
#include "json.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#define PORT_NUMBER 2003
#define LIBEVENT_LOOP_INTERVAL 30

class HttpServer {
public:
    HttpServer();
    virtual ~HttpServer();
    virtual void loop();
    
private:
    cv::Mat frame;
    struct event_base* base;
    struct evhttp* server;

    static void callback(struct evhttp_request* request, void* param);   
    static const std::string readContent(struct evhttp_request* request);
    static const std::string readHeader(struct evhttp_request* request);
    static void sendError(struct evhttp_request* request, int status, const std::string& message);
    static void sendJson(struct evhttp_request* request, int status, const json& j);
    static void sendText(struct evhttp_request* request, int status, const std::string& text);
    static void sendImage(struct evhttp_request *request, int status, const cv::Mat& frame);
    static const cv::Mat readImage(struct evhttp_request* request);
};


#endif /* HttpServer_hpp */
