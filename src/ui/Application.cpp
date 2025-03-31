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
