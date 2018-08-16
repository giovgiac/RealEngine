/**
 * PoolAllocator.h
 *
 * Todos os direitos reservados.
 *
 */

#ifndef POOLALLOCATOR_H_
#define POOLALLOCATOR_H_

#include "Allocator.h"

/**
 * O PoolAllocator é a classe do tipo de alocador mais fundamental presente na Real Engine. Devido ao fato da
 * Real Engine funcionar em um sistema de tiles, em que todos os sprites ocuparão o mesmo espaço de memória, o
 * PoolAllocator é a solução mais eficiente para distribuir memória de vídeo com grande eficiência e sem
 * perigo de fragmentação.
 *
 * Os PoolAllocators podem ser manejados somente através de shared_ptr e weak_ptr e devem ser criados e distribuídos
 * pelo subsistema de memória de vídeo, ou seja, o MemoryManager.
 *
 * A classe PoolAllocator necessita aplicar a regra dos 5 em C++, efetuando a deletação dos seguintes métodos:
 *      1. O construtor padrão que permite a criação de objetos resetados;
 *      2. O construtor de cópia que permite copiar outros objetos do mesmo tipo;
 *      3. O construtor de movimento que permite incorporar outros objetos através da std::move;
 *      4. O operador de atribuição que permite copiar outros objetos do mesmo tipo;
 *      5. O operador de atribuição que permite incorporar outros objetos através da std::move.
 *
 */
class PoolAllocator final : public IAllocator {
private:
    /* O atributo que guarda o alinhamento da memória do PoolAllocator, importante para que as operações sejam
     * realizadas em velocidade máxima. */
    uint64 alignment;

    /* O atributo que determina o tamanho de cada uma das regiões de memória que serão distribuídas. */
    uint64 chunkSize;

    /* O atributo que guarda as propriedades da zona de memória alocada. */
    uint32 flags;

    /* A lista linkada que armazena as regiões de memória que estão disponíveis para serem alocadas como unique_ptr
     * sob a autoridade do alocador. */
    std::forward_list<std::unique_ptr<class Memory>> freeList;

    /* O atributo que guarda em qual heap da memória do dispositivo físico a memória do PoolAllocator está
     * armazenada. */
    uint32 heap;

    /* O atributo que armazena o handle da memória do PoolAllocator, esta é a origem de todas as regiões de memória
     * que serão distribuídas pelo objeto. */
    struct VkDeviceMemory_T *memory;

    /* O atributo que guarda o tamanho do bloco de memória que o PoolAllocator alocou. Ao dividir-se o size pelo
     * chunkSize se tem o número de blocos de memória que podem ser distribuídos pelo alocador. */
    uint64 size;

private:
    /**
     * O construtor padrão e privado de objetos do tipo PoolAllocator. O objetivo deste construtor é criar um objeto
     * resetado com todos os seus atributos setados para valores padrões.
     *
     */
    explicit PoolAllocator();

    /**
     * Um método auxiliar cujo propósito está em particionar a extensa fatia de memória que foi alocada durante a
     * criação do objeto. Este método irá fatiar a memória de tamanho size em (chunkSize / size) pedaços de memória
     * de tamanho chunkSize.
     *
     * Cada pedaço de memória fatiado pelo método será representado por uma estrutura do tipo Memory, manipulada
     * através de um unique_ptr garantindo que apenas uma cópia do objeto possa existir. Estes vários pedaços serão
     * então utilizados para alimentar a lista linkada freeList como memória disponível para o PoolAllocator
     * alocar à objetos que requisitarem.
     *
     */
    void chunkMemory();

    /**
     * O método auxiliar getGraphicsDevice tem como objetivo adquirir o dispositivo lógico da aplicação através
     * da API Vulkan.
     *
     * Para atingir tal objetivo, o método obtém uma referência para o singleton de GraphicsManager e o utiliza para
     * acessar o objeto de tipo Device. Por fim, requisita a handle do VkDevice diretamente ao objeto responsável por
     * administrar o dispositivo lógico na Real Engine.
     *
     * O método retornará o dispositivo lógico caso encontrado, senão irá retornar um código de erro que providencie
     * maiores informações sobre o problema encontrado.
     *
     */
    Result<struct VkDevice_T *> getGraphicsDevice() const noexcept;

