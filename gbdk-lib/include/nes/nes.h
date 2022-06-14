/** @file nes/nes.h
    NES specific functions.
*/
#ifndef _NES_H
#define _NES_H

#include <types.h>
#include <stdint.h>
#include <gbdk/version.h>
#include <nes/hardware.h>

#define NINTENDO_ENTERTAINMENT_SYSTEM
#ifdef SEGA
#undef SEGA
#endif

#define RGB(r,g,b)        ((r) | ((g) << 2) | ((b) << 4))
#define RGB8(r,g,b)       (((r) >> 6) | (((g) >> 6) << 2) | (((b) >> 6) << 4))
#define RGBHTML(RGB24bit) (((RGB24bit) >> 22) | ((((RGB24bit) & 0xFFFF) >> 14) << 2) | ((((RGB24bit) & 0xFF) >> 6) << 4))

/** Common colors based on the EGA default palette.
 */
#define RGB_RED        RGB( 3,  0,  0)
#define RGB_DARKRED    RGB( 2,  0,  0)
#define RGB_GREEN      RGB( 0,  3,  0)
#define RGB_DARKGREEN  RGB( 0,  2,  0)
#define RGB_BLUE       RGB( 0,  0,  3)
#define RGB_DARKBLUE   RGB( 0,  0,  2)
#define RGB_YELLOW     RGB( 3,  3,  0)
#define RGB_DARKYELLOW RGB( 2,  2,  0)
#define RGB_CYAN       RGB( 0,  3,  3)
#define RGB_AQUA       RGB( 3,  1,  2)
#define RGB_PINK       RGB( 3,  0,  3)
#define RGB_PURPLE     RGB( 2,  0,  2)
#define RGB_BLACK      RGB( 0,  0,  0)
#define RGB_DARKGRAY   RGB( 1,  1,  1)
#define RGB_LIGHTGRAY  RGB( 2,  2,  2)
#define RGB_WHITE      RGB( 3,  3,  3)

typedef uint8_t palette_color_t;

/** Joypad bits.
    A logical OR of these is used in the wait_pad and joypad
    functions.  For example, to see if the B button is pressed
    try

    uint8_t keys;
    keys = joypad();
    if (keys & J_B) {
        ...
    }

    @see joypad
 */
#define J_UP         0x10U
#define J_DOWN       0x20U
#define J_LEFT       0x40U
#define J_RIGHT      0x80U
#define J_A          0x01U
#define J_B          0x02U
#define J_SELECT     0x04U
#define J_START      0x08U

/** Screen modes.
    Normally used by internal functions only.
    @see mode()
 */
#define M_DRAWING    0x01U
#define M_TEXT_OUT   0x02U
#define M_TEXT_INOUT 0x03U
/** Set this in addition to the others to disable scrolling

    If scrolling is disabled, the cursor returns to (0,0)
    @see mode()
*/
#define M_NO_SCROLL  0x04U
/** Set this to disable interpretation
    @see mode()
*/
#define M_NO_INTERP  0x08U

/** If this is set, sprite colours come from OBJ1PAL. Else
    they come from OBJ0PAL
    @see set_sprite_prop().
*/
#define S_PALETTE    0x10U
/** If set the sprite will be flipped horizontally.
    @see set_sprite_prop()
 */
#define S_FLIPX      0x40U
/** If set the sprite will be flipped vertically.
    @see set_sprite_prop()
 */
#define S_FLIPY      0x80U
/** If this bit is clear, then the sprite will be displayed
    on top of the background and window.
    @see set_sprite_prop()
*/
#define S_PRIORITY   0x20U

/* DMG Palettes */
#define DMG_BLACK     0x03
#define DMG_DARK_GRAY 0x02
#define DMG_LITE_GRAY 0x01
#define DMG_WHITE     0x00
/** Macro to create a DMG palette from 4 colors

    @param C0    Color for Index 0
    @param C1    Color for Index 1
    @param C2    Color for Index 2
    @param C3    Color for Index 3

    The resulting format is four greyscale colors
    packed into a single unsigned byte.

    Example:
    \code{.c}
    REG_BGP = DMG_PALETTE(DMG_BLACK, DMG_DARK_GRAY, DMG_LITE_GRAY, DMG_WHITE);
    \endcode

    @see OBP0_REG, OBP1_REG, BGP_REG
    @see DMG_BLACK, DMG_DARK_GRAY, DMG_LITE_GRAY, DMG_WHITE

 */
