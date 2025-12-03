#include <cstddef>
#include <raylib.h>
#include <atomic>
#include <vector>
#include "request.hpp"

enum Scene {
    Scene1,
    Scene2,
};

enum BackgroundImage {
    Image1,
    Image2,
};

class RayGui{
private:

    std::atomic<bool> counter = false;
    std::vector<Texture2D> background_image;
    BackgroundImage image_flags = Image1;

public:

    RayGui(const std::string &title,int width,int height) {
        InitWindow(width,height,title.c_str());
        SetTargetFPS(60);

        const char *_name_file[] = {
            "./assets/parallax.jpg",
            "./assets/background_story1.jpg"
        };

        for(auto &file : _name_file){
            Texture2D texture = LoadTexture(file);
            if(texture.id == 0){
                panic("file not found");
            }
            background_image.push_back(texture);
        }
    }

    RayGui& run(){
        counter.store(true,std::memory_order_relaxed);
        return *this;
    }

    void update(){
        int keystate = GetKeyPressed();
        switch(keystate){
            case KEY_Q:
                counter.store(false,std::memory_order_relaxed);
                break;
            case KEY_ONE:
                image_flags = BackgroundImage::Image1;
                break;
            case KEY_TWO:
                image_flags = BackgroundImage::Image2;
                break;
            default:
                //std::cout << "Nothin key" << std::endl;
                break;
        }
    }
    
    void build(){
        
        while(!WindowShouldClose() && counter.load(std::memory_order_relaxed)){
            int width = GetScreenWidth(),height = GetScreenHeight();
            //
            this->update();
            BeginDrawing();
            ClearBackground(BLACK);
            if(static_cast<size_t>(image_flags) < background_image.size()){
                DrawTexturePro(
                    this->background_image[image_flags],Rectangle {0,0,(float)background_image[image_flags].width,(float)background_image[image_flags].height},
                    Rectangle { 0,0,static_cast<float>(width),static_cast<float>(height)},Vector2 {0,0},0.0,WHITE
                );
            }
            //DrawTexturePro(Texture2D texture, Rectangle source, Rectangle dest, Vector2 origin, float rotation, Color tint)
            EndDrawing();
        }
    }
};

