#include <raylib.h>

enum PLAYER_TURN{
    NOUGHT  = 1,
    CROSS   = 2
};

// return has width = top 32 bits, height = bottom 32 bits
Vector2 get_section_width_height(int win_height, int win_width, int line_thickness, int border_offset){
    int section_width = (win_width - 2*border_offset - 2*line_thickness)/3;
    int section_height = (win_height - 2*border_offset - 2*line_thickness)/3;
    return (Vector2){section_width, section_height};
}

// returns the rectangle which fills the top left section of the board
Rectangle draw_board(int win_height, int win_width, int section_width, int section_height, int line_thickness, int border_offset){
    Rectangle vertical_line = {section_width + border_offset, border_offset, line_thickness, win_height-2*border_offset};
    DrawRectangleRec(vertical_line, WHITE);
    vertical_line.x += section_width + line_thickness;
    DrawRectangleRec(vertical_line, WHITE);

    Rectangle horizontal_line = {border_offset, section_height + border_offset, win_width-2*border_offset, line_thickness};
    DrawRectangleRec(horizontal_line, WHITE);
    horizontal_line.y += section_height + line_thickness;
    DrawRectangleRec(horizontal_line, WHITE);

    return (Rectangle){.x = border_offset, .y = border_offset, .width = section_width, .height = section_height};
}

int in_section(Rectangle* sections, int x, int y){
    for(int i = 0; i < 9; i++){
        if(x >= sections[i].x && x <= sections[i].x + sections[i].width){
            if(y >= sections[i].y && y <= sections[i].y + sections[i].height){
                return i;
            }
        }
    }
    return -1;
}

void draw_cross(Rectangle section){
    int border = 40;
    Vector2 line1_start = {section.x + border, section.y + border};
    Vector2 line1_end = {section.x + section.width - border, section.y + section.height - border};
    Vector2 line2_start = {section.x + border, section.y + section.height - border};
    Vector2 line2_end = {section.x + section.width - border, section.y + border};
    DrawLineEx(line1_start, line1_end, 15, WHITE);
    DrawLineEx(line2_start, line2_end, 15, WHITE);
}

void draw_nought(Rectangle section){
    int border = 40;
    Vector2 centre = {section.x + section.width/2, section.y + section.height/2};
    int larger_radius = (section.width/2) - border;
    int smaller_radius = larger_radius*0.85f;
    DrawRing(centre, smaller_radius, larger_radius, 0, 360, 72, WHITE);
}

bool is_winning(bool plays[9]){
    if(
        // 3 across
        (plays[0] && plays[1] && plays[2]) ||
        (plays[3] && plays[4] && plays[5]) ||
        (plays[6] && plays[7] && plays[8]) ||

        // 3 down
        (plays[0] && plays[3] && plays[6]) ||
        (plays[1] && plays[4] && plays[7]) ||
        (plays[2] && plays[5] && plays[8]) ||

        // 3 diagonal
        (plays[0] && plays[4] && plays[8]) ||
        (plays[2] && plays[4] && plays[6])
    ){
        return true;
    }
    return false;
}

bool board_is_full(bool noughts[9], bool crosses[9]){
    for(int i = 0; i < 9; i++){
        if(!(noughts[i] || crosses[i])){
            return false;
        }
    }
    return true;
}

/*
 *  -----------
 * |_0_|_1_|_2_|
 * |_3_|_4_|_5_|
 * | 6 | 7 | 8 |
 *  -----------
 *
 */
int main(){
    const int WIN_WIDTH = 1000;
    const int WIN_HEIGHT = 1000;
    const int FPS = 30;
    const int LINE_THICKNESS = 20;
    const int BORDER_OFFSET = 60;

    InitWindow(WIN_WIDTH, WIN_HEIGHT, "Noughts & Crosses");
    SetTargetFPS(FPS);

    enum PLAYER_TURN player = CROSS;
    bool game_is_over = false;

    Rectangle sections[9] = {0};
    bool noughts[9] = {false};
    bool crosses[9] = {false};
    
    Vector2 section_width_height = get_section_width_height(WIN_HEIGHT, WIN_WIDTH, LINE_THICKNESS, BORDER_OFFSET);
    int section_width = section_width_height.x;
    int section_height = section_width_height.y;

    // generate section boundaries
    sections[0] = (Rectangle){.x = BORDER_OFFSET, .y = BORDER_OFFSET, .width = section_width, .height = section_height};
    for(int i = 1; i <= 8; i++){
        if(i % 3 == 0){ // first square on new row
            sections[i] = sections[i-3];
            sections[i].y += sections[i].height + LINE_THICKNESS;
        }else{
            sections[i] = sections[i-1];
            sections[i].x += sections[i].width + LINE_THICKNESS;
        }
    }

    // main game loop
    while(!WindowShouldClose()){
        // process inputs
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !game_is_over){
            int mousex = GetMouseX();
            int mousey = GetMouseY();
            int clicked_section = in_section(sections, mousex, mousey);
            if(clicked_section > -1 && !(noughts[clicked_section] || crosses[clicked_section])){
                if(player == CROSS && !noughts[clicked_section]){
                    crosses[clicked_section] = true;
                    player = NOUGHT;
                }else if(player == NOUGHT && !crosses[clicked_section]){
                    noughts[clicked_section] = true;
                    player = CROSS;
                }
            }
        }

        // reset
        if(IsKeyPressed(KEY_R)){
            for(int i = 0; i < 9; i ++){
                crosses[i] = false;
                noughts[i] = false;
                player = CROSS;
                game_is_over = false;
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);
        //DrawFPS(10, 10);
        (void)draw_board(WIN_HEIGHT, WIN_WIDTH, section_width, section_height, LINE_THICKNESS, BORDER_OFFSET);
        
        // Draw sections
        for(int i = 0; i < 9; i++){
            if(crosses[i]){
                draw_cross(sections[i]);
            }else if(noughts[i]){
                draw_nought(sections[i]);
            }
        }

        // check win state
        if(is_winning(crosses)){
            char win_str[50] = "Crosses win!";
            int text_len = TextLength(win_str);
            int size = 50;
            DrawText(win_str, (WIN_WIDTH/2) - (text_len*size)/4, WIN_HEIGHT - size, size, WHITE);
            game_is_over = true;
        }else if(is_winning(noughts)){
            char win_str[50] = "Noughts win!";
            int text_len = TextLength(win_str);
            int size = 50;
            DrawText(win_str, (WIN_WIDTH/2) - (text_len*size)/4, WIN_HEIGHT - size, size, WHITE);
            game_is_over = true;
        }else if(board_is_full(noughts, crosses)){
            char win_str[50] = "Draw!";
            int text_len = TextLength(win_str);
            int size = 50;
            DrawText(win_str, (WIN_WIDTH/2) - (text_len*size)/4, WIN_HEIGHT - size, size, WHITE);
            game_is_over = true;
        }
        EndDrawing();
    }
    CloseWindow();

    return 0;
}