#define DMG_PALETTE(C0, C1, C2, C3) ((uint8_t)((((C3) & 0x03) << 6) | (((C2) & 0x03) << 4) | (((C1) & 0x03) << 2) | ((C0) & 0x03)))

/* Limits */
/** Width of the visible screen in pixels.
 */
#define SCREENWIDTH  DEVICE_SCREEN_PX_WIDTH
/** Height of the visible screen in pixels.
 */
#define SCREENHEIGHT DEVICE_SCREEN_PX_HEIGHT

/** Set the current screen mode - one of M_* modes

    Normally used by internal functions only.

    @see M_DRAWING, M_TEXT_OUT, M_TEXT_INOUT, M_NO_SCROLL, M_NO_INTERP
*/
void mode(uint8_t m) OLDCALL;

/** Returns the current mode

    @see M_DRAWING, M_TEXT_OUT, M_TEXT_INOUT, M_NO_SCROLL, M_NO_INTERP
*/
uint8_t get_mode() OLDCALL;

/** Global Time Counter in VBL periods (60Hz)

    Increments once per Frame

    Will wrap around every ~18 minutes (unsigned 16 bits = 65535 / 60 / 60 = 18.2)
*/
extern volatile uint16_t sys_time;

/** Tracks current active ROM bank @see SWITCH_ROM_MBC1(), SWITCH_ROM_MBC5()
    This variable is updated automatically when you call SWITCH_ROM_MBC1 or
    SWITCH_ROM_MBC5, or call a BANKED function.
*/
extern volatile uint8_t _current_bank;
#define CURRENT_BANK _current_bank

/** Obtains the __bank number__ of VARNAME

    @param VARNAME Name of the variable which has a __bank_VARNAME companion symbol which is adjusted by bankpack

    Use this to obtain the bank number from a bank reference
    created with @ref BANKREF().

    @see BANKREF_EXTERN(), BANKREF()
*/
#ifndef BANK
#define BANK(VARNAME) 0
#endif

/** Creates a reference for retrieving the bank number of a variable or function

    @param VARNAME Variable name to use, which may be an existing identifier

    @see BANK() for obtaining the bank number of the included data.

    More than one `BANKREF()` may be created per file, but each call should
    always use a unique VARNAME.

    Use @ref BANKREF_EXTERN() within another source file
    to make the variable and it's data accesible there.
*/
#define BANKREF(VARNAME)

/** Creates extern references for accessing a BANKREF() generated variable.

    @param VARNAME Name of the variable used with @ref BANKREF()

    This makes a @ref BANKREF() reference in another source
    file accessible in the current file for use with @ref BANK().

    @see BANKREF(), BANK()
*/
#define BANKREF_EXTERN(VARNAME) extern const void __bank_ ## VARNAME;

/** Dummy macro for no-bank-switching WIP prototype
    @param b   ROM bank to switch to
*/
#define SWITCH_ROM_DUMMY(b) 

/** Makes default mapper switch the active ROM bank
    @param b   ROM bank to switch to (max 255)

    @see SWITCH_ROM_UNROM
*/
#define SWITCH_ROM SWITCH_ROM_DUMMY

/** Delays the given number of milliseconds.
    Uses no timers or interrupts, and can be called with
    interrupts disabled
 */
void delay(uint16_t d) OLDCALL;

/** Reads and returns the current state of the joypad.
    Return value is an OR of J_*

    When testing for multiple different buttons, it's
    best to read the joypad state *once* into a variable
    and then test using that variable.

    @see J_START, J_SELECT, J_A, J_B, J_UP, J_DOWN, J_LEFT, J_RIGHT
*/
uint8_t joypad() OLDCALL;

