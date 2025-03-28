#pragma once

#include <imgui.h>
#include <imgui-SFML.h>
#include <SFML/Graphics.hpp>
#include <imfilebrowser.h>

class Application {
public:
    Application();
    void run();

private:
    void renderMenuBar();
    void renderFilterPanel();
    void handleImageLoading();

    sf::RenderWindow window;
    ImGui::FileBrowser fileDialog;

    sf::Texture texture;
    sf::Texture original_texture;
    sf::Sprite sprite;

    bool hasImage = false;
    bool hasChannelReseted = false;
    bool redSelected = true, greenSelected = true, blueSelected = true;

    bool showConvolutionWindow = false;

    int selectedFilter = 0;
    int selectedPadding = 0;
    int selectedBackend = 0;
    int selectedAlgorithm = 0;

    int maxKernelSize = 150;
    int kernelSize = 3;
    
    float PANEL_WIDTH = 120.0f;
};
