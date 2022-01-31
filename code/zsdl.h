#ifndef ZSDL_H
#define ZSDL_H

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_Mixer.h"
#include "assets.h"
#include "zmath.h"

/*vvvvvvvvvvvvvvvvvvvvvvvvvv CAMERA vvvvvvvvvvvvvvvvvvvvvvvvvv*/
typedef struct Camera
{
    r2 pos;
    r2 aim;
    i2 off;
    r32 zoom;
} Camera;

Camera* CreateCamera(r2 pos);
void FreeCamera(Camera* camera);




/*^^^^^^^^^^^^^^^^^^^^^^^^^^ CAMERA ^^^^^^^^^^^^^^^^^^^^^^^^^^*/


/*vvvvvvvvvvvvvvvvvvvvvvvvvv VIEWPORT vvvvvvvvvvvvvvvvvvvvvvvvvv*/
// 1/5th of full HD(1920x1080) =  384 x 216
#define ZSDL_INTERNAL_WIDTH 384
#define ZSDL_INTERNAL_HEIGHT 216
#define ZSDL_INTERNAL_HALFWIDTH 192
#define ZSDL_INTERNAL_HALFHEIGHT 108

#define ZSDL_RENDERLAYERS_MAX 5
typedef enum
{
    ZSDL_RENDERLAYER_BACKGROUND,
    ZSDL_RENDERLAYER_ENTITIES,
    ZSDL_RENDERLAYER_FOREGROUND,
    ZSDL_RENDERLAYER_UI,
    ZSDL_RENDERLAYER_POST_PROCESS,
}ZSDL_RENDERLAYER;


// viewport settings u64 layout
// bits for single flags and bytes for numbers
// bitpos   63                                                                            0
// bytepos  ____8____ ____7____ ____6____ ____5____ ____4____ ____3____ ____1____ ____0____
//          0000'0000 0000'0000 0000'0000 0000'0000 0000'0000 0000'0000 0000'0000 0000'0000
#define ZSDL_SETTINGS_BIT_SCANLINEFILTER 8
#define ZSDL_SETTINGS_BYTE_PIXELSCALE 6
#define ZSDL_SETTINGS_BYTE_ACTIVE_CURSOR 5
typedef struct
{
    SDL_Window*     window;
    SDL_Surface*    surface;
    SDL_Renderer*   renderer;
    SDL_Texture*    render_layer[ZSDL_RENDERLAYERS_MAX];
    Camera*         camera;
    SDL_Rect        screen;
    u64             settings;
} Viewport;

b8 SetupSDL();
Viewport* CreateViewport(const char* window_title);
void FreeViewport(Viewport* viewport);
void ComputePixelScale(Viewport* viewport);
void CalculateScreen(Viewport* viewport);
void ToggleFullscreen(Viewport* viewport);


/*^^^^^^^^^^^^^^^^^^^^^^^^^^ VIEWPORT ^^^^^^^^^^^^^^^^^^^^^^^^^^*/

/*vvvvvvvvvvvvvvvvvvvvvvvvvv SPEAKER vvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*^^^^^^^^^^^^^^^^^^^^^^^^^^ SPEAKER ^^^^^^^^^^^^^^^^^^^^^^^^^^*/

/*vvvvvvvvvvvvvvvvvvvvvvvvvv FONT vvvvvvvvvvvvvvvvvvvvvvvvvv*/
#define ZFONT_DEFAULT_MAX_COL 16
#define ZFONT_DEFAULT_MAX_ROW 6
#define ZFONT_ASCII_OFFSET 32
typedef struct
{
    SDL_Texture* glyphs;
    i2 siz;
    i2 spacing;
} zFont;

void DrawTextWorld(Viewport* viewport, zFont* font, SDL_Color color, r2 pos, const char* text);
void DrawTextScreen(Viewport* viewport, zFont* font, SDL_Color color, i2 loc, const char* text);
/*^^^^^^^^^^^^^^^^^^^^^^^^^^ FONT ^^^^^^^^^^^^^^^^^^^^^^^^^^*/

/*vvvvvvvvvvvvvvvvvvvvvvvvvv ASSETBANK vvvvvvvvvvvvvvvvvvvvvvvvvv*/
#define ASSETBANK_TEXTURES_MAX 16
#define ASSETBANK_SOUNDS_MAX 32
#define ASSETBANK_MUSIC_MAX 2
#define ASSETBANK_CURSORS_MAX 4
#define ASSETBANK_SURFACES_MAX 6
#define ASSETBANK_FONTS_MAX 2
#define ASSETBANK_STRINGS_MAX 1