/** Waits until at least one of the buttons given in mask are pressed.

    Normally only used for checking one key, but it will
    support many, even J_LEFT at the same time as J_RIGHT. :)

    @see joypad
    @see J_START, J_SELECT, J_A, J_B, J_UP, J_DOWN, J_LEFT, J_RIGHT
*/
uint8_t waitpad(uint8_t mask) OLDCALL;

/** Waits for the directional pad and all buttons to be released.

*/
void waitpadup();

/** Multiplayer joypad structure.

    Must be initialized with @ref joypad_init() first then it
    may be used to poll all avaliable joypads with @ref joypad_ex()
*/
typedef struct {
    uint8_t npads;
    union {
        struct {
            uint8_t joy0, joy1, joy2, joy3;
        };
        uint8_t joypads[4];
    };
} joypads_t;

/** Initializes joypads_t structure for polling multiple joypads
    @param npads    number of joypads requested (1, 2 or 4)
    @param joypads  pointer to joypads_t structure to be initialized

    Only required for @ref joypad_ex, not required for calls to regular @ref joypad()
    @returns number of joypads avaliable
    @see joypad_ex(), joypads_t
*/
uint8_t joypad_init(uint8_t npads, joypads_t * joypads) OLDCALL;

/** Polls all avaliable joypads

    @see joypad_init(), joypads_t
*/
void joypad_ex(joypads_t * joypads) OLDCALL;



/** Enables unmasked interrupts

    @note Use @ref CRITICAL {...} instead for creating a block of
          of code which should execute with interrupts  temporarily
          turned off.

    @see disable_interrupts, set_interrupts, CRITICAL
*/
inline void enable_interrupts() {
    __asm__("cli");
}

/** Disables interrupts

    @note Use @ref CRITICAL {...} instead for creating a block of
          of code which should execute with interrupts  temporarily
          turned off.

    This function may be called as many times as you like;
    however the first call to @ref enable_interrupts will re-enable
    them.

    @see enable_interrupts, set_interrupts, CRITICAL
*/
inline void disable_interrupts() {
    __asm__("sei");
}

/** Waits for the vertical blank interrupt (VBL) to finish.

    This is often used in main loops to idle the CPU
    until it's time to start the next frame. It's also useful for
    syncing animation with the screen re-draw.

    Warning: If the VBL interrupt is disabled, this function will
    never return.
*/
void wait_vbl_done();

/** Turns the display off.

    Waits until the VBL interrupt before turning the display off.
    @see DISPLAY_ON
*/
void display_off();

/** Copies data from shadow OAM to OAM
 */
void refresh_OAM();

/** Turns the display back on.
    @see display_off, DISPLAY_OFF
*/
#define DISPLAY_ON \
  display_on();

/** Turns the display off immediately.
    @see display_off, DISPLAY_ON
*/
#define DISPLAY_OFF \
  display_off();

/** Blanks leftmost column, so it is not garbaged when you use horizontal scroll
    @see SHOW_LEFT_COLUMN
*/
#define HIDE_LEFT_COLUMN \
    shadow_PPUMASK &= ~(PPUMASK_SHOW_BG_LC | PPUMASK_SHOW_SPR_LC); \

/** Shows leftmost column
    @see HIDE_LEFT_COLUMN
*/
#define SHOW_LEFT_COLUMN \
    shadow_PPUMASK |= (PPUMASK_SHOW_BG_LC | PPUMASK_SHOW_SPR_LC);

/** Turns on the background layer.
    Sets bit 0 of the LCDC register to 1.
*/
#define SHOW_BKG \
    shadow_PPUMASK |= PPUMASK_SHOW_BG;

/** Turns off the background layer.
    Sets bit 0 of the LCDC register to 0.
*/
#define HIDE_BKG \
    shadow_PPUMASK &= ~PPUMASK_SHOW_BG;

/** Turns on the sprites layer.
    Sets bit 1 of the LCDC register to 1.
*/
#define SHOW_SPRITES \
    shadow_PPUMASK |= PPUMASK_SHOW_SPR;

