#include "imgui.h" // necessary for ImGui::*, imgui-SFML.h doesn't include imgui.h
#include "imfilebrowser.h"
#include "imgui-SFML.h" // for ImGui::SFML::* functions and SFML-specific overloads

#include "../include/filters.h"
#include "../include/image_encoding.h"

#include <SFML/Graphics.hpp>
#include <iostream>

static float PANEL_WIDTH = 110.0f;
static bool showConvolutionWindow = false;
static int selectedFilter = 0;
static int kernelSize = 3;

int main() {
    const int windowWidth = 1250;
    const float aspectRatio = 16.0f / 9.0f;
    const int windowHeight = static_cast<int>(windowWidth / aspectRatio);

    sf::RenderWindow window(sf::VideoMode({ windowWidth, windowHeight }), "TTah l'itfi <3");
    window.setFramerateLimit(60);
    ImGui::SFML::Init(window);

    ImGui::FileBrowser fileDialog;

    fileDialog.SetTitle("Select an image");
    fileDialog.SetTypeFilters({ ".jpg", ".jpeg", ".png" });

    sf::Texture texture;
	sf::Texture original_texture;
    sf::Sprite sprite(texture);
    bool hasImage = false;
    bool hasChannelReseted = false;
    bool redSelected = true, greenSelected = true, blueSelected = true;

    sf::Clock deltaClock;
    while (window.isOpen()) {
        while (const auto event = window.pollEvent()) {
            ImGui::SFML::ProcessEvent(window, *event);

            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Load")) {
                    fileDialog.Open();
                }

                if (ImGui::MenuItem("Exit")) {
                    exit(0);
                }

                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Image", hasImage))
            {
                if (ImGui::MenuItem("Original", nullptr, false, hasImage)) {
                    texture = original_texture;
                    redSelected = true;
                    greenSelected = true;
                    blueSelected = true;
                    hasChannelReseted = false;
                    sprite.setTexture(texture, true);
                }

                if (ImGui::MenuItem("Greyscale", nullptr, false, hasImage)) {
                    texture = original_texture;
                    texture = gray_scale(texture);
                    redSelected = false;
                    greenSelected = false;
                    blueSelected = false;
                    hasChannelReseted = true;
                    sprite.setTexture(texture, true);
                }

                if (ImGui::BeginMenu("Channel", hasImage))
                {
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
                        texture = combine_channels(texture, redSelected, greenSelected, blueSelected);
                        sprite.setTexture(texture, true);
                    }

                    ImGui::EndMenu();
                }

                ImGui::EndMenu();
            }
        }
        ImGui::EndMainMenuBar();

        {
            ImGui::SetNextWindowPos(ImVec2(0, 20));
            ImGui::SetNextWindowSize(ImVec2(PANEL_WIDTH, ImGui::GetIO().DisplaySize.y - 20));

            ImGui::Begin("Filters", nullptr, ImGuiWindowFlags_NoMove);
            ImGui::BeginDisabled(!hasImage);

            if (ImGui::Button("Convolution")) {
				showConvolutionWindow = true;
            }
            if (showConvolutionWindow) {
                ImGui::Begin("Convolution Options", &showConvolutionWindow, ImGuiWindowFlags_AlwaysAutoResize);

                const char* filterTypes[] = { "Custom", "Identity", "Mean", "Gaussian", "Sharpen", "Laplacien", "Edge reinforcement (hor)", "Edge reinforcement (ver)"};
                ImGui::Combo("Filter Type", &selectedFilter, filterTypes, IM_ARRAYSIZE(filterTypes));

                static std::vector<double> customKernel(9, 0.0f);

                if (selectedFilter == 0) {
                    ImGui::Text("Custom Kernel");
                    int kernelArea = kernelSize * kernelSize;

                    if (customKernel.size() != kernelArea) {
                        customKernel.resize(kernelArea, 0.0f);
                    }

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

                bool enableKernelSize = (selectedFilter == 2 || selectedFilter == 3 || selectedFilter == 0);
                if (!enableKernelSize) ImGui::BeginDisabled();

                if (ImGui::SliderInt("Kernel Size", &kernelSize, 3, 15)) {
                    if (kernelSize % 2 == 0) {
                        kernelSize += 1;
                    }
                }

                if (selectedFilter == 1) {
                    ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.1f, 1.0f), "Default: 1x1 for this kernel.");
				}
				else if (!enableKernelSize) {
					ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.1f, 1.0f), "Default: 3x3 for this kernel.");
				}

                if (!enableKernelSize) ImGui::EndDisabled();

                if (ImGui::Button("Apply")) {
                    switch (selectedFilter) {
                    case 0:
                        texture = convolution_filter(texture, FilterType::CUSTOM, kernelSize, customKernel);
                        break;
                    case 1:
                        texture = convolution_filter(texture, FilterType::IDENTITY, kernelSize);
                        break;
                    case 2:
                        texture = convolution_filter(texture, FilterType::MEAN, kernelSize);
                        break;
                    case 3:
                        texture = convolution_filter(texture, FilterType::GAUSSIAN, kernelSize);
                        break;
                    case 4:
                        texture = convolution_filter(texture, FilterType::SHARPEN, kernelSize);
                        break;
                    case 5:
                        texture = convolution_filter(texture, FilterType::LAPLACIEN, kernelSize);
                        break;
					case 6:
						texture = convolution_filter(texture, FilterType::EDGE_REINFORCEMENT_HOR, kernelSize);
						break;
					case 7:
						texture = convolution_filter(texture, FilterType::EDGE_REINFORCEMENT_VER, kernelSize);
						break;
                    }
                    sprite.setTexture(texture, true);
                    showConvolutionWindow = false;
                }

                ImGui::SameLine();
                if (ImGui::Button("Cancel")) {
                    showConvolutionWindow = false;
                }

                ImGui::End();
            }

            ImGui::EndDisabled();

            ImGui::SameLine();
            ImGui::PushID("ResizePanel");
            ImGui::InvisibleButton("##resize", ImVec2(5, ImGui::GetContentRegionAvail().y), ImGuiButtonFlags_MouseButtonLeft);

            if (ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows) &&
                !ImGui::IsAnyItemHovered() &&
                !ImGui::IsAnyItemActive()) {
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

        fileDialog.Display();

        if (fileDialog.HasSelected())
        {
            if (!texture.loadFromFile(fileDialog.GetSelected().string())) {
                std::cerr << "Failed to load image from path." << std::endl;
                return EXIT_FAILURE;
            }

			original_texture.loadFromImage(texture.copyToImage());

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

        window.clear({42, 42, 42});
	    
		if (hasImage)
		    window.draw(sprite);

        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
}

