

Step 1: Install Package
  sudo apt-get update
  sudo apt-get install freeglut3-dev
  sudo apt-get install mesa-utils
  sudo apt-cache search glut
  sudo apt-get install libglfw3-dev
  sudo apt-get install libevent-dev 


Step 2: Git Clone nlohmann/json
 1> git clone https://github.com/nlohmann/json
 2> cd json/
 3> mkdir build
 4> cd build/
 5> cmake ..
 6> make
 7> make install (if "permission denied" then: sudo make install)


Step 3:
 -> Open project directory CamHub
   mkdir build
   cd build/
   cmake ..
   make
   ./BkavAI