/** Turns off the sprites layer.
    Clears bit 1 of the LCDC register to 0.
*/
#define HIDE_SPRITES \
    shadow_PPUMASK &= ~PPUMASK_SHOW_SPR;

/** Sets sprite size to 8x16 pixels, two tiles one above the other.
    Sets bit 2 of the LCDC register to 1.
*/
#define SPRITES_8x16 \
  shadow_PPUCTRL |= PPUCTRL_SPR_8X16;

/** Sets sprite size to 8x8 pixels, one tile.
    Clears bit 2 of the LCDC register to 0.
*/
#define SPRITES_8x8 \
  shadow_PPUCTRL &= ~PPUCTRL_SPR_8X16;



/**
 * Set byte in vram at given memory location
 *
 * @param addr address to write to
 * @param v value
 */
void set_vram_byte(uint8_t * addr, uint8_t v) OLDCALL;

/**
 * Get address of X,Y tile of background map
 */
uint8_t * get_bkg_xy_addr(uint8_t x, uint8_t y) OLDCALL;

#define COMPAT_PALETTE(C0,C1,C2,C3) ((uint8_t)(((C3) << 6) | ((C2) << 4) | ((C1) << 2) | (C0)))

/** Sets palette for 2bpp color translation for GG/SMS, does nothing on GB
 */
inline void set_2bpp_palette(uint16_t palette) {
    palette;
}

extern uint16_t _current_1bpp_colors;
void set_1bpp_colors_ex(uint8_t fgcolor, uint8_t bgcolor, uint8_t mode) OLDCALL;
inline void set_1bpp_colors(uint8_t fgcolor, uint8_t bgcolor) {
    set_1bpp_colors_ex(fgcolor, bgcolor, 0);
}

/** Sets VRAM Tile Pattern data for the Background

    Writes __nb_tiles__ tiles to VRAM starting at __first_tile__, tile data
    is sourced from __data__. Each Tile is 16 bytes in size (8x8 pixels, 2 bits-per-pixel).

    Note: Sprite Tiles 128-255 share the same memory region as Background Tiles 128-255.

    @see set_tile_data
*/
void set_bkg_data(uint8_t first_tile, uint8_t nb_tiles, const uint8_t *data) OLDCALL;
#define set_bkg_2bpp_data set_bkg_data

/** Sets VRAM Tile Pattern data for the Background using 1bpp source data

    Similar to @ref set_bkg_data, except source data is 1 bit-per-pixel
    which gets expanded into 2 bits-per-pixel.

    For a given bit that represent a pixel:
    \li 0 will be expanded into color 0
    \li 1 will be expanded into color 1, 2 or 3 depending on color argument

    @see SHOW_BKG, HIDE_BKG, set_bkg_tiles
*/
void set_bkg_1bpp_data(uint8_t first_tile, uint8_t nb_tiles, const uint8_t *data) OLDCALL;

/** Sets a rectangular region of Background Tile Map.

    Entries are copied from map at __tiles__ to the Background Tile Map starting at
    __x__, __y__ writing across for __w__ tiles and down for __h__ tiles.

    Use @ref set_bkg_submap() instead when:
    \li Source map is wider than 32 tiles.
    \li Writing a width that does not match the source map width __and__ more
    than one row high at a time.

    One byte per source tile map entry.

    Writes that exceed coordinate 31 on the x or y axis will wrap around to
    the Left and Top edges.

    @see SHOW_BKG
    @see set_bkg_data, set_bkg_submap, set_win_tiles, set_tiles
*/
void set_bkg_tiles(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *tiles) OLDCALL;
#define set_tile_map set_bkg_tiles


extern uint8_t _map_tile_offset;

