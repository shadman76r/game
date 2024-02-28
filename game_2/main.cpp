#include <GL/gl.h>
#include <GL/glut.h>
#include <bits/stdc++.h>
#include <windows.h>
#include <MMSystem.h>
#define STB_IMAGE_IMPLEMENTATION
//#include "image_loader.h"
using namespace std;


#define IN_GAME 12
#define GAME_OVER 13
#define NOT_START_YET 14
#define STARTING 15
#define SPAWNING 16
#define COUNTDOWN 17
#define ABOUT_US 18

#define ENABLE true
#define DISABLE false

///structure
struct Point{
    float x, y;
};


/// game control vars
float movement_speed = 0.05;
float initial_space_ship_speed = 0.001; // 0.001
float increament_space_ship_speed = 0.001;
float spaceship_speed = initial_space_ship_speed;
bool show_spaceship = ENABLE;
bool spaceship_immunity = DISABLE;
int spaceship_immunity_time = 0;
Point rocket_head = {0, -1.0f}; // y = -0.65
float x_translation = 0.0f;
float y_translation = 0.0f;
bool mouse_control = DISABLE;

float bullet_speed = 0.05; //0.06
int bullet_firing_speed = 500;
bool fire_3x_bullet = DISABLE;

int obstacle_firing_rate = 1;
int obstacle_count = 0;
int obstacle_max = 7;
float obstacle_width = 0.2;
const float obstacle_block_width = 0.004;
int obstacle_firing_interval = 2000;

const float star_block_width = 0.015;
float star_ytrans = 3;
float star_speed = 0.003;
int star_counter = 100;

float bg_testing_translateY = 1;

float heart_block_width = 0.003;
int heart_count = 3;

float game_name_animation_y = 0;
float start_text_animation_y = 0;
float start_rocket_position_y = 0;

int starting_countdown = 3;
float countdown_animation_x = 0;


/// loaded texture ids

GLuint bg_texture = 1;
GLuint normal = 2;

/// enums
enum Obstacle_type{
    DAMAGING_OBSTACLE,
    FIRING_RATE_3X_OBSTACLE
};


///structures
struct Color{
    int r, g, b;
};

struct Bullet{
    bool valid;
    Color c;
    Point b1;

    Bullet(float a, float b){
        b1.x = a, b1.y = b;
        valid = true;
    }

    void draw();
    void update(){b1.y += bullet_speed;}
};

struct Obstacle{
    Point p;
    bool valid;
    Obstacle_type obstacle_type;


    Obstacle(float x, float y, Obstacle_type _obstacle_type = DAMAGING_OBSTACLE){
        p.x = x;
        p.y = y;
        valid = true;
        obstacle_type = _obstacle_type;
    }

    void draw();

    void update(){p.y -= spaceship_speed;}
};

struct Star{
    bool isShining;
    float x, y;

    Star(float a, float b){
         x = a;
         y = b;
         isShining = false;
    }

    Star(){}

    void inverse_shining(){
        isShining = !isShining;
    }

    void draw();
};

/// game modes & power vars
bool power_mode = false;
int game_status = NOT_START_YET;



/// score
long long score = 0;



///spaceship specifications
void draw_spaceship();
float spaceship_block_width = 0.008;
float sp_min_x = -27*spaceship_block_width;
float sp_max_x = 15*spaceship_block_width;
float sp_min_y = -27*spaceship_block_width;
float sp_max_y = -2*spaceship_block_width;

/// fonts
//const int font1=(int)GLUT_BITMAP_TIMES_ROMAN_24;
//const int font2=(int)GLUT_BITMAP_HELVETICA_18 ;// check it any why need to
//const int font3=(int)GLUT_BITMAP_8_BY_13;

/// bullet specifications
void draw_bullet();
float bullet_block_width = 0.003;

///obstacles specifications


/// storage
vector<Obstacle> obstacles;
vector<Bullet> bullets;
vector<Star> stars;




/// Methods ------------------------------------------------

void renderBitmapString(float x, float y, void *font, const char* string){

    const char *c;
    glRasterPos2f(x, y);
    for (c=string; *c != '\0'; c++) {
        glutBitmapCharacter(font, *c);
    }
}

float getRand(float h = 1, float l = -1){
    float x = l + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(h-l)));
    //printf("%.2f\n", x);
    return x;
}

void load_image(char* filename, GLuint texture_id, float initx, float inity, float width, float height){
    glColor3ub(255, 255, 255);
    int channel;
    int width_;
    int height_;

    // load png into memory
//    stbi_set_flip_vertically_on_load(true);
//    unsigned char* image_data = stbi_load(filename, &width_, &height_, &channel, STBI_rgb_alpha);

    //if(image_data == nullptr) printf("error");
    //else printf("Image loaded");

    //generate opengl texture
    //GLuint texture_id = rand();
    glGenTextures(1, &texture_id);

    // bind the texture
    glBindTexture(GL_TEXTURE_2D, texture_id);

    // add image data
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Enable texture mapping
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(initx, inity);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(initx + width, inity);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(initx + width,  inity + height);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(initx, inity + height);
    glEnd();

    // Cleanup
    //stbi_image_free(image_data);
    glDeleteTextures(1, &texture_id);
}

void draw_quads_spaceship(float x, float y, Color c){
    glColor3ub(c.r, c.g, c.b);
    glBegin(GL_QUADS);
    glVertex2f(x,y);
    glVertex2f(x+spaceship_block_width,y);
    glVertex2f(x+spaceship_block_width,y+spaceship_block_width);
    glVertex2f(x,y+spaceship_block_width);
    glEnd();
}

void reset_sound(){

}

void play_sound(){
    sndPlaySound(NULL, 0);

    switch(game_status){
        case GAME_OVER:
        case NOT_START_YET:{
            sndPlaySound("audio/intro-outro.wav", SND_ASYNC | SND_LOOP);
            break;
        }

        case IN_GAME:{
            sndPlaySound("audio/mainsection2.wav", SND_ASYNC | SND_LOOP);
        }

    }

}

void draw_quads_obstacle(float x, float y, Color c){
    glColor3ub(c.r, c.g, c.b);
    glBegin(GL_QUADS);
    glVertex2f(x,y);
    glVertex2f(x+obstacle_block_width,y);
    glVertex2f(x+obstacle_block_width,y+obstacle_block_width);
    glVertex2f(x,y+obstacle_block_width);
    glEnd();
}

void draw_quads_bullet(float x, float y, Color c){
    glColor3ub(c.r, c.g, c.b);
    glBegin(GL_QUADS);
    glVertex2f(x,y);
    glVertex2f(x+bullet_block_width,y);
    glVertex2f(x+bullet_block_width,y+bullet_block_width);
    glVertex2f(x,y+bullet_block_width);
    glEnd();
}

void spaceship_blink(int x){
    if(spaceship_immunity) show_spaceship = !show_spaceship; // toggle
    glutTimerFunc(spaceship_immunity_time, spaceship_blink, 0);
}

void disable_fire_3x_rate(int x){
    fire_3x_bullet = DISABLE;
}

void recheck_rocket_show(int x){
    show_spaceship = ENABLE;
}

void disable_spaceship_immunity(int x){
    spaceship_immunity_time = 0;
    spaceship_immunity = DISABLE;
    show_spaceship = ENABLE; // without this there is a funny bug T-T
    glutTimerFunc(200, recheck_rocket_show, 0);

}

void enable_spaceship_immunity(){
    spaceship_immunity = ENABLE;
    glutTimerFunc(3000, disable_spaceship_immunity, 0);
    spaceship_immunity_time = 100;
    glutTimerFunc(spaceship_immunity_time, spaceship_blink, 0);
}

void enable_ingame(){
    game_status = IN_GAME;
    starting_countdown = 3;
    countdown_animation_x = 0;
    mouse_control = ENABLE;
    play_sound();

}

void reset_game_data(){
    play_sound();
    //resetting data
    bullet_firing_speed = 700;
    obstacle_firing_rate = 1;
    star_speed = 0.003; // default speed
    star_ytrans = 0;
    heart_count = 3;
    obstacle_count = 0;
    spaceship_speed = initial_space_ship_speed;
    obstacle_firing_interval = 2000;
    rocket_head.y = -1;
    score = 0;
    glutSetCursor(GLUT_CURSOR_INHERIT);

    bullets.clear();
    obstacles.clear();
}

void show_bullets(){

    int len = bullets.size();
    //printf("bullets count: %d\n", len);
    for(int i= 0; i < len; i++){
        Bullet b = bullets[i];
        if(b.b1.y > 1.05) {
            bullets.erase(bullets.begin() + i);
        }

        //printf("bullets %d Y axis value: %.2f\n\n", i, b.b1.y);

        if(b.valid){
            b.draw();
        }

    }
}

void update_bullets(int x){
    int len = bullets.size();
    for(int i= 0; i < len; i++){
        bullets[i].update();
    }

    //glutPostRedisplay();
    glutTimerFunc(10, update_bullets, 0);
}

void show_obstacles(){

    int len = obstacles.size();
    //printf("Obstacle count: %d\n", len);
    for(int i = 0; i < len; i++){
        if(obstacles[i].valid) obstacles[i].draw();
        if(obstacles[i].p.y < -1) {
                //testing
//                printf("GAME OVER");
//                exit(0);
                ///game_status = GAME_OVER;
                //
            obstacles.erase(obstacles.begin() + i);
            obstacle_count--;
        }

    }
}

void update_obstacles(int x){
    int len = obstacles.size();
    for(int i = 0; i < len; i++){
        obstacles[i].update();
    }

    glutTimerFunc(10, update_obstacles, 0);
}

void update_score(){
    score++;
    if(score%10 == 0) {
        spaceship_speed += increament_space_ship_speed;
        star_speed += increament_space_ship_speed;
        obstacle_firing_rate++;
        if(bullet_firing_speed- 25> 100) bullet_firing_speed -= 25;
        ///cout << bullet_firing_speed << endl;
        if(obstacle_firing_interval - 100 > 0) obstacle_firing_interval -= 100;
        printf("Score: %d\n", score);
    }

    if(score%100 == 0) obstacle_max++;

    if(score%50==0){
        fire_3x_bullet = ENABLE;
        glutTimerFunc(5000, disable_fire_3x_rate, 0);
    }


}

void update_countdown(int x){
    starting_countdown--;
    countdown_animation_x = 0;
    if(starting_countdown != -1) glutTimerFunc(2000, update_countdown, 0);
    else enable_ingame();
}

void clash_check(){
    for(int i = 0; i < obstacles.size(); i++){
        Obstacle obstacle = obstacles[i];
        for(int j = 0; j < bullets.size(); j++){
            Bullet bullet = bullets[j];

            //printf("obstacle x range: %.2f - %.2f\nObstacle Y range: %.2f\nbullet x,y: (%.2f, %.2f)\n\n\n", obstacle.p.x, obstacle.p.x+obstacle_width, obstacle.p.y, bullet.b1.x, bullet.b1.y);

            if((bullet.b1.x >= obstacle.p.x -0.03 && bullet.b1.x <= obstacle.p.x+obstacle_width-0.03) && (bullet.b1.y >= obstacle.p.y && bullet.b1.y <= obstacle.p.y+obstacle_width) && obstacle.valid){
                if(obstacle.obstacle_type == DAMAGING_OBSTACLE){
                    obstacle.valid = false;
                    obstacles.erase(obstacles.begin() + i);
                    obstacle_count--;

                    bullet.valid = false;
                    bullets.erase(bullets.begin() + j);
                    //printf("------Hit\n");
                    update_score();
                }
            }
        }


        ///laser gun clash
        if((rocket_head.x + x_translation >= obstacle.p.x && rocket_head.x + x_translation <= obstacle.p.x+obstacle_width) && power_mode){
            obstacle.valid = false;
            obstacles.erase(obstacles.begin() + i);
            obstacle_count--;

            update_score();
        }
    }
}

void spaceship_clash_check(){
    for(int i = 0; i < obstacles.size(); i++){
        Obstacle obstacle = obstacles[i];
        if((obstacle.p.x >= rocket_head.x + x_translation + sp_min_x && obstacle.p.x <= rocket_head.x + x_translation + sp_max_x) && (obstacle.p.y <= rocket_head.y + y_translation + sp_max_y && obstacle.p.y >= rocket_head.y + y_translation + sp_min_y)){
            if(obstacle.obstacle_type == DAMAGING_OBSTACLE && !spaceship_immunity){
                heart_count--;
                enable_spaceship_immunity();
                obstacle.valid = false;
                obstacles.erase(obstacles.begin() + i);

            }

            else if(obstacle.obstacle_type == FIRING_RATE_3X_OBSTACLE){
                if(obstacle.obstacle_type == FIRING_RATE_3X_OBSTACLE){
                    fire_3x_bullet = ENABLE;
                    glutTimerFunc(5000, disable_fire_3x_rate, 0);

                }
            }


            if(heart_count == 0){
                game_status = GAME_OVER;
                reset_game_data();
                //`````cout << heart_count << " : game over\n" << "Final score : " << score << endl;
            }
        }
    }
}

void fire_bullet(int x){
    if(game_status == IN_GAME){
        if(fire_3x_bullet){
            bullets.push_back(Bullet(rocket_head.x + x_translation - 9*spaceship_block_width, rocket_head.y + y_translation - 10*spaceship_block_width));
            bullets.push_back(Bullet(rocket_head.x + x_translation + 10*spaceship_block_width, rocket_head.y + y_translation - 10*spaceship_block_width));
        }

        bullets.push_back(Bullet(rocket_head.x + x_translation + spaceship_block_width, rocket_head.y + y_translation));
    }

    glutTimerFunc(bullet_firing_speed, fire_bullet, 0);
}

void fire_obstacle(int x){
    if(game_status == IN_GAME){
        for(int i = 0; i < obstacle_firing_rate; i++){
            int random = rand()%5;
            if(random == 3 && !fire_3x_bullet){
                //printf("random: %d\n", random);
                //obstacles.push_back(Obstacle(-0.9 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(0.9+0.9))), 0.9, FIRING_RATE_3X_OBSTACLE));

            }
            else if(obstacle_count <= obstacle_max && game_status == IN_GAME){
                obstacles.push_back(Obstacle(getRand(0.9, -0.9), getRand(1.8, 1.1)));
                obstacle_count++;
            }



        }
    }

    glutTimerFunc(obstacle_firing_interval, fire_obstacle, 0);
}

void keyboard_handle(unsigned char key, int x, int y){
    if(key == 'w') y_translation += movement_speed;
    else if(key == 's') y_translation -= movement_speed;
    else if(key == 'a') x_translation -= movement_speed;
    else if(key == 'd') x_translation += movement_speed;
    //dadelse if(key == 'p') power_mode = !power_mode;
    else if(key == 'n' || key == 'N') {
        if(game_status == GAME_OVER){
            game_status = SPAWNING;
            ///reset score
        }
    }
    else if(key == 27 && (game_status == GAME_OVER || game_status == ABOUT_US)){
        game_status = NOT_START_YET;
    }

    glutPostRedisplay();
}

void mouse_motion_handle(int x, int y){


}

void mouse_handle(int button, int state, int x, int y){
    int OldRange_x = (600 - 0);
    float NewRange_x = (1 + 1);
    float NewValue_x = (((x - 0) * NewRange_x) / OldRange_x) -1;

    int OldRange_y = (600 - 0);
    float NewRange_y = (1 + 1);
    float NewValue_y = (((y - 0) * NewRange_y) / OldRange_y) -1;

    NewValue_y = 1 - NewValue_y;
    //printf("%.2f, %.2f\n", NewValue_x, NewValue_y);

    if(NewValue_x >= -0.583 && NewValue_x <= 0.690 && NewValue_y >= 0.830 && NewValue_y <= 0.977 && game_status == NOT_START_YET) game_status = STARTING;
    if(NewValue_x >= -0.273 && NewValue_x <= 0.393 && NewValue_y >= 0.660 && NewValue_y <= 0.787 && game_status == NOT_START_YET){
        game_status = ABOUT_US;
        game_name_animation_y = 0;
        start_text_animation_y = 0;

    }
    if(NewValue_x >= -0.130 && NewValue_x <= 0.230 && NewValue_y >= 0.443 && NewValue_y <= 0.590 && game_status == NOT_START_YET) exit(0);

    if(NewValue_x >= -0.570 && NewValue_x <= 0.590 && NewValue_y >= 0.830 && NewValue_y <= 0.983 && game_status == GAME_OVER) game_status = SPAWNING;
    if(NewValue_x >= -0.590 && NewValue_x <= 0.590 && NewValue_y >= 0.540 && NewValue_y <= 0.683 && game_status == GAME_OVER)game_status = NOT_START_YET;
}

void passive_func(int x, int y){
    int OldRange_x = (600 - 0);
    float NewRange_x = (1 + 1);
    float NewValue_x = (((x - 0) * NewRange_x) / OldRange_x) -1;

    int OldRange_y = (600 - 0);
    float NewRange_y = (1 + 1);
    float NewValue_y = (((y - 0) * NewRange_y) / OldRange_y) -1;

    if(mouse_control == ENABLE){
        x_translation = NewValue_x;
        y_translation = 1-NewValue_y - 0.15;
    }


    glutPostRedisplay();
    //printf("(%.3f, %.3f)\n", NewValue_x, 1-NewValue_y);
}

void slow_update(int x){
    clash_check();
    spaceship_clash_check();

    glutPostRedisplay();
    glutTimerFunc(100, slow_update, 0);
}

void update_and_check(){
    clash_check();
    spaceship_clash_check();

    // update home animations
    if(game_status == NOT_START_YET) if(game_name_animation_y < 0.6) game_name_animation_y += 0.01;
    if(game_status == STARTING) if(game_name_animation_y >= 0) {
        game_name_animation_y -= 0.01;
        if(game_name_animation_y <= 0) game_status = SPAWNING;
    }

    if(game_status == NOT_START_YET) if(start_text_animation_y < 1) start_text_animation_y += 0.0167;
    if(game_status == STARTING) if(start_text_animation_y >= 0) start_text_animation_y -= 0.0167;

    if(game_status == SPAWNING){
        rocket_head.y += 0.001;
        if(rocket_head.y >= -0.65) {
            game_status = COUNTDOWN;
            glutSetCursor(GLUT_CURSOR_NONE); // hide cursor
            glutWarpPointer(300, 540); // reset cursor to start position
            glutTimerFunc(2000, update_countdown, 0);
        }
    }

    if(game_status == COUNTDOWN) countdown_animation_x += 0.002;



    glutPostRedisplay();
}

void star_shine_switch(int x){
    for(int i = 0; i < star_counter; i++){
        stars[i].inverse_shining();
    }
    glutTimerFunc(1000, star_shine_switch, 0);

}

void show_stars(){
    glPushMatrix();
    glTranslatef(0, star_ytrans, 0);
    for(int i = 0; i < stars.size(); i++){
        stars[i].draw();
    }
    glPopMatrix();
}

void update_stars(int x){
    if(game_status == IN_GAME || game_status == NOT_START_YET){
        star_ytrans -= star_speed;
        if(star_ytrans<-3) star_ytrans = 0;
        //printf("star_trans: %.2f\n", star_ytrans);
        glutPostRedisplay();
    }
    glutTimerFunc(50, update_stars, 0);
}

void show_score(){
    glColor3ub(255, 255, 255);
    stringstream score_int_to_str;
    score_int_to_str << score;
    string score_s;
    score_int_to_str >>  score_s;
    score_s = "Score : " + score_s;
//    renderBitmapString(0.71, 0.8, (void*) font3, score_s.c_str());
}

void draw_heart(float, float); // implementation below

void show_heart(){

    float sx = 0.9;
    float sy = 0.95;

    for(int i = 0; i < heart_count; i++){
        draw_heart(sx, sy);
        sx -= 0.08;
    }
}



void myDisplay(void){

   // printf("star_trans: %.2f\n", star_ytrans);

    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    if(game_status == IN_GAME){

        /// loading textures

        glPushMatrix(); // working
        glTranslatef(0, bg_testing_translateY, 0);
        //load_image("bg2.png", bg_texture, -2, -2, 3, 3);
        //bg_testing_translateY-=0.0005;
        //if(bg_testing_translateY < 0) bg_testing_translateY = 1;
        glPopMatrix();

        show_stars();
        show_bullets();
        show_obstacles();
        show_heart();
        show_score();


        glPushMatrix();
        glTranslatef(x_translation, y_translation, 0.0f);
        //shooter
        if(show_spaceship) draw_spaceship();



        //powers
        if(power_mode){
            glColor3ub(255, 0, 0);
            glBegin(GL_LINES);
            glVertex2f(0.01, rocket_head.y);
            glVertex2f(0.01, 0.95);
            glEnd();
            glColor3ub(255, 255, 255);
        }
        glPopMatrix();

        /// console + scoring
        ///system("cls");
        //instructions();
        ///
    }

    else if(game_status == SPAWNING){
        show_stars();
        draw_spaceship();

    }

    else if(game_status == COUNTDOWN){
        show_stars();
        draw_spaceship();

        glPushMatrix();
        glTranslatef(-countdown_animation_x, 0, 0);
        switch(starting_countdown){
            case 3:{
                load_image("img/3.jpg", bg_texture, 2, 0, 0.2, 0.2);
                break;
            }

            case 2:{
                load_image("img/2.jpg", bg_texture, 2, 0, 0.2, 0.2);
                break;
            }

            case 1:{
                load_image("img/1.jpg", bg_texture, 2, 0, 0.2, 0.2);
                break;
            }

            case 0:{
                load_image("img/GO.jpg", bg_texture, 2, 0, 0.4, 0.2);
                break;
            }
        }
        glPopMatrix();
    }

    else if(game_status == GAME_OVER){
        // game over img
        load_image("img/game_over.jpg", bg_texture, -0.68, 0.2, 1.4, 0.21); // game over

        load_image("img/new-game.jpg", bg_texture, -0.6, -0.2, 1.2, 0.2); // new game
        load_image("img/main-menu.jpg", bg_texture, -0.6, -0.5, 1.2, 0.2); // main menu
    }

    else if (game_status == NOT_START_YET || game_status == STARTING){

        show_stars();

        glPushMatrix();
        glTranslatef(0, -game_name_animation_y, 0);
        load_image("img/game_name.jpg", bg_texture, -0.72, 0.85, 1.5, 0.27); // game name banner
        glPopMatrix();


        glPushMatrix();
        glTranslatef(0, start_text_animation_y, 0);
        load_image("img/click-to-start.jpg", bg_texture, -0.6, -1.2, 1.3, 0.2); // click here to start banner
        load_image("img/about-us.jpg", bg_texture, -0.3, -1.4, 0.7, 0.2); // about us banner
        load_image("img/exit.jpg", bg_texture, -0.15, -1.6, 0.4, 0.2); // exit banner

        glPopMatrix();
    }

    else if(game_status == ABOUT_US){
            //names
        load_image("img/ratul.jpg", bg_texture, -0.57, 0.38, 1.2, 0.3);
        load_image("img/asiya.jpg", bg_texture, -0.36, 0.02, 0.75, 0.3);
        load_image("img/rafsi.jpg", bg_texture, -0.57, -0.34, 1.2, 0.3);
        load_image("img/rian.jpg", bg_texture, -0.37, -0.69, 0.75, 0.3);


    }

    glutSwapBuffers();

}

void myInit (void){

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glPointSize(3.0f);
    glMatrixMode(GL_PROJECTION);
    ///vsync off
    //bullets.push_back(Bullet(rocket_head.x + x_translation, rocket_head.y + y_translation));

    //init stars
    for(int i = 0; i < star_counter; i++){

        stars.push_back(Star(getRand(1, -1), getRand(4, -4)));
        //printf("initialized\n");
    }

}

int main(int argc, char** argv){

    srand(time(0));
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize (600, 600);
    glutInitWindowPosition (400, 0);
    glutCreateWindow ("SUPER SUS SHOOTER");
    glutDisplayFunc(myDisplay);
    glutKeyboardFunc(keyboard_handle);
    glutMouseFunc(mouse_handle);
    //glutMotionFunc(mouse_motion_handle);
    glutPassiveMotionFunc(passive_func);
    glutIdleFunc(update_and_check);

    /// timer functions
    //glutTimerFunc(1000, slow_update, 0);
    glutTimerFunc(1000, fire_bullet, 0);
    glutTimerFunc(1000, fire_obstacle, 0);
    glutTimerFunc(1000, update_bullets, 0);
    glutTimerFunc(1000, update_obstacles, 0);
    glutTimerFunc(1000, update_stars, 0);
    glutTimerFunc(1000, star_shine_switch, 0);

    play_sound();


    ///
    myInit ();
    glutMainLoop();
}

///---------------------------------------------------------------


/// implementations

