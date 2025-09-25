#include <ThING/core.h>

void ProtoThiApp::createBasicGraphicsPipeline() {
    auto basicVertShaderCode = readFile("../shaders/basicVert.spv");
    auto basicFragShaderCode = readFile("../shaders/basicFrag.spv");

    VkShaderModule basicVertShaderModule = createShaderModule(basicVertShaderCode);
    VkShaderModule basicFragShaderModule = createShaderModule(basicFragShaderCode);

    VkPipelineShaderStageCreateInfo basicVertShaderStageInfo{};
    basicVertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    basicVertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    basicVertShaderStageInfo.module = basicVertShaderModule;
    basicVertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo basicFragShaderStageInfo{};
    basicFragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    basicFragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    basicFragShaderStageInfo.module = basicFragShaderModule;
    basicFragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo basicShaderStages[] = {basicVertShaderStageInfo, basicFragShaderStageInfo};

    VkPipelineVertexInputStateCreateInfo basicVertexInputInfo{};
    basicVertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    auto basicBindingDescription = Vertex::getBindingDescription();
    auto basicAttributeDescriptions = Vertex::getAttributeDescriptions();

    basicVertexInputInfo.vertexBindingDescriptionCount = 1;
    basicVertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(basicAttributeDescriptions.size());
    basicVertexInputInfo.pVertexBindingDescriptions = &basicBindingDescription;
    basicVertexInputInfo.pVertexAttributeDescriptions = basicAttributeDescriptions.data();

    VkPipelineInputAssemblyStateCreateInfo basicInputAssembly{};
    basicInputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    basicInputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    basicInputAssembly.primitiveRestartEnable = VK_FALSE;

    VkPipelineViewportStateCreateInfo basicViewportState{};
    basicViewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    basicViewportState.viewportCount = 1;
    basicViewportState.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo basicRasterizer{};
    basicRasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    basicRasterizer.depthClampEnable = VK_FALSE;
    basicRasterizer.rasterizerDiscardEnable = VK_FALSE;
    basicRasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    basicRasterizer.lineWidth = 1.0f;
    //*----------------------------------------------------
    basicRasterizer.cullMode = VK_CULL_MODE_NONE;
    //*----------------------------------------------------
    basicRasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    basicRasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo basicMultisampling{};
    basicMultisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    basicMultisampling.sampleShadingEnable = VK_FALSE;
    basicMultisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState basicColorBlendAttachment{};
    basicColorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    basicColorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo basicColorBlending{};
    basicColorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    basicColorBlending.logicOpEnable = VK_FALSE;
    basicColorBlending.logicOp = VK_LOGIC_OP_COPY;
    basicColorBlending.attachmentCount = 1;
    basicColorBlending.pAttachments = &basicColorBlendAttachment;
    basicColorBlending.blendConstants[0] = 0.0f;
    basicColorBlending.blendConstants[1] = 0.0f;
    basicColorBlending.blendConstants[2] = 0.0f;
    basicColorBlending.blendConstants[3] = 0.0f;

    std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo basicDynamicState{};
    basicDynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    basicDynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    basicDynamicState.pDynamicStates = dynamicStates.data();

    // push constants for polygon transforms
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = Polygon::PushConstantSize();

    VkPipelineLayoutCreateInfo basicPipelineLayoutInfo{};
    basicPipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    basicPipelineLayoutInfo.setLayoutCount = 1;
    basicPipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
    basicPipelineLayoutInfo.pushConstantRangeCount = 1;
    basicPipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

    if (vkCreatePipelineLayout(device, &basicPipelineLayoutInfo, nullptr, &pipelineLayouts[0]) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    VkGraphicsPipelineCreateInfo basicPipelineInfo{};
    basicPipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    basicPipelineInfo.stageCount = 2;
    basicPipelineInfo.pStages = basicShaderStages;
    basicPipelineInfo.pVertexInputState = &basicVertexInputInfo;
    basicPipelineInfo.pInputAssemblyState = &basicInputAssembly;
    basicPipelineInfo.pViewportState = &basicViewportState;
    basicPipelineInfo.pRasterizationState = &basicRasterizer;
    basicPipelineInfo.pMultisampleState = &basicMultisampling;
    basicPipelineInfo.pColorBlendState = &basicColorBlending;
    basicPipelineInfo.pDynamicState = &basicDynamicState;
    basicPipelineInfo.layout = pipelineLayouts[0];
    basicPipelineInfo.renderPass = renderPass;
    basicPipelineInfo.subpass = 0;
    basicPipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &basicPipelineInfo, nullptr, &graphicsPipelines[0]) != VK_SUCCESS) {
        throw std::runtime_error("failed to create graphics pipeline!");
    }



    vkDestroyShaderModule(device, basicFragShaderModule, nullptr);
    vkDestroyShaderModule(device, basicVertShaderModule, nullptr);
}


