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

    // Variables pour stocker la texture et le sprite sélectionnés
    sf::Texture texture;
	sf::Texture original_texture;
    sf::Sprite sprite(texture);
    bool hasImage = false;
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
            // Bouton d'ouverture de fichier toujours actif
            if (ImGui::Button("Open file"))
                fileDialog.Open();

            // On désactive les autres boutons si aucune image n'est chargée
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

            // Boutons de sélection des canaux
            ImGui::Text("Choose channels :");
            ImGui::Checkbox("Red", &redSelected);
            ImGui::SameLine();
            ImGui::Checkbox("Green", &greenSelected);
            ImGui::SameLine();
            ImGui::Checkbox("Blue", &blueSelected);

            // Exemple d'application des sélections sur la texture
            if (redSelected || greenSelected || blueSelected) {
                texture = original_texture;
                // Fonction fictive qui combine les canaux sélectionnés
                texture = combine_channels(texture, redSelected, greenSelected, blueSelected);
                sprite.setTexture(texture, true);
            }

            ImGui::Text("Choose filter :");

            if (ImGui::Button("Average filter")) {
                texture = average_filter(texture);
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

			original_texture.loadFromImage(texture.copyToImage());

            // Create a sprite that uses the loaded texture.
			sprite.setTexture(texture, true);
            hasImage = true;

            // Taille de la fenêtre et de l'image (dans la texture)
            sf::Vector2u windowSize = window.getSize();
            sf::Vector2u textureSize = texture.getSize();

            // Calcul des facteurs d'échelle sur X et sur Y
            float scaleX = static_cast<float>(windowSize.x) / textureSize.x;
            float scaleY = static_cast<float>(windowSize.y) / textureSize.y;

            // Choisir le plus petit pour ne pas déformer l'image
            float scale = std::min(scaleX, scaleY);

            // Appliquer ce facteur sur le sprite
            sprite.setScale({ scale, scale });

            // (Optionnel) Centrer l'image dans la fenêtre
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

