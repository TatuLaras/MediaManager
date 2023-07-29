#pragma once
#include "InfoPanel.h"

/// <summary>
/// Parent class for image-based info panels
/// </summary>
class ImageInfoPanel : public InfoPanel
{
public:
	virtual void Render() = 0;
	void RenderImage();
	void SetPanelImage(std::string& path, ImageType type);

private:
	std::string panel_image_path = "";
	ImageType panel_image_type = ImageType::Poster;
	void LoadPanelImage();
	bool LoadTextureFromFile(const char* filename);
	void BindTexture();
	int image_width = 0;
	int image_height = 0;
	GLuint image_texture = 0;
	bool image_loading_started = false;
	bool image_loading_ram_finished = false;
	bool image_loading_gl_finished = false;
	unsigned char* image_data;
	std::thread subthread;

	std::string GetImagePath() {
		return PathManager::GetBaseDataFolder(mm_SUBFOLDER_IMAGE)
			+ std::string("\\")
			+ panel_image_path.substr(1, panel_image_path.size() - 1);
	}
};