typedef struct
{
	SDL_Texture* tex[ASSETBANK_TEXTURES_MAX];
	Mix_Chunk* sfx[ASSETBANK_SOUNDS_MAX];
    Mix_Music* mus[ASSETBANK_MUSIC_MAX];
	SDL_Cursor* cur[ASSETBANK_CURSORS_MAX];
	SDL_Surface* sur[ASSETBANK_SURFACES_MAX];
	zFont* fon[ASSETBANK_FONTS_MAX];
	char* str[ASSETBANK_STRINGS_MAX];
} Assets;

Assets* CreateAssets(Viewport* viewport);
void FreeAssets(Assets* assets);
void LoadSound(Assets* assets, i32 identifier, const char* path);
void LoadSurface(Assets* assets, i32 identifier, const char* path);
void LoadString(Assets* assets, i32 identifier, const char* path);
void LoadTexture(Assets* assets, i32 identifier, SDL_Renderer* renderer, const char* path);
void LoadCursor(Assets* assets, i32 identifier, i32 cursor_hotspot_x, i32 cursor_hotspot_y, const char* path);
void LoadFont(Assets* assets, i32 identifier, SDL_Renderer* renderer, const char* path);
/*^^^^^^^^^^^^^^^^^^^^^^^^^^ ASSETBANK ^^^^^^^^^^^^^^^^^^^^^^^^^^*/


/*vvvvvvvvvvvvvvvvvvvvvvvvvv INPUT CONTROLLER vvvvvvvvvvvvvvvvvvvvvvvvvv*/
// ACTION MASKS
//  usage: if (Actions.player & ACTION(A_PLR_JMP) ) -> doJumpAction
//  store current actions in lower 32 bits and previous actions in upper 32 bits
#define ACTION(X) ((u64)1 << ((X)-1))
#define ACTION_PRE(X) ((u64)1 << (((X)-1) + 32))

#define A_QUIT 1
#define A_PLAY 2
#define A_EDIT 3
#define A_DBUG 4
#define A_JUMP 5
#define A_WHLU 6 // mouse wheel up
#define A_WHLD 7
#define A_SHFT 8
#define A_MB_L 9
#define A_MB_R 10
#define A_MOVL 11
#define A_MOVR 12
#define A_MOVU 13
#define A_MOVD 14
#define A_FSCR 15 // fullscreen
#define A_ESC 16
#define A_RSIZ 17 // resize window

typedef struct 
{
    u64 actions;
	i2  move_vector;
	i2  directional_vector;
	i2  mouse_location;
} Controller;

Controller* CreateController();
void FreeController(Controller* controller);

void CollectInput(Controller* c);
i2 MouseLocation(Controller* c, Viewport* viewport);
b8 ActionPressed(Controller* c, u64 action);
b8 ActionReleased(Controller* c, u64 action);
b8 ActionHeld(Controller* c, u64 action);
/*^^^^^^^^^^^^^^^^^^^^^^^^^^ INPUT CONTROLLER ^^^^^^^^^^^^^^^^^^^^^^^^^^*/


/*vvvvvvvvvvvvvvvvvvvvvvvvvv DOT PARTICLES vvvvvvvvvvvvvvvvvvvvvvvvvv*/
#define DOTS_MAX 200
typedef struct
{
	u16 lifetime;
	u16 current_life;
	u8 r;
	u8 b;
	u8 g;
	u8 a;
	u8 r_0;
	u8 g_0;
	u8 b_0;
	u8 a_0;
	u8 r_1;
	u8 g_1;
	u8 b_1;
	u8 a_1;
	r2 acc;
	r2 vel;
	r2 pos;
} Dot;

typedef struct
{
	Dot dot[DOTS_MAX];
} Dots;

Dots* initDots();
b8 SpawnDot(Dots* dots, u16 lifetime, r2 pos, r2 vel, r2 acc, SDL_Color initial_color, SDL_Color final_color);
void tickDots(Dots* dots, u32 t, r32 dt);
void DrawDots(Viewport* viewport, u32 t, Dots* dots);
void FreeDots(Dots* dots);
/*^^^^^^^^^^^^^^^^^^^^^^^^^^ DOT PARTICLES ^^^^^^^^^^^^^^^^^^^^^^^^^^*/