    /**
     * O método auxiliar que tem como função criar e preencher a estrutura do tipo VkMemoryAllocateInfo. Esta
     * estrutura é necessária para requisitar uma alocação de memória ao dispositivo físico, através da API Vulkan.
     *
     */
    struct VkMemoryAllocateInfo getMemoryAllocateInfo() const noexcept;

    /**
     * O método auxiliar que tem como função criar e preencher a estrutura do tipo VkMemoryRequirements. Esta
     * estrutura é necessária para escolher a heap correta na qual a memória deste PoolAllocator deve se localizar
     * ao chamar o método estático de chooseHeapFromFlags.
     *
     */
    struct VkMemoryRequirements getMemoryRequirements() const noexcept;

public:
    /**
     * O destrutor de objetos do tipo PoolAllocator. Este é um método fundamental destes objetos, visto que é
     * responsável por dealocar a memória junto a API Vulkan e também limpar a lista de memória disponível.
     *
     * É importante notar que, como objetos do tipo PoolAllocator são manipulados por shared_ptr, este destrutor será
     * chamado se e, somente se, houver a destruição de todos os shared_ptr que referenciam este objeto.
     *
     * Deve-se manter em mente que os objetos do tipo weak_ptr que referenciarem este objeto não impedirão sua
     * destruição, no entanto, a partir do momento em que ele for destruído perderão suas referências.
     *
     */
    ~PoolAllocator();

    /**
     * Este método tem como objetivo simplesmente retornar o valor de alinhamento da memória neste alocador. O
     * alinhamento está guardado no atributo alignment.
     *
     */
    inline uint64 getAllocatorAlignment() const noexcept { return alignment; }

    /**
     * O método que os alocadores necessitam para que outros objetos possam efetuar requisições de alocações
     * de espaços de memória de vídeo.
     *
     * A memória é retornada na forma de um unique_ptr de forma a manter garantida a exclusividade do objeto
     * Memory e de evitar leaks de memória de vídeo.
     *
     * Por se tratar de um PoolAllocator, este método retornará o primeiro elemento da lista linkada freeList,
     * removendo-o da lista e transferindo a propriedade do objeto do tipo Memory para o requisitante. Este pedaço
     * de memória transferido terá tamanho igual a chunkSize, portanto, o parâmetro do método é ignorado.
     *
     */
    Result<std::unique_ptr<class Memory>> allocate(uint64 siz = 0) override;

    /**
     * O método que os alocadores necessitam para que outros objetos possam liberar alocações de memória de vídeo que
     * haviam feito anteriormente.
     *
     * A memória é tomada na forma de uma referência de unique_ptr e deverá nulificar esse ponteiro além de
     * retornar o objeto Memory para a propriedade do alocador.
     *
     * No PoolAllocator, este método meramente tomará a propriedade do objeto que lhe foi encaminhado e o
     * adicionará na frente  da lista linkada freeList.
     *
     */
    void free(std::unique_ptr<class Memory> &mem) override;

    /**
     * Este método estático é a forma correta e padrão de efetuar a criação de um objeto do tipo PoolAllocator.
     *
     * O método criará um PoolAllocator que conforme com as propriedades passadas nos parâmetros, alocará a memória
     * com a API Vulkan e, por fim, retornará um shared_ptr que permite compartilhar o objeto entre vários
     * outros objetos que necessitem utilizá-lo.
     *
     */
    static Result<std::shared_ptr<PoolAllocator>> createAllocator(uint64 initialSize,
                                                                  uint64 partitionSize,
                                                                  uint64 alignment,
                                                                  uint32 flags);

public:
    PoolAllocator(const PoolAllocator &) = delete;
    PoolAllocator(PoolAllocator &&) = delete;

    PoolAllocator &operator=(const PoolAllocator &) = delete;
    PoolAllocator &operator=(PoolAllocator &&) = delete;
};

#endif /* POOLALLOCATOR_H_ */
