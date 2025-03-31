
#include "ui/Application.h"

#include "core/ImageEncoder.h"

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
