// DETAILS: https://github.com/lvgl/lvgl/blob/v8.3.9/lv_conf_template.h
// This configutation is specific to WT32-SC01-PLUS
#ifndef LV_CONF_H
#define LV_CONF_H

#include <stdint.h>
/*====================
   COLOR SETTINGS
 *====================*/
#define LV_COLOR_DEPTH 16
#define LV_COLOR_16_SWAP 0
#define LV_COLOR_SCREEN_TRANSP 0
#define LV_COLOR_MIX_ROUND_OFS (LV_COLOR_DEPTH == 32 ? 0 : 128)
#define LV_COLOR_CHROMA_KEY lv_color_hex(0x00ff00)
/*=========================
   MEMORY SETTINGS
 *=========================*/
#define LV_MEM_CUSTOM 0
#if LV_MEM_CUSTOM == 0
#define LV_MEM_SIZE (32U * 1024U) /*[bytes]*/
#define LV_MEM_ADR 0              /*0: unused*/
#if LV_MEM_ADR == 0
//#define LV_MEM_POOL_INCLUDE your_alloc_library  /* Uncomment if using an external allocator*/
//#define LV_MEM_POOL_ALLOC   your_alloc          /* Uncomment if using an external allocator*/
#endif

#else                                    /*LV_MEM_CUSTOM*/
#define LV_MEM_CUSTOM_INCLUDE <stdlib.h> /*Header for the dynamic memory function*/
#define LV_MEM_CUSTOM_ALLOC malloc
#define LV_MEM_CUSTOM_FREE free
#define LV_MEM_CUSTOM_REALLOC realloc
#endif /*LV_MEM_CUSTOM*/

/*Number of the intermediate memory buffer used during rendering and other internal processing mechanisms.
 *You will see an error log message if there wasn't enough buffers. */
#define LV_MEM_BUF_MAX_NUM 128
#define LV_MEMCPY_MEMSET_STD 0
/*====================
   HAL SETTINGS
 *====================*/

/*Default display refresh period. LVG will redraw changed areas with this period time*/
#define LV_DISP_DEF_REFR_PERIOD 30  /*[ms]*/
#define LV_INDEV_DEF_READ_PERIOD 30 /*[ms]*/
#define LV_TICK_CUSTOM 1
#if LV_TICK_CUSTOM
#define LV_TICK_CUSTOM_INCLUDE "Arduino.h"      /*Header for the system time function*/
#define LV_TICK_CUSTOM_SYS_TIME_EXPR (millis()) /*Expression evaluating to current system time in ms*/
#endif                                          /*LV_TICK_CUSTOM*/
#define LV_DPI_DEF 130                          /*[px/inch]*/
/*=======================
 * FEATURE CONFIGURATION
 *=======================*/
/*-------------
 * Drawing
 *-----------*/
/*Enable complex draw engine.
 *Required to draw shadow, gradient, rounded corners, circles, arc, skew lines, image transformations or any masks*/
#define LV_DRAW_COMPLEX 1
#if LV_DRAW_COMPLEX != 0
/*Allow buffering some shadow calculation.
 *LV_SHADOW_CACHE_SIZE is the max. shadow size to buffer, where shadow size is `shadow_width + radius`
 *Caching has LV_SHADOW_CACHE_SIZE^2 RAM cost*/
#define LV_SHADOW_CACHE_SIZE 0
/* Set number of maximally cached circle data.
 * The circumference of 1/4 circle are saved for anti-aliasing
 * radius * 4 bytes are used per circle (the most often used radiuses are saved)
 * 0: to disable caching */
#define LV_CIRCLE_CACHE_SIZE 4
#endif /*LV_DRAW_COMPLEX*/

/*Default image cache size. Image caching keeps the images opened.
 *If only the built-in image formats are used there is no real advantage of caching. (I.e. if no new image decoder is added)
 *With complex image decoders (e.g. PNG or JPG) caching can save the continuous open/decode of images.
 *However the opened images might consume additional RAM.
 *0: to disable caching*/