void Bullet::draw(){
    float initX = b1.x;
    float initY = b1.y;

    draw_quads_bullet(initX, initY, {250, 0, 0});

    draw_quads_bullet(initX-bullet_block_width, initY+bullet_block_width, {250, 0, 0});
    draw_quads_bullet(initX, initY+bullet_block_width, {250, 71, 35});
    draw_quads_bullet(initX+bullet_block_width, initY+bullet_block_width, {250, 0, 0});

    draw_quads_bullet(initX-bullet_block_width, initY+bullet_block_width*2, {250, 0, 0});
    draw_quads_bullet(initX, initY+bullet_block_width*2, {250, 171, 52});
    draw_quads_bullet(initX+bullet_block_width, initY+bullet_block_width*2, {250, 0, 0});

    draw_quads_bullet(initX-bullet_block_width*2, initY+bullet_block_width*3, {250, 0, 0});
    draw_quads_bullet(initX-bullet_block_width, initY+bullet_block_width*3, {250, 71, 35});
    draw_quads_bullet(initX, initY+bullet_block_width*3, {250, 232, 70});
    draw_quads_bullet(initX+bullet_block_width, initY+bullet_block_width*3, {250, 71, 35});
    draw_quads_bullet(initX+bullet_block_width*2, initY+bullet_block_width*3, {250, 0, 0});

    draw_quads_bullet(initX-bullet_block_width*2, initY+bullet_block_width*4, {250, 0, 0});
    draw_quads_bullet(initX-bullet_block_width, initY+bullet_block_width*4, {250, 71, 35});
    draw_quads_bullet(initX, initY+bullet_block_width*4, {250, 232, 70});
    draw_quads_bullet(initX+bullet_block_width, initY+bullet_block_width*4, {250, 71, 35});
    draw_quads_bullet(initX+bullet_block_width*2, initY+bullet_block_width*4, {250, 0, 0});

    draw_quads_bullet(initX-bullet_block_width*3, initY+bullet_block_width*5, {250, 0, 0});
    draw_quads_bullet(initX-bullet_block_width*2, initY+bullet_block_width*5, {250, 71, 35});
    draw_quads_bullet(initX-bullet_block_width, initY+bullet_block_width*5, {250, 171, 52});
    draw_quads_bullet(initX, initY+bullet_block_width*5, {250, 232, 70});
    draw_quads_bullet(initX+bullet_block_width, initY+bullet_block_width*5, {250, 171, 52});
    draw_quads_bullet(initX+bullet_block_width*2, initY+bullet_block_width*5, {250, 71, 35});
    draw_quads_bullet(initX+bullet_block_width*3, initY+bullet_block_width*5, {250, 0, 0});

    draw_quads_bullet(initX-bullet_block_width*3, initY+bullet_block_width*6, {250, 0, 0});
    draw_quads_bullet(initX-bullet_block_width*2, initY+bullet_block_width*6, {250, 71, 35});
    draw_quads_bullet(initX-bullet_block_width, initY+bullet_block_width*6, {250, 171, 52});
    draw_quads_bullet(initX, initY+bullet_block_width*6, {250, 232, 70});
    draw_quads_bullet(initX+bullet_block_width, initY+bullet_block_width*6, {250, 171, 52});
    draw_quads_bullet(initX+bullet_block_width*2, initY+bullet_block_width*6, {250, 71, 35});
    draw_quads_bullet(initX+bullet_block_width*3, initY+bullet_block_width*6, {250, 0, 0});

    draw_quads_bullet(initX-bullet_block_width*4, initY+bullet_block_width*7, {250, 0, 0});
    draw_quads_bullet(initX-bullet_block_width*3, initY+bullet_block_width*7, {250, 71, 35});
    draw_quads_bullet(initX-bullet_block_width*2, initY+bullet_block_width*7, {250, 171, 52});
    draw_quads_bullet(initX-bullet_block_width, initY+bullet_block_width*7, {250, 232, 70});
    draw_quads_bullet(initX, initY+bullet_block_width*7, {250, 232, 70});
    draw_quads_bullet(initX+bullet_block_width, initY+bullet_block_width*7, {250, 232, 70});
    draw_quads_bullet(initX+bullet_block_width*2, initY+bullet_block_width*7, {250, 171, 52});
    draw_quads_bullet(initX+bullet_block_width*3, initY+bullet_block_width*7, {250, 71, 35});
    draw_quads_bullet(initX+bullet_block_width*4, initY+bullet_block_width*7, {250, 0, 0});

    draw_quads_bullet(initX-bullet_block_width*4, initY+bullet_block_width*8, {250, 0, 0});
    draw_quads_bullet(initX-bullet_block_width*3, initY+bullet_block_width*8, {250, 71, 35});
    draw_quads_bullet(initX-bullet_block_width*2, initY+bullet_block_width*8, {250, 171, 52});
    draw_quads_bullet(initX-bullet_block_width, initY+bullet_block_width*8, {250, 232, 70});
    draw_quads_bullet(initX, initY+bullet_block_width*8, {250, 232, 70});
    draw_quads_bullet(initX+bullet_block_width, initY+bullet_block_width*8, {250, 232, 70});
    draw_quads_bullet(initX+bullet_block_width*2, initY+bullet_block_width*8, {250, 171, 52});
    draw_quads_bullet(initX+bullet_block_width*3, initY+bullet_block_width*8, {250, 71, 35});
    draw_quads_bullet(initX+bullet_block_width*4, initY+bullet_block_width*8, {250, 0, 0});

    draw_quads_bullet(initX-bullet_block_width*5, initY+bullet_block_width*9, {250, 0, 0});
    draw_quads_bullet(initX-bullet_block_width*4, initY+bullet_block_width*9, {250, 71, 35});
    draw_quads_bullet(initX-bullet_block_width*3, initY+bullet_block_width*9, {250, 171, 52});
    draw_quads_bullet(initX-bullet_block_width*2, initY+bullet_block_width*9, {250, 232, 70});
    draw_quads_bullet(initX-bullet_block_width, initY+bullet_block_width*9, {250, 232, 70});
    draw_quads_bullet(initX, initY+bullet_block_width*9, {250, 232, 70});
    draw_quads_bullet(initX+bullet_block_width, initY+bullet_block_width*9, {250, 232, 70});
    draw_quads_bullet(initX+bullet_block_width*2, initY+bullet_block_width*9, {250, 232, 70});
    draw_quads_bullet(initX+bullet_block_width*3, initY+bullet_block_width*9, {250, 171, 52});
    draw_quads_bullet(initX+bullet_block_width*4, initY+bullet_block_width*9, {250, 71, 35});
    draw_quads_bullet(initX+bullet_block_width*5, initY+bullet_block_width*9, {250, 0, 0});

    draw_quads_bullet(initX-bullet_block_width*5, initY+bullet_block_width*10, {250, 0, 0});
    draw_quads_bullet(initX-bullet_block_width*4, initY+bullet_block_width*10, {250, 71, 35});
    draw_quads_bullet(initX-bullet_block_width*3, initY+bullet_block_width*10, {250, 171, 52});
    draw_quads_bullet(initX-bullet_block_width*2, initY+bullet_block_width*10, {250, 232, 70});
    draw_quads_bullet(initX-bullet_block_width, initY+bullet_block_width*10, {250, 232, 70});
    draw_quads_bullet(initX, initY+bullet_block_width*10, {250, 232, 70});
    draw_quads_bullet(initX+bullet_block_width, initY+bullet_block_width*10, {250, 232, 70});
    draw_quads_bullet(initX+bullet_block_width*2, initY+bullet_block_width*10, {250, 232, 70});
    draw_quads_bullet(initX+bullet_block_width*3, initY+bullet_block_width*10, {250, 171, 52});
    draw_quads_bullet(initX+bullet_block_width*4, initY+bullet_block_width*10, {250, 71, 35});
    draw_quads_bullet(initX+bullet_block_width*5, initY+bullet_block_width*10, {250, 0, 0});

    draw_quads_bullet(initX-bullet_block_width*5, initY+bullet_block_width*11, {250, 0, 0});
    draw_quads_bullet(initX-bullet_block_width*4, initY+bullet_block_width*11, {250, 71, 35});
    draw_quads_bullet(initX-bullet_block_width*3, initY+bullet_block_width*11, {250, 171, 52});
    draw_quads_bullet(initX-bullet_block_width*2, initY+bullet_block_width*11, {250, 232, 70});
    draw_quads_bullet(initX-bullet_block_width, initY+bullet_block_width*11, {250, 232, 70});
    draw_quads_bullet(initX, initY+bullet_block_width*11, {250, 232, 70});
    draw_quads_bullet(initX+bullet_block_width, initY+bullet_block_width*11, {250, 232, 70});
    draw_quads_bullet(initX+bullet_block_width*2, initY+bullet_block_width*11, {250, 232, 70});
    draw_quads_bullet(initX+bullet_block_width*3, initY+bullet_block_width*11, {250, 171, 52});
    draw_quads_bullet(initX+bullet_block_width*4, initY+bullet_block_width*11, {250, 71, 35});
    draw_quads_bullet(initX+bullet_block_width*5, initY+bullet_block_width*11, {250, 0, 0});

    draw_quads_bullet(initX-bullet_block_width*5, initY+bullet_block_width*12, {250, 0, 0});
    draw_quads_bullet(initX-bullet_block_width*4, initY+bullet_block_width*12, {250, 71, 35});
    draw_quads_bullet(initX-bullet_block_width*3, initY+bullet_block_width*12, {250, 171, 52});
    draw_quads_bullet(initX-bullet_block_width*2, initY+bullet_block_width*12, {250, 232, 70});
    draw_quads_bullet(initX-bullet_block_width, initY+bullet_block_width*12, {250, 232, 70});
    draw_quads_bullet(initX, initY+bullet_block_width*12, {250, 232, 70});
    draw_quads_bullet(initX+bullet_block_width, initY+bullet_block_width*12, {250, 232, 70});
    draw_quads_bullet(initX+bullet_block_width*2, initY+bullet_block_width*12, {250, 232, 70});
    draw_quads_bullet(initX+bullet_block_width*3, initY+bullet_block_width*12, {250, 171, 52});
    draw_quads_bullet(initX+bullet_block_width*4, initY+bullet_block_width*12, {250, 71, 35});
    draw_quads_bullet(initX+bullet_block_width*5, initY+bullet_block_width*12, {250, 0, 0});

    draw_quads_bullet(initX-bullet_block_width*5, initY+bullet_block_width*13, {250, 0, 0});
    draw_quads_bullet(initX-bullet_block_width*4, initY+bullet_block_width*13, {250, 71, 35});
    draw_quads_bullet(initX-bullet_block_width*3, initY+bullet_block_width*13, {250, 171, 52});
    draw_quads_bullet(initX-bullet_block_width*2, initY+bullet_block_width*13, {250, 171, 52});
    draw_quads_bullet(initX-bullet_block_width, initY+bullet_block_width*13, {250, 232, 70});
    draw_quads_bullet(initX, initY+bullet_block_width*13, {250, 232, 70});
    draw_quads_bullet(initX+bullet_block_width, initY+bullet_block_width*13, {250, 232, 70});
    draw_quads_bullet(initX+bullet_block_width*2, initY+bullet_block_width*13, {250, 171, 52});
    draw_quads_bullet(initX+bullet_block_width*3, initY+bullet_block_width*13, {250, 171, 52});
    draw_quads_bullet(initX+bullet_block_width*4, initY+bullet_block_width*13, {250, 71, 35});
    draw_quads_bullet(initX+bullet_block_width*5, initY+bullet_block_width*13, {250, 0, 0});

    draw_quads_bullet(initX-bullet_block_width*5, initY+bullet_block_width*14, {250, 0, 0});
    draw_quads_bullet(initX-bullet_block_width*4, initY+bullet_block_width*14, {250, 71, 35});
    draw_quads_bullet(initX-bullet_block_width*3, initY+bullet_block_width*14, {250, 171, 52});
    draw_quads_bullet(initX-bullet_block_width*2, initY+bullet_block_width*14, {250, 171, 52});
    draw_quads_bullet(initX-bullet_block_width, initY+bullet_block_width*14, {250, 171, 52});
    draw_quads_bullet(initX, initY+bullet_block_width*14, {250, 171, 52});
    draw_quads_bullet(initX+bullet_block_width, initY+bullet_block_width*14, {250, 171, 52});
    draw_quads_bullet(initX+bullet_block_width*2, initY+bullet_block_width*14, {250, 171, 52});
    draw_quads_bullet(initX+bullet_block_width*3, initY+bullet_block_width*14, {250, 171, 52});
    draw_quads_bullet(initX+bullet_block_width*4, initY+bullet_block_width*14, {250, 71, 35});
    draw_quads_bullet(initX+bullet_block_width*5, initY+bullet_block_width*14, {250, 0, 0});

    draw_quads_bullet(initX-bullet_block_width*4, initY+bullet_block_width*15, {250, 0, 0});
    draw_quads_bullet(initX-bullet_block_width*3, initY+bullet_block_width*15, {250, 71, 35});
    draw_quads_bullet(initX-bullet_block_width*2, initY+bullet_block_width*15, {250, 171, 52});
    draw_quads_bullet(initX-bullet_block_width, initY+bullet_block_width*15, {250, 171, 52});
    draw_quads_bullet(initX, initY+bullet_block_width*15, {250, 171, 52});
    draw_quads_bullet(initX+bullet_block_width, initY+bullet_block_width*15, {250, 171, 52});
    draw_quads_bullet(initX+bullet_block_width*2, initY+bullet_block_width*15, {250, 171, 52});
    draw_quads_bullet(initX+bullet_block_width*3, initY+bullet_block_width*15, {250, 71, 35});
    draw_quads_bullet(initX+bullet_block_width*4, initY+bullet_block_width*15, {250, 0, 0});

    draw_quads_bullet(initX-bullet_block_width*3, initY+bullet_block_width*16, {250, 0, 0});
    draw_quads_bullet(initX-bullet_block_width*2, initY+bullet_block_width*16, {250, 71, 35});
    draw_quads_bullet(initX-bullet_block_width, initY+bullet_block_width*16, {250, 71, 35});
    draw_quads_bullet(initX, initY+bullet_block_width*16, {250, 71, 35});
    draw_quads_bullet(initX+bullet_block_width, initY+bullet_block_width*16, {250, 71, 35});
    draw_quads_bullet(initX+bullet_block_width*2, initY+bullet_block_width*16, {250, 71, 35});
    draw_quads_bullet(initX+bullet_block_width*3, initY+bullet_block_width*16, {250, 0, 0});

    draw_quads_bullet(initX-bullet_block_width*2, initY+bullet_block_width*16, {250, 0, 0});
    draw_quads_bullet(initX-bullet_block_width, initY+bullet_block_width*16, {250, 0, 0});
    draw_quads_bullet(initX, initY+bullet_block_width*16, {250, 0, 0});
    draw_quads_bullet(initX+bullet_block_width, initY+bullet_block_width*16, {250, 0, 0});
    draw_quads_bullet(initX+bullet_block_width*2, initY+bullet_block_width*16, {250, 0, 0});

    glColor3ub(255, 255, 255);

}

