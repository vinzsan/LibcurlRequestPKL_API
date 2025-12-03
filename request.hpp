#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sched.h>
#include <sys/types.h>
#include <unistd.h>
#include <curl/curl.h>
#include <sys/signal.h>
#include <mutex>


#define panic(args) (({std::cout << "panic : signal abort thread 'main' PID(" << getpid() << ") restorer" << std::endl;\
                    std::cout << (args) << std::endl;\
                     _exit(EXIT_FAILURE);}))

class Request {
protected:

    CURL *curl = nullptr;
    CURLcode res;
    std::string buffer;

    std::mutex mtx;

    std::string url_buf;

    struct curl_slist *header = nullptr;

    static size_t callback_func(void *data,size_t n,size_t nmemb,void *user_data) {
        size_t total = n * nmemb;
        std::string* buff = reinterpret_cast<std::string*>(user_data);
        buff->append(reinterpret_cast<char*>(data),total);
        return total;
    }

public:

    Request(const std::string &url) : url_buf(url) {
        std:std::lock_guard<std::mutex> guard(mtx);
        
        curl = curl_easy_init();
        if(curl){
            curl_easy_setopt(curl,CURLOPT_URL,this->url_buf.c_str());
        }
    }

    std::string& operator*(){
        return this->buffer;
    }

    std::string get_result(){
        return this->buffer;
    }
    //get 
    
    Request& get_request() {
        std:std::lock_guard<std::mutex> guard(mtx);
        curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,callback_func);
        curl_easy_setopt(curl,CURLOPT_WRITEDATA,&this->buffer);
        res = curl_easy_perform(curl);
        return *this;
    }
    // post 

    Request& post_request(const std::string &header){
        std:std::lock_guard<std::mutex> guard(mtx);
        if(this->header) curl_slist_free_all(this->header);

        this->header = curl_slist_append(this->header,"Content-Type: application/json");
        
        curl_easy_setopt(curl,CURLOPT_POST,1L);
        curl_easy_setopt(curl,CURLOPT_HTTPHEADER,this->header);
        curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,callback_func);
        curl_easy_setopt(curl,CURLOPT_POSTFIELDS,header.c_str());
        curl_easy_setopt(curl,CURLOPT_WRITEDATA,&this->buffer);

        res = curl_easy_perform(curl);
        
        if(res != CURLE_OK){
            std::cerr << "ERROR : failed to request post to server" << curl_easy_strerror(res) << std::endl;
            panic("ERROR : failed request to server");
        }
        return *this;
    }
    // post token
    Request& post_token(const std::string &header,const std::string &token) {
        std::lock_guard<std::mutex> guard(mtx);
        if(this->header) curl_slist_free_all(this->header);

        this->header = curl_slist_append(this->header, "Content-Type: application/json");

        std::string bearer = "Authorization: Bearer " + token;
        
        this->header = curl_slist_append(this->header, bearer.c_str());
        
        curl_easy_setopt(curl,CURLOPT_POST,1L);
        curl_easy_setopt(curl,CURLOPT_HTTPHEADER,this->header);
        curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,callback_func);
        curl_easy_setopt(curl,CURLOPT_POSTFIELDS,header.c_str());
        curl_easy_setopt(curl,CURLOPT_WRITEDATA,&this->buffer);
        
        res = curl_easy_perform(curl);
        if(res != CURLE_OK){
            std::cerr << curl_easy_strerror(res) << std::endl;
            panic("ERROR : failed request server");
        }
        return *this;
    }
    // put 

    Request& put_request(const std::string &header) {
        std::lock_guard<std::mutex> guard(mtx);
        if(this->header) curl_slist_free_all(this->header);


        if(header.empty()){
            std::cerr << "error : header is empty" << std::endl;
            return *this;
        }
        
        this->header = curl_slist_append(this->header,"Content-Type: application/json");

        curl_easy_setopt(curl,CURLOPT_CUSTOMREQUEST,"PUT");
        
        curl_easy_setopt(curl,CURLOPT_WRITEDATA,&this->buffer);
        curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,callback_func);
        curl_easy_setopt(curl,CURLOPT_POSTFIELDS,header.c_str());
        
        res = curl_easy_perform(curl);
        if(res != CURLE_OK){
            panic("put error");
        }
        return *this;
    }
    // get token

    Request& get_token(const std::string &token){
        std::lock_guard<std::mutex> guard(mtx);
        if(this->header) curl_slist_free_all(this->header);
        
        if(token.empty()){
            std::cerr << "error : token empty" << std::endl;
            return *this;
        }
        std::string bearer = "Authorization: Bearer " + token;
        
        this->header = curl_slist_append(this->header,bearer.c_str());
        
        curl_easy_setopt(curl,CURLOPT_HTTPHEADER,this->header);
        curl_easy_setopt(curl,CURLOPT_WRITEDATA,&this->buffer);
        curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,callback_func);
        
        res = curl_easy_perform(curl);

        if(res != CURLE_OK){
            panic("error : get token");
        }
        return *this;
    }

    ~Request(){
        if(curl) curl_easy_cleanup(curl);
        if(header) curl_slist_free_all(header);
    }
};  