/** Sets a rectangular region of Background Tile Map.
    The offset value in __base_tile__ is added to
    the tile ID for each map entry.

    @param x      X Start position in Background Map tile coordinates. Range 0 - 31
    @param y      Y Start position in Background Map tile coordinates. Range 0 - 31
    @param w      Width of area to set in tiles. Range 1 - 32
    @param h      Height of area to set in tiles. Range 1 - 32
    @param tiles  Pointer to source tile map data
    @param base_tile Offset each tile ID entry of the source map by this value. Range 1 - 255

    This is identical to @ref set_bkg_tiles() except that it
    adds the __base_tile__ parameter for when a tile map's tiles don't
    start at index zero. (For example, the tiles used by the map
    range from 100 -> 120 in VRAM instead of 0 -> 20).

    @see set_bkg_tiles for more details
*/
inline void set_bkg_based_tiles(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *tiles, uint8_t base_tile) {
    _map_tile_offset = base_tile;
    set_bkg_tiles(x, y, w, h, tiles);
    _map_tile_offset = 0;
}


/** Sets a rectangular area of the Background Tile Map using a sub-region
    from a source tile map. Useful for scrolling implementations of maps
    larger than 32 x 32 tiles.

    @ param x      X Start position in Background Map tile coordinates. Range 0 - 31
    @ param y      Y Start position in Background Map tile coordinates. Range 0 - 31
    @ param w      Width of area to set in tiles. Range 1 - 255
    @ param h      Height of area to set in tiles. Range 1 - 255
    @ param map    Pointer to source tile map data
    @ param map_w  Width of source tile map in tiles. Range 1 - 255

    Entries are copied from __map__ to the Background Tile Map starting at
    __x__, __y__ writing across for __w__ tiles and down for __h__ tiles,
    using __map_w__ as the rowstride for the source tile map.

    Use this instead of @ref set_bkg_tiles when the source map is wider than
    32 tiles or when writing a width that does not match the source map width.

    One byte per source tile map entry.

    Writes that exceed coordinate 31 on the x or y axis will wrap around to
    the Left and Top edges.

    See @ref set_bkg_tiles for setting CGB attribute maps with @ref VBK_REG.

    @see SHOW_BKG
    @see set_bkg_data, set_bkg_tiles, set_win_submap, set_tiles
*/
void set_bkg_submap(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *map, uint8_t map_w) OLDCALL;
#define set_tile_submap set_bkg_submap


extern uint8_t _submap_tile_offset;

/** Sets a rectangular area of the Background Tile Map using a sub-region
    from a source tile map. The offset value in __base_tile__ is added to
    the tile ID for each map entry.

    @param x      X Start position in Background Map tile coordinates. Range 0 - 31
    @param y      Y Start position in Background Map tile coordinates. Range 0 - 31
    @param w      Width of area to set in tiles. Range 1 - 255
    @param h      Height of area to set in tiles. Range 1 - 255
    @param map    Pointer to source tile map data
    @param map_w  Width of source tile map in tiles. Range 1 - 255
    @param base_tile Offset each tile ID entry of the source map by this value. Range 1 - 255

    This is identical to @ref set_bkg_based_submap() except that it
    adds the __base_tile__ parameter for when a tile map's tiles don't
    start at index zero. (For example, the tiles used by the map
    range from 100 -> 120 in VRAM instead of 0 -> 20).

    @see set_bkg_based_submap for more details
*/
inline void set_bkg_based_submap(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *map, uint8_t map_w, uint8_t base_tile) {
    _submap_tile_offset = base_tile;
    set_bkg_submap(x, y, w, h, map, map_w);
    _submap_tile_offset = 0;
}


/** Copies a rectangular region of Background Tile Map entries into a buffer.

    Entries are copied into __tiles__ from the Background Tile Map starting at
    __x__, __y__ reading across for __w__ tiles and down for __h__ tiles.

    One byte per tile.

    The buffer pointed to by __tiles__ should be at least __x__ x __y__ bytes in size.

    @see get_bkg_tile_xy, get_tiles
*/
void get_bkg_tiles(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t *tiles) OLDCALL;


/**
 * Set single tile t on background layer at x,y
 * @param x X-coordinate
 * @param y Y-coordinate
 * @param t tile index
 * @return returns the address of tile, so you may use faster set_vram_byte() later
 */