#define LV_IMG_CACHE_DEF_SIZE 1

/*Maximum buffer size to allocate for rotation. Only used if software rotation is enabled in the display driver.*/
#define LV_DISP_ROT_MAX_BUF (12 * 1024)

/*-------------
 * GPU
 *-----------*/
#define LV_USE_GPU_STM32_DMA2D 0
#define LV_USE_GPU_NXP_PXP 0
#define LV_USE_GPU_NXP_VG_LITE 0
#define LV_USE_EXTERNAL_RENDERER 0
#define LV_USE_GPU_SDL 0
/*-------------
 * Logging
 *-----------*/
/*Enable the log module*/
#define LV_USE_LOG 0
#if LV_USE_LOG

/*How important log should be added:
 *LV_LOG_LEVEL_TRACE       A lot of logs to give detailed information
 *LV_LOG_LEVEL_INFO        Log important events
 *LV_LOG_LEVEL_WARN        Log if something unwanted happened but didn't cause a problem
 *LV_LOG_LEVEL_ERROR       Only critical issue, when the system may fail
 *LV_LOG_LEVEL_USER        Only logs added by the user
 *LV_LOG_LEVEL_NONE        Do not log anything*/
#define LV_LOG_LEVEL LV_LOG_LEVEL_WARN

/*1: Print the log with 'printf';
 *0: User need to register a callback with `lv_log_register_print_cb()`*/
#define LV_LOG_PRINTF 1

/*Enable/disable LV_LOG_TRACE in modules that produces a huge number of logs*/
#define LV_LOG_TRACE_MEM 1
#define LV_LOG_TRACE_TIMER 1
#define LV_LOG_TRACE_INDEV 1
#define LV_LOG_TRACE_DISP_REFR 1
#define LV_LOG_TRACE_EVENT 1
#define LV_LOG_TRACE_OBJ_CREATE 1
#define LV_LOG_TRACE_LAYOUT 1
#define LV_LOG_TRACE_ANIM 1
#endif /*LV_USE_LOG*/
/*-------------
 * Asserts
 *-----------*/
/*Enable asserts if an operation is failed or an invalid data is found.
 *If LV_USE_LOG is enabled an error message will be printed on failure*/
#define LV_USE_ASSERT_NULL 1          /*Check if the parameter is NULL. (Very fast, recommended)*/
#define LV_USE_ASSERT_MALLOC 1        /*Checks is the memory is successfully allocated or no. (Very fast, recommended)*/
#define LV_USE_ASSERT_STYLE 0         /*Check if the styles are properly initialized. (Very fast, recommended)*/
#define LV_USE_ASSERT_MEM_INTEGRITY 0 /*Check the integrity of `lv_mem` after critical operations. (Slow)*/
#define LV_USE_ASSERT_OBJ 0           /*Check the object's type and existence (e.g. not deleted). (Slow)*/

/*Add a custom handler when assert happens e.g. to restart the MCU*/
#define LV_ASSERT_HANDLER_INCLUDE <stdint.h>
#define LV_ASSERT_HANDLER \
  while (1)               \
    ; /*Halt by default*/
/*-------------
 * Others
 *-----------*/
/*1: Show CPU usage and FPS count in the right bottom corner*/
#define LV_USE_PERF_MONITOR 0
#if LV_USE_PERF_MONITOR
#define LV_USE_PERF_MONITOR_POS LV_ALIGN_BOTTOM_RIGHT
#endif
/*1: Show the used memory and the memory fragmentation in the left bottom corner
 * Requires LV_MEM_CUSTOM = 0*/
#define LV_USE_MEM_MONITOR 0
#if LV_USE_PERF_MONITOR
#define LV_USE_MEM_MONITOR_POS LV_ALIGN_BOTTOM_LEFT
#endif
/*1: Draw random colored rectangles over the redrawn areas*/
#define LV_USE_REFR_DEBUG 0

