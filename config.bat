cmake -S . -B build -D CMAKE_BUILD_TYPE=%1 -D FT_DISABLE_ZLIB=TRUE -D FT_DISABLE_BZIP2=TRUE -D FT_DISABLE_PNG=TRUE -D FT_DISABLE_HARFBUZZ=TRUE -D FT_DISABLE_BROTLI=TRUE -G "Visual Studio 17 2022"