void Obstacle::draw(){
    if(obstacle_type == DAMAGING_OBSTACLE) glColor3ub(205, 50, 125);
    else if(obstacle_type == FIRING_RATE_3X_OBSTACLE) glColor3ub(0, 50, 0); ///powers will be separate future

    float initX = p.x;

    float initY = p.y;

    draw_quads_obstacle(initX, initY, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width, initY, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*2, initY, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*3, initY, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*4, initY, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*5, initY, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*6, initY, {0, 0, 0});

    draw_quads_obstacle(initX-obstacle_block_width, initY-obstacle_block_width, {0, 0, 0});
    draw_quads_obstacle(initX, initY-obstacle_block_width, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width, initY-obstacle_block_width, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*2, initY-obstacle_block_width, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*3, initY-obstacle_block_width, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*4, initY-obstacle_block_width, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*5, initY-obstacle_block_width, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*6, initY-obstacle_block_width, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*7, initY-obstacle_block_width, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*8, initY-obstacle_block_width, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*9, initY-obstacle_block_width, {0, 0, 0});

    draw_quads_obstacle(initX-obstacle_block_width*2, initY-obstacle_block_width*2, {0, 0, 0});
    draw_quads_obstacle(initX-obstacle_block_width, initY-obstacle_block_width*2, {102, 102, 102});
    draw_quads_obstacle(initX, initY-obstacle_block_width*2, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width, initY-obstacle_block_width*2, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*2, initY-obstacle_block_width*2, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*3, initY-obstacle_block_width*2, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*4, initY-obstacle_block_width*2, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*5, initY-obstacle_block_width*2, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*6, initY-obstacle_block_width*2, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*7, initY-obstacle_block_width*2, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*8, initY-obstacle_block_width*2, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*9, initY-obstacle_block_width*2, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*10, initY-obstacle_block_width*2, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*11, initY-obstacle_block_width*2, {0, 0, 0});

    draw_quads_obstacle(initX-obstacle_block_width*3, initY-obstacle_block_width*3, {0, 0, 0});
    draw_quads_obstacle(initX-obstacle_block_width*2, initY-obstacle_block_width*3, {102, 102, 102});
    draw_quads_obstacle(initX-obstacle_block_width, initY-obstacle_block_width*3, {102, 102, 102});
    draw_quads_obstacle(initX, initY-obstacle_block_width*3, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width, initY-obstacle_block_width*3, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*2, initY-obstacle_block_width*3, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*3, initY-obstacle_block_width*3, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*4, initY-obstacle_block_width*3, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*5, initY-obstacle_block_width*3, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*6, initY-obstacle_block_width*3, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*7, initY-obstacle_block_width*3, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*8, initY-obstacle_block_width*3, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*9, initY-obstacle_block_width*3, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*10, initY-obstacle_block_width*3, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*11, initY-obstacle_block_width*3, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*12, initY-obstacle_block_width*3, {0, 0, 0});

    draw_quads_obstacle(initX-obstacle_block_width*4, initY-obstacle_block_width*4, {0, 0, 0});
    draw_quads_obstacle(initX-obstacle_block_width*3, initY-obstacle_block_width*4, {102, 102, 102});
    draw_quads_obstacle(initX-obstacle_block_width*2, initY-obstacle_block_width*4, {102, 102, 102});
    draw_quads_obstacle(initX-obstacle_block_width, initY-obstacle_block_width*4, {102, 102, 102});
    draw_quads_obstacle(initX, initY-obstacle_block_width*4, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width, initY-obstacle_block_width*4, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*2, initY-obstacle_block_width*4, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*3, initY-obstacle_block_width*4, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*4, initY-obstacle_block_width*4, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*5, initY-obstacle_block_width*4, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*6, initY-obstacle_block_width*4, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*7, initY-obstacle_block_width*4, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*8, initY-obstacle_block_width*4, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*9, initY-obstacle_block_width*4, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*10, initY-obstacle_block_width*4, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*11, initY-obstacle_block_width*4, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*12, initY-obstacle_block_width*4, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*13, initY-obstacle_block_width*4, {102, 102, 102});

    draw_quads_obstacle(initX-obstacle_block_width*5, initY-obstacle_block_width*5, {0, 0, 0});
    draw_quads_obstacle(initX-obstacle_block_width*4, initY-obstacle_block_width*5, {102, 102, 102});
    draw_quads_obstacle(initX-obstacle_block_width*3, initY-obstacle_block_width*5, {102, 102, 102});
    draw_quads_obstacle(initX-obstacle_block_width*2, initY-obstacle_block_width*5, {102, 102, 102});
    draw_quads_obstacle(initX-obstacle_block_width, initY-obstacle_block_width*5, {102, 102, 102});
    draw_quads_obstacle(initX, initY-obstacle_block_width*5, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width, initY-obstacle_block_width*5, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*2, initY-obstacle_block_width*5, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*3, initY-obstacle_block_width*5, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*4, initY-obstacle_block_width*5, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*5, initY-obstacle_block_width*5, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*6, initY-obstacle_block_width*5, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*7, initY-obstacle_block_width*5, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*8, initY-obstacle_block_width*5, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*9, initY-obstacle_block_width*5, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*10, initY-obstacle_block_width*5, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*11, initY-obstacle_block_width*5, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*12, initY-obstacle_block_width*5, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*13, initY-obstacle_block_width*5, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*14, initY-obstacle_block_width*5, {102, 102, 102});

    draw_quads_obstacle(initX-obstacle_block_width*5, initY-obstacle_block_width*6, {0, 0, 0});
    draw_quads_obstacle(initX-obstacle_block_width*4, initY-obstacle_block_width*6, {102, 102, 102});
    draw_quads_obstacle(initX-obstacle_block_width*3, initY-obstacle_block_width*6, {102, 102, 102});
    draw_quads_obstacle(initX-obstacle_block_width*2, initY-obstacle_block_width*6, {102, 102, 102});
    draw_quads_obstacle(initX-obstacle_block_width, initY-obstacle_block_width*6, {102, 102, 102});
    draw_quads_obstacle(initX, initY-obstacle_block_width*6, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width, initY-obstacle_block_width*6, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*2, initY-obstacle_block_width*6, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*3, initY-obstacle_block_width*6, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*4, initY-obstacle_block_width*6, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*5, initY-obstacle_block_width*6, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*6, initY-obstacle_block_width*6, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*7, initY-obstacle_block_width*6, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*8, initY-obstacle_block_width*6, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*9, initY-obstacle_block_width*6, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*10, initY-obstacle_block_width*6, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*11, initY-obstacle_block_width*6, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*12, initY-obstacle_block_width*6, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*13, initY-obstacle_block_width*6, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*14, initY-obstacle_block_width*6, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*15, initY-obstacle_block_width*6, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*16, initY-obstacle_block_width*6, {102, 102, 102});

    draw_quads_obstacle(initX-obstacle_block_width*5, initY-obstacle_block_width*7, {0, 0, 0});
    draw_quads_obstacle(initX-obstacle_block_width*4, initY-obstacle_block_width*7, {102, 102, 102});
    draw_quads_obstacle(initX-obstacle_block_width*3, initY-obstacle_block_width*7, {102, 102, 102});
    draw_quads_obstacle(initX-obstacle_block_width*2, initY-obstacle_block_width*7, {102, 102, 102});
    draw_quads_obstacle(initX-obstacle_block_width, initY-obstacle_block_width*7, {102, 102, 102});
    draw_quads_obstacle(initX, initY-obstacle_block_width*7, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width, initY-obstacle_block_width*7, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*2, initY-obstacle_block_width*7, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*3, initY-obstacle_block_width*7, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*4, initY-obstacle_block_width*7, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*5, initY-obstacle_block_width*7, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*6, initY-obstacle_block_width*7, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*7, initY-obstacle_block_width*7, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*8, initY-obstacle_block_width*7, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*9, initY-obstacle_block_width*7, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*10, initY-obstacle_block_width*7, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*11, initY-obstacle_block_width*7, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*12, initY-obstacle_block_width*7, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*13, initY-obstacle_block_width*7, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*14, initY-obstacle_block_width*7, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*15, initY-obstacle_block_width*7, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*16, initY-obstacle_block_width*7, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*17, initY-obstacle_block_width*7, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*18, initY-obstacle_block_width*7, {102, 102, 102});

    draw_quads_obstacle(initX-obstacle_block_width*5, initY-obstacle_block_width*8, {0, 0, 0});
    draw_quads_obstacle(initX-obstacle_block_width*4, initY-obstacle_block_width*8, {102, 102, 102});
    draw_quads_obstacle(initX-obstacle_block_width*3, initY-obstacle_block_width*8, {102, 102, 102});
    draw_quads_obstacle(initX-obstacle_block_width*2, initY-obstacle_block_width*8, {102, 102, 102});
    draw_quads_obstacle(initX-obstacle_block_width, initY-obstacle_block_width*8, {102, 102, 102});
    draw_quads_obstacle(initX, initY-obstacle_block_width*8, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width, initY-obstacle_block_width*8, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*2, initY-obstacle_block_width*8, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*3, initY-obstacle_block_width*8, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*4, initY-obstacle_block_width*8, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*5, initY-obstacle_block_width*8, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*6, initY-obstacle_block_width*8, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*7, initY-obstacle_block_width*8, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*8, initY-obstacle_block_width*8, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*9, initY-obstacle_block_width*8, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*10, initY-obstacle_block_width*8, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*11, initY-obstacle_block_width*8, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*12, initY-obstacle_block_width*8, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*13, initY-obstacle_block_width*8, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*14, initY-obstacle_block_width*8, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*15, initY-obstacle_block_width*8, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*16, initY-obstacle_block_width*8, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*17, initY-obstacle_block_width*8, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*18, initY-obstacle_block_width*8, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*19, initY-obstacle_block_width*8, {102, 102, 102});

    draw_quads_obstacle(initX-obstacle_block_width*5, initY-obstacle_block_width*9, {0, 87, 132});
    draw_quads_obstacle(initX-obstacle_block_width*4, initY-obstacle_block_width*9, {102, 102, 102});
    draw_quads_obstacle(initX-obstacle_block_width*3, initY-obstacle_block_width*9, {102, 102, 102});
    draw_quads_obstacle(initX-obstacle_block_width*2, initY-obstacle_block_width*9, {102, 102, 102});
    draw_quads_obstacle(initX-obstacle_block_width, initY-obstacle_block_width*9, {102, 102, 102});
    draw_quads_obstacle(initX, initY-obstacle_block_width*9, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width, initY-obstacle_block_width*9, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*2, initY-obstacle_block_width*9, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*3, initY-obstacle_block_width*9, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*4, initY-obstacle_block_width*9, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*5, initY-obstacle_block_width*9, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*6, initY-obstacle_block_width*9, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*7, initY-obstacle_block_width*9, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*8, initY-obstacle_block_width*9, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*9, initY-obstacle_block_width*9, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*10, initY-obstacle_block_width*9, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*11, initY-obstacle_block_width*9, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*12, initY-obstacle_block_width*9, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*13, initY-obstacle_block_width*9, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*14, initY-obstacle_block_width*9, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*15, initY-obstacle_block_width*9, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*16, initY-obstacle_block_width*9, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*17, initY-obstacle_block_width*9, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*18, initY-obstacle_block_width*9, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*19, initY-obstacle_block_width*9, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*20, initY-obstacle_block_width*9, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*21, initY-obstacle_block_width*9, {102, 102, 102});

    draw_quads_obstacle(initX-obstacle_block_width*5, initY-obstacle_block_width*10, {0, 87, 132});
    draw_quads_obstacle(initX-obstacle_block_width*4, initY-obstacle_block_width*10, {102, 102, 102});
    draw_quads_obstacle(initX-obstacle_block_width*3, initY-obstacle_block_width*10, {102, 102, 102});
    draw_quads_obstacle(initX-obstacle_block_width*2, initY-obstacle_block_width*10, {102, 102, 102});
    draw_quads_obstacle(initX-obstacle_block_width, initY-obstacle_block_width*10, {102, 102, 102});
    draw_quads_obstacle(initX, initY-obstacle_block_width*10, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width, initY-obstacle_block_width*10, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*2, initY-obstacle_block_width*10, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*3, initY-obstacle_block_width*10, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*4, initY-obstacle_block_width*10, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*5, initY-obstacle_block_width*10, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*6, initY-obstacle_block_width*10, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*7, initY-obstacle_block_width*10, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*8, initY-obstacle_block_width*10, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*9, initY-obstacle_block_width*10, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*10, initY-obstacle_block_width*10, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*11, initY-obstacle_block_width*10, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*12, initY-obstacle_block_width*10, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*13, initY-obstacle_block_width*10, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*14, initY-obstacle_block_width*10, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*15, initY-obstacle_block_width*10, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*16, initY-obstacle_block_width*10, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*17, initY-obstacle_block_width*10, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*18, initY-obstacle_block_width*10, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*19, initY-obstacle_block_width*10, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*20, initY-obstacle_block_width*10, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*21, initY-obstacle_block_width*10, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*22, initY-obstacle_block_width*10, {163, 163, 163});

    draw_quads_obstacle(initX-obstacle_block_width*6, initY-obstacle_block_width*11, {0, 87, 132});
    draw_quads_obstacle(initX-obstacle_block_width*5, initY-obstacle_block_width*11, {0, 87, 132});
    draw_quads_obstacle(initX-obstacle_block_width*4, initY-obstacle_block_width*11, {178, 220, 239});
    draw_quads_obstacle(initX-obstacle_block_width*3, initY-obstacle_block_width*11, {102, 102, 102});
    draw_quads_obstacle(initX-obstacle_block_width*2, initY-obstacle_block_width*11, {102, 102, 102});
    draw_quads_obstacle(initX-obstacle_block_width, initY-obstacle_block_width*11, {102, 102, 102});
    draw_quads_obstacle(initX, initY-obstacle_block_width*11, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width, initY-obstacle_block_width*11, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*2, initY-obstacle_block_width*11, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*3, initY-obstacle_block_width*11, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*4, initY-obstacle_block_width*11, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*5, initY-obstacle_block_width*11, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*6, initY-obstacle_block_width*11, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*7, initY-obstacle_block_width*11, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*8, initY-obstacle_block_width*11, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*9, initY-obstacle_block_width*11, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*10, initY-obstacle_block_width*11, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*11, initY-obstacle_block_width*11, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*12, initY-obstacle_block_width*11, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*13, initY-obstacle_block_width*11, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*14, initY-obstacle_block_width*11, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*15, initY-obstacle_block_width*11, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*16, initY-obstacle_block_width*11, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*17, initY-obstacle_block_width*11, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*18, initY-obstacle_block_width*11, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*19, initY-obstacle_block_width*11, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*20, initY-obstacle_block_width*11, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*21, initY-obstacle_block_width*11, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*22, initY-obstacle_block_width*11, {163, 163, 163});

    draw_quads_obstacle(initX-obstacle_block_width*6, initY-obstacle_block_width*12, {0, 87, 132});
    draw_quads_obstacle(initX-obstacle_block_width*5, initY-obstacle_block_width*12, {178, 220, 239});
    draw_quads_obstacle(initX-obstacle_block_width*4, initY-obstacle_block_width*12, {178, 220, 239});
    draw_quads_obstacle(initX-obstacle_block_width*3, initY-obstacle_block_width*12, {178, 220, 239});
    draw_quads_obstacle(initX-obstacle_block_width*2, initY-obstacle_block_width*12, {102, 102, 102});
    draw_quads_obstacle(initX-obstacle_block_width, initY-obstacle_block_width*12, {102, 102, 102});
    draw_quads_obstacle(initX, initY-obstacle_block_width*12, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width, initY-obstacle_block_width*12, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*2, initY-obstacle_block_width*12, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*3, initY-obstacle_block_width*12, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*4, initY-obstacle_block_width*12, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*5, initY-obstacle_block_width*12, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*6, initY-obstacle_block_width*12, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*7, initY-obstacle_block_width*12, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*8, initY-obstacle_block_width*12, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*9, initY-obstacle_block_width*12, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*10, initY-obstacle_block_width*12, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*11, initY-obstacle_block_width*12, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*12, initY-obstacle_block_width*12, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*13, initY-obstacle_block_width*12, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*14, initY-obstacle_block_width*12, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*15, initY-obstacle_block_width*12, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*16, initY-obstacle_block_width*12, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*17, initY-obstacle_block_width*12, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*18, initY-obstacle_block_width*12, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*19, initY-obstacle_block_width*12, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*20, initY-obstacle_block_width*12, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*21, initY-obstacle_block_width*12, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*22, initY-obstacle_block_width*12, {163, 163, 163});

    draw_quads_obstacle(initX-obstacle_block_width*7, initY-obstacle_block_width*13, {0, 0, 0});
    draw_quads_obstacle(initX-obstacle_block_width*6, initY-obstacle_block_width*13, {0, 0, 0});
    draw_quads_obstacle(initX-obstacle_block_width*5, initY-obstacle_block_width*13, {0, 87, 132});
    draw_quads_obstacle(initX-obstacle_block_width*4, initY-obstacle_block_width*13, {0, 87, 132});
    draw_quads_obstacle(initX-obstacle_block_width*3, initY-obstacle_block_width*13, {178, 220, 239});
    draw_quads_obstacle(initX-obstacle_block_width*2, initY-obstacle_block_width*13, {178, 220, 239});
    draw_quads_obstacle(initX-obstacle_block_width, initY-obstacle_block_width*13, {51, 51, 51});
    draw_quads_obstacle(initX, initY-obstacle_block_width*13, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width, initY-obstacle_block_width*13, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*2, initY-obstacle_block_width*13, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*3, initY-obstacle_block_width*13, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*4, initY-obstacle_block_width*13, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*5, initY-obstacle_block_width*13, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*6, initY-obstacle_block_width*13, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*7, initY-obstacle_block_width*13, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*8, initY-obstacle_block_width*13, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*9, initY-obstacle_block_width*13, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*10, initY-obstacle_block_width*13, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*11, initY-obstacle_block_width*13, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*12, initY-obstacle_block_width*13, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*13, initY-obstacle_block_width*13, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*14, initY-obstacle_block_width*13, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*15, initY-obstacle_block_width*13, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*16, initY-obstacle_block_width*13, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*17, initY-obstacle_block_width*13, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*18, initY-obstacle_block_width*13, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*19, initY-obstacle_block_width*13, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*20, initY-obstacle_block_width*13, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*21, initY-obstacle_block_width*13, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*22, initY-obstacle_block_width*13, {163, 163, 163});

    draw_quads_obstacle(initX-obstacle_block_width*7, initY-obstacle_block_width*14, {0, 0, 0});
    draw_quads_obstacle(initX-obstacle_block_width*6, initY-obstacle_block_width*14, {0, 0, 0});
    draw_quads_obstacle(initX-obstacle_block_width*5, initY-obstacle_block_width*14, {102, 102, 102});
    draw_quads_obstacle(initX-obstacle_block_width*4, initY-obstacle_block_width*14, {0, 0, 0});
    draw_quads_obstacle(initX-obstacle_block_width*3, initY-obstacle_block_width*14, {0, 0, 0});
    draw_quads_obstacle(initX-obstacle_block_width*2, initY-obstacle_block_width*14, {0, 87, 132});
    draw_quads_obstacle(initX-obstacle_block_width, initY-obstacle_block_width*14, {178, 220, 239});
    draw_quads_obstacle(initX, initY-obstacle_block_width*14, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width, initY-obstacle_block_width*14, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*2, initY-obstacle_block_width*14, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*3, initY-obstacle_block_width*14, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*4, initY-obstacle_block_width*14, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*5, initY-obstacle_block_width*14, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*6, initY-obstacle_block_width*14, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*7, initY-obstacle_block_width*14, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*8, initY-obstacle_block_width*14, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*9, initY-obstacle_block_width*14, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*10, initY-obstacle_block_width*14, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*11, initY-obstacle_block_width*14, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*12, initY-obstacle_block_width*14, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*13, initY-obstacle_block_width*14, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*14, initY-obstacle_block_width*14, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*15, initY-obstacle_block_width*14, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*16, initY-obstacle_block_width*14, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*17, initY-obstacle_block_width*14, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*18, initY-obstacle_block_width*14, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*19, initY-obstacle_block_width*14, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*20, initY-obstacle_block_width*14, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*21, initY-obstacle_block_width*14, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*22, initY-obstacle_block_width*14, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*23, initY-obstacle_block_width*14, {0, 0, 0});

    draw_quads_obstacle(initX-obstacle_block_width*7, initY-obstacle_block_width*15, {0, 0, 0});
    draw_quads_obstacle(initX-obstacle_block_width*6, initY-obstacle_block_width*15, {0, 0, 0});
    draw_quads_obstacle(initX-obstacle_block_width*5, initY-obstacle_block_width*15, {102, 102, 102});
    draw_quads_obstacle(initX-obstacle_block_width*4, initY-obstacle_block_width*15, {102, 102, 102});
    draw_quads_obstacle(initX-obstacle_block_width*3, initY-obstacle_block_width*15, {102, 102, 102});
    draw_quads_obstacle(initX-obstacle_block_width*2, initY-obstacle_block_width*15, {0, 0, 0});
    draw_quads_obstacle(initX-obstacle_block_width, initY-obstacle_block_width*15, {0, 0, 0});
    draw_quads_obstacle(initX, initY-obstacle_block_width*15, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width, initY-obstacle_block_width*15, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*2, initY-obstacle_block_width*15, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*3, initY-obstacle_block_width*15, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*4, initY-obstacle_block_width*15, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*5, initY-obstacle_block_width*15, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*6, initY-obstacle_block_width*15, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*7, initY-obstacle_block_width*15, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*8, initY-obstacle_block_width*15, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*9, initY-obstacle_block_width*15, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*10, initY-obstacle_block_width*15, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*11, initY-obstacle_block_width*15, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*12, initY-obstacle_block_width*15, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*13, initY-obstacle_block_width*15, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*14, initY-obstacle_block_width*15, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*15, initY-obstacle_block_width*15, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*16, initY-obstacle_block_width*15, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*17, initY-obstacle_block_width*15, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*18, initY-obstacle_block_width*15, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*19, initY-obstacle_block_width*15, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*20, initY-obstacle_block_width*15, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*21, initY-obstacle_block_width*15, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*22, initY-obstacle_block_width*15, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*23, initY-obstacle_block_width*15, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*24, initY-obstacle_block_width*15, {102, 102, 102});

    draw_quads_obstacle(initX-obstacle_block_width*7, initY-obstacle_block_width*16, {0, 0, 0});
    draw_quads_obstacle(initX-obstacle_block_width*6, initY-obstacle_block_width*16, {0, 0, 0});
    draw_quads_obstacle(initX-obstacle_block_width*5, initY-obstacle_block_width*16, {102, 102, 102});
    draw_quads_obstacle(initX-obstacle_block_width*4, initY-obstacle_block_width*16, {102, 102, 102});
    draw_quads_obstacle(initX-obstacle_block_width*3, initY-obstacle_block_width*16, {102, 102, 102});
    draw_quads_obstacle(initX-obstacle_block_width*2, initY-obstacle_block_width*16, {102, 102, 102});
    draw_quads_obstacle(initX-obstacle_block_width, initY-obstacle_block_width*16, {0, 0, 0});
    draw_quads_obstacle(initX, initY-obstacle_block_width*16, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width, initY-obstacle_block_width*16, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*2, initY-obstacle_block_width*16, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*3, initY-obstacle_block_width*16, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*4, initY-obstacle_block_width*16, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*5, initY-obstacle_block_width*16, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*6, initY-obstacle_block_width*16, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*7, initY-obstacle_block_width*16, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*8, initY-obstacle_block_width*16, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*9, initY-obstacle_block_width*16, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*10, initY-obstacle_block_width*16, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*11, initY-obstacle_block_width*16, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*12, initY-obstacle_block_width*16, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*13, initY-obstacle_block_width*16, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*14, initY-obstacle_block_width*16, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*15, initY-obstacle_block_width*16, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*16, initY-obstacle_block_width*16, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*17, initY-obstacle_block_width*16, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*18, initY-obstacle_block_width*16, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*19, initY-obstacle_block_width*16, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*20, initY-obstacle_block_width*16, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*21, initY-obstacle_block_width*16, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*22, initY-obstacle_block_width*16, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*23, initY-obstacle_block_width*16, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*24, initY-obstacle_block_width*16, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*25, initY-obstacle_block_width*16, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*26, initY-obstacle_block_width*16, {102, 102, 102});

    draw_quads_obstacle(initX-obstacle_block_width*7, initY-obstacle_block_width*17, {0, 0, 0});
    draw_quads_obstacle(initX-obstacle_block_width*6, initY-obstacle_block_width*17, {0, 0, 0});
    draw_quads_obstacle(initX-obstacle_block_width*5, initY-obstacle_block_width*17, {51, 51, 51});
    draw_quads_obstacle(initX-obstacle_block_width*4, initY-obstacle_block_width*17, {163, 163, 163});
    draw_quads_obstacle(initX-obstacle_block_width*3, initY-obstacle_block_width*17, {0, 0, 0});
    draw_quads_obstacle(initX-obstacle_block_width*2, initY-obstacle_block_width*17, {51, 51, 51});
    draw_quads_obstacle(initX-obstacle_block_width, initY-obstacle_block_width*17, {102, 102, 102});
    draw_quads_obstacle(initX, initY-obstacle_block_width*17, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width, initY-obstacle_block_width*17, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*2, initY-obstacle_block_width*17, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*3, initY-obstacle_block_width*17, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*4, initY-obstacle_block_width*17, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*5, initY-obstacle_block_width*17, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*6, initY-obstacle_block_width*17, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*7, initY-obstacle_block_width*17, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*8, initY-obstacle_block_width*17, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*9, initY-obstacle_block_width*17, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*10, initY-obstacle_block_width*17, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*11, initY-obstacle_block_width*17, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*12, initY-obstacle_block_width*17, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*13, initY-obstacle_block_width*17, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*14, initY-obstacle_block_width*17, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*15, initY-obstacle_block_width*17, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*16, initY-obstacle_block_width*17, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*17, initY-obstacle_block_width*17, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*18, initY-obstacle_block_width*17, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*19, initY-obstacle_block_width*17, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*20, initY-obstacle_block_width*17, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*21, initY-obstacle_block_width*17, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*22, initY-obstacle_block_width*17, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*23, initY-obstacle_block_width*17, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*24, initY-obstacle_block_width*17, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*25, initY-obstacle_block_width*17, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*26, initY-obstacle_block_width*17, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*27, initY-obstacle_block_width*17, {102, 102, 102});

    draw_quads_obstacle(initX-obstacle_block_width*7, initY-obstacle_block_width*18, {0, 0, 0});
    draw_quads_obstacle(initX-obstacle_block_width*6, initY-obstacle_block_width*18, {0, 87, 132});
    draw_quads_obstacle(initX-obstacle_block_width*5, initY-obstacle_block_width*18, {51, 51, 51});
    draw_quads_obstacle(initX-obstacle_block_width*4, initY-obstacle_block_width*18, {163, 163, 163});
    draw_quads_obstacle(initX-obstacle_block_width*3, initY-obstacle_block_width*18, {0, 0, 0});
    draw_quads_obstacle(initX-obstacle_block_width*2, initY-obstacle_block_width*18, {0, 0, 0});
    draw_quads_obstacle(initX-obstacle_block_width, initY-obstacle_block_width*18, {102, 102, 102});
    draw_quads_obstacle(initX, initY-obstacle_block_width*18, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width, initY-obstacle_block_width*18, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*2, initY-obstacle_block_width*18, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*3, initY-obstacle_block_width*18, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*4, initY-obstacle_block_width*18, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*5, initY-obstacle_block_width*18, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*6, initY-obstacle_block_width*18, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*7, initY-obstacle_block_width*18, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*8, initY-obstacle_block_width*18, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*9, initY-obstacle_block_width*18, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*10, initY-obstacle_block_width*18, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*11, initY-obstacle_block_width*18, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*12, initY-obstacle_block_width*18, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*13, initY-obstacle_block_width*18, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*14, initY-obstacle_block_width*18, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*15, initY-obstacle_block_width*18, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*16, initY-obstacle_block_width*18, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*17, initY-obstacle_block_width*18, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*18, initY-obstacle_block_width*18, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*19, initY-obstacle_block_width*18, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*20, initY-obstacle_block_width*18, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*21, initY-obstacle_block_width*18, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*22, initY-obstacle_block_width*18, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*23, initY-obstacle_block_width*18, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*24, initY-obstacle_block_width*18, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*25, initY-obstacle_block_width*18, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*26, initY-obstacle_block_width*18, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*27, initY-obstacle_block_width*18, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*28, initY-obstacle_block_width*18, {102, 102, 102});

    draw_quads_obstacle(initX-obstacle_block_width*7, initY-obstacle_block_width*19, {0, 0, 0});
    draw_quads_obstacle(initX-obstacle_block_width*6, initY-obstacle_block_width*19, {0, 87, 132});
    draw_quads_obstacle(initX-obstacle_block_width*5, initY-obstacle_block_width*19, {51, 51, 51});
    draw_quads_obstacle(initX-obstacle_block_width*4, initY-obstacle_block_width*19, {102, 102, 102});
    draw_quads_obstacle(initX-obstacle_block_width*3, initY-obstacle_block_width*19, {163, 163, 163});
    draw_quads_obstacle(initX-obstacle_block_width*2, initY-obstacle_block_width*19, {163, 163, 163});
    draw_quads_obstacle(initX-obstacle_block_width, initY-obstacle_block_width*19, {102, 102, 102});
    draw_quads_obstacle(initX, initY-obstacle_block_width*19, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width, initY-obstacle_block_width*19, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*2, initY-obstacle_block_width*19, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*3, initY-obstacle_block_width*19, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*4, initY-obstacle_block_width*19, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*5, initY-obstacle_block_width*19, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*6, initY-obstacle_block_width*19, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*7, initY-obstacle_block_width*19, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*8, initY-obstacle_block_width*19, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*9, initY-obstacle_block_width*19, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*10, initY-obstacle_block_width*19, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*11, initY-obstacle_block_width*19, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*12, initY-obstacle_block_width*19, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*13, initY-obstacle_block_width*19, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*14, initY-obstacle_block_width*19, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*15, initY-obstacle_block_width*19, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*16, initY-obstacle_block_width*19, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*17, initY-obstacle_block_width*19, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*18, initY-obstacle_block_width*19, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*19, initY-obstacle_block_width*19, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*20, initY-obstacle_block_width*19, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*21, initY-obstacle_block_width*19, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*22, initY-obstacle_block_width*19, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*23, initY-obstacle_block_width*19, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*24, initY-obstacle_block_width*19, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*25, initY-obstacle_block_width*19, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*26, initY-obstacle_block_width*19, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*27, initY-obstacle_block_width*19, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*28, initY-obstacle_block_width*19, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*29, initY-obstacle_block_width*19, {163, 163, 163});

    draw_quads_obstacle(initX-obstacle_block_width*7, initY-obstacle_block_width*20, {0, 87, 132});
    draw_quads_obstacle(initX-obstacle_block_width*6, initY-obstacle_block_width*20, {0, 87, 132});
    draw_quads_obstacle(initX-obstacle_block_width*5, initY-obstacle_block_width*20, {51, 51, 51});
    draw_quads_obstacle(initX-obstacle_block_width*4, initY-obstacle_block_width*20, {51, 51, 51});
    draw_quads_obstacle(initX-obstacle_block_width*3, initY-obstacle_block_width*20, {102, 102, 102});
    draw_quads_obstacle(initX-obstacle_block_width*2, initY-obstacle_block_width*20, {102, 102, 102});
    draw_quads_obstacle(initX-obstacle_block_width, initY-obstacle_block_width*20, {102, 102, 102});
    draw_quads_obstacle(initX, initY-obstacle_block_width*20, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width, initY-obstacle_block_width*20, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*2, initY-obstacle_block_width*20, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*3, initY-obstacle_block_width*20, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*4, initY-obstacle_block_width*20, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*5, initY-obstacle_block_width*20, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*6, initY-obstacle_block_width*20, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*7, initY-obstacle_block_width*20, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*8, initY-obstacle_block_width*20, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*9, initY-obstacle_block_width*20, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*10, initY-obstacle_block_width*20, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*11, initY-obstacle_block_width*20, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*12, initY-obstacle_block_width*20, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*13, initY-obstacle_block_width*20, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*14, initY-obstacle_block_width*20, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*15, initY-obstacle_block_width*20, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*16, initY-obstacle_block_width*20, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*17, initY-obstacle_block_width*20, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*18, initY-obstacle_block_width*20, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*19, initY-obstacle_block_width*20, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*20, initY-obstacle_block_width*20, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*21, initY-obstacle_block_width*20, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*22, initY-obstacle_block_width*20, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*23, initY-obstacle_block_width*20, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*24, initY-obstacle_block_width*20, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*25, initY-obstacle_block_width*20, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*26, initY-obstacle_block_width*20, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*27, initY-obstacle_block_width*20, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*28, initY-obstacle_block_width*20, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*29, initY-obstacle_block_width*20, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*30, initY-obstacle_block_width*20, {163, 163, 163});

    draw_quads_obstacle(initX-obstacle_block_width*7, initY-obstacle_block_width*21, {0, 87, 132});
    draw_quads_obstacle(initX-obstacle_block_width*6, initY-obstacle_block_width*21, {178, 220, 239});
    draw_quads_obstacle(initX-obstacle_block_width*5, initY-obstacle_block_width*21, {0, 87, 132});
    draw_quads_obstacle(initX-obstacle_block_width*4, initY-obstacle_block_width*21, {51, 51, 51});
    draw_quads_obstacle(initX-obstacle_block_width*3, initY-obstacle_block_width*21, {51, 51, 51});
    draw_quads_obstacle(initX-obstacle_block_width*2, initY-obstacle_block_width*21, {51, 51, 51});
    draw_quads_obstacle(initX-obstacle_block_width, initY-obstacle_block_width*21, {163, 163, 163});
    draw_quads_obstacle(initX, initY-obstacle_block_width*21, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width, initY-obstacle_block_width*21, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*2, initY-obstacle_block_width*21, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*3, initY-obstacle_block_width*21, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*4, initY-obstacle_block_width*21, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*5, initY-obstacle_block_width*21, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*6, initY-obstacle_block_width*21, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*7, initY-obstacle_block_width*21, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*8, initY-obstacle_block_width*21, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*9, initY-obstacle_block_width*21, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*10, initY-obstacle_block_width*21, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*11, initY-obstacle_block_width*21, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*12, initY-obstacle_block_width*21, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*13, initY-obstacle_block_width*21, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*14, initY-obstacle_block_width*21, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*15, initY-obstacle_block_width*21, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*16, initY-obstacle_block_width*21, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*17, initY-obstacle_block_width*21, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*18, initY-obstacle_block_width*21, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*19, initY-obstacle_block_width*21, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*20, initY-obstacle_block_width*21, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*21, initY-obstacle_block_width*21, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*22, initY-obstacle_block_width*21, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*23, initY-obstacle_block_width*21, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*24, initY-obstacle_block_width*21, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*25, initY-obstacle_block_width*21, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*26, initY-obstacle_block_width*21, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*27, initY-obstacle_block_width*21, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*28, initY-obstacle_block_width*21, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*29, initY-obstacle_block_width*21, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*30, initY-obstacle_block_width*21, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*31, initY-obstacle_block_width*21, {163, 163, 163});

    draw_quads_obstacle(initX-obstacle_block_width*7, initY-obstacle_block_width*22, {0, 87, 132});
    draw_quads_obstacle(initX-obstacle_block_width*6, initY-obstacle_block_width*22, {0, 87, 132});
    draw_quads_obstacle(initX-obstacle_block_width*5, initY-obstacle_block_width*22, {178, 220, 239});
    draw_quads_obstacle(initX-obstacle_block_width*4, initY-obstacle_block_width*22, {51, 51, 51});
    draw_quads_obstacle(initX-obstacle_block_width*3, initY-obstacle_block_width*22, {51, 51, 51});
    draw_quads_obstacle(initX-obstacle_block_width*2, initY-obstacle_block_width*22, {0, 0, 0});
    draw_quads_obstacle(initX-obstacle_block_width, initY-obstacle_block_width*22, {51, 51, 51});
    draw_quads_obstacle(initX, initY-obstacle_block_width*22, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width, initY-obstacle_block_width*22, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*2, initY-obstacle_block_width*22, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*3, initY-obstacle_block_width*22, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*4, initY-obstacle_block_width*22, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*5, initY-obstacle_block_width*22, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*6, initY-obstacle_block_width*22, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*7, initY-obstacle_block_width*22, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*8, initY-obstacle_block_width*22, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*9, initY-obstacle_block_width*22, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*10, initY-obstacle_block_width*22, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*11, initY-obstacle_block_width*22, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*12, initY-obstacle_block_width*22, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*13, initY-obstacle_block_width*22, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*14, initY-obstacle_block_width*22, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*15, initY-obstacle_block_width*22, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*16, initY-obstacle_block_width*22, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*17, initY-obstacle_block_width*22, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*18, initY-obstacle_block_width*22, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*19, initY-obstacle_block_width*22, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*20, initY-obstacle_block_width*22, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*21, initY-obstacle_block_width*22, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*22, initY-obstacle_block_width*22, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*23, initY-obstacle_block_width*22, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*24, initY-obstacle_block_width*22, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*25, initY-obstacle_block_width*22, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*26, initY-obstacle_block_width*22, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*27, initY-obstacle_block_width*22, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*28, initY-obstacle_block_width*22, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*29, initY-obstacle_block_width*22, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*30, initY-obstacle_block_width*22, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*31, initY-obstacle_block_width*22, {163, 163, 163});

    draw_quads_obstacle(initX-obstacle_block_width*6, initY-obstacle_block_width*23, {0, 87, 132});
    draw_quads_obstacle(initX-obstacle_block_width*5, initY-obstacle_block_width*23, {178, 220, 239});
    draw_quads_obstacle(initX-obstacle_block_width*4, initY-obstacle_block_width*23, {178, 220, 239});
    draw_quads_obstacle(initX-obstacle_block_width*3, initY-obstacle_block_width*23, {51, 51, 51});
    draw_quads_obstacle(initX-obstacle_block_width*2, initY-obstacle_block_width*23, {51, 51, 51});
    draw_quads_obstacle(initX-obstacle_block_width, initY-obstacle_block_width*23, {51, 51, 51});
    draw_quads_obstacle(initX, initY-obstacle_block_width*23, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width, initY-obstacle_block_width*23, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*2, initY-obstacle_block_width*23, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*3, initY-obstacle_block_width*23, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*4, initY-obstacle_block_width*23, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*5, initY-obstacle_block_width*23, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*6, initY-obstacle_block_width*23, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*7, initY-obstacle_block_width*23, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*8, initY-obstacle_block_width*23, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*9, initY-obstacle_block_width*23, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*10, initY-obstacle_block_width*23, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*11, initY-obstacle_block_width*23, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*12, initY-obstacle_block_width*23, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*13, initY-obstacle_block_width*23, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*14, initY-obstacle_block_width*23, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*15, initY-obstacle_block_width*23, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*16, initY-obstacle_block_width*23, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*17, initY-obstacle_block_width*23, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*18, initY-obstacle_block_width*23, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*19, initY-obstacle_block_width*23, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*20, initY-obstacle_block_width*23, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*21, initY-obstacle_block_width*23, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*22, initY-obstacle_block_width*23, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*23, initY-obstacle_block_width*23, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*24, initY-obstacle_block_width*23, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*25, initY-obstacle_block_width*23, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*26, initY-obstacle_block_width*23, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*27, initY-obstacle_block_width*23, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*28, initY-obstacle_block_width*23, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*29, initY-obstacle_block_width*23, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*30, initY-obstacle_block_width*23, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*31, initY-obstacle_block_width*23, {163, 163, 163});

    draw_quads_obstacle(initX-obstacle_block_width*6, initY-obstacle_block_width*24, {0, 0, 0});
    draw_quads_obstacle(initX-obstacle_block_width*5, initY-obstacle_block_width*24, {0, 0, 0});
    draw_quads_obstacle(initX-obstacle_block_width*4, initY-obstacle_block_width*24, {178, 220, 239});
    draw_quads_obstacle(initX-obstacle_block_width*3, initY-obstacle_block_width*24, {178, 220, 239});
    draw_quads_obstacle(initX-obstacle_block_width*2, initY-obstacle_block_width*24, {0, 87, 132});
    draw_quads_obstacle(initX-obstacle_block_width, initY-obstacle_block_width*24, {51, 51, 51});
    draw_quads_obstacle(initX, initY-obstacle_block_width*24, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width, initY-obstacle_block_width*24, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*2, initY-obstacle_block_width*24, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*3, initY-obstacle_block_width*24, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*4, initY-obstacle_block_width*24, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*5, initY-obstacle_block_width*24, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*6, initY-obstacle_block_width*24, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*7, initY-obstacle_block_width*24, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*8, initY-obstacle_block_width*24, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*9, initY-obstacle_block_width*24, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*10, initY-obstacle_block_width*24, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*11, initY-obstacle_block_width*24, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*12, initY-obstacle_block_width*24, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*13, initY-obstacle_block_width*24, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*14, initY-obstacle_block_width*24, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*15, initY-obstacle_block_width*24, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*16, initY-obstacle_block_width*24, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*17, initY-obstacle_block_width*24, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*18, initY-obstacle_block_width*24, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*19, initY-obstacle_block_width*24, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*20, initY-obstacle_block_width*24, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*21, initY-obstacle_block_width*24, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*22, initY-obstacle_block_width*24, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*23, initY-obstacle_block_width*24, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*24, initY-obstacle_block_width*24, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*25, initY-obstacle_block_width*24, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*26, initY-obstacle_block_width*24, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*27, initY-obstacle_block_width*24, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*28, initY-obstacle_block_width*24, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*29, initY-obstacle_block_width*24, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*30, initY-obstacle_block_width*24, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*31, initY-obstacle_block_width*24, {163, 163, 163});

    draw_quads_obstacle(initX-obstacle_block_width*6, initY-obstacle_block_width*25, {0, 87, 132});
    draw_quads_obstacle(initX-obstacle_block_width*5, initY-obstacle_block_width*25, {0, 87, 132});
    draw_quads_obstacle(initX-obstacle_block_width*4, initY-obstacle_block_width*25, {0, 0, 0});
    draw_quads_obstacle(initX-obstacle_block_width*3, initY-obstacle_block_width*25, {0, 0, 0});
    draw_quads_obstacle(initX-obstacle_block_width*2, initY-obstacle_block_width*25, {0, 87, 132});
    draw_quads_obstacle(initX-obstacle_block_width, initY-obstacle_block_width*25, {0, 0, 0});
    draw_quads_obstacle(initX, initY-obstacle_block_width*25, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width, initY-obstacle_block_width*25, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*2, initY-obstacle_block_width*25, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*3, initY-obstacle_block_width*25, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*4, initY-obstacle_block_width*25, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*5, initY-obstacle_block_width*25, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*6, initY-obstacle_block_width*25, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*7, initY-obstacle_block_width*25, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*8, initY-obstacle_block_width*25, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*9, initY-obstacle_block_width*25, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*10, initY-obstacle_block_width*25, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*11, initY-obstacle_block_width*25, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*12, initY-obstacle_block_width*25, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*13, initY-obstacle_block_width*25, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*14, initY-obstacle_block_width*25, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*15, initY-obstacle_block_width*25, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*16, initY-obstacle_block_width*25, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*17, initY-obstacle_block_width*25, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*18, initY-obstacle_block_width*25, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*19, initY-obstacle_block_width*25, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*20, initY-obstacle_block_width*25, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*21, initY-obstacle_block_width*25, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*22, initY-obstacle_block_width*25, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*23, initY-obstacle_block_width*25, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*24, initY-obstacle_block_width*25, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*25, initY-obstacle_block_width*25, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*26, initY-obstacle_block_width*25, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*27, initY-obstacle_block_width*25, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*28, initY-obstacle_block_width*25, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*29, initY-obstacle_block_width*25, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*30, initY-obstacle_block_width*25, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*31, initY-obstacle_block_width*25, {102, 102, 102});

    draw_quads_obstacle(initX-obstacle_block_width*6, initY-obstacle_block_width*26, {0, 87, 132});
    draw_quads_obstacle(initX-obstacle_block_width*5, initY-obstacle_block_width*26, {178, 220, 239});
    draw_quads_obstacle(initX-obstacle_block_width*4, initY-obstacle_block_width*26, {51, 51, 51});
    draw_quads_obstacle(initX-obstacle_block_width*3, initY-obstacle_block_width*26, {51, 51, 51});
    draw_quads_obstacle(initX-obstacle_block_width*2, initY-obstacle_block_width*26, {0, 0, 0});
    draw_quads_obstacle(initX-obstacle_block_width, initY-obstacle_block_width*26, {0, 0, 0});
    draw_quads_obstacle(initX, initY-obstacle_block_width*26, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width, initY-obstacle_block_width*26, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*2, initY-obstacle_block_width*26, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*3, initY-obstacle_block_width*26, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*4, initY-obstacle_block_width*26, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*5, initY-obstacle_block_width*26, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*6, initY-obstacle_block_width*26, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*7, initY-obstacle_block_width*26, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*8, initY-obstacle_block_width*26, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*9, initY-obstacle_block_width*26, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*10, initY-obstacle_block_width*26, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*11, initY-obstacle_block_width*26, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*12, initY-obstacle_block_width*26, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*13, initY-obstacle_block_width*26, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*14, initY-obstacle_block_width*26, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*15, initY-obstacle_block_width*26, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*16, initY-obstacle_block_width*26, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*17, initY-obstacle_block_width*26, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*18, initY-obstacle_block_width*26, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*19, initY-obstacle_block_width*26, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*20, initY-obstacle_block_width*26, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*21, initY-obstacle_block_width*26, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*22, initY-obstacle_block_width*26, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*23, initY-obstacle_block_width*26, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*24, initY-obstacle_block_width*26, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*25, initY-obstacle_block_width*26, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*26, initY-obstacle_block_width*26, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*27, initY-obstacle_block_width*26, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*28, initY-obstacle_block_width*26, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*29, initY-obstacle_block_width*26, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*30, initY-obstacle_block_width*26, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*31, initY-obstacle_block_width*26, {51, 51, 51});

    draw_quads_obstacle(initX-obstacle_block_width*6, initY-obstacle_block_width*27, {0, 87, 132});
    draw_quads_obstacle(initX-obstacle_block_width*5, initY-obstacle_block_width*27, {178, 220, 239});
    draw_quads_obstacle(initX-obstacle_block_width*4, initY-obstacle_block_width*27, {178, 220, 239});
    draw_quads_obstacle(initX-obstacle_block_width*3, initY-obstacle_block_width*27, {51, 51, 51});
    draw_quads_obstacle(initX-obstacle_block_width*2, initY-obstacle_block_width*27, {51, 51, 51});
    draw_quads_obstacle(initX-obstacle_block_width, initY-obstacle_block_width*27, {0, 0, 0});
    draw_quads_obstacle(initX, initY-obstacle_block_width*27, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width, initY-obstacle_block_width*27, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*2, initY-obstacle_block_width*27, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*3, initY-obstacle_block_width*27, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*4, initY-obstacle_block_width*27, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*5, initY-obstacle_block_width*27, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*6, initY-obstacle_block_width*27, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*7, initY-obstacle_block_width*27, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*8, initY-obstacle_block_width*27, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*9, initY-obstacle_block_width*27, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*10, initY-obstacle_block_width*27, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*11, initY-obstacle_block_width*27, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*12, initY-obstacle_block_width*27, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*13, initY-obstacle_block_width*27, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*14, initY-obstacle_block_width*27, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*15, initY-obstacle_block_width*27, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*16, initY-obstacle_block_width*27, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*17, initY-obstacle_block_width*27, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*18, initY-obstacle_block_width*27, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*19, initY-obstacle_block_width*27, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*20, initY-obstacle_block_width*27, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*21, initY-obstacle_block_width*27, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*22, initY-obstacle_block_width*27, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*23, initY-obstacle_block_width*27, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*24, initY-obstacle_block_width*27, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*25, initY-obstacle_block_width*27, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*26, initY-obstacle_block_width*27, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*27, initY-obstacle_block_width*27, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*28, initY-obstacle_block_width*27, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*29, initY-obstacle_block_width*27, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*30, initY-obstacle_block_width*27, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*31, initY-obstacle_block_width*27, {51, 51, 51});

    draw_quads_obstacle(initX-obstacle_block_width*5, initY-obstacle_block_width*28, {0, 87, 132});
    draw_quads_obstacle(initX-obstacle_block_width*4, initY-obstacle_block_width*28, {178, 220, 239});
    draw_quads_obstacle(initX-obstacle_block_width*3, initY-obstacle_block_width*28, {51, 51, 51});
    draw_quads_obstacle(initX-obstacle_block_width*2, initY-obstacle_block_width*28, {51, 51, 51});
    draw_quads_obstacle(initX-obstacle_block_width, initY-obstacle_block_width*28, {0, 0, 0});
    draw_quads_obstacle(initX, initY-obstacle_block_width*28, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width, initY-obstacle_block_width*28, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*2, initY-obstacle_block_width*28, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*3, initY-obstacle_block_width*28, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*4, initY-obstacle_block_width*28, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*5, initY-obstacle_block_width*28, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*6, initY-obstacle_block_width*28, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*7, initY-obstacle_block_width*28, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*8, initY-obstacle_block_width*28, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*9, initY-obstacle_block_width*28, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*10, initY-obstacle_block_width*28, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*11, initY-obstacle_block_width*28, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*12, initY-obstacle_block_width*28, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*13, initY-obstacle_block_width*28, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*14, initY-obstacle_block_width*28, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*15, initY-obstacle_block_width*28, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*16, initY-obstacle_block_width*28, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*17, initY-obstacle_block_width*28, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*18, initY-obstacle_block_width*28, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*19, initY-obstacle_block_width*28, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*20, initY-obstacle_block_width*28, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*21, initY-obstacle_block_width*28, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*22, initY-obstacle_block_width*28, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*23, initY-obstacle_block_width*28, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*24, initY-obstacle_block_width*28, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*25, initY-obstacle_block_width*28, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*26, initY-obstacle_block_width*28, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*27, initY-obstacle_block_width*28, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*28, initY-obstacle_block_width*28, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*29, initY-obstacle_block_width*28, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*30, initY-obstacle_block_width*28, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*31, initY-obstacle_block_width*28, {0, 0, 0});

    draw_quads_obstacle(initX-obstacle_block_width*4, initY-obstacle_block_width*29, {0, 87, 132});
    draw_quads_obstacle(initX-obstacle_block_width*3, initY-obstacle_block_width*29, {178, 220, 239});
    draw_quads_obstacle(initX-obstacle_block_width*2, initY-obstacle_block_width*29, {51, 51, 51});
    draw_quads_obstacle(initX-obstacle_block_width, initY-obstacle_block_width*29, {51, 51, 51});
    draw_quads_obstacle(initX, initY-obstacle_block_width*29, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width, initY-obstacle_block_width*29, {0, 87, 132});
    draw_quads_obstacle(initX+obstacle_block_width*2, initY-obstacle_block_width*29, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*3, initY-obstacle_block_width*29, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*4, initY-obstacle_block_width*29, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*5, initY-obstacle_block_width*29, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*6, initY-obstacle_block_width*29, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*7, initY-obstacle_block_width*29, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*8, initY-obstacle_block_width*29, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*9, initY-obstacle_block_width*29, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*10, initY-obstacle_block_width*29, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*11, initY-obstacle_block_width*29, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*12, initY-obstacle_block_width*29, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*13, initY-obstacle_block_width*29, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*14, initY-obstacle_block_width*29, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*15, initY-obstacle_block_width*29, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*16, initY-obstacle_block_width*29, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*17, initY-obstacle_block_width*29, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*18, initY-obstacle_block_width*29, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*19, initY-obstacle_block_width*29, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*20, initY-obstacle_block_width*29, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*21, initY-obstacle_block_width*29, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*22, initY-obstacle_block_width*29, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*23, initY-obstacle_block_width*29, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*24, initY-obstacle_block_width*29, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*25, initY-obstacle_block_width*29, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*26, initY-obstacle_block_width*29, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*27, initY-obstacle_block_width*29, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*28, initY-obstacle_block_width*29, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*29, initY-obstacle_block_width*29, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*30, initY-obstacle_block_width*29, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*31, initY-obstacle_block_width*29, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*32, initY-obstacle_block_width*29, {0, 0, 0});

    draw_quads_obstacle(initX-obstacle_block_width*3, initY-obstacle_block_width*30, {0, 87, 132});
    draw_quads_obstacle(initX-obstacle_block_width*2, initY-obstacle_block_width*30, {178, 220, 239});
    draw_quads_obstacle(initX-obstacle_block_width, initY-obstacle_block_width*30, {51, 51, 51});
    draw_quads_obstacle(initX, initY-obstacle_block_width*30, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width, initY-obstacle_block_width*30, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*2, initY-obstacle_block_width*30, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*3, initY-obstacle_block_width*30, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*4, initY-obstacle_block_width*30, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*5, initY-obstacle_block_width*30, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*6, initY-obstacle_block_width*30, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*7, initY-obstacle_block_width*30, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*8, initY-obstacle_block_width*30, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*9, initY-obstacle_block_width*30, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*10, initY-obstacle_block_width*30, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*11, initY-obstacle_block_width*30, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*12, initY-obstacle_block_width*30, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*13, initY-obstacle_block_width*30, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*14, initY-obstacle_block_width*30, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*15, initY-obstacle_block_width*30, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*16, initY-obstacle_block_width*30, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*17, initY-obstacle_block_width*30, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*18, initY-obstacle_block_width*30, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*19, initY-obstacle_block_width*30, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*20, initY-obstacle_block_width*30, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*21, initY-obstacle_block_width*30, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*22, initY-obstacle_block_width*30, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*23, initY-obstacle_block_width*30, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*24, initY-obstacle_block_width*30, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*25, initY-obstacle_block_width*30, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*26, initY-obstacle_block_width*30, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*27, initY-obstacle_block_width*30, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*28, initY-obstacle_block_width*30, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*29, initY-obstacle_block_width*30, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*30, initY-obstacle_block_width*30, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*31, initY-obstacle_block_width*30, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*32, initY-obstacle_block_width*30, {102, 102, 102});

    draw_quads_obstacle(initX-obstacle_block_width*2, initY-obstacle_block_width*31, {0, 87, 132});
    draw_quads_obstacle(initX-obstacle_block_width, initY-obstacle_block_width*31, {178, 220, 239});
    draw_quads_obstacle(initX, initY-obstacle_block_width*31, {178, 220, 239});
    draw_quads_obstacle(initX+obstacle_block_width, initY-obstacle_block_width*31, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*2, initY-obstacle_block_width*31, {178, 220, 239});
    draw_quads_obstacle(initX+obstacle_block_width*3, initY-obstacle_block_width*31, {0, 87, 132});
    draw_quads_obstacle(initX+obstacle_block_width*4, initY-obstacle_block_width*31, {0, 87, 132});
    draw_quads_obstacle(initX+obstacle_block_width*5, initY-obstacle_block_width*31, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*6, initY-obstacle_block_width*31, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*7, initY-obstacle_block_width*31, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*8, initY-obstacle_block_width*31, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*9, initY-obstacle_block_width*31, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*10, initY-obstacle_block_width*31, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*11, initY-obstacle_block_width*31, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*12, initY-obstacle_block_width*31, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*13, initY-obstacle_block_width*31, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*14, initY-obstacle_block_width*31, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*15, initY-obstacle_block_width*31, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*16, initY-obstacle_block_width*31, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*17, initY-obstacle_block_width*31, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*18, initY-obstacle_block_width*31, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*19, initY-obstacle_block_width*31, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*20, initY-obstacle_block_width*31, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*21, initY-obstacle_block_width*31, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*22, initY-obstacle_block_width*31, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*23, initY-obstacle_block_width*31, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*24, initY-obstacle_block_width*31, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*25, initY-obstacle_block_width*31, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*26, initY-obstacle_block_width*31, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*27, initY-obstacle_block_width*31, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*28, initY-obstacle_block_width*31, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*29, initY-obstacle_block_width*31, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*30, initY-obstacle_block_width*31, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*31, initY-obstacle_block_width*31, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*32, initY-obstacle_block_width*31, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*33, initY-obstacle_block_width*31, {163, 163, 163});

    draw_quads_obstacle(initX-obstacle_block_width, initY-obstacle_block_width*32, {0, 87, 132});
    draw_quads_obstacle(initX, initY-obstacle_block_width*32, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width, initY-obstacle_block_width*32, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*2, initY-obstacle_block_width*32, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*3, initY-obstacle_block_width*32, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*4, initY-obstacle_block_width*32, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*5, initY-obstacle_block_width*32, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*6, initY-obstacle_block_width*32, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*7, initY-obstacle_block_width*32, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*8, initY-obstacle_block_width*32, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*9, initY-obstacle_block_width*32, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*10, initY-obstacle_block_width*32, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*11, initY-obstacle_block_width*32, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*12, initY-obstacle_block_width*32, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*13, initY-obstacle_block_width*32, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*14, initY-obstacle_block_width*32, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*15, initY-obstacle_block_width*32, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*16, initY-obstacle_block_width*32, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*17, initY-obstacle_block_width*32, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*18, initY-obstacle_block_width*32, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*19, initY-obstacle_block_width*32, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*20, initY-obstacle_block_width*32, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*21, initY-obstacle_block_width*32, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*22, initY-obstacle_block_width*32, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*23, initY-obstacle_block_width*32, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*24, initY-obstacle_block_width*32, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*25, initY-obstacle_block_width*32, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*26, initY-obstacle_block_width*32, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*27, initY-obstacle_block_width*32, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*28, initY-obstacle_block_width*32, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*29, initY-obstacle_block_width*32, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*30, initY-obstacle_block_width*32, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*31, initY-obstacle_block_width*32, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*32, initY-obstacle_block_width*32, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*33, initY-obstacle_block_width*32, {102, 102, 102});

    draw_quads_obstacle(initX-obstacle_block_width, initY-obstacle_block_width*33, {0, 0, 0});
    draw_quads_obstacle(initX, initY-obstacle_block_width*33, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width, initY-obstacle_block_width*33, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*2, initY-obstacle_block_width*33, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*3, initY-obstacle_block_width*33, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*4, initY-obstacle_block_width*33, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*5, initY-obstacle_block_width*33, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*6, initY-obstacle_block_width*33, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*7, initY-obstacle_block_width*33, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*8, initY-obstacle_block_width*33, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*9, initY-obstacle_block_width*33, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*10, initY-obstacle_block_width*33, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*11, initY-obstacle_block_width*33, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*12, initY-obstacle_block_width*33, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*13, initY-obstacle_block_width*33, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*14, initY-obstacle_block_width*33, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*15, initY-obstacle_block_width*33, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*16, initY-obstacle_block_width*33, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*17, initY-obstacle_block_width*33, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*18, initY-obstacle_block_width*33, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*19, initY-obstacle_block_width*33, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*20, initY-obstacle_block_width*33, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*21, initY-obstacle_block_width*33, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*22, initY-obstacle_block_width*33, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*23, initY-obstacle_block_width*33, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*24, initY-obstacle_block_width*33, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*25, initY-obstacle_block_width*33, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*26, initY-obstacle_block_width*33, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*27, initY-obstacle_block_width*33, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*28, initY-obstacle_block_width*33, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*29, initY-obstacle_block_width*33, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*30, initY-obstacle_block_width*33, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*31, initY-obstacle_block_width*33, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*32, initY-obstacle_block_width*33, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*33, initY-obstacle_block_width*33, {163, 163, 163});

    draw_quads_obstacle(initX, initY-obstacle_block_width*34, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width, initY-obstacle_block_width*34, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*2, initY-obstacle_block_width*34, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*3, initY-obstacle_block_width*34, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*4, initY-obstacle_block_width*34, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*5, initY-obstacle_block_width*34, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*6, initY-obstacle_block_width*34, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*7, initY-obstacle_block_width*34, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*8, initY-obstacle_block_width*34, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*9, initY-obstacle_block_width*34, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*10, initY-obstacle_block_width*34, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*11, initY-obstacle_block_width*34, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*12, initY-obstacle_block_width*34, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*13, initY-obstacle_block_width*34, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*14, initY-obstacle_block_width*34, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*15, initY-obstacle_block_width*34, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*16, initY-obstacle_block_width*34, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*17, initY-obstacle_block_width*34, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*18, initY-obstacle_block_width*34, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*19, initY-obstacle_block_width*34, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*20, initY-obstacle_block_width*34, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*21, initY-obstacle_block_width*34, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*22, initY-obstacle_block_width*34, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*23, initY-obstacle_block_width*34, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*24, initY-obstacle_block_width*34, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*25, initY-obstacle_block_width*34, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*26, initY-obstacle_block_width*34, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*27, initY-obstacle_block_width*34, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*28, initY-obstacle_block_width*34, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*29, initY-obstacle_block_width*34, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*30, initY-obstacle_block_width*34, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*31, initY-obstacle_block_width*34, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*32, initY-obstacle_block_width*34, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*33, initY-obstacle_block_width*34, {163, 163, 163});

    draw_quads_obstacle(initX, initY-obstacle_block_width*35, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width, initY-obstacle_block_width*35, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*2, initY-obstacle_block_width*35, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*3, initY-obstacle_block_width*35, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*4, initY-obstacle_block_width*35, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*5, initY-obstacle_block_width*35, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*6, initY-obstacle_block_width*35, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*7, initY-obstacle_block_width*35, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*8, initY-obstacle_block_width*35, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*9, initY-obstacle_block_width*35, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*10, initY-obstacle_block_width*35, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*11, initY-obstacle_block_width*35, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*12, initY-obstacle_block_width*35, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*13, initY-obstacle_block_width*35, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*14, initY-obstacle_block_width*35, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*15, initY-obstacle_block_width*35, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*16, initY-obstacle_block_width*35, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*17, initY-obstacle_block_width*35, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*18, initY-obstacle_block_width*35, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*19, initY-obstacle_block_width*35, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*20, initY-obstacle_block_width*35, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*21, initY-obstacle_block_width*35, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*22, initY-obstacle_block_width*35, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*23, initY-obstacle_block_width*35, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*24, initY-obstacle_block_width*35, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*25, initY-obstacle_block_width*35, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*26, initY-obstacle_block_width*35, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*27, initY-obstacle_block_width*35, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*28, initY-obstacle_block_width*35, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*29, initY-obstacle_block_width*35, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*30, initY-obstacle_block_width*35, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*31, initY-obstacle_block_width*35, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*32, initY-obstacle_block_width*35, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*33, initY-obstacle_block_width*35, {163, 163, 163});

    draw_quads_obstacle(initX+obstacle_block_width*2, initY-obstacle_block_width*36, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*3, initY-obstacle_block_width*36, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*4, initY-obstacle_block_width*36, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*5, initY-obstacle_block_width*36, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*6, initY-obstacle_block_width*36, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*7, initY-obstacle_block_width*36, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*8, initY-obstacle_block_width*36, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*9, initY-obstacle_block_width*36, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*10, initY-obstacle_block_width*36, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*11, initY-obstacle_block_width*36, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*12, initY-obstacle_block_width*36, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*13, initY-obstacle_block_width*36, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*14, initY-obstacle_block_width*36, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*15, initY-obstacle_block_width*36, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*16, initY-obstacle_block_width*36, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*17, initY-obstacle_block_width*36, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*18, initY-obstacle_block_width*36, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*19, initY-obstacle_block_width*36, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*20, initY-obstacle_block_width*36, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*21, initY-obstacle_block_width*36, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*22, initY-obstacle_block_width*36, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*23, initY-obstacle_block_width*36, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*24, initY-obstacle_block_width*36, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*25, initY-obstacle_block_width*36, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*26, initY-obstacle_block_width*36, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*27, initY-obstacle_block_width*36, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*28, initY-obstacle_block_width*36, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*29, initY-obstacle_block_width*36, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*30, initY-obstacle_block_width*36, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*31, initY-obstacle_block_width*36, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*32, initY-obstacle_block_width*36, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*33, initY-obstacle_block_width*36, {163, 163, 163});

    draw_quads_obstacle(initX+obstacle_block_width*3, initY-obstacle_block_width*37, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*4, initY-obstacle_block_width*37, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*5, initY-obstacle_block_width*37, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*6, initY-obstacle_block_width*37, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*7, initY-obstacle_block_width*37, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*8, initY-obstacle_block_width*37, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*9, initY-obstacle_block_width*37, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*10, initY-obstacle_block_width*37, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*11, initY-obstacle_block_width*37, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*12, initY-obstacle_block_width*37, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*13, initY-obstacle_block_width*37, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*14, initY-obstacle_block_width*37, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*15, initY-obstacle_block_width*37, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*16, initY-obstacle_block_width*37, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*17, initY-obstacle_block_width*37, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*18, initY-obstacle_block_width*37, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*19, initY-obstacle_block_width*37, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*20, initY-obstacle_block_width*37, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*21, initY-obstacle_block_width*37, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*22, initY-obstacle_block_width*37, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*23, initY-obstacle_block_width*37, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*24, initY-obstacle_block_width*37, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*25, initY-obstacle_block_width*37, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*26, initY-obstacle_block_width*37, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*27, initY-obstacle_block_width*37, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*28, initY-obstacle_block_width*37, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*29, initY-obstacle_block_width*37, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*30, initY-obstacle_block_width*37, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*31, initY-obstacle_block_width*37, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*32, initY-obstacle_block_width*37, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*33, initY-obstacle_block_width*37, {51, 51, 51});

    draw_quads_obstacle(initX+obstacle_block_width*6, initY-obstacle_block_width*38, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*7, initY-obstacle_block_width*38, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*8, initY-obstacle_block_width*38, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*9, initY-obstacle_block_width*38, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*10, initY-obstacle_block_width*38, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*11, initY-obstacle_block_width*38, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*12, initY-obstacle_block_width*38, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*13, initY-obstacle_block_width*38, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*14, initY-obstacle_block_width*38, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*15, initY-obstacle_block_width*38, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*16, initY-obstacle_block_width*38, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*17, initY-obstacle_block_width*38, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*18, initY-obstacle_block_width*38, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*19, initY-obstacle_block_width*38, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*20, initY-obstacle_block_width*38, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*21, initY-obstacle_block_width*38, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*22, initY-obstacle_block_width*38, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*23, initY-obstacle_block_width*38, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*24, initY-obstacle_block_width*38, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*25, initY-obstacle_block_width*38, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*26, initY-obstacle_block_width*38, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*27, initY-obstacle_block_width*38, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*28, initY-obstacle_block_width*38, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*29, initY-obstacle_block_width*38, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*30, initY-obstacle_block_width*38, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*31, initY-obstacle_block_width*38, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*32, initY-obstacle_block_width*38, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*33, initY-obstacle_block_width*38, {0, 0, 0});

    draw_quads_obstacle(initX+obstacle_block_width*7, initY-obstacle_block_width*39, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*8, initY-obstacle_block_width*39, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*9, initY-obstacle_block_width*39, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*10, initY-obstacle_block_width*39, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*11, initY-obstacle_block_width*39, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*12, initY-obstacle_block_width*39, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*13, initY-obstacle_block_width*39, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*14, initY-obstacle_block_width*39, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*15, initY-obstacle_block_width*39, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*16, initY-obstacle_block_width*39, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*17, initY-obstacle_block_width*39, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*18, initY-obstacle_block_width*39, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*19, initY-obstacle_block_width*39, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*20, initY-obstacle_block_width*39, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*21, initY-obstacle_block_width*39, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*22, initY-obstacle_block_width*39, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*23, initY-obstacle_block_width*39, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*24, initY-obstacle_block_width*39, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*25, initY-obstacle_block_width*39, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*26, initY-obstacle_block_width*39, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*27, initY-obstacle_block_width*39, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*28, initY-obstacle_block_width*39, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*29, initY-obstacle_block_width*39, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*30, initY-obstacle_block_width*39, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*31, initY-obstacle_block_width*39, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*32, initY-obstacle_block_width*39, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*33, initY-obstacle_block_width*39, {0, 0, 0});

    draw_quads_obstacle(initX+obstacle_block_width*10, initY-obstacle_block_width*40, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*11, initY-obstacle_block_width*40, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*12, initY-obstacle_block_width*40, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*13, initY-obstacle_block_width*40, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*14, initY-obstacle_block_width*40, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*15, initY-obstacle_block_width*40, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*16, initY-obstacle_block_width*40, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*17, initY-obstacle_block_width*40, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*18, initY-obstacle_block_width*40, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*19, initY-obstacle_block_width*40, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*20, initY-obstacle_block_width*40, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*21, initY-obstacle_block_width*40, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*22, initY-obstacle_block_width*40, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*23, initY-obstacle_block_width*40, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*24, initY-obstacle_block_width*40, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*25, initY-obstacle_block_width*40, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*26, initY-obstacle_block_width*40, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*27, initY-obstacle_block_width*40, {102, 102, 102});
    draw_quads_obstacle(initX+obstacle_block_width*28, initY-obstacle_block_width*40, {163, 163, 163});
    draw_quads_obstacle(initX+obstacle_block_width*29, initY-obstacle_block_width*40, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*30, initY-obstacle_block_width*40, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*31, initY-obstacle_block_width*40, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*32, initY-obstacle_block_width*40, {0, 0, 0});

    draw_quads_obstacle(initX+obstacle_block_width*11, initY-obstacle_block_width*41, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*12, initY-obstacle_block_width*41, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*13, initY-obstacle_block_width*41, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*14, initY-obstacle_block_width*41, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*15, initY-obstacle_block_width*41, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*16, initY-obstacle_block_width*41, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*17, initY-obstacle_block_width*41, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*18, initY-obstacle_block_width*41, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*19, initY-obstacle_block_width*41, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*20, initY-obstacle_block_width*41, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*21, initY-obstacle_block_width*41, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*22, initY-obstacle_block_width*41, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*23, initY-obstacle_block_width*41, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*24, initY-obstacle_block_width*41, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*25, initY-obstacle_block_width*41, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*26, initY-obstacle_block_width*41, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*27, initY-obstacle_block_width*41, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*28, initY-obstacle_block_width*41, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*29, initY-obstacle_block_width*41, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*30, initY-obstacle_block_width*41, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*31, initY-obstacle_block_width*41, {0, 0, 0});

    draw_quads_obstacle(initX+obstacle_block_width*13, initY-obstacle_block_width*42, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*14, initY-obstacle_block_width*42, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*15, initY-obstacle_block_width*42, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*16, initY-obstacle_block_width*42, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*17, initY-obstacle_block_width*42, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*18, initY-obstacle_block_width*42, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*19, initY-obstacle_block_width*42, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*20, initY-obstacle_block_width*42, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*21, initY-obstacle_block_width*42, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*22, initY-obstacle_block_width*42, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*23, initY-obstacle_block_width*42, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*24, initY-obstacle_block_width*42, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*25, initY-obstacle_block_width*42, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*26, initY-obstacle_block_width*42, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*27, initY-obstacle_block_width*42, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*28, initY-obstacle_block_width*42, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*29, initY-obstacle_block_width*42, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*30, initY-obstacle_block_width*42, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*31, initY-obstacle_block_width*42, {0, 0, 0});

    draw_quads_obstacle(initX+obstacle_block_width*22, initY-obstacle_block_width*43, {51, 51, 51});
    draw_quads_obstacle(initX+obstacle_block_width*23, initY-obstacle_block_width*43, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*24, initY-obstacle_block_width*43, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*25, initY-obstacle_block_width*43, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*26, initY-obstacle_block_width*43, {0, 0, 0});
    draw_quads_obstacle(initX+obstacle_block_width*27, initY-obstacle_block_width*43, {0, 0, 0});
}

