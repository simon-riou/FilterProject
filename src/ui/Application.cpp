#include "ui/Application.h"

#include "core/ImageEncoder.h"
#include "filters/ConvolutionFilter.h"

#include <iostream>
#include <algorithm>
#include <iostream>

Application::Application()
    : fileDialog(ImGui::FileBrowser()), sprite(texture)
{
    fileDialog.SetTitle("Select an image");
    fileDialog.SetTypeFilters({ ".jpg", ".jpeg", ".png" });
}

void Application::run() {
    const int windowWidth = 1250;
    const float aspectRatio = 16.0f / 9.0f;
    const int windowHeight = static_cast<int>(windowWidth / aspectRatio);

    window.create(sf::VideoMode({ windowWidth, windowHeight }), "TTah l'itfi <3");
    window.setFramerateLimit(60);
    bool launched = ImGui::SFML::Init(window);
    if (!launched) {
        std::cerr << "Erreur lors de la creation de la fenetre." << std::endl;
        exit(1);
    }

    fileDialog.SetTitle("Select an image");
    fileDialog.SetTypeFilters({ ".jpg", ".jpeg", ".png" });

    sf::Clock deltaClock;
    while (window.isOpen()) {
        while (const auto event = window.pollEvent()) {
            ImGui::SFML::ProcessEvent(window, *event);
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        renderMenuBar();
        renderFilterPanel();

        fileDialog.Display();
        handleImageLoading();

        window.clear({42, 42, 42});
        if (hasImage) window.draw(sprite);
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
}

void Application::renderMenuBar() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Load")) fileDialog.Open();
            if (ImGui::MenuItem("Exit")) exit(0);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Image", hasImage)) {
            if (ImGui::MenuItem("Original", nullptr, false, hasImage)) {
                texture = original_texture;
                redSelected = greenSelected = blueSelected = true;
                hasChannelReseted = false;
                sprite.setTexture(texture, true);
            }

            if (ImGui::MenuItem("Greyscale", nullptr, false, hasImage)) {
                texture = original_texture;
                texture = ImageEncoder::toGrayscale(texture);
                redSelected = greenSelected = blueSelected = false;
                hasChannelReseted = true;
                sprite.setTexture(texture, true);
            }

            if (ImGui::BeginMenu("Channel", hasImage)) {
                bool channelsChanged = false;

                if (ImGui::MenuItem("Red", nullptr, redSelected)) {
                    redSelected = !redSelected;
                    channelsChanged = true;
                }
                if (ImGui::MenuItem("Green", nullptr, greenSelected)) {
                    greenSelected = !greenSelected;
                    channelsChanged = true;
                }
                if (ImGui::MenuItem("Blue", nullptr, blueSelected)) {
                    blueSelected = !blueSelected;
                    channelsChanged = true;
                }

                if (channelsChanged) {
                    hasChannelReseted = false;
                    texture = original_texture;
                    texture = ImageEncoder::combineChannels(texture, redSelected, greenSelected, blueSelected);
                    sprite.setTexture(texture, true);
                }
                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void Application::renderFilterPanel() {
    ImGui::SetNextWindowPos(ImVec2(0, 20));
    ImGui::SetNextWindowSize(ImVec2(PANEL_WIDTH, ImGui::GetIO().DisplaySize.y - 20));

    ImGui::Begin("Filters", nullptr, ImGuiWindowFlags_NoMove);
    ImGui::BeginDisabled(!hasImage);

    if (ImGui::Button("Convolution")) showConvolutionWindow = true;

    if (showConvolutionWindow) {
        ImGui::Begin("Convolution Options", &showConvolutionWindow, ImGuiWindowFlags_AlwaysAutoResize);

        // It has to replicate the order of PaddingType in ConvolutionType.h
        const char* ConvolutionTypes[] = { "Custom", "Identity", "Mean", "Gaussian", "Sharpen", "Laplacien", "Edge reinforcement (hor)", "Edge reinforcement (ver)" };
        ImGui::Combo("Filter Type", &selectedFilter, ConvolutionTypes, IM_ARRAYSIZE(ConvolutionTypes));

        static std::vector<double> customKernel(9, 0.0f);
        if (selectedFilter == 0) {
            ImGui::Text("Custom Kernel");
            int kernelArea = kernelSize * kernelSize;
            if (customKernel.size() != kernelArea) customKernel.resize(kernelArea, 0.0f);

            for (int i = 0; i < kernelSize; i++) {
                for (int j = 0; j < kernelSize; j++) {
                    ImGui::PushID(i * kernelSize + j);
                    ImGui::SetNextItemWidth(40);
                    ImGui::InputDouble("##UserValue", &customKernel[i * kernelSize + j], 0.0f, 0.0f, "%.4f");
                    ImGui::PopID();
                    ImGui::SameLine();
                }
                ImGui::NewLine();
            }
        }

        bool enableKernelSize = (selectedFilter == 0 || selectedFilter == 2 || selectedFilter == 3);
        if (!enableKernelSize) ImGui::BeginDisabled();

        if (ImGui::SliderInt("Kernel Size", &kernelSize, 3, 15)) {
            if (kernelSize % 2 == 0) kernelSize++;
        }

        if (selectedFilter == 1) {
            ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.1f, 1.0f), "Default: 1x1 for this kernel.");
        } else if (!enableKernelSize) {
            ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.1f, 1.0f), "Default: 3x3 for this kernel.");
        }

        if (!enableKernelSize) ImGui::EndDisabled();

        // It has to replicate the order of PaddingType in ConvolutionType.h
        const char* PaddingTypes[] = { "Zero", "Replicate" };
        ImGui::Combo("Padding Type", &selectedPadding, PaddingTypes, IM_ARRAYSIZE(PaddingTypes));

        if (ImGui::Button("Apply")) {
            ConvolutionFilter filter(static_cast<ConvolutionType>(selectedFilter), kernelSize, customKernel, static_cast<PaddingType>(selectedPadding));
            texture = filter.apply(texture);
            sprite.setTexture(texture, true);
            showConvolutionWindow = false;
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) showConvolutionWindow = false;

        ImGui::End();
    }

    ImGui::EndDisabled();

    ImGui::SameLine();
    ImGui::PushID("ResizePanel");
    ImGui::InvisibleButton("##resize", ImVec2(5, ImGui::GetContentRegionAvail().y), ImGuiButtonFlags_MouseButtonLeft);
    if (ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows) && !ImGui::IsAnyItemHovered() && !ImGui::IsAnyItemActive()) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
    }
    if (ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows) && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
        PANEL_WIDTH += ImGui::GetIO().MouseDelta.x;
        PANEL_WIDTH = std::clamp(PANEL_WIDTH, 100.0f, 250.0f);
        float scaleX = static_cast<float>(window.getSize().x - 2 * PANEL_WIDTH) / texture.getSize().x;
        float scaleY = static_cast<float>(window.getSize().y) / texture.getSize().y;
        float scale = std::min(scaleX, scaleY);
        sprite.setScale({ scale, scale });
        sprite.setPosition({ (window.getSize().x - sprite.getGlobalBounds().size.x + PANEL_WIDTH) / 2,
                             (window.getSize().y - sprite.getGlobalBounds().size.y) / 2 });
    }
    ImGui::PopID();
    ImGui::End();
}

