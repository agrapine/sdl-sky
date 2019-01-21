#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>


const int SCREEN_WIDTH = 800, SCREEN_HEIGHT = 600;

class MainApp {
public:
  void run(){
    init();
    loop();
    clean();
  };
  
private:
  SDL_Window* window = nullptr;
  SDL_GLContext glContext = nullptr;
  Uint32 target_fps = 60;
  Uint32 target_fps_ticks;
  
  void init(){
    initVsync();
    initWindow();
    initGL();
  }
  
  void initVsync(){
    double period = 1.0 / target_fps;
    target_fps_ticks = (Uint32)(period * 1000);
  }
  
  void initGL(){
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();    
  }
  
  void glCheckError(){
    const auto error = glGetError();
    if(error != GL_NO_ERROR){
      throw std::runtime_error("gl : general error");
    }
  }
  
  void initWindow(){
    if( SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
      std::cout << "fail: sdl failed to init\n";
    }
    
    window =
    SDL_CreateWindow("SCai",
                     SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                     SCREEN_WIDTH, SCREEN_HEIGHT,
                     SDL_WINDOW_OPENGL);
    if(nullptr == window)
      throw std::runtime_error(SDL_GetError());
    
    SDL_StartTextInput();
    glContext = SDL_GL_CreateContext(window);
  }
  
  void loop(){
    SDL_Event e;
    bool keepAlive = true;
    while (keepAlive) {
      
      //vsync start
      Uint32 frameStart = SDL_GetTicks();
      
      if(SDL_PollEvent(&e)){
        switch (e.type) {
          case SDL_QUIT:
            keepAlive = false;
            break;
          case SDL_TEXTINPUT:
            handleKeys(e.text.text[0]);
            break;
          default:
            break;
        }
      }
      
      render();

      //swap buffer
      SDL_GL_SwapWindow(window);
      
      //vsync
      Uint32 frameEnd = SDL_GetTicks();
      int sleep = (target_fps_ticks - (frameEnd - frameStart));
      if(sleep < 0){
        sleep = 0;
      }
      //schedule next tick
      SDL_Delay(sleep);
    }
  };
  
  bool gRenderQuad = true;
  
  void handleKeys(u_char key){
    gRenderQuad = !gRenderQuad;
  }
  
  void render(){
    glClearColor(0.5,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    renderQuad();
  }
  
  void renderQuad(){
    if(!gRenderQuad)
      return;
    
    glBegin( GL_QUADS );
    glColor3f(1, 0, 0);
    glVertex2f( -0.5f, -0.5f );
    glColor3f(1, 1, 1);
    glVertex2f( 0.5f, -0.5f );
    glVertex2f( 0.5f, 0.5f );
    glVertex2f( -0.5f, 0.5f );
    glEnd();
  }
  
  void clean(){
    SDL_StopTextInput();
    SDL_GL_DeleteContext(glContext);
    
    //    SDL_FreeSurface(imageSurface);
    //    SDL_FreeSurface(windowSurface);
    SDL_DestroyWindow(window);
    SDL_Quit();
  };
};

int main(int argc, const char * argv[]) {
  MainApp app;
  try{
    app.run();
  }catch(const std::exception& e){
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