void draw_spaceship(){
    float initX = rocket_head.x;
    float initY = rocket_head.y;

    draw_quads_spaceship(initX, initY, {255, 203, 78});
    draw_quads_spaceship(initX + spaceship_block_width, initY, {255, 203, 78});

    draw_quads_spaceship(initX - spaceship_block_width, initY - spaceship_block_width, {228, 228, 232});
    draw_quads_spaceship(initX, initY - spaceship_block_width, {255, 143, 31});
    draw_quads_spaceship(initX + spaceship_block_width, initY - spaceship_block_width, {255, 203, 78});
    draw_quads_spaceship(initX + spaceship_block_width*2, initY - spaceship_block_width, {228, 228, 232});

    draw_quads_spaceship(initX - spaceship_block_width*2, initY - spaceship_block_width*2, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width, initY - spaceship_block_width*2, {228, 228, 232});
    draw_quads_spaceship(initX, initY - spaceship_block_width*2, {255, 143, 31});
    draw_quads_spaceship(initX + spaceship_block_width, initY - spaceship_block_width*2, {255, 203, 78});
    draw_quads_spaceship(initX + spaceship_block_width*2, initY - spaceship_block_width*2, {228, 228, 232});
    draw_quads_spaceship(initX + spaceship_block_width*3, initY - spaceship_block_width*2, {228, 228, 232});

    draw_quads_spaceship(initX - spaceship_block_width*3, initY - spaceship_block_width*3, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*2, initY - spaceship_block_width*3, {194, 194, 196});
    draw_quads_spaceship(initX - spaceship_block_width, initY - spaceship_block_width*3, {194, 194, 196});
    draw_quads_spaceship(initX, initY - spaceship_block_width*3, {255, 143, 31});
    draw_quads_spaceship(initX + spaceship_block_width, initY - spaceship_block_width*3, {255, 203, 78});
    draw_quads_spaceship(initX + spaceship_block_width*2, initY - spaceship_block_width*3, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*3, initY - spaceship_block_width*3, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*4, initY - spaceship_block_width*3, {228, 228, 232});

    draw_quads_spaceship(initX - spaceship_block_width*3, initY - spaceship_block_width*4, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*2, initY - spaceship_block_width*4, {194, 194, 196});
    draw_quads_spaceship(initX - spaceship_block_width, initY - spaceship_block_width*4, {25, 5, 150});
    draw_quads_spaceship(initX, initY - spaceship_block_width*4, {25, 5, 150});
    draw_quads_spaceship(initX + spaceship_block_width, initY - spaceship_block_width*4, {25, 5, 150});
    draw_quads_spaceship(initX + spaceship_block_width*2, initY - spaceship_block_width*4, {105, 119, 254});
    draw_quads_spaceship(initX + spaceship_block_width*3, initY - spaceship_block_width*4, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*4, initY - spaceship_block_width*4, {228, 228, 232});

    draw_quads_spaceship(initX - spaceship_block_width*4, initY - spaceship_block_width*5, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*3, initY - spaceship_block_width*5, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*2, initY - spaceship_block_width*5, {194, 194, 196});
    draw_quads_spaceship(initX - spaceship_block_width, initY - spaceship_block_width*5, {25, 5, 150});
    draw_quads_spaceship(initX, initY - spaceship_block_width*5, {43, 9, 255});
    draw_quads_spaceship(initX + spaceship_block_width, initY - spaceship_block_width*5, {43, 9, 255});
    draw_quads_spaceship(initX + spaceship_block_width*2, initY - spaceship_block_width*5, {105, 119, 254});
    draw_quads_spaceship(initX + spaceship_block_width*3, initY - spaceship_block_width*5, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*4, initY - spaceship_block_width*5, {228, 228, 232});
    draw_quads_spaceship(initX + spaceship_block_width*5, initY - spaceship_block_width*5, {228, 228, 232});

    draw_quads_spaceship(initX - spaceship_block_width*4, initY - spaceship_block_width*6, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*3, initY - spaceship_block_width*6, {194, 194, 196});
    draw_quads_spaceship(initX - spaceship_block_width*2, initY - spaceship_block_width*6, {25, 5, 150});
    draw_quads_spaceship(initX - spaceship_block_width, initY - spaceship_block_width*6, {55, 79, 145});
    draw_quads_spaceship(initX, initY - spaceship_block_width*6, {43, 9, 255});
    draw_quads_spaceship(initX + spaceship_block_width, initY - spaceship_block_width*6, {43, 9, 255});
    draw_quads_spaceship(initX + spaceship_block_width*2, initY - spaceship_block_width*6, {55, 79, 145});
    draw_quads_spaceship(initX + spaceship_block_width*3, initY - spaceship_block_width*6, {105, 119, 254});
    draw_quads_spaceship(initX + spaceship_block_width*4, initY - spaceship_block_width*6, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*5, initY - spaceship_block_width*6, {228, 228, 232});

    draw_quads_spaceship(initX - spaceship_block_width*4, initY - spaceship_block_width*7, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*3, initY - spaceship_block_width*7, {194, 194, 196});
    draw_quads_spaceship(initX - spaceship_block_width*2, initY - spaceship_block_width*7, {25, 5, 150});
    draw_quads_spaceship(initX - spaceship_block_width, initY - spaceship_block_width*7, {55, 79, 145});
    draw_quads_spaceship(initX, initY - spaceship_block_width*7, {43, 9, 255});
    draw_quads_spaceship(initX + spaceship_block_width, initY - spaceship_block_width*7, {43, 9, 255});
    draw_quads_spaceship(initX + spaceship_block_width*2, initY - spaceship_block_width*7, {55, 79, 145});
    draw_quads_spaceship(initX + spaceship_block_width*3, initY - spaceship_block_width*7, {105, 119, 254});
    draw_quads_spaceship(initX + spaceship_block_width*4, initY - spaceship_block_width*7, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*5, initY - spaceship_block_width*7, {228, 228, 232});

    draw_quads_spaceship(initX - spaceship_block_width*4, initY - spaceship_block_width*8, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*3, initY - spaceship_block_width*8, {194, 194, 196});
    draw_quads_spaceship(initX - spaceship_block_width*2, initY - spaceship_block_width*8, {25, 5, 150});
    draw_quads_spaceship(initX - spaceship_block_width, initY - spaceship_block_width*8, {55, 79, 145});
    draw_quads_spaceship(initX, initY - spaceship_block_width*8, {43, 9, 255});
    draw_quads_spaceship(initX + spaceship_block_width, initY - spaceship_block_width*8, {43, 9, 255});
    draw_quads_spaceship(initX + spaceship_block_width*2, initY - spaceship_block_width*8, {55, 79, 145});
    draw_quads_spaceship(initX + spaceship_block_width*3, initY - spaceship_block_width*8, {105, 119, 254});
    draw_quads_spaceship(initX + spaceship_block_width*4, initY - spaceship_block_width*8, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*5, initY - spaceship_block_width*8, {228, 228, 232});

    draw_quads_spaceship(initX - spaceship_block_width*10, initY - spaceship_block_width*9, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*9, initY - spaceship_block_width*9, {168, 95, 0});
    draw_quads_spaceship(initX - spaceship_block_width*8, initY - spaceship_block_width*9, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*6, initY - spaceship_block_width*9, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*5, initY - spaceship_block_width*9, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*4, initY - spaceship_block_width*9, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*3, initY - spaceship_block_width*9, {194, 194, 196});
    draw_quads_spaceship(initX - spaceship_block_width*2, initY - spaceship_block_width*9, {25, 5, 150});
    draw_quads_spaceship(initX - spaceship_block_width, initY - spaceship_block_width*9, {55, 79, 145});
    draw_quads_spaceship(initX, initY - spaceship_block_width*9, {228, 228, 232});
    draw_quads_spaceship(initX + spaceship_block_width, initY - spaceship_block_width*9, {228, 228, 232});
    draw_quads_spaceship(initX + spaceship_block_width*2, initY - spaceship_block_width*9, {55, 79, 145});
    draw_quads_spaceship(initX + spaceship_block_width*3, initY - spaceship_block_width*9, {105, 119, 254});
    draw_quads_spaceship(initX + spaceship_block_width*4, initY - spaceship_block_width*9, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*5, initY - spaceship_block_width*9, {228, 228, 232});
    draw_quads_spaceship(initX + spaceship_block_width*6, initY - spaceship_block_width*9, {76, 76, 77});
    draw_quads_spaceship(initX + spaceship_block_width*7, initY - spaceship_block_width*9, {76, 76, 77});
    draw_quads_spaceship(initX + spaceship_block_width*9, initY - spaceship_block_width*9, {76, 76, 77});
    draw_quads_spaceship(initX + spaceship_block_width*10, initY - spaceship_block_width*9, {168, 95, 0});
    draw_quads_spaceship(initX + spaceship_block_width*11, initY - spaceship_block_width*9, {76, 76, 77});

    draw_quads_spaceship(initX - spaceship_block_width*10, initY - spaceship_block_width*10, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*9, initY - spaceship_block_width*10, {255, 143, 31});
    draw_quads_spaceship(initX - spaceship_block_width*8, initY - spaceship_block_width*10, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*6, initY - spaceship_block_width*10, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*5, initY - spaceship_block_width*10, {155, 155, 158});
    draw_quads_spaceship(initX - spaceship_block_width*4, initY - spaceship_block_width*10, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*3, initY - spaceship_block_width*10, {194, 194, 196});
    draw_quads_spaceship(initX - spaceship_block_width*2, initY - spaceship_block_width*10, {25, 5, 150});
    draw_quads_spaceship(initX - spaceship_block_width, initY - spaceship_block_width*10, {76, 76, 77});
    draw_quads_spaceship(initX, initY - spaceship_block_width*10, {204, 204, 207});
    draw_quads_spaceship(initX + spaceship_block_width, initY - spaceship_block_width*10, {204, 204, 207});
    draw_quads_spaceship(initX + spaceship_block_width*2, initY - spaceship_block_width*10, {228, 228, 232});
    draw_quads_spaceship(initX + spaceship_block_width*3, initY - spaceship_block_width*10, {105, 119, 254});
    draw_quads_spaceship(initX + spaceship_block_width*4, initY - spaceship_block_width*10, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*5, initY - spaceship_block_width*10, {228, 228, 232});
    draw_quads_spaceship(initX + spaceship_block_width*6, initY - spaceship_block_width*10, {155, 155, 158});
    draw_quads_spaceship(initX + spaceship_block_width*7, initY - spaceship_block_width*10, {76, 76, 77});
    draw_quads_spaceship(initX + spaceship_block_width*9, initY - spaceship_block_width*10, {76, 76, 77});
    draw_quads_spaceship(initX + spaceship_block_width*10, initY - spaceship_block_width*10, {255, 143, 31});
    draw_quads_spaceship(initX + spaceship_block_width*11, initY - spaceship_block_width*10, {76, 76, 77});

    draw_quads_spaceship(initX - spaceship_block_width*10, initY - spaceship_block_width*11, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*9, initY - spaceship_block_width*11, {155, 155, 158});
    draw_quads_spaceship(initX - spaceship_block_width*8, initY - spaceship_block_width*11, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*6, initY - spaceship_block_width*11, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*5, initY - spaceship_block_width*11, {155, 155, 158});
    draw_quads_spaceship(initX - spaceship_block_width*4, initY - spaceship_block_width*11, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*3, initY - spaceship_block_width*11, {194, 194, 196});
    draw_quads_spaceship(initX - spaceship_block_width*2, initY - spaceship_block_width*11, {25, 5, 150});
    draw_quads_spaceship(initX - spaceship_block_width, initY - spaceship_block_width*11, {76, 76, 77});
    draw_quads_spaceship(initX, initY - spaceship_block_width*11, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width, initY - spaceship_block_width*11, {204, 204, 207});
    draw_quads_spaceship(initX + spaceship_block_width*2, initY - spaceship_block_width*11, {228, 228, 232});
    draw_quads_spaceship(initX + spaceship_block_width*3, initY - spaceship_block_width*11, {105, 119, 254});
    draw_quads_spaceship(initX + spaceship_block_width*4, initY - spaceship_block_width*11, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*5, initY - spaceship_block_width*11, {228, 228, 232});
    draw_quads_spaceship(initX + spaceship_block_width*6, initY - spaceship_block_width*11, {155, 155, 158});
    draw_quads_spaceship(initX + spaceship_block_width*7, initY - spaceship_block_width*11, {76, 76, 77});
    draw_quads_spaceship(initX + spaceship_block_width*9, initY - spaceship_block_width*11, {76, 76, 77});
    draw_quads_spaceship(initX + spaceship_block_width*10, initY - spaceship_block_width*11, {155, 155, 158});
    draw_quads_spaceship(initX + spaceship_block_width*11, initY - spaceship_block_width*11, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*10, initY - spaceship_block_width*12, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*9, initY - spaceship_block_width*12, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*8, initY - spaceship_block_width*12, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*7, initY - spaceship_block_width*12, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*6, initY - spaceship_block_width*12, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*5, initY - spaceship_block_width*12, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*4, initY - spaceship_block_width*12, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*3, initY - spaceship_block_width*12, {194, 194, 196});
    draw_quads_spaceship(initX - spaceship_block_width*2, initY - spaceship_block_width*12, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width, initY - spaceship_block_width*12, {194, 194, 196});
    draw_quads_spaceship(initX, initY - spaceship_block_width*12, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width, initY - spaceship_block_width*12, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*2, initY - spaceship_block_width*12, {204, 204, 207});
    draw_quads_spaceship(initX + spaceship_block_width*3, initY - spaceship_block_width*12, {228, 228, 232});
    draw_quads_spaceship(initX + spaceship_block_width*4, initY - spaceship_block_width*12, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*5, initY - spaceship_block_width*12, {228, 228, 232});
    draw_quads_spaceship(initX + spaceship_block_width*6, initY - spaceship_block_width*12, {228, 228, 232});
    draw_quads_spaceship(initX + spaceship_block_width*7, initY - spaceship_block_width*12, {228, 228, 232});
    draw_quads_spaceship(initX + spaceship_block_width*8, initY - spaceship_block_width*12, {228, 228, 232});
    draw_quads_spaceship(initX + spaceship_block_width*9, initY - spaceship_block_width*12, {228, 228, 232});
    draw_quads_spaceship(initX + spaceship_block_width*10, initY - spaceship_block_width*12, {228, 228, 232});
    draw_quads_spaceship(initX + spaceship_block_width*11, initY - spaceship_block_width*12, {76, 76, 77});

    draw_quads_spaceship(initX - spaceship_block_width*10, initY - spaceship_block_width*13, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*9, initY - spaceship_block_width*13, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*8, initY - spaceship_block_width*13, {194, 194, 196});
    draw_quads_spaceship(initX - spaceship_block_width*7, initY - spaceship_block_width*13, {194, 194, 196});
    draw_quads_spaceship(initX - spaceship_block_width*6, initY - spaceship_block_width*13, {194, 194, 196});
    draw_quads_spaceship(initX - spaceship_block_width*5, initY - spaceship_block_width*13, {194, 194, 196});
    draw_quads_spaceship(initX - spaceship_block_width*4, initY - spaceship_block_width*13, {194, 194, 196});
    draw_quads_spaceship(initX - spaceship_block_width*3, initY - spaceship_block_width*13, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*2, initY - spaceship_block_width*13, {194, 194, 196});
    draw_quads_spaceship(initX - spaceship_block_width, initY - spaceship_block_width*13, {194, 194, 196});
    draw_quads_spaceship(initX, initY - spaceship_block_width*13, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width, initY - spaceship_block_width*13, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*2, initY - spaceship_block_width*13, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*3, initY - spaceship_block_width*13, {204, 204, 207});
    draw_quads_spaceship(initX + spaceship_block_width*4, initY - spaceship_block_width*13, {228, 228, 232});
    draw_quads_spaceship(initX + spaceship_block_width*5, initY - spaceship_block_width*13, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*6, initY - spaceship_block_width*13, {204, 204, 207});
    draw_quads_spaceship(initX + spaceship_block_width*7, initY - spaceship_block_width*13, {204, 204, 207});
    draw_quads_spaceship(initX + spaceship_block_width*8, initY - spaceship_block_width*13, {204, 204, 207});
    draw_quads_spaceship(initX + spaceship_block_width*9, initY - spaceship_block_width*13, {204, 204, 207});
    draw_quads_spaceship(initX + spaceship_block_width*10, initY - spaceship_block_width*13, {228, 228, 232});
    draw_quads_spaceship(initX + spaceship_block_width*11, initY - spaceship_block_width*13, {76, 76, 77});

    draw_quads_spaceship(initX - spaceship_block_width*10, initY - spaceship_block_width*14, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*9, initY - spaceship_block_width*14, {194, 194, 196});
    draw_quads_spaceship(initX - spaceship_block_width*8, initY - spaceship_block_width*14, {194, 194, 196});
    draw_quads_spaceship(initX - spaceship_block_width*7, initY - spaceship_block_width*14, {194, 194, 196});
    draw_quads_spaceship(initX - spaceship_block_width*6, initY - spaceship_block_width*14, {194, 194, 196});
    draw_quads_spaceship(initX - spaceship_block_width*5, initY - spaceship_block_width*14, {194, 194, 196});
    draw_quads_spaceship(initX - spaceship_block_width*4, initY - spaceship_block_width*14, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*3, initY - spaceship_block_width*14, {194, 194, 196});
    draw_quads_spaceship(initX - spaceship_block_width*2, initY - spaceship_block_width*14, {194, 194, 196});
    draw_quads_spaceship(initX - spaceship_block_width, initY - spaceship_block_width*14, {194, 194, 196});
    draw_quads_spaceship(initX, initY - spaceship_block_width*14, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width, initY - spaceship_block_width*14, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*2, initY - spaceship_block_width*14, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*3, initY - spaceship_block_width*14, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*4, initY - spaceship_block_width*14, {204, 204, 207});
    draw_quads_spaceship(initX + spaceship_block_width*5, initY - spaceship_block_width*14, {228, 228, 232});
    draw_quads_spaceship(initX + spaceship_block_width*6, initY - spaceship_block_width*14, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*7, initY - spaceship_block_width*14, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*8, initY - spaceship_block_width*14, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*9, initY - spaceship_block_width*14, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*10, initY - spaceship_block_width*14, {204, 204, 207});
    draw_quads_spaceship(initX + spaceship_block_width*11, initY - spaceship_block_width*14, {228, 228, 232});

    draw_quads_spaceship(initX - spaceship_block_width*11, initY - spaceship_block_width*15, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*10, initY - spaceship_block_width*15, {194, 194, 196});
    draw_quads_spaceship(initX - spaceship_block_width*9, initY - spaceship_block_width*15, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*8, initY - spaceship_block_width*15, {116, 116, 117});
    draw_quads_spaceship(initX - spaceship_block_width*7, initY - spaceship_block_width*15, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*6, initY - spaceship_block_width*15, {194, 194, 196});
    draw_quads_spaceship(initX - spaceship_block_width*5, initY - spaceship_block_width*15, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*4, initY - spaceship_block_width*15, {194, 194, 196});
    draw_quads_spaceship(initX - spaceship_block_width*3, initY - spaceship_block_width*15, {194, 194, 196});
    draw_quads_spaceship(initX - spaceship_block_width*2, initY - spaceship_block_width*15, {194, 194, 196});
    draw_quads_spaceship(initX - spaceship_block_width, initY - spaceship_block_width*15, {194, 194, 196});
    draw_quads_spaceship(initX, initY - spaceship_block_width*15, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width, initY - spaceship_block_width*15, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*2, initY - spaceship_block_width*15, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*3, initY - spaceship_block_width*15, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*4, initY - spaceship_block_width*15, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*5, initY - spaceship_block_width*15, {204, 204, 207});
    draw_quads_spaceship(initX + spaceship_block_width*6, initY - spaceship_block_width*15, {228, 228, 232});
    draw_quads_spaceship(initX + spaceship_block_width*7, initY - spaceship_block_width*15, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*8, initY - spaceship_block_width*15, {76, 76, 77});
    draw_quads_spaceship(initX + spaceship_block_width*9, initY - spaceship_block_width*15, {116, 116, 117});
    draw_quads_spaceship(initX + spaceship_block_width*10, initY - spaceship_block_width*15, {76, 76, 77});
    draw_quads_spaceship(initX + spaceship_block_width*11, initY - spaceship_block_width*15, {204, 204, 207});
    draw_quads_spaceship(initX + spaceship_block_width*12, initY - spaceship_block_width*15, {228, 228, 232});

    draw_quads_spaceship(initX - spaceship_block_width*11, initY - spaceship_block_width*16, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*10, initY - spaceship_block_width*16, {194, 194, 196});
    draw_quads_spaceship(initX - spaceship_block_width*9, initY - spaceship_block_width*16, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*8, initY - spaceship_block_width*16, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*7, initY - spaceship_block_width*16, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*6, initY - spaceship_block_width*16, {194, 194, 196});
    draw_quads_spaceship(initX - spaceship_block_width*5, initY - spaceship_block_width*16, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*4, initY - spaceship_block_width*16, {194, 194, 196});
    draw_quads_spaceship(initX - spaceship_block_width*3, initY - spaceship_block_width*16, {194, 194, 196});
    draw_quads_spaceship(initX - spaceship_block_width*2, initY - spaceship_block_width*16, {194, 194, 196});
    draw_quads_spaceship(initX - spaceship_block_width, initY - spaceship_block_width*16, {194, 194, 196});
    draw_quads_spaceship(initX, initY - spaceship_block_width*16, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width, initY - spaceship_block_width*16, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*2, initY - spaceship_block_width*16, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*3, initY - spaceship_block_width*16, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*4, initY - spaceship_block_width*16, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*5, initY - spaceship_block_width*16, {204, 204, 207});
    draw_quads_spaceship(initX + spaceship_block_width*6, initY - spaceship_block_width*16, {228, 228, 232});
    draw_quads_spaceship(initX + spaceship_block_width*7, initY - spaceship_block_width*16, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*8, initY - spaceship_block_width*16, {76, 76, 77});
    draw_quads_spaceship(initX + spaceship_block_width*9, initY - spaceship_block_width*16, {76, 76, 77});
    draw_quads_spaceship(initX + spaceship_block_width*10, initY - spaceship_block_width*16, {76, 76, 77});
    draw_quads_spaceship(initX + spaceship_block_width*11, initY - spaceship_block_width*16, {204, 204, 207});
    draw_quads_spaceship(initX + spaceship_block_width*12, initY - spaceship_block_width*16, {228, 228, 232});

    draw_quads_spaceship(initX - spaceship_block_width*12, initY - spaceship_block_width*17, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*11, initY - spaceship_block_width*17, {156, 156, 158});
    draw_quads_spaceship(initX - spaceship_block_width*10, initY - spaceship_block_width*17, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*9, initY - spaceship_block_width*17, {194, 194, 196});
    draw_quads_spaceship(initX - spaceship_block_width*8, initY - spaceship_block_width*17, {194, 194, 196});
    draw_quads_spaceship(initX - spaceship_block_width*7, initY - spaceship_block_width*17, {204, 204, 207});
    draw_quads_spaceship(initX - spaceship_block_width*6, initY - spaceship_block_width*17, {228, 228, 232});
    draw_quads_spaceship(initX - spaceship_block_width*5, initY - spaceship_block_width*17, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*4, initY - spaceship_block_width*17, {194, 194, 196});
    draw_quads_spaceship(initX - spaceship_block_width*3, initY - spaceship_block_width*17, {194, 194, 196});
    draw_quads_spaceship(initX - spaceship_block_width*2, initY - spaceship_block_width*17, {194, 194, 196});
    draw_quads_spaceship(initX - spaceship_block_width, initY - spaceship_block_width*17, {76, 76, 77});
    draw_quads_spaceship(initX, initY - spaceship_block_width*17, {76, 76, 77});
    draw_quads_spaceship(initX + spaceship_block_width, initY - spaceship_block_width*17, {76, 76, 77});
    draw_quads_spaceship(initX + spaceship_block_width*2, initY - spaceship_block_width*17, {76, 76, 77});
    draw_quads_spaceship(initX + spaceship_block_width*3, initY - spaceship_block_width*17, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*4, initY - spaceship_block_width*17, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*5, initY - spaceship_block_width*17, {204, 204, 207});
    draw_quads_spaceship(initX + spaceship_block_width*6, initY - spaceship_block_width*17, {228, 228, 232});
    draw_quads_spaceship(initX + spaceship_block_width*7, initY - spaceship_block_width*17, {76, 76, 77});
    draw_quads_spaceship(initX + spaceship_block_width*8, initY - spaceship_block_width*17, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*9, initY - spaceship_block_width*17, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*10, initY - spaceship_block_width*17, {228, 228, 232});
    draw_quads_spaceship(initX + spaceship_block_width*11, initY - spaceship_block_width*17, {228, 228, 232});
    draw_quads_spaceship(initX + spaceship_block_width*12, initY - spaceship_block_width*17, {204, 204, 207});
    draw_quads_spaceship(initX + spaceship_block_width*13, initY - spaceship_block_width*17, {228, 228, 232});

    draw_quads_spaceship(initX - spaceship_block_width*13, initY - spaceship_block_width*18, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*12, initY - spaceship_block_width*18, {228, 228, 232});
    draw_quads_spaceship(initX - spaceship_block_width*11, initY - spaceship_block_width*18, {156, 156, 158});
    draw_quads_spaceship(initX - spaceship_block_width*10, initY - spaceship_block_width*18, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*9, initY - spaceship_block_width*18, {194, 194, 196});
    draw_quads_spaceship(initX - spaceship_block_width*8, initY - spaceship_block_width*18, {194, 194, 196});
    draw_quads_spaceship(initX - spaceship_block_width*7, initY - spaceship_block_width*18, {204, 204, 207});
    draw_quads_spaceship(initX - spaceship_block_width*6, initY - spaceship_block_width*18, {228, 228, 232});
    draw_quads_spaceship(initX - spaceship_block_width*5, initY - spaceship_block_width*18, {194, 194, 196});
    draw_quads_spaceship(initX - spaceship_block_width*4, initY - spaceship_block_width*18, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*3, initY - spaceship_block_width*18, {194, 194, 196});
    draw_quads_spaceship(initX - spaceship_block_width*2, initY - spaceship_block_width*18, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width, initY - spaceship_block_width*18, {194, 194, 196});
    draw_quads_spaceship(initX, initY - spaceship_block_width*18, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width, initY - spaceship_block_width*18, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*2, initY - spaceship_block_width*18, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*3, initY - spaceship_block_width*18, {76, 76, 77});
    draw_quads_spaceship(initX + spaceship_block_width*4, initY - spaceship_block_width*18, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*5, initY - spaceship_block_width*18, {76, 76, 77});
    draw_quads_spaceship(initX + spaceship_block_width*6, initY - spaceship_block_width*18, {156, 156, 158});
    draw_quads_spaceship(initX + spaceship_block_width*7, initY - spaceship_block_width*18, {76, 76, 77});
    draw_quads_spaceship(initX + spaceship_block_width*8, initY - spaceship_block_width*18, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*9, initY - spaceship_block_width*18, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*10, initY - spaceship_block_width*18, {228, 228, 232});
    draw_quads_spaceship(initX + spaceship_block_width*11, initY - spaceship_block_width*18, {228, 228, 232});
    draw_quads_spaceship(initX + spaceship_block_width*12, initY - spaceship_block_width*18, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*13, initY - spaceship_block_width*18, {204, 204, 207});
    draw_quads_spaceship(initX + spaceship_block_width*14, initY - spaceship_block_width*18, {228, 228, 232});

    draw_quads_spaceship(initX - spaceship_block_width*14, initY - spaceship_block_width*19, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*13, initY - spaceship_block_width*19, {228, 228, 232});
    draw_quads_spaceship(initX - spaceship_block_width*12, initY - spaceship_block_width*19, {228, 228, 232});
    draw_quads_spaceship(initX - spaceship_block_width*11, initY - spaceship_block_width*19, {156, 156, 158});
    draw_quads_spaceship(initX - spaceship_block_width*10, initY - spaceship_block_width*19, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*9, initY - spaceship_block_width*19, {194, 194, 196});
    draw_quads_spaceship(initX - spaceship_block_width*8, initY - spaceship_block_width*19, {194, 194, 196});
    draw_quads_spaceship(initX - spaceship_block_width*7, initY - spaceship_block_width*19, {204, 204, 207});
    draw_quads_spaceship(initX - spaceship_block_width*6, initY - spaceship_block_width*19, {228, 228, 232});
    draw_quads_spaceship(initX - spaceship_block_width*5, initY - spaceship_block_width*19, {194, 194, 196});
    draw_quads_spaceship(initX - spaceship_block_width*4, initY - spaceship_block_width*19, {194, 194, 196});
    draw_quads_spaceship(initX - spaceship_block_width*3, initY - spaceship_block_width*19, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*2, initY - spaceship_block_width*19, {194, 194, 196});
    draw_quads_spaceship(initX - spaceship_block_width, initY - spaceship_block_width*19, {194, 194, 196});
    draw_quads_spaceship(initX, initY - spaceship_block_width*19, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width, initY - spaceship_block_width*19, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*2, initY - spaceship_block_width*19, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*3, initY - spaceship_block_width*19, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*4, initY - spaceship_block_width*19, {76, 76, 77});
    draw_quads_spaceship(initX + spaceship_block_width*5, initY - spaceship_block_width*19, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*6, initY - spaceship_block_width*19, {156, 156, 158});
    draw_quads_spaceship(initX + spaceship_block_width*7, initY - spaceship_block_width*19, {76, 76, 77});
    draw_quads_spaceship(initX + spaceship_block_width*8, initY - spaceship_block_width*19, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*9, initY - spaceship_block_width*19, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*10, initY - spaceship_block_width*19, {228, 228, 232});
    draw_quads_spaceship(initX + spaceship_block_width*11, initY - spaceship_block_width*19, {228, 228, 232});
    draw_quads_spaceship(initX + spaceship_block_width*12, initY - spaceship_block_width*19, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*13, initY - spaceship_block_width*19, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*14, initY - spaceship_block_width*19, {204, 204, 207});
    draw_quads_spaceship(initX + spaceship_block_width*15, initY - spaceship_block_width*19, {228, 228, 232});

    draw_quads_spaceship(initX - spaceship_block_width*14, initY - spaceship_block_width*20, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*13, initY - spaceship_block_width*20, {255, 143, 31});
    draw_quads_spaceship(initX - spaceship_block_width*12, initY - spaceship_block_width*20, {255, 143, 31});
    draw_quads_spaceship(initX - spaceship_block_width*11, initY - spaceship_block_width*20, {168, 95, 0});
    draw_quads_spaceship(initX - spaceship_block_width*10, initY - spaceship_block_width*20, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*9, initY - spaceship_block_width*20, {194, 194, 196});
    draw_quads_spaceship(initX - spaceship_block_width*8, initY - spaceship_block_width*20, {194, 194, 196});
    draw_quads_spaceship(initX - spaceship_block_width*7, initY - spaceship_block_width*20, {204, 204, 207});
    draw_quads_spaceship(initX - spaceship_block_width*6, initY - spaceship_block_width*20, {228, 228, 232});
    draw_quads_spaceship(initX - spaceship_block_width*5, initY - spaceship_block_width*20, {255, 143, 31});
    draw_quads_spaceship(initX - spaceship_block_width*4, initY - spaceship_block_width*20, {194, 194, 196});
    draw_quads_spaceship(initX - spaceship_block_width*3, initY - spaceship_block_width*20, {194, 194, 196});
    draw_quads_spaceship(initX - spaceship_block_width*2, initY - spaceship_block_width*20, {194, 194, 196});
    draw_quads_spaceship(initX - spaceship_block_width, initY - spaceship_block_width*20, {255, 143, 31});
    draw_quads_spaceship(initX, initY - spaceship_block_width*20, {255, 143, 31});
    draw_quads_spaceship(initX + spaceship_block_width, initY - spaceship_block_width*20, {255, 143, 31});
    draw_quads_spaceship(initX + spaceship_block_width*2, initY - spaceship_block_width*20, {255, 143, 31});
    draw_quads_spaceship(initX + spaceship_block_width*3, initY - spaceship_block_width*20, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*4, initY - spaceship_block_width*20, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*5, initY - spaceship_block_width*20, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*6, initY - spaceship_block_width*20, {168, 95, 0});
    draw_quads_spaceship(initX + spaceship_block_width*7, initY - spaceship_block_width*20, {76, 76, 77});
    draw_quads_spaceship(initX + spaceship_block_width*8, initY - spaceship_block_width*20, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*9, initY - spaceship_block_width*20, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*10, initY - spaceship_block_width*20, {228, 228, 232});
    draw_quads_spaceship(initX + spaceship_block_width*11, initY - spaceship_block_width*20, {228, 228, 232});
    draw_quads_spaceship(initX + spaceship_block_width*12, initY - spaceship_block_width*20, {255, 203, 78});
    draw_quads_spaceship(initX + spaceship_block_width*13, initY - spaceship_block_width*20, {255, 203, 78});
    draw_quads_spaceship(initX + spaceship_block_width*14, initY - spaceship_block_width*20, {255, 203, 78});
    draw_quads_spaceship(initX + spaceship_block_width*15, initY - spaceship_block_width*20, {228, 228, 232});

    draw_quads_spaceship(initX - spaceship_block_width*13, initY - spaceship_block_width*21, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*12, initY - spaceship_block_width*21, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*11, initY - spaceship_block_width*21, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*9, initY - spaceship_block_width*21, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*8, initY - spaceship_block_width*21, {255, 203, 78});
    draw_quads_spaceship(initX - spaceship_block_width*7, initY - spaceship_block_width*21, {255, 203, 78});
    draw_quads_spaceship(initX - spaceship_block_width*6, initY - spaceship_block_width*21, {228, 228, 232});
    draw_quads_spaceship(initX - spaceship_block_width*5, initY - spaceship_block_width*21, {194, 194, 196});
    draw_quads_spaceship(initX - spaceship_block_width*4, initY - spaceship_block_width*21, {255, 143, 31});
    draw_quads_spaceship(initX - spaceship_block_width*3, initY - spaceship_block_width*21, {194, 194, 196});
    draw_quads_spaceship(initX - spaceship_block_width*2, initY - spaceship_block_width*21, {255, 143, 31});
    draw_quads_spaceship(initX - spaceship_block_width, initY - spaceship_block_width*21, {194, 194, 196});
    draw_quads_spaceship(initX, initY - spaceship_block_width*21, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width, initY - spaceship_block_width*21, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*2, initY - spaceship_block_width*21, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*3, initY - spaceship_block_width*21, {255, 143, 31});
    draw_quads_spaceship(initX + spaceship_block_width*4, initY - spaceship_block_width*21, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*5, initY - spaceship_block_width*21, {255, 143, 31});
    draw_quads_spaceship(initX + spaceship_block_width*6, initY - spaceship_block_width*21, {156, 156, 158});
    draw_quads_spaceship(initX + spaceship_block_width*7, initY - spaceship_block_width*21, {76, 76, 77});
    draw_quads_spaceship(initX + spaceship_block_width*8, initY - spaceship_block_width*21, {255, 203, 78});
    draw_quads_spaceship(initX + spaceship_block_width*9, initY - spaceship_block_width*21, {255, 203, 78});
    draw_quads_spaceship(initX + spaceship_block_width*10, initY - spaceship_block_width*21, {228, 228, 232});
    draw_quads_spaceship(initX + spaceship_block_width*12, initY - spaceship_block_width*21, {228, 228, 232});
    draw_quads_spaceship(initX + spaceship_block_width*13, initY - spaceship_block_width*21, {228, 228, 232});
    draw_quads_spaceship(initX + spaceship_block_width*14, initY - spaceship_block_width*21, {228, 228, 232});


    draw_quads_spaceship(initX - spaceship_block_width*9, initY - spaceship_block_width*22, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*8, initY - spaceship_block_width*22, {194, 194, 196});
    draw_quads_spaceship(initX - spaceship_block_width*7, initY - spaceship_block_width*22, {204, 204, 207});
    draw_quads_spaceship(initX - spaceship_block_width*6, initY - spaceship_block_width*22, {228, 228, 232});
    draw_quads_spaceship(initX - spaceship_block_width*5, initY - spaceship_block_width*22, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*4, initY - spaceship_block_width*22, {194, 194, 196});
    draw_quads_spaceship(initX - spaceship_block_width*3, initY - spaceship_block_width*22, {255, 143, 31});
    draw_quads_spaceship(initX - spaceship_block_width*2, initY - spaceship_block_width*22, {194, 194, 196});
    draw_quads_spaceship(initX - spaceship_block_width, initY - spaceship_block_width*22, {194, 194, 196});
    draw_quads_spaceship(initX, initY - spaceship_block_width*22, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width, initY - spaceship_block_width*22, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*2, initY - spaceship_block_width*22, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*3, initY - spaceship_block_width*22, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*4, initY - spaceship_block_width*22, {255, 143, 31});
    draw_quads_spaceship(initX + spaceship_block_width*5, initY - spaceship_block_width*22, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*6, initY - spaceship_block_width*22, {76, 76, 77});
    draw_quads_spaceship(initX + spaceship_block_width*7, initY - spaceship_block_width*22, {76, 76, 77});
    draw_quads_spaceship(initX + spaceship_block_width*8, initY - spaceship_block_width*22, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*9, initY - spaceship_block_width*22, {204, 204, 207});
    draw_quads_spaceship(initX + spaceship_block_width*10, initY - spaceship_block_width*22, {228, 228, 232});

    draw_quads_spaceship(initX - spaceship_block_width*9, initY - spaceship_block_width*23, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*8, initY - spaceship_block_width*23, {194, 194, 196});
    draw_quads_spaceship(initX - spaceship_block_width*7, initY - spaceship_block_width*23, {204, 204, 207});
    draw_quads_spaceship(initX - spaceship_block_width*6, initY - spaceship_block_width*23, {228, 228, 232});
    draw_quads_spaceship(initX - spaceship_block_width*4, initY - spaceship_block_width*23, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*3, initY - spaceship_block_width*23, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*2, initY - spaceship_block_width*23, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width, initY - spaceship_block_width*23, {76, 76, 77});
    draw_quads_spaceship(initX, initY - spaceship_block_width*23, {76, 76, 77});
    draw_quads_spaceship(initX + spaceship_block_width, initY - spaceship_block_width*23, {76, 76, 77});
    draw_quads_spaceship(initX + spaceship_block_width*2, initY - spaceship_block_width*23, {76, 76, 77});
    draw_quads_spaceship(initX + spaceship_block_width*3, initY - spaceship_block_width*23, {76, 76, 77});
    draw_quads_spaceship(initX + spaceship_block_width*4, initY - spaceship_block_width*23, {76, 76, 77});
    draw_quads_spaceship(initX + spaceship_block_width*5, initY - spaceship_block_width*23, {76, 76, 77});
    draw_quads_spaceship(initX + spaceship_block_width*7, initY - spaceship_block_width*23, {76, 76, 77});
    draw_quads_spaceship(initX + spaceship_block_width*8, initY - spaceship_block_width*23, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*9, initY - spaceship_block_width*23, {204, 204, 207});
    draw_quads_spaceship(initX + spaceship_block_width*10, initY - spaceship_block_width*23, {228, 228, 232});

    draw_quads_spaceship(initX - spaceship_block_width*8, initY - spaceship_block_width*24, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*7, initY - spaceship_block_width*24, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*4, initY - spaceship_block_width*24, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*3, initY - spaceship_block_width*24, {194, 194, 196});
    draw_quads_spaceship(initX - spaceship_block_width*2, initY - spaceship_block_width*24, {194, 194, 196});
    draw_quads_spaceship(initX - spaceship_block_width, initY - spaceship_block_width*24, {76, 76, 77});
    draw_quads_spaceship(initX + spaceship_block_width*2, initY - spaceship_block_width*24, {76, 76, 77});
    draw_quads_spaceship(initX + spaceship_block_width*3, initY - spaceship_block_width*24, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*4, initY - spaceship_block_width*24, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*5, initY - spaceship_block_width*24, {76, 76, 77});
    draw_quads_spaceship(initX + spaceship_block_width*8, initY - spaceship_block_width*24, {76, 76, 77});
    draw_quads_spaceship(initX + spaceship_block_width*9, initY - spaceship_block_width*24, {76, 76, 77});

    draw_quads_spaceship(initX - spaceship_block_width*4, initY - spaceship_block_width*25, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*3, initY - spaceship_block_width*25, {255, 143, 31});
    draw_quads_spaceship(initX - spaceship_block_width*2, initY - spaceship_block_width*25, {255, 143, 31});
    draw_quads_spaceship(initX - spaceship_block_width, initY - spaceship_block_width*25, {76, 76, 77});
    draw_quads_spaceship(initX + spaceship_block_width*2, initY - spaceship_block_width*25, {76, 76, 77});
    draw_quads_spaceship(initX + spaceship_block_width*3, initY - spaceship_block_width*25, {255, 143, 31});
    draw_quads_spaceship(initX + spaceship_block_width*4, initY - spaceship_block_width*25, {255, 143, 31});
    draw_quads_spaceship(initX + spaceship_block_width*5, initY - spaceship_block_width*25, {76, 76, 77});

    draw_quads_spaceship(initX - spaceship_block_width*4, initY - spaceship_block_width*26, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*3, initY - spaceship_block_width*26, {194, 194, 196});
    draw_quads_spaceship(initX - spaceship_block_width*2, initY - spaceship_block_width*26, {194, 194, 196});
    draw_quads_spaceship(initX - spaceship_block_width, initY - spaceship_block_width*26, {76, 76, 77});
    draw_quads_spaceship(initX + spaceship_block_width*2, initY - spaceship_block_width*26, {76, 76, 77});
    draw_quads_spaceship(initX + spaceship_block_width*3, initY - spaceship_block_width*26, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*4, initY - spaceship_block_width*26, {194, 194, 196});
    draw_quads_spaceship(initX + spaceship_block_width*5, initY - spaceship_block_width*26, {76, 76, 77});

    draw_quads_spaceship(initX - spaceship_block_width*3, initY - spaceship_block_width*27, {76, 76, 77});
    draw_quads_spaceship(initX - spaceship_block_width*2, initY - spaceship_block_width*27, {76, 76, 77});
    draw_quads_spaceship(initX + spaceship_block_width*3, initY - spaceship_block_width*27, {76, 76, 77});
    draw_quads_spaceship(initX + spaceship_block_width*4, initY - spaceship_block_width*27, {76, 76, 77});
}