/*vvvvvvvvvvvvvvvvvvvvvvvvvv GUI vvvvvvvvvvvvvvvvvvvvvvvvvv*/
#define BUTTON_STATE_MASK_CURR	0x0f
#define BUTTON_STATE_MASK_PREV	0xf0
#define BUTTON_STATE_MAX        6
typedef enum
{
    BUTTON_STATE_INACTIVE   = 0x00,
    BUTTON_STATE_ACTIVE     = 0x01,
    BUTTON_STATE_HOVERED    = 0x02,
    BUTTON_STATE_PRESSED    = 0x03,
    BUTTON_STATE_HELD       = 0x04,
    BUTTON_STATE_RELEASED   = 0x05
} E_BUTTON_STATE;
typedef struct 
{
    SDL_Rect src;
    SDL_Rect dst;
    b8       state;
} Button;

#define BTN_PLAY 0
#define BTN_QUIT 1
#define MENU_TITLE_NUM_BTN 2 //play, quit
#define MENU_VICTORY_NUM_BTN 2 //retry, quit
typedef struct
{
    Button* title[MENU_TITLE_NUM_BTN];
    Button* victory[MENU_VICTORY_NUM_BTN];
} Menu;

Button* CreateButton(SDL_Rect source, SDL_Rect destination);
void FreeButton(Button* button);
Menu* CreateMenu();
void FreeMenu(Menu* menu);

E_BUTTON_STATE ButtonStateTransition(Button* btn, E_BUTTON_STATE next_state);
char* ButtonStateName(E_BUTTON_STATE state);
/*^^^^^^^^^^^^^^^^^^^^^^^^^^ GUI ^^^^^^^^^^^^^^^^^^^^^^^^^^*/


/*vvvvvvvvvvvvvvvvvvvvvvvvvv CURSOR vvvvvvvvvvvvvvvvvvvvvvvvvv*/
#define ZSDL_CURSOR_BASE_SIZE 15
#define ZSDL_CURSOR_POINT_HOT_X 1
#define ZSDL_CURSOR_POINT_HOT_Y 1
#define ZSDL_CURSOR_HAND_HOT_X 3
#define ZSDL_CURSOR_HAND_HOT_Y 8
#define ZSDL_CURSOR_GRAB_HOT_X 3
#define ZSDL_CURSOR_GRAB_HOT_Y 8
#define ZSDL_CURSOR_CROSS_HOT_X 7
#define ZSDL_CURSOR_CROSS_HOT_Y 7
typedef enum
{
    ZSDL_CURSOR_POINT,
    ZSDL_CURSOR_HAND,
    ZSDL_CURSOR_GRAB,
    ZSDL_CURSOR_CROSS,
}ZSDL_CURSOR;

void SetCursor(Viewport* viewport, Assets* assets, u8 new_cursor);
void RefreshCursors(Viewport* viewport, Assets* assets);
/*^^^^^^^^^^^^^^^^^^^^^^^^^^ CURSOR ^^^^^^^^^^^^^^^^^^^^^^^^^^*/




/*vvvvvvvvvvvvvvvvvvvvvvvvvv RENDER SUPPORT FUNCTIONS vvvvvvvvvvvvvvvvvvvvvvvvvv*/
void BlurRenderlayer(Viewport* viewport, ZSDL_RENDERLAYER renderlayer, i32 amount, i32 opacity);
void CleanRenderTargets(Viewport* viewport);
void FinalizeRenderAndPresent(Viewport* viewport);

void DrawNumber(Viewport* viewport, SDL_Texture* texture, u32 number, i2 size_src, i2 size_dst, i2 location, u32 max_digits);

i2 PosToCam(r2 pos, Viewport* viewport);
r2 CamToPos(i2 cam, Viewport* viewport);

#define COLOR(r,g,b,a) (SDL_Color){r, g, b, a}
#define COLOR_RED COLOR(0xff, 0x00, 0x00, 0xff)
#define COLOR_GREEN COLOR(0x00, 0xff, 0x00, 0xff)
#define COLOR_BLUE COLOR(0x00, 0x00, 0xff, 0xff)
#define COLOR_YELLOW COLOR(0xff, 0xff, 0x00, 0xff)
#define COLOR_CYAN COLOR(0x00, 0xff, 0xff, 0xff)
#define COLOR_PURPLE COLOR(0xff, 0x00, 0xff, 0xff)
#define COLOR_BLACK COLOR(0x00, 0x00, 0x00, 0xff)
#define COLOR_WHITE COLOR(0xff, 0xff, 0xff, 0xff)
/*^^^^^^^^^^^^^^^^^^^^^^^^^^ RENDER SUPPORT FUNCTIONS ^^^^^^^^^^^^^^^^^^^^^^^^^^*/

#endif // ZSDL_H
