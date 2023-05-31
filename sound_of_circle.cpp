// Reference https://www.youtube.com/watch?v=Kt3DavtVGVE

#include "raylib.h"
#include <math.h>

#define APP_TITLE           "Sound of Arcs"
#define WINDOW_WIDTH        1280
#define WINDOW_HEIGHT       720
#define FONT_SIZE           40

#define BOTTOM_PADDING      100
#define SIDE_PADDING        60

#define ARC_SPACING         25
#define ARC_WIDTH           5
#define NUMBER_OF_ARCS      21

#define RINGER_RADIUS       10

#define LINE_WIDTH          4

const int MAX_ARC_RADIUS = WINDOW_WIDTH/2;

struct Ringer
{
    double angle;
    Color color;
    Sound sound;
    double velocity;
    double nextImpactTime;
};


struct Arc
{
    int radius;
    Color color;
    Ringer ringer;
};


Arc arcs[NUMBER_OF_ARCS];

#define ARC_COLOR        GRAY
#define RINGER_COLOR     WHITE
#define LINE_COLOR       ARC_COLOR

// todo: sound generation
// todo: color generation
// todo: (maybe) show text of how much "ring" it would take to finish cycle for each ringer

double to_radians(double angle){
    return M_PI * angle / 180.0;
}


#define MIN_LOOP                2
const double SECONDS_FOR_ONE_LOOP = 60;
const double VELOCITY_OF_ONE_LOOP = 2*M_PI/SECONDS_FOR_ONE_LOOP;

#define FRAMES_PER_SECOND       30

double getRingerSpeedByNumberOfLoops(int i) {
    return (MIN_LOOP + (NUMBER_OF_ARCS - i))* VELOCITY_OF_ONE_LOOP;
}

// starts drawing from inner arc first
void setRadius(int i) {
    arcs[i].radius = (int)(MAX_ARC_RADIUS - (NUMBER_OF_ARCS - i) * ARC_SPACING - SIDE_PADDING);
}

double getNextImpactTime(double currentImpactTime, double velocity){
    return currentImpactTime + M_PI/velocity;
}

void setSound(int i){
    if(i%8 == 0) {
        arcs[i].ringer.sound = LoadSound("sounds/kalimba_f3.wav");
    }
    if(i%8 == 1) {
        arcs[i].ringer.sound = LoadSound("sounds/kalimba_g3.wav");
    }
    if(i%8 == 2) {
        arcs[i].ringer.sound = LoadSound("sounds/kalimba_a3.wav");
    }
    if(i%8 == 3) {
        arcs[i].ringer.sound = LoadSound("sounds/kalimba_b3.wav");
    }
    if(i%8 == 4) {
        arcs[i].ringer.sound = LoadSound("sounds/kalimba_c4.wav");
    }
    if(i%8 == 5) {
        arcs[i].ringer.sound = LoadSound("sounds/kalimba_d3.wav");
    }
    if(i%8 == 6) {
        arcs[i].ringer.sound = LoadSound("sounds/kalimba_d4.wav");
    }
    if(i%8 == 7) {
        arcs[i].ringer.sound = LoadSound("sounds/kalimba_e3.wav");
    }
}

void setupData() {
     for(int i=0; i< NUMBER_OF_ARCS; i++) {
        arcs[i].ringer.angle = 0.0;
        setRadius(i);
        setSound(i);
        double velocity = getRingerSpeedByNumberOfLoops(i);
        arcs[i].ringer.velocity = velocity;
        arcs[i].ringer.nextImpactTime = getNextImpactTime(0.0, velocity);
        
        arcs[i].color = ARC_COLOR;
        arcs[i].ringer.color = RINGER_COLOR;
    }
}

bool isWithinDelta(double value, double compareValue, double delta){
    return value - delta < compareValue && value + delta > compareValue;
}

int main(void)
{   
    // Need anti-aliasing in order to make arcs more smother 
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, APP_TITLE);
    InitAudioDevice();   

    //Arc center
    Vector2 center;
    center.x = WINDOW_WIDTH/2;
    center.y = WINDOW_HEIGHT - BOTTOM_PADDING;
    
    SetTargetFPS(FRAMES_PER_SECOND);   
    
    setupData();
    
    int titleTextWidth = MeasureText(APP_TITLE, FONT_SIZE);
    int playCount = 0;
    while (!WindowShouldClose())
    {
        BeginDrawing();
            ClearBackground(BLACK);
            DrawRectangle(
                SIDE_PADDING,
                center.y + LINE_WIDTH/2,
                WINDOW_WIDTH - 2*SIDE_PADDING, 
                LINE_WIDTH, 
                LINE_COLOR
            );  
            double time = GetTime();
            for(int i = 0; i < NUMBER_OF_ARCS; i++) {
                int radius = arcs[i].radius;
                double velocity = arcs[i].ringer.velocity;
                double angle = time * velocity;
                Sound sound = arcs[i].ringer.sound;
                
                DrawRing(
                    center, 
                    radius - ARC_WIDTH/2, 
                    radius + ARC_WIDTH/2, 
                    270, 
                    90, 
                    0, 
                    arcs[i].color
                );   
                
                int x = radius * cos(angle) + center.x;
                int y = (-1)*radius * abs(sin(angle)) + center.y;
                
               double nextImpactTime = arcs[i].ringer.nextImpactTime;
               double deltaTime = GetFrameTime();

               //todo: test correctess of collision detection
               if(isWithinDelta(nextImpactTime, time, deltaTime)) {
                    arcs[i].ringer.nextImpactTime = getNextImpactTime(time, velocity);
                    playCount++;
                    PlaySound(sound);
               }
               // Other approach for detecting collision by angle
                // double previousAngle = (time - deltaTime)*velocity;
                // if(sin(previousAngle) >= 0.0 && sin(angle) <= 0.0) {
                //     playCount++;
                //     PlaySound(sound);
                // }
                //  if(sin(previousAngle) <= 0.0 && sin(angle) >= 0.0) {
                //     playCount++;
                //     PlaySound(sound);
                // }

                DrawCircle(x, y, RINGER_RADIUS, arcs[i].ringer.color);
            }
            
            DrawText(TextFormat("FRAME: %.4f", time), 0, 0, FONT_SIZE, LIGHTGRAY);
            DrawText(TextFormat("playCount: %d", playCount), 0, FONT_SIZE, FONT_SIZE, LIGHTGRAY);
            DrawText(APP_TITLE, (WINDOW_WIDTH - titleTextWidth)/2, WINDOW_HEIGHT - FONT_SIZE - BOTTOM_PADDING/4, FONT_SIZE, LIGHTGRAY);

        EndDrawing();
    }
    CloseAudioDevice();
    CloseWindow();

    return 0;
}