/*Change the built in (v)snprintf functions*/
#define LV_SPRINTF_CUSTOM 0
#if LV_SPRINTF_CUSTOM
#define LV_SPRINTF_INCLUDE <stdio.h>
#define lv_snprintf snprintf
#define lv_vsnprintf vsnprintf
#else /*LV_SPRINTF_CUSTOM*/
#define LV_SPRINTF_USE_FLOAT 1
#endif /*LV_SPRINTF_CUSTOM*/

#define LV_USE_USER_DATA 1

/*Garbage Collector settings
 *Used if lvgl is bound to higher level language and the memory is managed by that language*/
#define LV_ENABLE_GC 0
#if LV_ENABLE_GC != 0
#define LV_GC_INCLUDE "gc.h" /*Include Garbage Collector related things*/
#endif                       /*LV_ENABLE_GC*/
/*=====================
 *  COMPILER SETTINGS
 *====================*/
#define LV_BIG_ENDIAN_SYSTEM 0        /*For big endian systems set to 1*/
#define LV_ATTRIBUTE_TICK_INC         /*Define a custom attribute to `lv_tick_inc` function*/
#define LV_ATTRIBUTE_TIMER_HANDLER    /*Define a custom attribute to `lv_timer_handler` function*/
#define LV_ATTRIBUTE_FLUSH_READY      /*Define a custom attribute to `lv_disp_flush_ready` function*/
#define LV_ATTRIBUTE_MEM_ALIGN_SIZE 1 /*Required alignment size for buffers*/
/*Will be added where memories needs to be aligned (with -Os data might not be aligned to boundary by default).
 * E.g. __attribute__((aligned(4)))*/
#define LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_LARGE_CONST                                   /*Attribute to mark large constant arrays for example font's bitmaps*/
#define LV_ATTRIBUTE_LARGE_RAM_ARRAY                               /*Complier prefix for a big array declaration in RAM*/
#define LV_ATTRIBUTE_FAST_MEM                                      /*Place performance critical functions into a faster memory (e.g RAM)*/
#define LV_ATTRIBUTE_DMA                                           /*Prefix variables that are used in GPU accelerated operations, often these need to be placed in RAM sections that are DMA accessible*/
#define LV_EXPORT_CONST_INT(int_value) struct _silence_gcc_warning /*The default value just prevents GCC warning*/
#define LV_USE_LARGE_COORD 0                                       /*Extend the default -32k..32k coordinate range to -4M..4M by using int32_t for coordinates instead of int16_t*/
/*==================
 *   FONT USAGE
 *===================*/
/*Montserrat fonts with ASCII range and some symbols using bpp = 4
 *https://fonts.google.com/specimen/Montserrat*/
#define LV_FONT_MONTSERRAT_8 1
#define LV_FONT_MONTSERRAT_10 1
#define LV_FONT_MONTSERRAT_12 1
#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_MONTSERRAT_16 1
#define LV_FONT_MONTSERRAT_18 1
#define LV_FONT_MONTSERRAT_20 1
#define LV_FONT_MONTSERRAT_22 1
#define LV_FONT_MONTSERRAT_24 1
#define LV_FONT_MONTSERRAT_26 1
#define LV_FONT_MONTSERRAT_28 1
#define LV_FONT_MONTSERRAT_30 1
#define LV_FONT_MONTSERRAT_32 1
#define LV_FONT_MONTSERRAT_34 0
#define LV_FONT_MONTSERRAT_36 1
#define LV_FONT_MONTSERRAT_38 0
#define LV_FONT_MONTSERRAT_40 1
#define LV_FONT_MONTSERRAT_42 1
#define LV_FONT_MONTSERRAT_44 0
#define LV_FONT_MONTSERRAT_46 0
#define LV_FONT_MONTSERRAT_48 1

/*Demonstrate special features*/
#define LV_FONT_MONTSERRAT_12_SUBPX 0
#define LV_FONT_MONTSERRAT_28_COMPRESSED 0 /*bpp = 3*/
#define LV_FONT_DEJAVU_16_PERSIAN_HEBREW 0 /*Hebrew, Arabic, Perisan letters and all their forms*/
#define LV_FONT_SIMSUN_16_CJK 0            /*1000 most common CJK radicals*/

