#include "Camera.hpp"

int main( int argc, char** argv ){
    Camera c(0, "temp");

    while(true){
        if(!c.update())
            break;
    }

    cout<< "sucess2"<<endl;
    return 0;
}


