#include "imgui.h" // necessary for ImGui::*, imgui-SFML.h doesn't include imgui.h
#include "imfilebrowser.h"
#include "imgui-SFML.h" // for ImGui::SFML::* functions and SFML-specific overloads

#include "../include/filters.h"
#include "../include/image_encoding.h"

#include <SFML/Graphics.hpp>
#include <iostream>

int main() {
    sf::RenderWindow window(sf::VideoMode({ 640, 480 }), "ImGui + SFML = <3");
    window.setFramerateLimit(60);
    ImGui::SFML::Init(window);

    // create a file browser instance
    ImGui::FileBrowser fileDialog;

    // (optional) set browser properties
    fileDialog.SetTitle("Select an image");
    fileDialog.SetTypeFilters({ ".jpg", ".jpeg", ".png" });

    sf::Texture texture;
	sf::Texture original_texture;
    sf::Sprite sprite(texture);
    bool hasImage = false;
    bool hasChannelReseted = false;
    bool redSelected = false, greenSelected = false, blueSelected = false;

    sf::Clock deltaClock;
    while (window.isOpen()) {
        while (const auto event = window.pollEvent()) {
            ImGui::SFML::ProcessEvent(window, *event);

            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());


        if (ImGui::Begin("Window"))
        {
            if (ImGui::Button("Open file"))
                fileDialog.Open();

            ImGui::BeginDisabled(!hasImage);

            if (ImGui::Button("Original")) {
                texture = original_texture;
                sprite.setTexture(texture, true);
            }

            if (ImGui::Button("Gray Scale")) {
                texture = original_texture;
                texture = gray_scale(texture);
                sprite.setTexture(texture, true);
            }

            ImGui::Text("Choose channels :");
            ImGui::Checkbox("Red", &redSelected);
            ImGui::SameLine();
            ImGui::Checkbox("Green", &greenSelected);
            ImGui::SameLine();
            ImGui::Checkbox("Blue", &blueSelected);

            if (redSelected || greenSelected || blueSelected) {
                hasChannelReseted = false;
                texture = original_texture;
                texture = combine_channels(texture, redSelected, greenSelected, blueSelected);
                sprite.setTexture(texture, true);
            }
            else if (hasChannelReseted == false) {
                hasChannelReseted = true;
				texture = original_texture;
				sprite.setTexture(texture, true);
            }

            ImGui::Text("Choose filter :");

            if (ImGui::Button("Average filter")) {
                texture = convolution_filter(texture, FilterType::MEAN, 15, PaddingType::REPLICATE);
                sprite.setTexture(texture, true);
            }

            ImGui::EndDisabled();
        }
        ImGui::End();

        fileDialog.Display();

        if (fileDialog.HasSelected())
        {
            if (!texture.loadFromFile(fileDialog.GetSelected().string())) {
                std::cerr << "Failed to load image from path." << std::endl;
                return EXIT_FAILURE;
            }

			// save the original texture, not sure it's the best way but it works
			original_texture.loadFromImage(texture.copyToImage());

			// Rescale the image to fit the window
			sprite.setTexture(texture, true);
            hasImage = true;
            sf::Vector2u windowSize = window.getSize();
            sf::Vector2u textureSize = texture.getSize();
            float scaleX = static_cast<float>(windowSize.x) / textureSize.x;
            float scaleY = static_cast<float>(windowSize.y) / textureSize.y;
            float scale = std::min(scaleX, scaleY);
            sprite.setScale({ scale, scale });
            sf::FloatRect spriteBounds = sprite.getGlobalBounds();
            sprite.setPosition({ (windowSize.x - spriteBounds.size.x) / 2,
                (windowSize.y - spriteBounds.size.y) / 2 });
			std::cout << "Selected filename: " << fileDialog.GetSelected().string() << std::endl;
            fileDialog.ClearSelected();
        }

        window.clear(sf::Color::Black);
	    
		if (hasImage)
		    window.draw(sprite);

        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
}