uint8_t * set_bkg_tile_xy(uint8_t x, uint8_t y, uint8_t t) OLDCALL;
#define set_tile_xy set_bkg_tile_xy

/**
 * Get single tile t on background layer at x,y
 * @param x X-coordinate
 * @param y Y-coordinate
 * @return returns tile index
 */
uint8_t get_bkg_tile_xy(uint8_t x, uint8_t y) OLDCALL;


/** Moves the Background Layer to the position specified in __x__ and __y__ in pixels.

    @param x   X axis screen coordinate for Left edge of the Background
    @param y   Y axis screen coordinate for Top edge of the Background

    0,0 is the top left corner of the GB screen. The Background Layer wraps around the screen,
    so when part of it goes off the screen it appears on the opposite side (factoring in the
    larger size of the Background Layer versus the screen size).

    The background layer is always under the Window Layer.

    @see SHOW_BKG, HIDE_BKG
*/
inline void move_bkg(uint8_t x, uint8_t y) {
    bkg_scroll_x = x, bkg_scroll_y = y;
}


/** Moves the Background relative to it's current position.

    @param x   Number of pixels to move the Background on the __X axis__
               \n Range: -128 - 127
    @param y   Number of pixels to move the Background on the __Y axis__
               \n Range: -128 - 127

    @see move_bkg
*/
inline void scroll_bkg(int8_t x, int8_t y) {
    bkg_scroll_x += x, bkg_scroll_y += y;
}


/** Sets VRAM Tile Pattern data for Sprites

    Writes __nb_tiles__ tiles to VRAM starting at __first_tile__, tile data
    is sourced from __data__. Each Tile is 16 bytes in size (8x8 pixels, 2 bits-per-pixel).

    Note: Sprite Tiles 128-255 share the same memory region as Background Tiles 128-255.

    GBC only: @ref VBK_REG determines which bank of Background tile patterns are written to.
    \li VBK_REG=0 indicates the first bank
    \li VBK_REG=1 indicates the second
*/
void set_sprite_data(uint8_t first_tile, uint8_t nb_tiles, const uint8_t *data) OLDCALL;
#define set_sprite_2bpp_data set_sprite_data

/** Sets VRAM Tile Pattern data for Sprites using 1bpp source data

    Similar to @ref set_sprite_data, except source data is 1 bit-per-pixel
    which gets expanded into 2 bits-per-pixel.

    For a given bit that represent a pixel:
    \li 0 will be expanded into color 0
    \li 1 will be expanded into color 3

    @see SHOW_SPRITES, HIDE_SPRITES, set_sprite_tile
*/
void set_sprite_1bpp_data(uint8_t first_tile, uint8_t nb_tiles, const uint8_t *data) OLDCALL;

/** Sprite Attributes structure
    @param x     X Coordinate of the sprite on screen
    @param y     Y Coordinate of the sprite on screen - 1
    @param tile  Sprite tile number (see @ref set_sprite_tile)
    @param prop  OAM Property Flags (see @ref set_sprite_prop)
*/
typedef struct OAM_item_t {
    uint8_t y;     //< Y coordinate of the sprite on screen - 1
    uint8_t tile;  //< Sprite tile number
    uint8_t prop;  //< OAM Property Flags
    uint8_t x;     //< X coordinate of the sprite on screen
} OAM_item_t;


/** Shadow OAM array in WRAM, that is DMA-transferred into the real OAM each VBlank
*/
extern volatile struct OAM_item_t shadow_OAM[];

/** MSB of shadow_OAM address is used by OAM DMA copying routine
*/
extern uint8_t _shadow_OAM_base;

#define DISABLE_OAM_DMA \
    _shadow_OAM_base = 0

/** Disable OAM DMA copy each VBlank
*/
#define DISABLE_VBL_TRANSFER DISABLE_OAM_DMA

#define ENABLE_OAM_DMA \
    _shadow_OAM_base = (uint8_t)((uint16_t)&shadow_OAM >> 8)

