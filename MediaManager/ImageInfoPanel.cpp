#include "pch.h"
#include "ImageInfoPanel.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_WINDOWS_UTF8
#include "stb_image.h"

inline bool ImageInfoPanel::LoadTextureFromFile(const char* filename)
{
    // Load from file
    image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
    return image_data != NULL;
}

inline void ImageInfoPanel::BindTexture()
{
    if (image_data == NULL)
        return;

    // Create a OpenGL texture identifier
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP); // This is required on WebGL for non power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP); // Same

                                                                 // Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
}

void ImageInfoPanel::LoadPanelImage() {
    if (panel_image_path.size() == 0) return;
    // If doesn't exist, try downloading
    std::string image_path = GetImagePath();
    if (!FsHelpers::PathExists(image_path)) {
        TMDB tmdb(Config::tmdb_key.c_str());
        tmdb.DownloadImage(panel_image_path, Config::GetImageSize(panel_image_type));
    }

    image_loading_ram_finished = LoadTextureFromFile(GetImagePath().c_str());

}

void ImageInfoPanel::RenderImage()
{
    if (!image_loading_started) {
        image_loading_started = true;
        subthread = std::thread(&ImageInfoPanel::LoadPanelImage, this);
        subthread.detach();
        return;
    }

    if (image_loading_ram_finished && !image_loading_gl_finished) {
        image_loading_gl_finished = true;
        BindTexture();
    };

    ImGui::Image((void*)(intptr_t)image_texture, ImVec2(image_width, image_height));
}

void ImageInfoPanel::SetPanelImage(std::string& path, ImageType type) {
    panel_image_path = path;
    panel_image_type = type;
}

