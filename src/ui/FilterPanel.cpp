
#include "ui/Application.h"

#include "filters/ConvolutionFilter.h"

void Application::renderFilterPanel() {
    ImGui::SetNextWindowPos(ImVec2(0, 20));
    ImGui::SetNextWindowSize(ImVec2(PANEL_WIDTH, ImGui::GetIO().DisplaySize.y - 20));

    ImGui::Begin("Filters", nullptr, ImGuiWindowFlags_NoMove);
    ImGui::BeginDisabled(!hasImage);

    renderConvolutionButton();
    if (showConvolutionWindow) renderConvolutionOptionsWindow();

    ImGui::EndDisabled();

    // Resize panel
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

void Application::renderConvolutionButton() {
    if (ImGui::Button("Convolution")) {
        showConvolutionWindow = true;
    }
}

void Application::renderConvolutionOptionsWindow() {
    ImGui::Begin("Convolution Options", &showConvolutionWindow, ImGuiWindowFlags_AlwaysAutoResize);

    const char* ConvolutionTypes[] = { "Custom", "Identity", "Mean", "Gaussian", "Sharpen", "Laplacien", "Edge reinforcement (hor)", "Edge reinforcement (ver)" };
    ImGui::Combo("Filter Type", &selectedConvolution, ConvolutionTypes, IM_ARRAYSIZE(ConvolutionTypes));

    static std::vector<double> customKernel(9, 0.0f);
    if (static_cast<ConvolutionType>(selectedConvolution) == ConvolutionType::CUSTOM) {
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

    bool enableKernelSize = (static_cast<ConvolutionType>(selectedConvolution) == ConvolutionType::CUSTOM ||
                             static_cast<ConvolutionType>(selectedConvolution) == ConvolutionType::MEAN ||
                             static_cast<ConvolutionType>(selectedConvolution) == ConvolutionType::GAUSSIAN);
    if (!enableKernelSize) ImGui::BeginDisabled();

    maxKernelSize = (static_cast<ConvolutionType>(selectedConvolution) == ConvolutionType::CUSTOM) ? 7 : 150;

    if (ImGui::SliderInt("Kernel Size", &kernelSize, 1, maxKernelSize)) {
        if (kernelSize % 2 == 0) kernelSize++; // force kernel to be odd
    }

    if (static_cast<ConvolutionType>(selectedConvolution) == ConvolutionType::GAUSSIAN) {
        ImGui::SliderFloat("Sigma", &sigma, 0.3f, 20.0f);
    }

    if (static_cast<ConvolutionType>(selectedConvolution) == ConvolutionType::IDENTITY) {
        ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.1f, 1.0f), "Default: 1x1 for this kernel.");
    } else if (!enableKernelSize) {
        kernelSize = 3;
        ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.1f, 1.0f), "Default: 3x3 for this kernel.");
    }

    if (!enableKernelSize) ImGui::EndDisabled();

    const char* PaddingTypes[] = { "Zero", "Replicate" };
    ImGui::Combo("Padding Type", &selectedPadding, PaddingTypes, IM_ARRAYSIZE(PaddingTypes));

    const char* backendOptions[] = { "CPU", "GPU (custom)", "GPU (built-in)" };
    ImGui::Combo("Computation Backend", &selectedBackend, backendOptions, IM_ARRAYSIZE(backendOptions));
    if ((static_cast<ConvolutionType>(selectedConvolution) == ConvolutionType::GAUSSIAN &&
        static_cast<BackendType>(selectedBackend) == BackendType::GPU_BUILD_IN)) {
        ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.1f, 1.0f), "Error: This backend is not available.");
    }

    const char* algorithmOptions[] = { "Naive", "Separable", "FFT", "Tiled" };
    ImGui::Combo("Algorithm", &selectedAlgorithm, algorithmOptions, IM_ARRAYSIZE(algorithmOptions));
    if (static_cast<AlgorithmType>(selectedAlgorithm) != AlgorithmType::Naive) {
        ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.1f, 1.0f), "Error: This algorithm is not implemented.");
    }

    if (ImGui::Button("Apply")) {
        ConvolutionFilter filter(static_cast<ConvolutionType>(selectedConvolution), kernelSize, sigma,
                                 static_cast<PaddingType>(selectedPadding), static_cast<BackendType>(selectedBackend),
                                 static_cast<AlgorithmType>(selectedAlgorithm), customKernel);
        texture = filter.apply(texture);
        sprite.setTexture(texture, true);
        showConvolutionWindow = false;
    }

    ImGui::SameLine();
    if (ImGui::Button("Cancel")) showConvolutionWindow = false;

    ImGui::End();
}