/** Enable OAM DMA copy each VBlank and set it to transfer default shadow_OAM array
*/
#define ENABLE_VBL_TRANSFER ENABLE_OAM_DMA

/** Amount of hardware sprites in OAM
*/
#define MAX_HARDWARE_SPRITES 64

/** Enable OAM DMA copy each VBlank and set it to transfer any 256-byte aligned array
*/
inline void SET_SHADOW_OAM_ADDRESS(void * address) {
    _shadow_OAM_base = (uint8_t)((uint16_t)address >> 8);
}

/** Sets sprite number __nb__in the OAM to display tile number __tile__.

    @param nb    Sprite number, range 0 - 63
    @param tile  Selects a tile (0 - 255) from PPU memory at 0000h - 0FFFh / 1000h - 1FFFh

    In 8x16 mode:
    \li The sprite will also display the next tile (__tile__ + 1)
        directly below (y + 8) the first tile.
    \li The lower bit of the tile number is ignored:
        the upper 8x8 tile is (__tile__ & 0xFE), and
        the lower 8x8 tile is (__tile__ | 0x01).
    \li See: @ref SPRITES_8x16
*/
inline void set_sprite_tile(uint8_t nb, uint8_t tile) {
    shadow_OAM[nb].tile=tile;
}


/** Returns the tile number of sprite number __nb__ in the OAM.

@param nb    Sprite number, range 0 - 63

@see set_sprite_tile for more details
*/
inline uint8_t get_sprite_tile(uint8_t nb) {
    return shadow_OAM[nb].tile;
}


/** Sets the OAM Property Flags of sprite number __nb__ to those defined in __prop__.

    @param nb    Sprite number, range 0 - 39
    @param prop  Property setting (see bitfield description)

    The bits in __prop__ represent:
    \li Bit 7 - Priority flag. When this is set the sprites appear behind the
              background and window layer.
              \n 0: infront
              \n 1: behind
    \li Bit 6 - Vertical flip. Dictates which way up the sprite is drawn
              vertically.
              \n 0: normal
              \n 1:upside down
    \li Bit 5 - Horizontal flip. Dictates which way up the sprite is
              drawn horizontally.
              \n 0: normal
              \n  1:back to front
    \li Bit 4 - DMG/Non-CGB Mode Only. Assigns either one of the two b/w palettes to the sprite.
              \n 0: OBJ palette 0
              \n 1: OBJ palette 1
    \li Bit 3 - GBC only. Dictates from which bank of Sprite Tile Patterns the tile
              is taken.
              \n 0: Bank 0
              \n 1: Bank 1
    \li Bit 2 - See bit 0.
    \li Bit 1 - See bit 0.
    \li Bit 0 - GBC only. Bits 0-2 indicate which of the 7 OBJ colour palettes the
              sprite is assigned.
*/
inline void set_sprite_prop(uint8_t nb, uint8_t prop) {
    shadow_OAM[nb].prop=prop;
}


/** Returns the OAM Property Flags of sprite number __nb__.

    @param nb    Sprite number, range 0 - 39
    @see set_sprite_prop for property bitfield settings
*/
inline uint8_t get_sprite_prop(uint8_t nb) {
    return shadow_OAM[nb].prop;
}


/** Moves sprite number __nb__ to the __x__, __y__ position on the screen.

    @param nb  Sprite number, range 0 - 63
    @param x   X Position. Specifies the sprites horizontal position on the screen (minus 8).
    @param y   Y Position. Specifies the sprites vertical position on the screen (minus 16).
               \n An offscreen value (Y>=240) hides the sprite.

    Moving the sprite to 0,0 (or similar off-screen location) will hide it.
*/
inline void move_sprite(uint8_t nb, uint8_t x, uint8_t y) {
    OAM_item_t * itm = &shadow_OAM[nb];
    itm->y=y, itm->x=x;
}


/** Moves sprite number __nb__ relative to its current position.

    @param nb  Sprite number, range 0 - 63
    @param x   Number of pixels to move the sprite on the __X axis__
               \n Range: -128 - 127
    @param y   Number of pixels to move the sprite on the __Y axis__
               \n Range: -128 - 127

    @see move_sprite for more details about the X and Y position
 */
