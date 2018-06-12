# GameCenter
A mini game center with one game (Space Tube Ship) made for the 'Object Oriented Programming with C++' course at Linnaeus University.

The 'Space Tube Ship' mini game is based on the Java version of same game I made for the 'Software Testing' course. I thought making it in 'C++' language is going to be kind of easy, but I was wrong and it took a while, and a lot of fixings, to work properly. I hope You'll enjoy it.. So.. Enjoy.. :)

## Build
VC++:
```
cl /EHsc /W4 /WX Loader.cpp Console.cpp Menu.cpp SpaceTubeGame.cpp SpaceTubePlayer.cpp /FeGameCenter.exe
```

G++:
```
g++ Loader.cpp Console.cpp Menu.cpp SpaceTubeGame.cpp SpaceTubePlayer.cpp -o GameCenter -std=c++11 -lpthread
```