void ProtoThiApp::createCircleGraphicsPipeline() {
    auto circleVertShaderCode = readFile("../shaders/circleVert.spv");
    auto circleFragShaderCode = readFile("../shaders/circleFrag.spv");

    VkShaderModule circleVertShaderModule = createShaderModule(circleVertShaderCode);
    VkShaderModule circleFragShaderModule = createShaderModule(circleFragShaderCode);

    VkPipelineShaderStageCreateInfo circleVertShaderStageInfo{};
    circleVertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    circleVertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    circleVertShaderStageInfo.module = circleVertShaderModule;
    circleVertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo circleFragShaderStageInfo{};
    circleFragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    circleFragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    circleFragShaderStageInfo.module = circleFragShaderModule;
    circleFragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo circleShaderStages[] = {circleVertShaderStageInfo, circleFragShaderStageInfo};

    VkPipelineVertexInputStateCreateInfo circleVertexInputInfo{};
    circleVertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    auto quadBindingDescription = Quad::getBindingDescription();
    auto quadAttributeDescriptions = Quad::getAttributeDescriptions();
    auto circleBindingDescription = Circle::getBindingDescription();
    auto circleAttributeDescriptions = Circle::getAttributeDescriptions();

    VkVertexInputBindingDescription bindingDescription[] = {quadBindingDescription, circleBindingDescription};
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
    attributeDescriptions.insert(attributeDescriptions.end(), quadAttributeDescriptions.begin(), quadAttributeDescriptions.end());
    attributeDescriptions.insert(attributeDescriptions.end(), circleAttributeDescriptions.begin(), circleAttributeDescriptions.end());


    circleVertexInputInfo.vertexBindingDescriptionCount = 2;
    circleVertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(quadAttributeDescriptions.size() + circleAttributeDescriptions.size());
    circleVertexInputInfo.pVertexBindingDescriptions = bindingDescription;
    circleVertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    VkPipelineInputAssemblyStateCreateInfo circleInputAssembly{};
    circleInputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    circleInputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    circleInputAssembly.primitiveRestartEnable = VK_FALSE;

    VkPipelineViewportStateCreateInfo circleViewportState{};
    circleViewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    circleViewportState.viewportCount = 1;
    circleViewportState.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo circleRasterizer{};
    circleRasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    circleRasterizer.depthClampEnable = VK_FALSE;
    circleRasterizer.rasterizerDiscardEnable = VK_FALSE;
    circleRasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    circleRasterizer.lineWidth = 1.0f;
    //*----------------------------------------------------
    circleRasterizer.cullMode = VK_CULL_MODE_NONE;
    //*----------------------------------------------------
    circleRasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    circleRasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo circleMultisampling{};
    circleMultisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    circleMultisampling.sampleShadingEnable = VK_FALSE;
    circleMultisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState circleColorBlendAttachment{};
    circleColorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    circleColorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo circleColorBlending{};
    circleColorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    circleColorBlending.logicOpEnable = VK_FALSE;
    circleColorBlending.logicOp = VK_LOGIC_OP_COPY;
    circleColorBlending.attachmentCount = 1;
    circleColorBlending.pAttachments = &circleColorBlendAttachment;
    circleColorBlending.blendConstants[0] = 0.0f;
    circleColorBlending.blendConstants[1] = 0.0f;
    circleColorBlending.blendConstants[2] = 0.0f;
    circleColorBlending.blendConstants[3] = 0.0f;

    std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo circleDynamicState{};
    circleDynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    circleDynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    circleDynamicState.pDynamicStates = dynamicStates.data();

    VkPipelineLayoutCreateInfo circlePipelineLayoutInfo{};
    circlePipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    circlePipelineLayoutInfo.setLayoutCount = 1;
    circlePipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
    circlePipelineLayoutInfo.pushConstantRangeCount = 0;
    circlePipelineLayoutInfo.pPushConstantRanges = nullptr;

    if (vkCreatePipelineLayout(device, &circlePipelineLayoutInfo, nullptr, &pipelineLayouts[1]) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    VkGraphicsPipelineCreateInfo circlePipelineInfo{};
    circlePipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    circlePipelineInfo.stageCount = 2;
    circlePipelineInfo.pStages = circleShaderStages;
    circlePipelineInfo.pVertexInputState = &circleVertexInputInfo;
    circlePipelineInfo.pInputAssemblyState = &circleInputAssembly;
    circlePipelineInfo.pViewportState = &circleViewportState;
    circlePipelineInfo.pRasterizationState = &circleRasterizer;
    circlePipelineInfo.pMultisampleState = &circleMultisampling;
    circlePipelineInfo.pColorBlendState = &circleColorBlending;
    circlePipelineInfo.pDynamicState = &circleDynamicState;
    circlePipelineInfo.layout = pipelineLayouts[1];
    circlePipelineInfo.renderPass = renderPass;
    circlePipelineInfo.subpass = 0;
    circlePipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &circlePipelineInfo, nullptr, &graphicsPipelines[1]) != VK_SUCCESS) {
        throw std::runtime_error("failed to create graphics pipeline!");
    }



    vkDestroyShaderModule(device, circleFragShaderModule, nullptr);
    vkDestroyShaderModule(device, circleVertShaderModule, nullptr);
}
