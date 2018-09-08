/**
 * TestResources.cpp
 *
 * Todos os direitos reservados.
 *
 */

#include "Buffer.h"
#include "Image.h"

#include "TestManagerFixture.h"

#include <vulkan/vulkan.h>

/*
TEST_F(TestManagerFixture, TestBuffer) {
    Result<std::shared_ptr<Buffer>> result = Buffer::createBuffer(128, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    ASSERT_EQ(result.getError(), Error::None);

    auto buffer = static_cast<std::shared_ptr<Buffer>>(result);
    ASSERT_NE(buffer, nullptr);

    Result<VkBuffer> rslt = buffer->getVulkanBuffer();
    ASSERT_EQ(rslt.getError(), Error::None);

    auto vkBuffer = static_cast<VkBuffer>(rslt);
    ASSERT_NE(vkBuffer, nullptr);
}

TEST_F(TestManagerFixture, TestImage) {
    Result<std::shared_ptr<Image>> result = Image::createImage({1024, 1024, 1},
                                                               VK_IMAGE_TYPE_2D,
                                                               1,
                                                               8,
                                                               VK_IMAGE_USAGE_SAMPLED_BIT &
                                                               VK_IMAGE_USAGE_STORAGE_BIT,
                                                               VK_FORMAT_R8G8B8A8_UNORM,
                                                               VK_IMAGE_TILING_OPTIMAL);
    ASSERT_EQ(result.getError(), Error::None);

    auto image = static_cast<std::shared_ptr<Image>>(result);
    ASSERT_NE(image, nullptr);

    Result<VkImage> rslt = image->getVulkanImage();
    ASSERT_EQ(rslt.getError(), Error::None);

    auto vkImage = static_cast<VkImage>(rslt);
    ASSERT_NE(vkImage, nullptr);
}
*/