inline void scroll_sprite(uint8_t nb, int8_t x, int8_t y) {
    OAM_item_t * itm = &shadow_OAM[nb];
    itm->y+=y, itm->x+=x;
}


/** Hides sprite number __nb__ by moving it to Y position 240.

    @param nb  Sprite number, range 0 - 63
 */
inline void hide_sprite(uint8_t nb) {
    shadow_OAM[nb].y = 240;
}



/** Copies arbitrary data to an address in VRAM
    without taking into account the state of LCDC bits 3 or 4.

    Copies __len__ bytes from a buffer at __data__ to VRAM starting at __vram_addr__.

    @see set_bkg_data, set_win_data, set_bkg_tiles, set_win_tiles, set_tile_data, set_tiles
*/
void set_data(uint8_t *vram_addr, const uint8_t *data, uint16_t len) OLDCALL;


/** Sets a rectangular region of Tile Map entries at a given VRAM Address.

    @param x         X Start position in Map tile coordinates. Range 0 - 31
    @param y         Y Start position in Map tile coordinates. Range 0 - 31
    @param w         Width of area to set in tiles. Range 1 - 32
    @param h         Height of area to set in tiles.   Range 1 - 32
    @param vram_addr Pointer to destination VRAM Address
    @param tiles     Pointer to source Tile Map data

    Entries are copied from __tiles__ to Tile Map at address vram_addr starting at
    __x__, __y__ writing across for __w__ tiles and down for __h__ tiles.

    One byte per source tile map entry.

    There are two 32x30 Tile Maps in VRAM at addresses 2000h-23FFh and 2400h-27FFh.

    @see set_bkg_tiles
*/
void set_tiles(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t *vram_addr, const uint8_t *tiles) OLDCALL;

/** Sets VRAM Tile Pattern data starting from given base address
    without taking into account the state of PPUMASK.

    @see set_bkg_data, set_data
*/
void set_tile_data(uint8_t first_tile, uint8_t nb_tiles, const uint8_t *data, uint8_t base) OLDCALL;


/** Sets VRAM Tile Pattern data in the native format

    @param first_tile  Index of the first tile to write (0 - 511)
    @param nb_tiles    Number of tiles to write
    @param data        Pointer to source Tile Pattern data.

    When `first_tile` is larger than 256 on the GB/AP, it
    will write to sprite data instead of background data.

    The bit depth of the source Tile Pattern data depends
    on which console is being used:
    \li NES: loads 2bpp tiles data
 */
inline void set_native_tile_data(uint16_t first_tile, uint8_t nb_tiles, const uint8_t *data) {
    if (first_tile < 256) {
        set_bkg_data(first_tile, nb_tiles, data);
    } else {
        set_sprite_data(first_tile - 256, nb_tiles, data);
    }
}

/** Initializes the entire Background Tile Map with Tile Number __c__
    @param c   Tile number to fill with

    Note: This function avoids writes during modes 2 & 3
*/
void init_bkg(uint8_t c) OLDCALL;

/** Fills the VRAM memory region __s__ of size __n__ with Tile Number __c__
    @param s   Start address in VRAM
    @param c   Tile number to fill with
    @param n   Size of memory region (in bytes) to fill

    Note: This function avoids writes during modes 2 & 3
*/
void vmemset (void *s, uint8_t c, size_t n) OLDCALL;

/** Fills a rectangular region of Tile Map entries for the Background layer with tile.

    @param x      X Start position in Background Map tile coordinates. Range 0 - 31
    @param y      Y Start position in Background Map tile coordinates. Range 0 - 31
    @param w      Width of area to set in tiles. Range 0 - 31
    @param h      Height of area to set in tiles. Range 0 - 31
    @param tile   Fill value
*/
void fill_bkg_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t tile) OLDCALL;
#define fill_rect fill_bkg_rect

#endif /* _NES_H */