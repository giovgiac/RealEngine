/**
 * Memory.h
 *
 * Todos os direitos reservados.
 *
 */

#ifndef MEMORY_H_
#define MEMORY_H_

#include "Result.h"

/**
 * A classe Memory cria uma abstração simples sobre o objeto VkDeviceMemory da API Vulkan. A ideia básica desta classe
 * é armazenar a handle para uma área de memória em particular e o offset da memória em questão, visto que várias
 * regiões de memória irão compartilhar o mesmo handle para minimizar o número de alocações.
 *
 * Uma constatação importante é que objetos do tipo Memory podem ser utilizados somente através de unique_ptr, o que
 * garante que cada objeto seja exclusivo e tenha apenas um dono em um determinado momento.
 *
 * A classe Memory necessita aplicar a regra dos 5 em C++, efetuando a deletação dos seguintes métodos:
 *      1. O construtor padrão que permite a criação de objetos resetados;
 *      2. O construtor de cópia que permite copiar outros objetos do mesmo tipo;
 *      3. O construtor de movimento que permite incorporar outros objetos através da std::move;
 *      4. O operador de atribuição que permite copiar outros objetos do mesmo tipo;
 *      5. O operador de atribuição que permite incorporar outros objetos através da std::move.
 *
 */
class Memory {
protected:
    /* A heap do dispositivo físico em que esta região de memória está localizada. */
    uint32 heap;

    /* O atributo que armazena a handle para a memória de vídeo diretamente. Este é o mesmo endereço do alocador que
     * obteve este pedaço específico de memória. */
    struct VkDeviceMemory_T *memory;

    /* O offset que deve-se adicionar ao handle da memória de vídeo para chegar-se na região de memória representada
     * por este objeto. */
    uint64 offset;

protected:
    /**
     * O construtor padrão de objetos Memory, sua única função é dar valores nulos aos atributos da classe. O
     * construtor é privado de maneira que o objeto pode ser criado apenas através do método estático createMemory que
     * retorna um unique_ptr, impedindo duplicações deste tipo de objeto.
     *
     */
    explicit Memory();

public:
    virtual ~Memory();

    /**
     * Um método cujo objetivo é determinar a heap ideal da memória do dispositivo físico (GPU) na qual deve-se
     * estabelecer um objeto que tenha dados requerimentos de memórias (VkMemoryRequirements) e necessidades
     * específicas para essa memória, como localização compartilhada entre host e dispositivo, localização somente no
     * dispositivo, etc. (VkMemoryPropertyFlags)
     *
     * Este método é utilizado principalmente pelos alocadores quando precisam decidir em qual das várias heaps da GPU
     * irão realizar a larga alocação de memória que estes objetos necessitam.
     *
     */
    static Result<uint32> chooseHeapFromFlags(const struct VkMemoryRequirements &memoryRequirements,
                                              uint32 requiredFlags) noexcept;

    /**
     * O método createMemory é a forma padrão de criar objetos do tipo Memory, configurando os atributos conforme os
     * parâmetros recebidos pelo método e retornando um unique_ptr que impede a replicação do objeto.
     *
     */
    static std::unique_ptr<Memory> createMemory(struct VkDeviceMemory_T *mem,
                                                uint64 off, uint32 hp) noexcept;

    inline struct VkDeviceMemory_T *getMemory() const noexcept { return this->memory; }

    inline uint64 getMemoryOffset() const noexcept { return this->offset; }

public:
    Memory(const Memory &) = delete;
    Memory(Memory &&) = delete;

    Memory &operator=(const Memory &) = delete;
    Memory &operator=(Memory &&) = delete;
};

#endif /* MEMORY_H_ */
