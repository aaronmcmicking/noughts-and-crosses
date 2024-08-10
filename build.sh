CFLAGS="-O3 -std=c18 -Wextra -Wall"
RAYLIB="-lraylib -lGL -lm -lpthread -ldl -lrt -lX11"

gcc $CFLAGS -o noughts_and_crosses noughts_and_crosses.c $RAYLIB
