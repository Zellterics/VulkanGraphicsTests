#include <ThING/core.h>
#include <vulkan/vulkan_core.h>

// Dear ImGui
#include "imgui.h"

// Backends (GLFW + Vulkan)
#include "backends/imgui_impl_vulkan.h"

void ProtoThiApp::createFramebuffers() {
    swapChainFramebuffers.resize(swapChainImageViews.size());

    for (size_t i = 0; i < swapChainImageViews.size(); i++) {
        VkImageView attachments[] = {
            swapChainImageViews[i]
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = pipelineManager.getrenderPass();
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = swapChainExtent.width;
        framebufferInfo.height = swapChainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

void ProtoThiApp::createCustomBuffers(){
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
    for(int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++){
        vertexBuffers[i].device = device;
        bufferManager.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffers[i].buffer, vertexBuffers[i].memory);
        bufferManager.uploadBuffer(bufferSize, &vertexBuffers[i].buffer, vertices.data());
    }

    bufferSize = sizeof(indices[0]) * indices.size();
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++){
        indexBuffers[i].device = device;
        bufferManager.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffers[i].buffer, indexBuffers[i].memory);
        bufferManager.uploadBuffer(bufferSize, &indexBuffers[i].buffer, indices.data());
    }

    bufferSize = sizeof(quadVertices[0]) * quadVertices.size();
    quadBuffer.device = device;
    bufferManager.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, quadBuffer.buffer, quadBuffer.memory);
    bufferManager.uploadBuffer(bufferSize, &quadBuffer.buffer, quadVertices.data());

    bufferSize = sizeof(quadIndices[0]) * quadIndices.size();
    quadIndexBuffer.device = device;
    bufferManager.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, quadIndexBuffer.buffer, quadIndexBuffer.memory);
    bufferManager.uploadBuffer(bufferSize, &quadIndexBuffer.buffer, quadIndices.data());

    bufferSize = sizeof(circleCenters[0]) * circleCenters.size();
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++){
        circleBuffers[i].device = device;
        bufferManager.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, circleBuffers[i].buffer, circleBuffers[i].memory);
        bufferManager.uploadBuffer(bufferSize, &circleBuffers[i].buffer, circleCenters.data());
    }
}

void ProtoThiApp::createCommandBuffers() {
    commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t) commandBuffers.size();
    if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

void ProtoThiApp::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = pipelineManager.getrenderPass();
    renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = swapChainExtent;

    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float) swapChainExtent.width;
        viewport.height = (float) swapChainExtent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = swapChainExtent;
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
        {
            vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineManager.getGraphicsPipelines()[0]);
            vkCmdBindDescriptorSets(
                commandBuffer,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                pipelineManager.getLayouts()[0],
                0, 1, &pipelineManager.getDescriptorSets()[currentFrame],
                0, nullptr
            );

            VkBuffer vb = vertexBuffers[currentFrame].buffer;
            VkDeviceSize offsets[] = {0};
            vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vb, offsets);
            
            VkBuffer ib = indexBuffers[currentFrame].buffer;
            vkCmdBindIndexBuffer(commandBuffer, ib, 0, VK_INDEX_TYPE_UINT16);
            for (const auto& poly : polygons) {
                if (!poly.alive) continue;

                vkCmdPushConstants(
                    commandBuffer,
                    pipelineManager.getLayouts()[0],
                    VK_SHADER_STAGE_VERTEX_BIT,
                    0,
                    Polygon::PushConstantSize(),
                    &poly.pushConstant
                );

                vkCmdDrawIndexed(
                    commandBuffer,
                    poly.indexCount,
                    1,
                    poly.indexOffset,
                    poly.vertexOffset,
                    0
                );
            }
        }
        {
            vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineManager.getGraphicsPipelines()[1]);
            vkCmdBindDescriptorSets(
                commandBuffer,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                pipelineManager.getLayouts()[1],
                0, 1, &pipelineManager.getDescriptorSets()[currentFrame],
                0, nullptr
            );

            VkBuffer vb[] = {quadBuffer.buffer, circleBuffers[currentFrame].buffer};
            VkDeviceSize offsets[] = {0,0};
            vkCmdBindVertexBuffers(commandBuffer, 0, 2, vb, offsets);
            
            VkBuffer ib = quadIndexBuffer.buffer;
            vkCmdBindIndexBuffer(commandBuffer, ib, 0, VK_INDEX_TYPE_UINT16);
            vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(quadIndices.size()), circleCenters.size(), 0, 0, 0);
        }
    
        // -------- integración de ImGui --------
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);

        // -------- fin --------
    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}