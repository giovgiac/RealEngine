/**
 * Buffer.h
 *
 * Todos os direitos reservados.
 *
 */

#ifndef BUFFER_H_
#define BUFFER_H_

#include "Result.h"

/**
 * A classe Buffer é responsável por criar uma fina camada de abstração sobre um dos dois tipos de recursos
 * disponíveis na API Vulkan: o VkBuffer. O VkBuffer representa uma área contígua de memória de vídeo, onde
 * podem ser armazenadas informações provenientes da memória utilizada diretamente pela CPU através do
 * barramento PCI Express.
 *
 * Os Buffers podem ser utilizados de múltiplas maneiras, como vertex buffers, index buffers, constant buffers, etc.
 * Além disso, podem ser compartilhados entre múltiplas filas de processamento da GPU. Por fim, cada Buffer
 * possui uma fatia de memória de vídeo, alocada durante sua criação através de um Allocator adequado.
 *
 * É importante manter em mente que os Buffers, podem ser manipulados somente através de shared_ptr e weak_ptr,
 * o que permite que estes objetos sejam compartilhados entre vários objetos diferentes e ainda assim mantendo
 * o grau de importância relativo a cada um.
 *
 * A classe Buffer necessita aplicar a regra dos 5 em C++, efetuando a deletação dos seguintes métodos:
 *      1. O construtor padrão que permite a criação de objetos resetados;
 *      2. O construtor de cópia que permite copiar outros objetos do mesmo tipo;
 *      3. O construtor de movimento que permite incorporar outros objetos através da std::move;
 *      4. O operador de atribuição que permite copiar outros objetos do mesmo tipo;
 *      5. O operador de atribuição que permite incorporar outros objetos através da std::move.
 *
 */
class Buffer {
protected:
    /* O atributo que armazena a handle para o recurso do tipo VkBuffer na API Vulkan. */
    struct VkBuffer_T *buffer;

    /* O atributo que armazena o modo de compartilhamento de um Buffer, ou seja, se ele é exclusivo
     * ou concorrente. */
    uint32 sharingMode;

    /* O atributo que armazena o tamanho do espaço que o Buffer ocupa na memória de vídeo, em bytes. */
    uint64 size;

    /* O atributo que contém o uso para o qual este Buffer é destinado. */
    uint32 usage;

    /* O atributo que guarda o unique_ptr da memória de vídeo associada à este Buffer. */
    std::unique_ptr<class Memory> memory;

    /* O atributo que lista as múltiplas filas de processamento que poderão utilizar este Buffer,
     * se ele for compartilhado. */
    std::vector<std::weak_ptr<class Queue>> queueList;

protected:
    /**
     * O construtor padrão e privado de objetos do tipo Buffer, seu único objetivo é criar um objeto com seus
     * atributos completamente resetados.
     *
     * Observação Importante: Para construir este objeto corretamente, deve-se utilizar para construção os
     * seguintes métodos: createBuffer ou createSharedBuffer.
     *
     */
    explicit Buffer();

    /**
     * Este método auxiliar tem como função efetivar a alocação de memória de vídeo para sustentar o buffer, através
     * de um Allocator apropriado, e de efetuar a ligação entre o buffer e tal região de memória.
     *
     * Após a execução deste método, que deve acontecer nos métodos que criam o buffer, este objeto terá memória
     * reservada no dispositivo físico, onde poderá realizar múltiplas operações.
     *
     */
    Result<void> allocateMemory();

    /**
     * O método auxiliar que tem como função criar e preencher a estrutura do tipo VkBufferCreateInfo. Esta estrutura
     * é necessária para requisitar a criação de um objeto VkBuffer ao dispositivo lógico, através da API Vulkan.
     *
     */
    struct VkBufferCreateInfo getBufferCreateInfo() const noexcept;

    /**
     * O método auxiliar getGraphicsDevice tem como objetivo adquirir o dispositivo lógico da aplicação através
     * da API Vulkan.
     *
     * Para atingir tal objetivo, o método obtém uma referência para o singleton de GraphicsManager e o utiliza
     * para acessar o objeto de tipo Device. Por fim, requisita a handle do VkDevice diretamente ao objeto
     * responsável por administrar o dispositivo lógico na Real Engine.
     *
     * O método retornará o dispositivo lógico caso encontrado, senão irá retornar um código de erro que
     * providencie maiores informações sobre o problema encontrado.
     *
     */
    Result<struct VkDevice_T *> getGraphicsDevice() const noexcept;

public:
    /**
     * O destrutor padrão de Buffer, cujo objetivo é retornar a memória associada para o alocador que a
     * providenciou, além de destruir o recurso de tipo VkBuffer junto à API Vulkan, nulificando, portanto, o
     * atributo que armazena o handle: buffer.
     *
     */
    virtual ~Buffer();

    /**
     * O método createBuffer é o que permite a criação de objetos do tipo Buffer que sejam exclusivos, ou seja,
     * não precisam ser compartilhados entre múltiplas filas de processamento gráfico e, portanto, não necessitará
     * receber uma estrutura que contenha uma lista de objetos do tipo Queue.
     *
     * Este método retorna um Buffer criado a partir dos parâmetros especificados, realiza a alocação de memória
     * necessária e efetua a ligação entre o Buffer e a região de memória alocada. Por fim, retorna o Buffer através
     * de um shared_ptr, permitindo o compartilhamento de objetos do tipo Buffer entre múltiplos objetos enquanto
     * evita leaks de memória do dispositivo físico.
     *
     */
    static Result<std::shared_ptr<Buffer>> createBuffer(uint64 siz, uint32 usg);

    /**
     * O método createShaderBuffer é o que permite a criação de objetos do tipo Buffer que sejam concorrentes,
     * ou seja, que precisam ser compartilhados entre múltiplas filas de processamento gráfico e, portanto,
     * necessitará receber uma estrutura que contenha uma lista de objetos do tipo Queue.
     *
     * Este método retorna um Buffer criado a partir dos parâmetros especificados, realiza a alocação de memória
     * necessária e efetua a ligação entre o Buffer e a região de memória alocada. Por fim, retorna o Buffer
     * através de um shared_ptr, permitindo o compartilhamento de objetos do tipo Buffer entre múltiplos objetos
     * enquanto evita leaks de memória do dispositivo físico.
     *
     */
    static Result<std::shared_ptr<Buffer>> createSharedBuffer(uint64 siz,
                                                              uint32 usg,
                                                              std::vector<std::weak_ptr<class Queue>> &queues);

    /**
     * Este método tem como objetivo permitir a obtenção da handle ao objeto do tipo VkBuffer para que outros
     * objetos possam realizar operações relacionadas à API Vulkan que necessitem utilizar a handle.
     *
     * O método irá retornar a handle em um objeto do tipo Result caso ela exista, senão, irá
     * retornar um código de erro no objeto.
     *
     */
    Result<struct VkBuffer_T *> getVulkanBuffer() const noexcept;

public:
    Buffer(const Buffer &) = delete;
    Buffer(Buffer &&) = delete;

    Buffer &operator=(const Buffer &) = delete;
    Buffer &operator=(Buffer &&) = delete;

};

#endif /* BUFFER_H_ */
