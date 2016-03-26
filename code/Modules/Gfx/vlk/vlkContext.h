#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::vlkContext
    @brief Vulkan instance, device, swap-chain helpers
*/
#include "Core/Types.h"
#include "Core/Containers/Array.h"
#include "Gfx/Setup/GfxSetup.h"
#include "Gfx/vlk/vlk_impl.h"
#include "Gfx/vlk/vlkConfig.h"
#include "Gfx/vlk/vlkSyncPool.h"
#include "Gfx/vlk/vlkResAllocator.h"
#include "Gfx/vlk/vlkExt.h"

namespace Oryol {
namespace _priv {

class vlkContext {
public:
    /// destructor
    ~vlkContext();
    /// perform setup before GLFW window has been opened
    void setupBeforeWindow(const Array<const char*>& layers, const Array<const char*>& exts);
    /// perform setup after GLFW window has been opened
    DisplayAttrs setupAfterWindow(const GfxSetup& setup, const DisplayAttrs& attrs, const Array<const char*>& layers, const Array<const char*>& exts, VkSurfaceKHR surf);
    /// discard everything
    void discard();

    /// start a new frame
    VkCommandBuffer beginFrame();
    /// present the current swap chain image and handle frame sync
    void present(VkCommandBuffer cmdBuf);
    /// get current default framebuffer for this frame
    VkFramebuffer curFramebuffer() const;
    /// get current swapchain image
    VkImage curSwapChainImage() const;

    VkInstance Instance = nullptr;
    VkPhysicalDevice GPU = nullptr;
    VkDevice Device = nullptr;
    VkSurfaceKHR Surface = nullptr;
    VkQueue Queue = nullptr; 
    VkSwapchainKHR SwapChain = nullptr;
    VkRenderPass RenderPass = nullptr;
    uint64 CurFrameIndex = 0;
    uint64 CurFrameRotateIndex = 0;
private:
    /// setup the Vulkan instance
    void setupInstance(const Array<const char*>& layers, const Array<const char*>& exts);
    /// discard the Vulkan instance
    void discardInstance();
    /// setup the physical device
    void setupGPU();
    /// discard the physical device
    void discardGPU();
    /// setup the logical device
    void setupDevice(const Array<const char*>& layers, const Array<const char*>& exts);
    /// discard the logical device
    void discardDevice();
    /// setup the swapchain and default framebuffer
    DisplayAttrs setupSwapchain(const GfxSetup& setup, const DisplayAttrs& attrs);
    /// discard swapchain and default framebuffer
    void discardSwapchain(bool forResize);

    /// allocate a new command buffer and begin recording
    VkCommandBuffer beginCmdBuffer();
    /// submit and free command buffer
    void submitCmdBuffer(VkCommandBuffer cmdBuf, VkSemaphore waitSem, VkSemaphore doneSem);

    /// find matching memory type index from memory properties, return InvalidIndex if no match
    int findMemoryType(uint32 typeBits, VkFlags requirementsMask);

    vlkExt ext;
    vlkSyncPool syncPool;
    vlkResAllocator resAllocator;

    VkPhysicalDeviceMemoryProperties memoryProps = { };
    int graphicsQueueIndex = InvalidIndex;
    int presentQueueIndex = InvalidIndex;
    VkFormat pixelFormat = VK_FORMAT_MAX_ENUM;
    VkColorSpaceKHR colorSpace = VK_COLORSPACE_MAX_ENUM;
    VkCommandPool cmdPool = nullptr;
    struct SwapChainBuffer {
        VkFramebuffer framebuffer = nullptr;
        VkImage image = nullptr;
        VkImageView view = nullptr;
        VkSemaphore imageAcquiredSemaphore = nullptr;
        VkSemaphore renderingFinishedSemaphore = nullptr;
    };
    static const int NumSwapChainBuffers = vlkConfig::NumFrames;    // FIXME: NumSwapChainBuffers may have to be dynamic (minRequired + NumFrames)
    uint32 curSwapChainBufferIndex = 0;
    SwapChainBuffer swapChainBuffers[NumSwapChainBuffers];
    struct DepthBuffer {
        VkFormat format = VK_FORMAT_UNDEFINED;
        VkImage image = nullptr;
        VkDeviceMemory mem = nullptr;
        VkImageView view= nullptr;
    };
    DepthBuffer depthBuffer;
    struct FrameData {
        bool presentFenceSet = false;
        VkFence presentFence = nullptr;
    };
    FrameData frameDatas[vlkConfig::NumFrames];
};

//------------------------------------------------------------------------------
inline VkFramebuffer 
vlkContext::curFramebuffer() const {
    return this->swapChainBuffers[this->curSwapChainBufferIndex].framebuffer;
}

//------------------------------------------------------------------------------
inline VkImage 
vlkContext::curSwapChainImage() const {
    return this->swapChainBuffers[this->curSwapChainBufferIndex].image;
}

} // namespace _priv
} // namespace Oryol

