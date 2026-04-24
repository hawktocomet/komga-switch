#pragma once

// ---- Server Configuration ----
// Server host/port/credentials are now stored at runtime in AppConfig
// and persisted to sdmc:/switch/komga-switch/config.json.

// ---- Display ----
#define SCREEN_W           1280

#define SCREEN_H           720

#define FONT_SIZE_LARGE    36

#define FONT_SIZE_MEDIUM   24

#define FONT_SIZE_SMALL    18

// ---- Reader Portrait Viewport ----
#define READER_VIEW_W      720

#define READER_VIEW_H      1280

// ---- Grid Layout ----
#define GRID_COLS          5

#define GRID_ROWS          2

#define THUMB_W            200

#define THUMB_H            280

#define GRID_PAD_X         30

#define GRID_PAD_Y         20

#define GRID_MARGIN_X      60

#define GRID_MARGIN_Y      80

// ---- Pagination ----
#define PAGE_SIZE          (GRID_COLS * GRID_ROWS)

// ---- Colors (RGBA) ----
#define COL_BG             {  18,  18,  24, 255 }

#define COL_PANEL          {  28,  28,  38, 255 }

#define COL_ACCENT         {  99, 179, 237, 255 }

#define COL_ACCENT2        { 246, 135,  79, 255 }

#define COL_TEXT           { 240, 240, 248, 255 }

#define COL_SUBTEXT        { 140, 140, 160, 255 }

#define COL_HIGHLIGHT      {  99, 179, 237,  60 }

#define COL_SEL_BORDER     {  99, 179, 237, 255 }

// ---- HTTP ----
#define HTTP_TIMEOUT_SEC   15L