void Star::draw(){

    float initX = x;

    float initY = y;


    glBegin(GL_QUADS);

    glColor3ub(111, 145, 185);

    glVertex2f(initX, initY);
    glVertex2f(initX+star_block_width, initY);
    glVertex2f(initX+star_block_width, initY+star_block_width);
    glVertex2f(initX, initY+star_block_width);

    glColor3ub(180, 206, 229);

    glVertex2f(initX, initY+(star_block_width/3)*2);
    glVertex2f(initX+star_block_width, initY+(star_block_width/3)*2);
    glVertex2f(initX+star_block_width, initY+star_block_width/3);
    glVertex2f(initX, initY+star_block_width/3);

    glVertex2f(initX+(star_block_width/3), initY);
    glVertex2f(initX+(star_block_width/3)*2, initY);
    glVertex2f(initX+(star_block_width/3)*2, initY+star_block_width);
    glVertex2f(initX+(star_block_width/3), initY+star_block_width);


    glColor3ub(225, 231, 231);

    glVertex2f(initX+(star_block_width/3), initY+(star_block_width/3));
    glVertex2f(initX+(star_block_width/3)*2, initY+(star_block_width/3));
    glVertex2f(initX+(star_block_width/3)*2, initY+(star_block_width/3)*2);
    glVertex2f(initX+(star_block_width/3), initY+(star_block_width/3)*2);

    glEnd();


    if(isShining){


        glBegin(GL_QUADS);

        glColor3ub(111, 145, 185);
        glVertex2f(initX+(star_block_width/3), initY+star_block_width);
        glVertex2f(initX+(star_block_width/3)*2, initY+star_block_width);
        glVertex2f(initX+(star_block_width/3)*2, initY+(star_block_width/3)*4);
        glVertex2f(initX+(star_block_width/3), initY+(star_block_width/3)*4);

        glVertex2f(initX+(star_block_width/3), initY);
        glVertex2f(initX+(star_block_width/3)*2, initY);
        glVertex2f(initX+(star_block_width/3)*2, initY-(star_block_width/3));
        glVertex2f(initX+(star_block_width/3), initY-(star_block_width/3));

        glVertex2f(initX, initY+star_block_width/3);
        glVertex2f(initX, initY+star_block_width/3*2);
        glVertex2f(initX - star_block_width/3, initY+star_block_width/3*2);
        glVertex2f(initX - star_block_width/3, initY+star_block_width/3);

        glVertex2f(initX + star_block_width + star_block_width/3, initY+star_block_width/3);
        glVertex2f(initX + star_block_width + star_block_width/3, initY+star_block_width/3*2);
        glVertex2f(initX + star_block_width, initY+star_block_width/3*2);
        glVertex2f(initX + star_block_width, initY+star_block_width/3);



        glColor3ub(73, 74, 111);
        glVertex2f(initX+(star_block_width/3), initY+star_block_width/3*4);
        glVertex2f(initX+(star_block_width/3)*2, initY+star_block_width/3*4);
        glVertex2f(initX+(star_block_width/3)*2, initY+(star_block_width/3)*5);
        glVertex2f(initX+(star_block_width/3), initY+(star_block_width/3)*5);

        glVertex2f(initX+(star_block_width/3), initY-star_block_width/3);
        glVertex2f(initX+(star_block_width/3)*2, initY-star_block_width/3);
        glVertex2f(initX+(star_block_width/3)*2, initY-(star_block_width/3)*2);
        glVertex2f(initX+(star_block_width/3), initY-(star_block_width/3)*2);

        glVertex2f(initX - star_block_width/3, initY+star_block_width/3);
        glVertex2f(initX - star_block_width/3, initY+star_block_width/3*2);
        glVertex2f(initX - star_block_width/3*2, initY+star_block_width/3*2);
        glVertex2f(initX - star_block_width/3*2, initY+star_block_width/3);

        glVertex2f(initX + star_block_width + star_block_width/3 * 2, initY+star_block_width/3);
        glVertex2f(initX + star_block_width + star_block_width/3 * 2, initY+star_block_width/3*2);
        glVertex2f(initX + star_block_width + star_block_width/3, initY+star_block_width/3*2);
        glVertex2f(initX + star_block_width + star_block_width/3, initY+star_block_width/3);

        glEnd();
    }
}