/*Pixel perfect monospace fonts*/
#define LV_FONT_UNSCII_8 0
#define LV_FONT_UNSCII_16 0

/*Optionally declare custom fonts here.
 *You can use these fonts as default font too and they will be available globally.
 *E.g. #define LV_FONT_CUSTOM_DECLARE   LV_FONT_DECLARE(my_font_1) LV_FONT_DECLARE(my_font_2)*/
#define LV_FONT_CUSTOM_DECLARE  LV_FONT_DECLARE(simplymono8) LV_FONT_DECLARE(simplymono12) LV_FONT_DECLARE(simplymono16) LV_FONT_DECLARE(simplymono24) LV_FONT_DECLARE(simplymono32) LV_FONT_DECLARE(simplymono48) LV_FONT_DECLARE(simplymono_bold48) LV_FONT_DECLARE(simplymono64) 

/*Always set a default font*/
#define LV_FONT_DEFAULT &simplymono16

/*Enable handling large font and/or fonts with a lot of characters.
 *The limit depends on the font size, font face and bpp.
 *Compiler error will be triggered if a font needs it.*/
#define LV_FONT_FMT_TXT_LARGE 0
#define LV_USE_FONT_COMPRESSED 0
#define LV_USE_FONT_SUBPX 0
#if LV_USE_FONT_SUBPX
/*Set the pixel order of the display. Physical order of RGB channels. Doesn't matter with "normal" fonts.*/
#define LV_FONT_SUBPX_BGR 0 /*0: RGB; 1:BGR order*/
#endif
/*=================
 *  TEXT SETTINGS
 *=================*/
#define LV_TXT_ENC LV_TXT_ENC_UTF8
#define LV_TXT_BREAK_CHARS " ,.;:-_"
#define LV_TXT_LINE_BREAK_LONG_LEN 0
#define LV_TXT_LINE_BREAK_LONG_PRE_MIN_LEN 3
#define LV_TXT_LINE_BREAK_LONG_POST_MIN_LEN 3
#define LV_TXT_COLOR_CMD "#"
#define LV_USE_BIDI 0
#define LV_USE_ARABIC_PERSIAN_CHARS 0
/*==================
 *  WIDGET USAGE
 *================*/
#define LV_USE_ARC 1
#define LV_USE_ANIMIMG 1
#define LV_USE_BAR 1
#define LV_USE_BTN 1
#define LV_USE_BTNMATRIX 1
#define LV_USE_CANVAS 1
#define LV_USE_CHECKBOX 1
#define LV_USE_DROPDOWN 1 /*Requires: lv_label*/
#define LV_USE_IMG 1      /*Requires: lv_label*/
#define LV_USE_LABEL 1
#if LV_USE_LABEL
#define LV_LABEL_TEXT_SELECTION 1 /*Enable selecting text of the label*/
#define LV_LABEL_LONG_TXT_HINT 1  /*Store some extra info in labels to speed up drawing of very long texts*/
#endif
#define LV_USE_LINE 1
#define LV_USE_ROLLER 1 /*Requires: lv_label*/
#if LV_USE_ROLLER
#define LV_ROLLER_INF_PAGES 7 /*Number of extra "pages" when the roller is infinite*/
#endif
#define LV_USE_SLIDER 1 /*Requires: lv_bar*/
#define LV_USE_SWITCH 1
#define LV_USE_TEXTAREA 1 /*Requires: lv_label*/
#if LV_USE_TEXTAREA != 0
#define LV_TEXTAREA_DEF_PWD_SHOW_TIME 1500 /*ms*/
#endif
#define LV_USE_TABLE 1
/*==================
 * EXTRA COMPONENTS
 *==================*/
/*-----------
 * Widgets
 *----------*/
