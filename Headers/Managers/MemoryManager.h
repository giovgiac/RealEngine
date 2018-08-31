/**
 * MemoryManager.h
 *
 * Todos os direitos reservados.
 *
 */

#ifndef MEMORYMANAGER_H_
#define MEMORYMANAGER_H_

#include "Result.h"

/**
 * O MemoryManager é a classe que gerencia o subsistema de memória. Suas principais funcionalidades são as seguintes:
 *      1. Gerenciar os diversos alocadores e seus tipos, sendo capaz de providenciar os alocadores adequados quando
 *  forem requisitados por outros objetos;
 *      2. Manter as propriedades físicas da memória para que outros objetos, principalmente alocadores, possam
 *  consultá-los e decidir qual das heaps da GPU é mais adequada para se localizarem;
 *      3. Vigiar alocadores que estejam sem realizar alocações há um determinado tempo e eliminá-los, liberando a
 *  memória de vídeo que haviam alocado para distribuição.
 *
 * A classe MemoryManager necessita aplicar a regra dos 5 em C++, efetuando a deletação dos seguintes métodos:
 *      1. O construtor padrão que permite a criação de objetos resetados;
 *      2. O construtor de cópia que permite copiar outros objetos do mesmo tipo;
 *      3. O construtor de movimento que permite incorporar outros objetos através da std::move;
 *      4. O operador de atribuição que permite copiar outros objetos do mesmo tipo;
 *      5. O operador de atribuição que permite incorporar outros objetos através da std::move.
 *
 */
class MemoryManager final {
private:
    std::forward_list<std::shared_ptr<class PoolAllocator>> allocatorList;

    /* O atributo que guarda as propriedades da memória do dispositivo físico escolhido para rodar a aplicação. */
    std::unique_ptr<struct VkPhysicalDeviceMemoryProperties> memoryProperties;

private:
    /**
     * O construtor padrão de MemoryManager e que não pode ser utilizado. O seu único objetivo é resetar os valores dos
     * atributos de tal maneira que eles possam ser setados apropriadamente no método startup.
     *
     */
    explicit MemoryManager();

    ~MemoryManager();

    /**
     * O método auxiliar getPhysicalDevice tem como objetivo adquirir o dispositivo físico da aplicação através da
     * API Vulkan.
     *
     * Para atingir tal objetivo, o método obtém uma referência para o singleton de GraphicsManager e o utiliza para
     * acessar o objeto de tipo Device. Por fim, requisita a handle do VkPhysicalDevice diretamente ao objeto
     * responsável por administrar o dispositivo lógico na Real Engine.
     *
     * O método retornará o dispositivo físico caso encontrado, senão irá retornar um código de erro que providencie
     * maiores informações sobre o problema encontrado.
     *
     */
    Result<struct VkPhysicalDevice_T *> getPhysicalDevice() const noexcept;

    /**
     * O método getPhysicalDeviceMemoryProperties é um método auxiliar responsável por utilizar o dispositivo físico
     * obtido através do GraphicsManager para requisitar as propriedades da memória deste dispositivo à API Vulkan.
     *
     */
    Result<void> getPhysicalDeviceMemoryProperties() noexcept;

public:
    /**
     * O método getManager tem como objetivo retornar uma referência para a instância única do MemoryManager, com a
     * finalidade de que os seus métodos possam ser invocados de qualquer posição do código, até mesmo dentro
     * de outros Managers.
     *
     * Se a ocasião for a primeira vez em que o método getManager está sendo invocado, então o objeto de tipo
     * MemoryManager será criado uma única vez e, antes de ser utilizado, deve ser corretamente inicializado através
     * do método startup.
     *
     */
    inline static MemoryManager &getManager() noexcept {
        static MemoryManager inst;
        return inst;
    }

    /**
     * O método getMemoryProperties tem como função retornar a estrutura VkPhysicalDeviceMemoryProperties que descreve
     * os tipos de memória e as heaps que existem no dispositivo físico escolhido pelo GraphicsManager
     * para executar a aplicação.
     *
     */
    Result<struct VkPhysicalDeviceMemoryProperties> getMemoryProperties() const noexcept;

    Result<std::shared_ptr<class PoolAllocator>> requestPoolAllocator(uint64 alignment,
                                                                      uint64 chunkSize) noexcept;

    /**
     * O método startup é fundamental e deve ser utilizado para inicializar o objeto do tipo MemoryManager antes de
     * qualquer tentativa de utilizar alocadores da Real Engine.
     *
     */
    Result<void> startup();

    /**
     * O método shutdown é importante para finalizar corretamente o uso da aplicação e deve ser chamado no fim da
     * aplicação, após terem sido executadas todas as ações pendentes no dispositivo físico (GPU).
     *
     */
    void shutdown();

public:
    MemoryManager(const MemoryManager &) = delete;
    MemoryManager(MemoryManager &&) = delete;

    MemoryManager &operator=(const MemoryManager &) = delete;
    MemoryManager &operator=(MemoryManager &&) = delete;
};

#endif /* MEMORYMANAGER_H_ */
