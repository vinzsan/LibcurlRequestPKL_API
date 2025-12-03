#include <iostream>
#include <format>
#include "request.hpp"
#include <nlohmann/json.hpp>

int main(){
    auto r = Request("https://apidev-pkl.hummatech.com/api/login")
            .post_request(
                "{\"email\":\"pesertademo@gmail.com\",\"password\":\"12345678\"}"
            ).get_result();

    auto json_parse = nlohmann::json::parse(r);
    auto token = json_parse["data"]["token"].get<std::string>();
    auto id = json_parse["data"]["user"]["id"].get<std::string>();

    std::cout << "vinzsan token : " << token << std::endl;
    
    auto assign_peserta = Request("https://apidev-pkl.hummatech.com/api/assign/peserta")
            .post_request(std::format(R"({{"id_user":"{}"}})", id))
            .get_result();
            
    std::cout << assign_peserta << std::endl;

    auto s = Request("https://apidev-pkl.hummatech.com/api/peserta/detail")
            .get_token(token)
            .get_request()
            .get_result();

    std::cout << s << std::endl;
    
    return 0;
}