void draw_heart(float x, float y){


    float initX = x;

    float initY = y;



    glBegin(GL_QUADS);

    glColor3ub(0, 0, 0);

    glVertex2f(initX, initY);
    glVertex2f(initX+(heart_block_width*5), initY);
    glVertex2f(initX+(heart_block_width*5), initY+heart_block_width);
    glVertex2f(initX, initY+heart_block_width);

    glVertex2f(initX+(heart_block_width*10), initY);
    glVertex2f(initX+(heart_block_width*16), initY);
    glVertex2f(initX+(heart_block_width*16), initY+heart_block_width);
    glVertex2f(initX+(heart_block_width*10), initY+heart_block_width);

    glVertex2f(initX-(heart_block_width*2), initY-heart_block_width);
    glVertex2f(initX, initY-heart_block_width);
    glVertex2f(initX, initY);
    glVertex2f(initX-(heart_block_width*2), initY);

    glVertex2f(initX+(heart_block_width*4), initY-heart_block_width);
    glVertex2f(initX+(heart_block_width*6), initY-heart_block_width);
    glVertex2f(initX+(heart_block_width*6), initY);
    glVertex2f(initX+(heart_block_width*4), initY);

    glVertex2f(initX+(heart_block_width*9), initY-heart_block_width);
    glVertex2f(initX+(heart_block_width*11), initY-heart_block_width);
    glVertex2f(initX+(heart_block_width*11), initY);
    glVertex2f(initX+(heart_block_width*9), initY);

    glVertex2f(initX+(heart_block_width*15), initY-heart_block_width);
    glVertex2f(initX+(heart_block_width*17), initY-heart_block_width);
    glVertex2f(initX+(heart_block_width*17), initY);
    glVertex2f(initX+(heart_block_width*15), initY);

    glVertex2f(initX-(heart_block_width*3), initY-heart_block_width*2);
    glVertex2f(initX-(heart_block_width*2), initY-heart_block_width*2);
    glVertex2f(initX-(heart_block_width*2), initY-heart_block_width);
    glVertex2f(initX-(heart_block_width*3), initY-heart_block_width);

    glVertex2f(initX+(heart_block_width*6), initY-heart_block_width*2);
    glVertex2f(initX+(heart_block_width*7), initY-heart_block_width*2);
    glVertex2f(initX+(heart_block_width*7), initY-heart_block_width);
    glVertex2f(initX+(heart_block_width*6), initY-heart_block_width);

    glVertex2f(initX+(heart_block_width*8), initY-heart_block_width*2);
    glVertex2f(initX+(heart_block_width*9), initY-heart_block_width*2);
    glVertex2f(initX+(heart_block_width*9), initY-heart_block_width);
    glVertex2f(initX+(heart_block_width*8), initY-heart_block_width);

    glVertex2f(initX+(heart_block_width*17), initY-heart_block_width*2);
    glVertex2f(initX+(heart_block_width*18), initY-heart_block_width*2);
    glVertex2f(initX+(heart_block_width*18), initY-heart_block_width);
    glVertex2f(initX+(heart_block_width*17), initY-heart_block_width);


    glVertex2f(initX-(heart_block_width*4), initY-heart_block_width*3);
    glVertex2f(initX-(heart_block_width*2), initY-heart_block_width*3);
    glVertex2f(initX-(heart_block_width*2), initY-heart_block_width*2);
    glVertex2f(initX-(heart_block_width*4), initY-heart_block_width*2);

    glVertex2f(initX+(heart_block_width*7), initY-heart_block_width*3);
    glVertex2f(initX+(heart_block_width*9), initY-heart_block_width*3);
    glVertex2f(initX+(heart_block_width*9), initY-heart_block_width*2);
    glVertex2f(initX+(heart_block_width*7), initY-heart_block_width*2);

    glVertex2f(initX+(heart_block_width*18), initY-heart_block_width*3);
    glVertex2f(initX+(heart_block_width*19), initY-heart_block_width*3);
    glVertex2f(initX+(heart_block_width*19), initY-heart_block_width*2);
    glVertex2f(initX+(heart_block_width*18), initY-heart_block_width*2);

    glVertex2f(initX-(heart_block_width*4), initY-heart_block_width*4);
    glVertex2f(initX-(heart_block_width*3), initY-heart_block_width*4);
    glVertex2f(initX-(heart_block_width*3), initY-heart_block_width*3);
    glVertex2f(initX-(heart_block_width*4), initY-heart_block_width*3);

    glVertex2f(initX+heart_block_width*18, initY-heart_block_width*4);
    glVertex2f(initX+heart_block_width*19, initY-heart_block_width*4);
    glVertex2f(initX+heart_block_width*19, initY-heart_block_width*3);
    glVertex2f(initX+heart_block_width*18, initY-heart_block_width*3);

    glVertex2f(initX-(heart_block_width*5), initY-heart_block_width*5);
    glVertex2f(initX-(heart_block_width*3), initY-heart_block_width*5);
    glVertex2f(initX-(heart_block_width*3), initY-heart_block_width*4);
    glVertex2f(initX-(heart_block_width*5), initY-heart_block_width*4);

    glVertex2f(initX+heart_block_width*19, initY-heart_block_width*5);
    glVertex2f(initX+heart_block_width*20, initY-heart_block_width*5);
    glVertex2f(initX+heart_block_width*20, initY-heart_block_width*4);
    glVertex2f(initX+heart_block_width*19, initY-heart_block_width*4);

    glVertex2f(initX-(heart_block_width*5), initY-heart_block_width*6);
    glVertex2f(initX-(heart_block_width*4), initY-heart_block_width*6);
    glVertex2f(initX-(heart_block_width*4), initY-heart_block_width*5);
    glVertex2f(initX-(heart_block_width*5), initY-heart_block_width*5);

    glVertex2f(initX+heart_block_width*19, initY-heart_block_width*6);
    glVertex2f(initX+heart_block_width*20, initY-heart_block_width*6);
    glVertex2f(initX+heart_block_width*20, initY-heart_block_width*5);
    glVertex2f(initX+heart_block_width*19, initY-heart_block_width*5);

    glVertex2f(initX-(heart_block_width*5), initY-heart_block_width*7);
    glVertex2f(initX-(heart_block_width*4), initY-heart_block_width*7);
    glVertex2f(initX-(heart_block_width*4), initY-heart_block_width*6);
    glVertex2f(initX-(heart_block_width*5), initY-heart_block_width*6);

    glVertex2f(initX+(heart_block_width*19), initY-heart_block_width*7);
    glVertex2f(initX+(heart_block_width*20), initY-heart_block_width*7);
    glVertex2f(initX+(heart_block_width*20), initY-heart_block_width*6);
    glVertex2f(initX+(heart_block_width*19), initY-heart_block_width*6);

    glVertex2f(initX-(heart_block_width*5), initY-heart_block_width*8);
    glVertex2f(initX-(heart_block_width*4), initY-heart_block_width*8);
    glVertex2f(initX-(heart_block_width*4), initY-heart_block_width*7);
    glVertex2f(initX-(heart_block_width*5), initY-heart_block_width*7);

    glVertex2f(initX+heart_block_width*19, initY-heart_block_width*8);
    glVertex2f(initX+heart_block_width*20, initY-heart_block_width*8);
    glVertex2f(initX+heart_block_width*20, initY-heart_block_width*7);
    glVertex2f(initX+heart_block_width*19, initY-heart_block_width*7);

    glVertex2f(initX-(heart_block_width*5), initY-heart_block_width*9);
    glVertex2f(initX-(heart_block_width*4), initY-heart_block_width*9);
    glVertex2f(initX-(heart_block_width*4), initY-heart_block_width*8);
    glVertex2f(initX-(heart_block_width*5), initY-heart_block_width*8);

    glVertex2f(initX+heart_block_width*19, initY-heart_block_width*9);
    glVertex2f(initX+heart_block_width*20, initY-heart_block_width*9);
    glVertex2f(initX+heart_block_width*20, initY-heart_block_width*8);
    glVertex2f(initX+heart_block_width*19, initY-heart_block_width*8);

    glVertex2f(initX-(heart_block_width*5), initY-heart_block_width*10);
    glVertex2f(initX-(heart_block_width*4), initY-heart_block_width*10);
    glVertex2f(initX-(heart_block_width*4), initY-heart_block_width*9);
    glVertex2f(initX-(heart_block_width*5), initY-heart_block_width*9);

    glVertex2f(initX+heart_block_width*19, initY-heart_block_width*10);
    glVertex2f(initX+heart_block_width*20, initY-heart_block_width*10);
    glVertex2f(initX+heart_block_width*20, initY-heart_block_width*9);
    glVertex2f(initX+heart_block_width*19, initY-heart_block_width*9);

    glVertex2f(initX-(heart_block_width*5), initY-heart_block_width*11);
    glVertex2f(initX-(heart_block_width*3), initY-heart_block_width*11);
    glVertex2f(initX-(heart_block_width*3), initY-heart_block_width*10);
    glVertex2f(initX-(heart_block_width*5), initY-heart_block_width*10);

    glVertex2f(initX+heart_block_width*19, initY-heart_block_width*11);
    glVertex2f(initX+heart_block_width*20, initY-heart_block_width*11);
    glVertex2f(initX+heart_block_width*20, initY-heart_block_width*10);
    glVertex2f(initX+heart_block_width*19, initY-heart_block_width*10);

    glVertex2f(initX-(heart_block_width*4), initY-heart_block_width*12);
    glVertex2f(initX-(heart_block_width*3), initY-heart_block_width*12);
    glVertex2f(initX-(heart_block_width*3), initY-heart_block_width*11);
    glVertex2f(initX-(heart_block_width*4), initY-heart_block_width*11);

    glVertex2f(initX+heart_block_width*18, initY-heart_block_width*12);
    glVertex2f(initX+heart_block_width*19, initY-heart_block_width*12);
    glVertex2f(initX+heart_block_width*19, initY-heart_block_width*11);
    glVertex2f(initX+heart_block_width*18, initY-heart_block_width*11);

    glVertex2f(initX-(heart_block_width*4), initY-heart_block_width*13);
    glVertex2f(initX-(heart_block_width*3), initY-heart_block_width*13);
    glVertex2f(initX-(heart_block_width*3), initY-heart_block_width*12);
    glVertex2f(initX-(heart_block_width*4), initY-heart_block_width*12);

    glVertex2f(initX+(heart_block_width*18), initY-heart_block_width*13);
    glVertex2f(initX+(heart_block_width*19), initY-heart_block_width*13);
    glVertex2f(initX+(heart_block_width*19), initY-heart_block_width*12);
    glVertex2f(initX+(heart_block_width*18), initY-heart_block_width*12);

    glVertex2f(initX-(heart_block_width*3), initY-heart_block_width*14);
    glVertex2f(initX-(heart_block_width*2), initY-heart_block_width*14);
    glVertex2f(initX-(heart_block_width*2), initY-heart_block_width*13);
    glVertex2f(initX-(heart_block_width*3), initY-heart_block_width*13);

    glVertex2f(initX+heart_block_width*17, initY-heart_block_width*14);
    glVertex2f(initX+heart_block_width*19, initY-heart_block_width*14);
    glVertex2f(initX+heart_block_width*19, initY-heart_block_width*13);
    glVertex2f(initX+heart_block_width*17, initY-heart_block_width*13);

    glVertex2f(initX-(heart_block_width*3), initY-heart_block_width*15);
    glVertex2f(initX-(heart_block_width*2), initY-heart_block_width*15);
    glVertex2f(initX-(heart_block_width*2), initY-heart_block_width*14);
    glVertex2f(initX-(heart_block_width*3), initY-heart_block_width*14);

    glVertex2f(initX+(heart_block_width*17), initY-heart_block_width*15);
    glVertex2f(initX+(heart_block_width*18), initY-heart_block_width*15);
    glVertex2f(initX+(heart_block_width*18), initY-heart_block_width*14);
    glVertex2f(initX+(heart_block_width*17), initY-heart_block_width*14);

    glVertex2f(initX-(heart_block_width*2), initY-heart_block_width*16);
    glVertex2f(initX-heart_block_width, initY-heart_block_width*16);
    glVertex2f(initX-heart_block_width, initY-heart_block_width*15);
    glVertex2f(initX-(heart_block_width*2), initY-heart_block_width*15);

    glVertex2f(initX+heart_block_width*16, initY-heart_block_width*16);
    glVertex2f(initX+heart_block_width*17, initY-heart_block_width*16);
    glVertex2f(initX+heart_block_width*17, initY-heart_block_width*15);
    glVertex2f(initX+heart_block_width*16, initY-heart_block_width*15);

    glVertex2f(initX-(heart_block_width*2), initY-heart_block_width*17);
    glVertex2f(initX, initY-heart_block_width*17);
    glVertex2f(initX, initY-heart_block_width*16);
    glVertex2f(initX-(heart_block_width*2), initY-heart_block_width*16);

    glVertex2f(initX+(heart_block_width*15), initY-heart_block_width*17);
    glVertex2f(initX+(heart_block_width*17), initY-heart_block_width*17);
    glVertex2f(initX+(heart_block_width*17), initY-heart_block_width*16);
    glVertex2f(initX+(heart_block_width*15), initY-heart_block_width*16);

    glVertex2f(initX-heart_block_width, initY-heart_block_width*18);
    glVertex2f(initX+heart_block_width, initY-heart_block_width*18);
    glVertex2f(initX+heart_block_width, initY-heart_block_width*17);
    glVertex2f(initX-heart_block_width, initY-heart_block_width*17);

    glVertex2f(initX+heart_block_width*14, initY-heart_block_width*18);
    glVertex2f(initX+heart_block_width*16, initY-heart_block_width*18);
    glVertex2f(initX+heart_block_width*16, initY-heart_block_width*17);
    glVertex2f(initX+heart_block_width*14, initY-heart_block_width*17);

    glVertex2f(initX, initY-heart_block_width*19);
    glVertex2f(initX+heart_block_width*2, initY-heart_block_width*19);
    glVertex2f(initX+heart_block_width*2, initY-heart_block_width*18);
    glVertex2f(initX, initY-heart_block_width*18);

    glVertex2f(initX+heart_block_width*13, initY-heart_block_width*19);
    glVertex2f(initX+heart_block_width*15, initY-heart_block_width*19);
    glVertex2f(initX+heart_block_width*15, initY-heart_block_width*18);
    glVertex2f(initX+heart_block_width*13, initY-heart_block_width*18);

    glVertex2f(initX+heart_block_width, initY-heart_block_width*20);
    glVertex2f(initX+heart_block_width*3, initY-heart_block_width*20);
    glVertex2f(initX+heart_block_width*3, initY-heart_block_width*19);
    glVertex2f(initX+heart_block_width, initY-heart_block_width*19);

    glVertex2f(initX+heart_block_width*12, initY-heart_block_width*20);
    glVertex2f(initX+heart_block_width*14, initY-heart_block_width*20);
    glVertex2f(initX+heart_block_width*14, initY-heart_block_width*19);
    glVertex2f(initX+heart_block_width*12, initY-heart_block_width*19);

    glVertex2f(initX+heart_block_width*2, initY-heart_block_width*21);
    glVertex2f(initX+heart_block_width*4, initY-heart_block_width*21);
    glVertex2f(initX+heart_block_width*4, initY-heart_block_width*20);
    glVertex2f(initX+heart_block_width*2, initY-heart_block_width*20);

    glVertex2f(initX+heart_block_width*11, initY-heart_block_width*21);
    glVertex2f(initX+heart_block_width*13, initY-heart_block_width*21);
    glVertex2f(initX+heart_block_width*13, initY-heart_block_width*20);
    glVertex2f(initX+heart_block_width*11, initY-heart_block_width*20);

    glVertex2f(initX+heart_block_width*3, initY-heart_block_width*22);
    glVertex2f(initX+heart_block_width*5, initY-heart_block_width*22);
    glVertex2f(initX+heart_block_width*5, initY-heart_block_width*21);
    glVertex2f(initX+heart_block_width*3, initY-heart_block_width*21);

    glVertex2f(initX+heart_block_width*10, initY-heart_block_width*22);
    glVertex2f(initX+heart_block_width*12, initY-heart_block_width*22);
    glVertex2f(initX+heart_block_width*12, initY-heart_block_width*21);
    glVertex2f(initX+heart_block_width*10, initY-heart_block_width*21);

    glVertex2f(initX+heart_block_width*5, initY-heart_block_width*23);
    glVertex2f(initX+heart_block_width*7, initY-heart_block_width*23);
    glVertex2f(initX+heart_block_width*7, initY-heart_block_width*22);
    glVertex2f(initX+heart_block_width*5, initY-heart_block_width*22);

    glVertex2f(initX+heart_block_width*8, initY-heart_block_width*23);
    glVertex2f(initX+heart_block_width*10, initY-heart_block_width*23);
    glVertex2f(initX+heart_block_width*10, initY-heart_block_width*22);
    glVertex2f(initX+heart_block_width*8, initY-heart_block_width*22);

    glVertex2f(initX+heart_block_width*6, initY-heart_block_width*24);
    glVertex2f(initX+heart_block_width*9, initY-heart_block_width*24);
    glVertex2f(initX+heart_block_width*9, initY-heart_block_width*23);
    glVertex2f(initX+heart_block_width*6, initY-heart_block_width*23);


    glColor3ub(238, 43, 47);

    glVertex2f(initX, initY-heart_block_width);
    glVertex2f(initX+(heart_block_width*4), initY-heart_block_width);
    glVertex2f(initX+(heart_block_width*4), initY);
    glVertex2f(initX, initY);

    glVertex2f(initX+(heart_block_width*11), initY-heart_block_width);
    glVertex2f(initX+(heart_block_width*15), initY-heart_block_width);
    glVertex2f(initX+(heart_block_width*15), initY);
    glVertex2f(initX+(heart_block_width*11), initY);

    glVertex2f(initX-(heart_block_width*2), initY-heart_block_width*2);
    glVertex2f(initX+(heart_block_width*6), initY-heart_block_width*2);
    glVertex2f(initX+(heart_block_width*6), initY-heart_block_width);
    glVertex2f(initX-(heart_block_width*2), initY-heart_block_width);

    glVertex2f(initX+(heart_block_width*9), initY-heart_block_width*2);
    glVertex2f(initX+(heart_block_width*12), initY-heart_block_width*2);
    glVertex2f(initX+(heart_block_width*12), initY-heart_block_width);
    glVertex2f(initX+(heart_block_width*9), initY-heart_block_width);

    glVertex2f(initX+(heart_block_width*13), initY-heart_block_width*2);
    glVertex2f(initX+(heart_block_width*17), initY-heart_block_width*2);
    glVertex2f(initX+(heart_block_width*17), initY-heart_block_width);
    glVertex2f(initX+(heart_block_width*13), initY-heart_block_width);

    glVertex2f(initX-(heart_block_width*2), initY-heart_block_width*3);
    glVertex2f(initX, initY-heart_block_width*3);
    glVertex2f(initX, initY-heart_block_width*2);
    glVertex2f(initX-(heart_block_width*2), initY-heart_block_width*2);

    glVertex2f(initX+(heart_block_width*2), initY-heart_block_width*3);
    glVertex2f(initX+(heart_block_width*7), initY-heart_block_width*3);
    glVertex2f(initX+(heart_block_width*7), initY-heart_block_width*2);
    glVertex2f(initX+(heart_block_width*2), initY-heart_block_width*2);

    glVertex2f(initX+(heart_block_width*9), initY-heart_block_width*3);
    glVertex2f(initX+(heart_block_width*11), initY-heart_block_width*3);
    glVertex2f(initX+(heart_block_width*11), initY-heart_block_width*2);
    glVertex2f(initX+(heart_block_width*9), initY-heart_block_width*2);

    glVertex2f(initX+(heart_block_width*14), initY-heart_block_width*3);
    glVertex2f(initX+(heart_block_width*17), initY-heart_block_width*3);
    glVertex2f(initX+(heart_block_width*17), initY-heart_block_width*2);
    glVertex2f(initX+(heart_block_width*14), initY-heart_block_width*2);

    glVertex2f(initX-(heart_block_width*3), initY-heart_block_width*4);
    glVertex2f(initX-heart_block_width, initY-heart_block_width*4);
    glVertex2f(initX-heart_block_width, initY-heart_block_width*3);
    glVertex2f(initX-(heart_block_width*3), initY-heart_block_width*3);

    glVertex2f(initX+heart_block_width*2, initY-heart_block_width*4);
    glVertex2f(initX+heart_block_width*2, initY-heart_block_width*4);
    glVertex2f(initX+heart_block_width*2, initY-heart_block_width*3);
    glVertex2f(initX+heart_block_width*2, initY-heart_block_width*3);

    glVertex2f(initX+heart_block_width*3, initY-heart_block_width*4);
    glVertex2f(initX+heart_block_width*10, initY-heart_block_width*4);
    glVertex2f(initX+heart_block_width*10, initY-heart_block_width*3);
    glVertex2f(initX+heart_block_width*3, initY-heart_block_width*3);

    glVertex2f(initX+heart_block_width*13, initY-heart_block_width*4);
    glVertex2f(initX+heart_block_width*18, initY-heart_block_width*4);
    glVertex2f(initX+heart_block_width*18, initY-heart_block_width*3);
    glVertex2f(initX+heart_block_width*13, initY-heart_block_width*3);

    glVertex2f(initX-(heart_block_width*3), initY-heart_block_width*5);
    glVertex2f(initX-heart_block_width, initY-heart_block_width*5);
    glVertex2f(initX-heart_block_width, initY-heart_block_width*4);
    glVertex2f(initX-(heart_block_width*3), initY-heart_block_width*4);

    glVertex2f(initX+heart_block_width, initY-heart_block_width*5);
    glVertex2f(initX+heart_block_width*10, initY-heart_block_width*5);
    glVertex2f(initX+heart_block_width*10, initY-heart_block_width*4);
    glVertex2f(initX+heart_block_width, initY-heart_block_width*4);

    glVertex2f(initX+heart_block_width*12, initY-heart_block_width*5);
    glVertex2f(initX+heart_block_width*18, initY-heart_block_width*5);
    glVertex2f(initX+heart_block_width*18, initY-heart_block_width*4);
    glVertex2f(initX+heart_block_width*12, initY-heart_block_width*4);

    glVertex2f(initX-(heart_block_width*4), initY-heart_block_width*6);
    glVertex2f(initX-(heart_block_width*2), initY-heart_block_width*6);
    glVertex2f(initX-(heart_block_width*2), initY-heart_block_width*5);
    glVertex2f(initX-(heart_block_width*4), initY-heart_block_width*5);

    glVertex2f(initX+heart_block_width*18, initY-heart_block_width*6);
    glVertex2f(initX+heart_block_width, initY-heart_block_width*6);
    glVertex2f(initX+heart_block_width, initY-heart_block_width*5);
    glVertex2f(initX+heart_block_width*18, initY-heart_block_width*5);

    glVertex2f(initX-(heart_block_width*4), initY-heart_block_width*7);
    glVertex2f(initX-(heart_block_width*2), initY-heart_block_width*7);
    glVertex2f(initX-(heart_block_width*2), initY-heart_block_width*6);
    glVertex2f(initX-(heart_block_width*4), initY-heart_block_width*6);

    glVertex2f(initX, initY-heart_block_width*7);
    glVertex2f(initX+(heart_block_width*7), initY-heart_block_width*7);
    glVertex2f(initX+(heart_block_width*7), initY-heart_block_width*6);
    glVertex2f(initX, initY-heart_block_width*6);

    glVertex2f(initX+(heart_block_width*9), initY-heart_block_width*7);
    glVertex2f(initX+(heart_block_width*18), initY-heart_block_width*7);
    glVertex2f(initX+(heart_block_width*18), initY-heart_block_width*6);
    glVertex2f(initX+(heart_block_width*9), initY-heart_block_width*6);

    glVertex2f(initX-(heart_block_width*4), initY-heart_block_width*8);
    glVertex2f(initX-(heart_block_width*2), initY-heart_block_width*8);
    glVertex2f(initX-(heart_block_width*2), initY-heart_block_width*7);
    glVertex2f(initX-(heart_block_width*4), initY-heart_block_width*7);

    glVertex2f(initX+heart_block_width*17, initY-heart_block_width*8);
    glVertex2f(initX-heart_block_width, initY-heart_block_width*8);
    glVertex2f(initX-heart_block_width, initY-heart_block_width*7);
    glVertex2f(initX+heart_block_width*17, initY-heart_block_width*7);

    glVertex2f(initX-(heart_block_width*4), initY-heart_block_width*9);
    glVertex2f(initX+(heart_block_width*17), initY-heart_block_width*9);
    glVertex2f(initX+(heart_block_width*17), initY-heart_block_width*8);
    glVertex2f(initX-(heart_block_width*4), initY-heart_block_width*8);

    glVertex2f(initX-(heart_block_width*4), initY-heart_block_width*10);
    glVertex2f(initX-(heart_block_width*3), initY-heart_block_width*10);
    glVertex2f(initX-(heart_block_width*3), initY-heart_block_width*9);
    glVertex2f(initX-(heart_block_width*4), initY-heart_block_width*9);

    glVertex2f(initX-(heart_block_width*3), initY-heart_block_width*10);
    glVertex2f(initX+(heart_block_width*17), initY-heart_block_width*10);
    glVertex2f(initX+(heart_block_width*17), initY-heart_block_width*9);
    glVertex2f(initX-(heart_block_width*3), initY-heart_block_width*9);

    glVertex2f(initX-(heart_block_width*2), initY-heart_block_width*11);
    glVertex2f(initX+(heart_block_width*16), initY-heart_block_width*11);
    glVertex2f(initX+(heart_block_width*16), initY-heart_block_width*10);
    glVertex2f(initX-(heart_block_width*2), initY-heart_block_width*10);

    glVertex2f(initX-(heart_block_width*3), initY-heart_block_width*12);
    glVertex2f(initX+(heart_block_width*15), initY-heart_block_width*12);
    glVertex2f(initX+(heart_block_width*15), initY-heart_block_width*11);
    glVertex2f(initX-(heart_block_width*3), initY-heart_block_width*11);

    glVertex2f(initX-(heart_block_width*3), initY-heart_block_width*13);
    glVertex2f(initX+(heart_block_width*15), initY-heart_block_width*13);
    glVertex2f(initX+(heart_block_width*15), initY-heart_block_width*12);
    glVertex2f(initX-(heart_block_width*3), initY-heart_block_width*12);

    glVertex2f(initX-heart_block_width, initY-heart_block_width*14);
    glVertex2f(initX+heart_block_width*14, initY-heart_block_width*14);
    glVertex2f(initX+heart_block_width*14, initY-heart_block_width*13);
    glVertex2f(initX-heart_block_width, initY-heart_block_width*13);

    glVertex2f(initX, initY-heart_block_width*15);
    glVertex2f(initX+(heart_block_width*13), initY-heart_block_width*15);
    glVertex2f(initX+(heart_block_width*13), initY-heart_block_width*14);
    glVertex2f(initX, initY-heart_block_width*14);

    glVertex2f(initX-heart_block_width, initY-heart_block_width*16);
    glVertex2f(initX+heart_block_width*12, initY-heart_block_width*16);
    glVertex2f(initX+heart_block_width*12, initY-heart_block_width*15);
    glVertex2f(initX-heart_block_width, initY-heart_block_width*15);

    glVertex2f(initX+(heart_block_width*3), initY-heart_block_width*17);
    glVertex2f(initX+(heart_block_width*10), initY-heart_block_width*17);
    glVertex2f(initX+(heart_block_width*10), initY-heart_block_width*16);
    glVertex2f(initX+(heart_block_width*3), initY-heart_block_width*16);


    glColor3ub(255, 255, 255);


    glVertex2f(initX+(heart_block_width*12), initY-heart_block_width*2);
    glVertex2f(initX+(heart_block_width*13), initY-heart_block_width*2);
    glVertex2f(initX+(heart_block_width*13), initY-heart_block_width);
    glVertex2f(initX+(heart_block_width*12), initY-heart_block_width);

    glVertex2f(initX, initY-heart_block_width*3);
    glVertex2f(initX+(heart_block_width*2), initY-heart_block_width*3);
    glVertex2f(initX+(heart_block_width*2), initY-heart_block_width*2);
    glVertex2f(initX, initY-heart_block_width*2);

    glVertex2f(initX+(heart_block_width*11), initY-heart_block_width*3);
    glVertex2f(initX+(heart_block_width*14), initY-heart_block_width*3);
    glVertex2f(initX+(heart_block_width*14), initY-heart_block_width*2);
    glVertex2f(initX+(heart_block_width*11), initY-heart_block_width*2);


    glVertex2f(initX-heart_block_width, initY-heart_block_width*4);
    glVertex2f(initX+heart_block_width*3, initY-heart_block_width*4);
    glVertex2f(initX+heart_block_width*3, initY-heart_block_width*3);
    glVertex2f(initX-heart_block_width, initY-heart_block_width*3);

    glVertex2f(initX+heart_block_width*10, initY-heart_block_width*4);
    glVertex2f(initX+heart_block_width*13, initY-heart_block_width*4);
    glVertex2f(initX+heart_block_width*13, initY-heart_block_width*3);
    glVertex2f(initX+heart_block_width*10, initY-heart_block_width*3);

    glVertex2f(initX-heart_block_width, initY-heart_block_width*5);
    glVertex2f(initX+heart_block_width, initY-heart_block_width*5);
    glVertex2f(initX+heart_block_width, initY-heart_block_width*4);
    glVertex2f(initX-heart_block_width, initY-heart_block_width*4);

    glVertex2f(initX+heart_block_width*10, initY-heart_block_width*5);
    glVertex2f(initX+heart_block_width*12, initY-heart_block_width*5);
    glVertex2f(initX+heart_block_width*12, initY-heart_block_width*4);
    glVertex2f(initX+heart_block_width*10, initY-heart_block_width*4);

    glVertex2f(initX-(heart_block_width*2), initY-heart_block_width*6);
    glVertex2f(initX+heart_block_width, initY-heart_block_width*6);
    glVertex2f(initX+heart_block_width, initY-heart_block_width*5);
    glVertex2f(initX-(heart_block_width*2), initY-heart_block_width*5);


    glVertex2f(initX-(heart_block_width*2), initY-heart_block_width*7);
    glVertex2f(initX, initY-heart_block_width*7);
    glVertex2f(initX, initY-heart_block_width*6);
    glVertex2f(initX-(heart_block_width*2), initY-heart_block_width*6);

    glVertex2f(initX+(heart_block_width*7), initY-heart_block_width*7);
    glVertex2f(initX+(heart_block_width*9), initY-heart_block_width*7);
    glVertex2f(initX+(heart_block_width*9), initY-heart_block_width*6);
    glVertex2f(initX+(heart_block_width*7), initY-heart_block_width*6);

    glVertex2f(initX-(heart_block_width*2), initY-heart_block_width*8);
    glVertex2f(initX-heart_block_width, initY-heart_block_width*8);
    glVertex2f(initX-heart_block_width, initY-heart_block_width*7);
    glVertex2f(initX-(heart_block_width*2), initY-heart_block_width*7);


    glVertex2f(initX-(heart_block_width*3), initY-heart_block_width*10);
    glVertex2f(initX-(heart_block_width*2), initY-heart_block_width*10);
    glVertex2f(initX-(heart_block_width*2), initY-heart_block_width*9);
    glVertex2f(initX-(heart_block_width*3), initY-heart_block_width*9);

    glVertex2f(initX-(heart_block_width*3), initY-heart_block_width*11);
    glVertex2f(initX-(heart_block_width*2), initY-heart_block_width*11);
    glVertex2f(initX-(heart_block_width*2), initY-heart_block_width*10);
    glVertex2f(initX-(heart_block_width*3), initY-heart_block_width*10);


    glColor3ub(196, 33, 38);

    glVertex2f(initX+(heart_block_width*17), initY-heart_block_width*3);
    glVertex2f(initX+(heart_block_width*18), initY-heart_block_width*3);
    glVertex2f(initX+(heart_block_width*18), initY-heart_block_width*2);
    glVertex2f(initX+(heart_block_width*17), initY-heart_block_width*2);

    glVertex2f(initX+heart_block_width*18, initY-heart_block_width*5);
    glVertex2f(initX+heart_block_width*19, initY-heart_block_width*5);
    glVertex2f(initX+heart_block_width*19, initY-heart_block_width*4);
    glVertex2f(initX+heart_block_width*18, initY-heart_block_width*4);

    glVertex2f(initX+heart_block_width*18, initY-heart_block_width*6);
    glVertex2f(initX+heart_block_width*19, initY-heart_block_width*6);
    glVertex2f(initX+heart_block_width*19, initY-heart_block_width*5);
    glVertex2f(initX+heart_block_width*18, initY-heart_block_width*5);

    glVertex2f(initX+(heart_block_width*18), initY-heart_block_width*7);
    glVertex2f(initX+(heart_block_width*19), initY-heart_block_width*7);
    glVertex2f(initX+(heart_block_width*19), initY-heart_block_width*6);
    glVertex2f(initX+(heart_block_width*18), initY-heart_block_width*6);

    glVertex2f(initX+heart_block_width*17, initY-heart_block_width*8);
    glVertex2f(initX+heart_block_width*19, initY-heart_block_width*8);
    glVertex2f(initX+heart_block_width*19, initY-heart_block_width*7);
    glVertex2f(initX+heart_block_width*17, initY-heart_block_width*7);

    glVertex2f(initX+heart_block_width*17, initY-heart_block_width*9);
    glVertex2f(initX+heart_block_width*19, initY-heart_block_width*9);
    glVertex2f(initX+heart_block_width*19, initY-heart_block_width*8);
    glVertex2f(initX+heart_block_width*17, initY-heart_block_width*8);

    glVertex2f(initX+(heart_block_width*17), initY-heart_block_width*10);
    glVertex2f(initX+(heart_block_width*19), initY-heart_block_width*10);
    glVertex2f(initX+(heart_block_width*19), initY-heart_block_width*9);
    glVertex2f(initX+(heart_block_width*17), initY-heart_block_width*9);

    glVertex2f(initX+(heart_block_width*16), initY-heart_block_width*11);
    glVertex2f(initX+(heart_block_width*19), initY-heart_block_width*11);
    glVertex2f(initX+(heart_block_width*19), initY-heart_block_width*10);
    glVertex2f(initX+(heart_block_width*16), initY-heart_block_width*10);

    glVertex2f(initX+(heart_block_width*15), initY-heart_block_width*12);
    glVertex2f(initX+(heart_block_width*18), initY-heart_block_width*12);
    glVertex2f(initX+(heart_block_width*18), initY-heart_block_width*11);
    glVertex2f(initX+(heart_block_width*15), initY-heart_block_width*11);

    glVertex2f(initX-(heart_block_width*3), initY-heart_block_width*13);
    glVertex2f(initX-(heart_block_width*2), initY-heart_block_width*13);
    glVertex2f(initX-(heart_block_width*2), initY-heart_block_width*12);
    glVertex2f(initX-(heart_block_width*3), initY-heart_block_width*12);

    glVertex2f(initX+(heart_block_width*15), initY-heart_block_width*13);
    glVertex2f(initX+(heart_block_width*18), initY-heart_block_width*13);
    glVertex2f(initX+(heart_block_width*18), initY-heart_block_width*12);
    glVertex2f(initX+(heart_block_width*15), initY-heart_block_width*12);

    glVertex2f(initX-(heart_block_width*2), initY-heart_block_width*14);
    glVertex2f(initX-heart_block_width, initY-heart_block_width*14);
    glVertex2f(initX-heart_block_width, initY-heart_block_width*13);
    glVertex2f(initX-(heart_block_width*2), initY-heart_block_width*13);

    glVertex2f(initX+heart_block_width*14, initY-heart_block_width*14);
    glVertex2f(initX+heart_block_width*17, initY-heart_block_width*14);
    glVertex2f(initX+heart_block_width*17, initY-heart_block_width*13);
    glVertex2f(initX+heart_block_width*14, initY-heart_block_width*13);

    glVertex2f(initX-(heart_block_width*2), initY-heart_block_width*15);
    glVertex2f(initX, initY-heart_block_width*15);
    glVertex2f(initX, initY-heart_block_width*14);
    glVertex2f(initX-(heart_block_width*2), initY-heart_block_width*14);

    glVertex2f(initX+(heart_block_width*13), initY-heart_block_width*15);
    glVertex2f(initX+(heart_block_width*17), initY-heart_block_width*15);
    glVertex2f(initX+(heart_block_width*17), initY-heart_block_width*14);
    glVertex2f(initX+(heart_block_width*13), initY-heart_block_width*14);

    glVertex2f(initX-heart_block_width, initY-heart_block_width*16);
    glVertex2f(initX+heart_block_width, initY-heart_block_width*16);
    glVertex2f(initX+heart_block_width, initY-heart_block_width*15);
    glVertex2f(initX-heart_block_width, initY-heart_block_width*15);

    glVertex2f(initX+heart_block_width*12, initY-heart_block_width*16);
    glVertex2f(initX+heart_block_width*16, initY-heart_block_width*16);
    glVertex2f(initX+heart_block_width*16, initY-heart_block_width*15);
    glVertex2f(initX+heart_block_width*12, initY-heart_block_width*15);

    glVertex2f(initX, initY-heart_block_width*17);
    glVertex2f(initX+(heart_block_width*3), initY-heart_block_width*17);
    glVertex2f(initX+(heart_block_width*3), initY-heart_block_width*16);
    glVertex2f(initX, initY-heart_block_width*16);

    glVertex2f(initX+(heart_block_width*10), initY-heart_block_width*17);
    glVertex2f(initX+(heart_block_width*15), initY-heart_block_width*17);
    glVertex2f(initX+(heart_block_width*15), initY-heart_block_width*16);
    glVertex2f(initX+(heart_block_width*10), initY-heart_block_width*16);

    glVertex2f(initX+heart_block_width, initY-heart_block_width*18);
    glVertex2f(initX+heart_block_width*14, initY-heart_block_width*18);
    glVertex2f(initX+heart_block_width*14, initY-heart_block_width*17);
    glVertex2f(initX+heart_block_width, initY-heart_block_width*17);


    glVertex2f(initX+heart_block_width*2, initY-heart_block_width*19);
    glVertex2f(initX+heart_block_width*13, initY-heart_block_width*19);
    glVertex2f(initX+heart_block_width*13, initY-heart_block_width*18);
    glVertex2f(initX+heart_block_width*2, initY-heart_block_width*18);

    glVertex2f(initX+heart_block_width*3, initY-heart_block_width*20);
    glVertex2f(initX+heart_block_width*12, initY-heart_block_width*20);
    glVertex2f(initX+heart_block_width*12, initY-heart_block_width*19);
    glVertex2f(initX+heart_block_width*3, initY-heart_block_width*19);

    glVertex2f(initX+heart_block_width*4, initY-heart_block_width*21);
    glVertex2f(initX+heart_block_width*11, initY-heart_block_width*21);
    glVertex2f(initX+heart_block_width*11, initY-heart_block_width*20);
    glVertex2f(initX+heart_block_width*4, initY-heart_block_width*20);

    glVertex2f(initX+heart_block_width*5, initY-heart_block_width*22);
    glVertex2f(initX+heart_block_width*10, initY-heart_block_width*22);
    glVertex2f(initX+heart_block_width*10, initY-heart_block_width*21);
    glVertex2f(initX+heart_block_width*5, initY-heart_block_width*21);

    glVertex2f(initX+heart_block_width*7, initY-heart_block_width*23);
    glVertex2f(initX+heart_block_width*8, initY-heart_block_width*23);
    glVertex2f(initX+heart_block_width*8, initY-heart_block_width*22);
    glVertex2f(initX+heart_block_width*7, initY-heart_block_width*22);


    glEnd();
}



