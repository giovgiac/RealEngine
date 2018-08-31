/**
 * Device.h
 *
 * Todos os direitos reservados.
 *
 */

#ifndef DEVICE_H_
#define DEVICE_H_

#include "Result.h"

/**
 * A classe Device é responsável por abstrair os tipos VkDevice e VkPhysicalDevice da API Vulkan. Devido a isto, esta
 * classe é extensa e têm incubida para si, várias funções importantes para o funcionamento da Real Engine. Entre estas
 * funcionalidades, destacam-se as seguintes:
 *      1. Procurar por dispositivos físicos (GPUs) que atendam as necessidades da Real Engine, em termos de suporte às
 *  extensões pelos seus drivers, suporte as funcionalidades requeridas pelos processos de renderização e pelos limites
 *  inerentes ao hardware em questão;
 *      2. Escolher entre os dispositivos físicos encontrados, aquele que é mais adequado a ser utilizado e dele extrair
 *  várias informações a respeito de suas filas de processamento e quais delas tem capacidade de processamento gráfico;
 *      3. Montar uma lista linear com todas as filas do dispositivo físico que possuem capacidades gráficas e as fornecer
 *  para a criação de um dispositivo lógico;
 *      4. A criação de um dispositivo lógico (VkDevice) que irá gerenciar todas as filas de processamento gráfico do
 *  dispositivo físico selecionado.
 *
 *  A classe Device necessita aplicar a regra dos 5 em C++, efetuando a deletação dos seguintes métodos:
 *      1. O construtor padrão que permite a criação de objetos resetados;
 *      2. O construtor de cópia que permite copiar outros objetos do mesmo tipo;
 *      3. O construtor de movimento que permite incorporar outros objetos através da std::move;
 *      4. O operador de atribuição que permite copiar outros objetos do mesmo tipo;
 *      5. O operador de atribuição que permite incorporar outros objetos através da std::move.
 *
 */
class Device final {
private:
    /* O atributo que informa se as camadas de logging, depuração e perfilamento da API Vulkan serão ativadas no
     * dispositivo lógico. */
    bool bIsDebug = false;

    /* O atributo que armazena a handle do dispositivo lógico da API Vulkan. O dispositivo lógico é utilizado para
     * realizar a maior parte das operações, tanto de renderização como de movimentação de dados. */
    struct VkDevice_T *device;

    /* O atributo que armazena a handle do dispositivo físico da API Vulkan, pode ser utilizado para obter várias
     * informações a respeito do dispositivo, como seus limites e funcionalidades disponíveis. */
    struct VkPhysicalDevice_T *physicalDevice;

    std::vector<std::shared_ptr<class Queue>> queues;

    /* Um vetor com todas as extensões requeridas pela aplicação para serem procuradas no dispositivo físico, deve
     * ser lido de um arquivo de texto. */
    std::vector<const utf8 *> requiredExtensions;

    /* O atributo que armazena a estrutura que contém todas as funcionalidades requeridas pela aplicação e
     * pela engine. */
    std::unique_ptr<struct VkPhysicalDeviceFeatures> requiredFeatures;

    /* O atributo que armazena os limites que a engine requer do dispositivo físico a ser escolhido. */
    std::unique_ptr<struct VkPhysicalDeviceLimits> requiredLimits;

private:
    /**
     * O método auxiliar que verifica se um dado dispositivo físico possui todas as extensões requisitadas pela engine
     * e pela aplicação. Se este for o caso retornará verdadeiro, caso contrário, retornará falso.
     *
     */
    bool checkPhysicalDeviceExtensions(struct VkPhysicalDevice_T *pd) const noexcept;

    /**
     * O método auxiliar cujo objetivo é verificar se o dispositivo físico fornecido possui todas as funcionalidades
     * que são requeridas pela engine e aplicação, e.g. multiamostragem, renderização indireta, etc.
     *
     * Se todas as funcionalidades necessárias, especificadas no atributo requiredFeatures, estiverem disponíveis, o
     * metódo retornará verdadeiro, senão, retornará falso.
     *
     */
    bool checkPhysicalDeviceFeatures(struct VkPhysicalDevice_T *pd) const noexcept;

    /**
     * O método auxiliar que verifica se o dispositivo físico especificado por parâmetro possui limites iguais ou
     * maiores que os requeridos pela aplicação e engine e, que estão especificados no atributo requiredLimits
     * do objeto.
     *
     */
    bool checkPhysicalDeviceLimits(struct VkPhysicalDevice_T *pd) const noexcept;

    Result<void> createQueues(
            std::vector<struct VkDeviceQueueCreateInfo> *deviceQueueCreateInfo);

    /**
     * Um método auxiliar que tem como função criar um dispositivo lógico junto a API Vulkan.
     *
     * Este método auxiliar faz uso de outros dois métodos auxiliares: getDeviceCreateInfo e getDeviceQueueCreateInfo,
     * para obter as informações necessárias que precisam ser informadas para a API.
     *
     */
    Result<void> createVulkanDevice();

    /**
     * Um método auxiliar cujo objetivo é preencher a estrutura que detalha as informações necessárias para criar o
     * dispositivo lógico junto a API Vulkan. É responsável por ativar funcionalidades, camadas e extensões junto ao
     * dispositivo e, portanto, será afetado pelos limites do dispositivo físico e pelo atributo que especifica a
     * ativação das camadas de logging, perfilamento e depuração.
     *
     */
    struct VkDeviceCreateInfo getDeviceCreateInfo(
            std::vector<struct VkDeviceQueueCreateInfo> *deviceQueueCreateInfo) const noexcept;