void Application::handleImageLoading() {
    if (!fileDialog.HasSelected()) return;

    if (!texture.loadFromFile(fileDialog.GetSelected().string())) {
        std::cerr << "Failed to load image from path." << std::endl;
        return;
    }

    bool hasLoaded = original_texture.loadFromImage(texture.copyToImage());
    if (!hasLoaded) {
        std::cerr << "Erreur lors du chargement de la texture de l'image." << std::endl;
        exit(1);
    }

    sprite.setTexture(texture, true);
    hasImage = true;

    sf::Vector2u windowSize = window.getSize();
    sf::Vector2u textureSize = texture.getSize();
    float scaleX = static_cast<float>(windowSize.x - 2 * PANEL_WIDTH) / textureSize.x;
    float scaleY = static_cast<float>(windowSize.y) / textureSize.y;
    float scale = std::min(scaleX, scaleY);
    sprite.setScale({ scale, scale });
    sf::FloatRect spriteBounds = sprite.getGlobalBounds();
    sprite.setPosition({ (windowSize.x - spriteBounds.size.x + PANEL_WIDTH) / 2,
                         (windowSize.y - spriteBounds.size.y) / 2 });

    std::cout << "Selected filename: " << fileDialog.GetSelected().string() << std::endl;
    fileDialog.ClearSelected();
}
