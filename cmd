32 bit:
i686-w64-mingw32-g++ --static -lwsock32 -std=gnu++11 -DNDEBUG -g3 -O3 -Wall -Wextra -o test.exe main.cpp -lws2_32 -lgdi32

64 bit:
x86_64-w64-mingw32-g++ --static -lwsock32 -std=gnu++11 -DNDEBUG -g3 -O3 -Wall -Wextra -o test.exe main.cpp -lws2_32 -lgdi32


dll:
x86_64-w64-mingw32-g++ --static -lwsock32 -std=gnu++11 -DNDEBUG -g3 -O3 -Wall -Wextra -lws2_32 -lgdi32 -c main_dll.cpp
x86_64-w64-mingw32-g++ -o dll64.dll -s -shared main_dll.o --static -lwsock32 -std=gnu++11 -DNDEBUG -g3 -O3 -Wall -Wextra -lws2_32 -lgdi32


i686-w64-mingw32-g++ --static -lwsock32 -std=gnu++11 -DNDEBUG -g3 -O3 -Wall -Wextra -lws2_32 -lgdi32 -c main_dll.cpp
i686-w64-mingw32-g++ -o dll32.dll -s -shared main_dll.o --static -lwsock32 -std=gnu++11 -DNDEBUG -g3 -O3 -Wall -Wextra -lws2_32 -lgdi32