    /**
     * Um método auxiliar que percorre as filas de processamento do dispositivo físico para selecionar aqueles que
     * são capazes de processamento gráfico e as retorna em um vetor para serem utilizadas na criação do dispositivo
     * lógico, reservando-as para o uso da aplicação.
     *
     */
    std::vector<struct VkDeviceQueueCreateInfo> getDeviceQueueCreateInfo() const noexcept;

    /**
     * Um método auxiliar que obtém as propriedades das famílias de filas de processamento gráfico do dispositivo
     * físico e as retorna em um vetor. Estas famílias podem, então, serem verificadas por capacidades gráficas e
     * posteriormente utilizadas junto ao dispositivo lógico.
     *
     */
    std::vector<struct VkQueueFamilyProperties> getPhysicalDeviceQueueFamilyProperties() const noexcept;

    /**
     * Um método auxiliar que encontra todos os dispositivos físicos em uma instância e os coloca em um vetor de
     * tamanho apropriado. Por fim, o método retorna esse vetor para que as propriedades dos dispositivos físicos
     * possam ser analisadas e o mais apto entre eles ser escolhido.
     *
     */
    Result<std::vector<struct VkPhysicalDevice_T *>> getPhysicalDevices() const noexcept;

    /**
     * O método auxiliar responsável por selecionar o dispositivo físico mais apto a ser utilizado pela aplicação
     * e engine.
     *
     * O dispositivo escolhido deverá possuir todos os requerimentos mínimos de funcionalidades, extensões e
     * limites físicos, além disso, possuirá o maior número de filas de processamento gráfico e todas elas serão
     * exploradas pelo dispositivo lógico criado futuramente.
     *
     */
    Result<void> selectVulkanPhysicalDevice();

public:
    /**
     * O construtor padrão para objetos do tipo Device. Ele requer uma referência para um objeto Instance que não será
     * modificado, apenas utilizado para acessar os dispositivos físicos contidos no computador.
     *
     * Além disso, o construtor também recebe um booleano que determina se ativará as camadas de logging, perfilamento
     * e depuração no dispositivo lógico.
     *
     * Observação Importante: O construtor não inicializa os objetos Vulkan e, portanto, antes deste objeto ser
     * utilizado, o seu método startup precisa ser chamado ou os outros métodos retornarão erros.
     *
     */
    explicit Device(std::vector<const utf8 *> extensions,
                    struct VkPhysicalDeviceFeatures features,
                    struct VkPhysicalDeviceLimits limits,
                    bool bDebug = false);

    ~Device();

    /**
     * Esse método tem como objetivo retornar todos os dispositivos físicos que são aptos a rodar a aplicação e serve
     * para fornecer ao usuário uma lista caso ele deseje escolher outro dispositivo além do padrão.
     *
     */
    Result<std::vector<struct VkPhysicalDevice_T *>> getAvailablePhysicalDevices() const noexcept;

    const std::vector<std::shared_ptr<class Queue>>& getDeviceQueues() const noexcept;

    /**
     * Esse método serve para retornar a handle para o dispositivo lógico da API Vulkan e, só irá retornar o valor caso
     * este tenha sido apropriadamente criado, senão irá retornar um erro através do objeto Result.
     *
     * Observação Importante: Este método só deve ser invocado após a inicialização deste objeto de tipo Device pelo
     * método startup.
     *
     */
    Result<struct VkDevice_T *> getVulkanDevice() const noexcept;

    /**
     * Esse método serve para retornar a handle para o dispositivo físicoda API Vulkan e, só irá retornar o valor caso
     * este tenha sido apropriadamente criado, senão irá retornar um erro através do objeto Result.
     *
     * Observação Importante: Este método só deve ser invocado após a inicialização deste objeto de tipo Device pelo
     * método startup.
     *
     */
    Result<struct VkPhysicalDevice_T *> getVulkanPhysicalDevice() const noexcept;

    /**
     * O método startup tem o propósito de invocar os dois métodos auxiliares selectVulkanPhysicalDevice e
     * createVulkanDevice e verificar se os dois rodaram sem erros.
     *
     * Este método é o que de fato inicializa os objetos Vulkan e deve ser chamado após o startup do objeto Instance
     * cuja referência está armazenada nesse objeto do tipo Device ou irá causar erros quando tentar obter os
     * dispositivos físicos relacionados a instância Vulkan.
     *
     */
    Result<void> startup();

    /**
     * O método cuja função é limpar e destruir objetos, incluindo objetos da API Vulkan, que são utilizados para suas
     * funções. Em particular, irá destruir o dispositivo lógico e nullificar sua handle.
     *
     * É importante manter em mente que esse método deve ser invocado antes do shutdown do objeto Instance ou irá
     * gerar erros na API Vulkan, no entanto, este método não invoca o shutdown do objeto Instance que deverá ser
     * invocado manualmente.
     *
     */
    void shutdown();

public:
    Device(const Device &) = delete;
    Device(Device &&) = delete;

    Device &operator=(const Device &) = delete;
    Device &operator=(Device &&) = delete;
};

#endif /* DEVICE_H_ */
