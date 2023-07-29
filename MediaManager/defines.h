#pragma once
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#define mm_SLASH "\\"
#define mm_def_PLATFORM_WINDOWS
#else
#define mm_SLASH "/"
#endif

#define mm_SPACE "  -  "

#define mm_TMDB__BASE_URL_IMAGE "https://image.tmdb.org/t/p"
#define mm_TMDB__IMAGE_SIZE "/w185"
#define mm_TMDB__IMAGE_SIZE_POSTER_LARGE "/w342"
#define mm_TMDB__IMAGE_SIZE_STILL_LARGE "/w300"
#define mm_TMDB__BASE_URL "https://api.themoviedb.org/3"
#define mm_FONT_USED "NotoSansJP-Medium.ttf"

#define mm_INFOPANEL_WRAP_WIDTH 350

#define mm_DEFAULT_PLAYER ""
#define mm_DEFAULT_FONT_SIZE 18
#define mm_DEFAULT_LABEL_MAX_LENGTH 80
#define mm_DEFAULT_USE_LARGE_IMAGES false
#define mm_DEFAULT_SHOW_SUBLABELS false

#define mm_SUBFOLDER_IMAGE "images"
#define mm_SUBFOLDER_METADATA "metadata"
#define mm_SUBFOLDER_OTHER "other"
