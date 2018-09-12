/**
 * Error.h
 *
 * Todos os direitos reservados.
 *
 */

#ifndef ERROR_H_
#define ERROR_H_

#include "Types.h"

/**
 * O Error é uma enumeração que contém os diversos tipos de erros que podem ser retornados de métodos e funções através
 * de objetos do tipo Result. Os erros contidos aqui podem ocorrer em vários dos objetos espalhados através da
 * Real Engine e, a respectiva forma de lidar com eles ou reportá-los ao usuário/desenvolvedor está contida em outro
 * objeto que é responsável por medidas de depuração, este sendo o DebugManager.
 *
 */
enum class Error : uint16 {
    None = 0,
    FailedToCreateInstance,
    FailedToCreateDevice,
    FailedToAcquireVulkanImageBuffers,
    FailedToAllocateCommandBuffer,
    FailedToAllocateDescriptorSets,
    FailedToAllocateDeviceMemory,
    FailedToBindBufferMemory,
    FailedToBindImageMemory,
    FailedToCreateBuffer,
    FailedToCreateImage,
    FailedToCreateCommandPool,
    FailedToCreateDescriptorPool,
    FailedToCreateDescriptorSetLayout,
    FailedToCreateImageView,
    FailedToCreatePipelineLayout,
    FailedToCreateSurface,
    FailedToCreateSwapchain,
    FailedToCreateVertexShader,
    FailedToCreateFragmentShader,
    FailedToFlushRenderer,
    FailedToInitializeGLFW,
    FailedToMapMemory,
    FailedToRetrieveBuffer,
    FailedToRetrieveImage,
    FailedToRetrieveQueue,
    FailedToSubmitQueue,
    FailedToOpenShaderFile,
    FailedToLoadImage,
    FailedToLockPointer,
    IndexOutOfRange,
    InstanceNotStartedUp,
    DeviceNotStartedUp,
    GraphicsManagerNotStartedUp,
    MemoryManagerNotStartedUp,
    NoPhysicalDeviceAvailable,
    FailedRetrievingPhysicalDevices,
    NoMemoryAvailableInAllocator,
    SubmitParametersNotMatching,
    UnknownImageFormat
};

#endif /* ERROR_H_ */