#define LV_USE_CALENDAR 1
#if LV_USE_CALENDAR
#define LV_CALENDAR_WEEK_STARTS_MONDAY 1
#if LV_CALENDAR_WEEK_STARTS_MONDAY
#define LV_CALENDAR_DEFAULT_DAY_NAMES        \
  {                                          \
    "Mo", "Tu", "We", "Th", "Fr", "Sa", "Su" \
  }
#else
#define LV_CALENDAR_DEFAULT_DAY_NAMES        \
  {                                          \
    "Su", "Mo", "Tu", "We", "Th", "Fr", "Sa" \
  }
#endif
#define LV_CALENDAR_DEFAULT_MONTH_NAMES                                                                                      \
  {                                                                                                                          \
    "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" \
  }
#define LV_USE_CALENDAR_HEADER_ARROW 1
#define LV_USE_CALENDAR_HEADER_DROPDOWN 1
#endif /*LV_USE_CALENDAR*/

#define LV_USE_CHART 1
#define LV_USE_COLORWHEEL 1
#define LV_USE_IMGBTN 1
#define LV_USE_KEYBOARD 1
#define LV_USE_LED 1
#define LV_USE_LIST 1
#define LV_USE_METER 1
#define LV_USE_MSGBOX 1
#define LV_USE_MSG 1
#define LV_USE_SPINBOX 1
#define LV_USE_SPINNER 1
#define LV_USE_TABVIEW 1
#define LV_USE_TILEVIEW 1
#define LV_USE_WIN 1
#define LV_USE_SPAN 1
#if LV_USE_SPAN
#define LV_SPAN_SNIPPET_STACK_SIZE 64
#endif

/*-----------
 * Themes
 *----------*/
#define LV_USE_THEME_DEFAULT 1
#if LV_USE_THEME_DEFAULT
#define LV_THEME_DEFAULT_DARK 1            /*0: Light mode; 1: Dark mode*/
#define LV_THEME_DEFAULT_GROW 1            /*1: Enable grow on press*/
#define LV_THEME_DEFAULT_TRANSITION_TIME 0 /*Default transition time in [ms]*/
#endif                                     /*LV_USE_THEME_DEFAULT*/
#define LV_USE_THEME_BASIC 1
#define LV_USE_THEME_MONO 0
/*-----------
 * Layouts
 *----------*/
#define LV_USE_FLEX 1
#define LV_USE_GRID 1
/*---------------------
 * 3rd party libraries
 *--------------------*/
/*File system interfaces for common APIs
 *To enable set a driver letter for that API*/
#define LV_USE_FS_STDIO 1
#if LV_USE_FS_STDIO
#define LV_FS_STDIO_LETTER 'S'     /*Set an upper cased letter on which the drive will accessible (e.g. 'A')*/
#define LV_FS_STDIO_PATH "/sdcard" /*Set the working directory. File/directory paths will be appended to it.*/
#define LV_FS_STDIO_CACHE_SIZE 0   /*>0 to cache this number of bytes in lv_fs_read()*/
#endif

#define LV_USE_FS_FATFS 0
#if LV_USE_FS_FATFS
#define LV_FS_FATFS_LETTER 'S'    /*Set an upper cased letter on which the drive will accessible (e.g. 'A')*/
#define LV_FS_FATFS_CACHE_SIZE 10 /*>0 to cache this number of bytes in lv_fs_read()*/
#endif

#define LV_USE_PNG 0
#define LV_USE_BMP 0
#define LV_USE_SJPG 0
#define LV_USE_GIF 0
#define LV_USE_QRCODE 0
#define LV_USE_FREETYPE 0
#if LV_USE_FREETYPE
/*Memory used by FreeType to cache characters [bytes] (-1: no caching)*/
#define LV_FREETYPE_CACHE_SIZE (16 * 1024)
#endif

#define LV_USE_RLOTTIE 0
#define LV_USE_SNAPSHOT 1
#define LV_BUILD_EXAMPLES 0
#endif /*LV_CONF_H*/
