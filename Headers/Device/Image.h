/**
 * Image.h
 *
 * Todos os direitos reservados.
 *
 */

#ifndef IMAGE_H_
#define IMAGE_H_

#include "Result.h"

/**
 * A classe Image é responsável por criar uma fina camada de abstração sobre um dos dois tipos de recursos
 * disponíveis na API Vulkan: o VkImage. O VkImage representa uma estrutura que pode ser armazenada na GPU
 * de maneiras extremamente otimizadas e em diversos formatos. Estas funcionalidades incluem uma distribuição
 * otimizada dos dados na memória de vídeo, compreensão destes dados, etc.
 *
 * As Images podem ser utilizadas de várias maneiras, sendo a principal utilidade delas a criação de texturas
 * e sprites. As Images podem possuir múltiplos mipLevels, podem sofrer operações de anti-aliasing como
 * multisampling, podem ser compartilhadas entre múltiplas filas de processamento gráfico e podem
 * ser utilizadas para vários propósitos.
 *
 * É importante manter em mente que as Images podem ser manipuladas somente através de shared_ptr e weak_ptr,
 * o que permite que estes objetos sejam compartilhados entre vários objetos diferentes e ainda assim
 * mantendo o grau de importância relativo a cada um.
 *
 * A classe Image necessita aplicar a regra dos 5 em C++, efetuando a deletação dos seguintes métodos:
 *      1. O construtor padrão que permite a criação de objetos resetados;
 *      2. O construtor de cópia que permite copiar outros objetos do mesmo tipo;
 *      3. O construtor de movimento que permite incorporar outros objetos através da std::move;
 *      4. O operador de atribuição que permite copiar outros objetos do mesmo tipo;
 *      5. O operador de atribuição que permite incorporar outros objetos através da std::move.
 *
 */
class Image {
protected:
    /* O atributo que guarda a handle do objeto do tipo VkImage que representa o recurso dentro
     * da API Vulkan. */
    struct VkImage_T *image;

    /* O atributo que armazena o modo de compartilhamento de um Image, ou seja, se ele é exclusivo
     * ou concorrente. */
    uint32 sharingMode;

    /* O atributo que contém o uso para o qual este Image é destinado. */
    uint32 usage;

    /* O atributo que armazena se trata-se de um Image 1D, 2D ou 3D. */
    uint32 type;

    /* O atributo que armazena o formato referente aos texels do Image. */
    uint32 format;

    /* O atributo que armazena a intensidade do antisserilhamento que deve ser aplicado à este Image. */
    uint32 samples;

    /* O atributo que guarda se deve ser feito uma disposição otimizada ou linear do Image na
     * memória de vídeo. */
    uint32 tiling;

    /* O atributo que determina o layout no qual o Image se encontra atualmente, este será alterado
     * durante as operações. */
    uint32 layout;

    /* O atributo que determina quantos mipmaps terá este Image. */
    uint32 mipLevels;

    /* O atributo que armazena quantos valores estarão presentes na lista de Image, só pode ser
     * utilizado com Images de tipo 1D ou 2D, caso contrário deve ser 1. */
    uint32 arrayLayers;

    /* O atributo que guarda o tamanho do Image, determinado por largura, altura e profundidade e,
     * deve-se especificar os tamanhos em texels. */
    std::unique_ptr<struct VkExtent3D> extent;

    /* O atributo que guarda o unique_ptr da memória de vídeo associada à este Image. */
    std::unique_ptr<class Memory> memory;

    /* O atributo que lista as múltiplas filas de processamento que poderão utilizar este Image,
     * se ele for compartilhado. */
    std::vector<std::weak_ptr<class Queue>> queueList;

protected:
    /**
     * O construtor padrão e privado de objetos do tipo Image, seu único objetivo é criar um objeto com seus
     * atributos completamente resetados.
     *
     * Observação Importante: Para construir este objeto corretamente, deve-se utilizar para construção os
     * seguintes métodos: createImage ou createSharedImage.
     *
     */
    explicit Image();

    /**
     * Este método auxiliar tem como função efetivar a alocação de memória de vídeo para sustentar a image, através
     * de um Allocator apropriado, e de efetuar a ligação entre a image e tal região de memória.
     *
     * Após a execução deste método, que deve acontecer nos métodos que criam a image, este objeto terá memória
     * reservada no dispositivo físico, onde poderá realizar múltiplas operações.
     *
     */
    void allocateMemory();

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

    /**
     * O método auxiliar que tem como função criar e preencher a estrutura do tipo VkImageCreateInfo. Esta
     * estrutura é necessária para requisitar a criação de um objeto VkImage ao dispositivo lógico,
     * através da API Vulkan.
     *
     */
    struct VkImageCreateInfo getImageCreateInfo() const noexcept;

public:
    /**
     * O destrutor padrão de Image, cujo objetivo é retornar a memória associada para o alocador que a
     * providenciou, além de destruir o recurso de tipo VkImage junto à API Vulkan, nulificando, portanto,
     * o atributo que armazena o handle: image.
     *
     */
    virtual ~Image();

    /**
     * O método createImage é o que permite a criação de objetos do tipo Image que sejam exclusivos, ou seja,
     * não precisam ser compartilhados entre múltiplas filas de processamento gráfico e, portanto, não necessitará
     * receber uma estrutura que contenha uma lista de objetos do tipo Queue.
     *
     * Este método retorna um Image criado a partir dos parâmetros especificados, realiza a alocação de memória
     * necessária e efetua a ligação entre o Image e a região de memória alocada. Por fim, retorna o Image através
     * de um shared_ptr, permitindo o compartilhamento de objetos do tipo Image entre múltiplos objetos enquanto
     * evita leaks de memória do dispositivo físico.
     *
     */
    static Result<std::shared_ptr<Image>> createImage(struct VkExtent3D ext,
                                                      uint32 tp,
                                                      uint32 layers,
                                                      uint32 mips,
                                                      uint32 usg,
                                                      uint32 fmt,
                                                      uint32 tlng);

    /**
     * O método createShaderImage é o que permite a criação de objetos do tipo Image que sejam concorrentes,
     * ou seja, que precisam ser compartilhados entre múltiplas filas de processamento gráfico e, portanto,
     * necessitará receber uma estrutura que contenha uma lista de objetos do tipo Queue.
     *
     * Este método retorna um Image criado a partir dos parâmetros especificados, realiza a alocação de memória
     * necessária e efetua a ligação entre o Image e a região de memória alocada. Por fim, retorna o Image através
     * de um shared_ptr, permitindo o compartilhamento de objetos do tipo Image entre múltiplos objetos enquanto
     * evita leaks de memória do dispositivo físico.
     *
     */
    static Result<std::shared_ptr<Image>> createSharedImage(struct VkExtent3D ext,
                                                            uint32 tp,
                                                            uint32 layers,
                                                            uint32 mips,
                                                            uint32 usg,
                                                            uint32 fmt,
                                                            uint32 tlng,
                                                            std::vector<std::weak_ptr<class Queue>> &queues);

    /**
     * Este método tem como objetivo permitir a obtenção da handle ao objeto do tipo VkImage para que outros
     * objetos possam realizar operações relacionadas à API Vulkan que necessitem utilizar a handle.
     *
     * O método irá retornar a handle em um objeto do tipo Result caso ela exista, senão, irá retornar um código
     * de erro no objeto.
     *
     */
    Result<struct VkImage_T *> getVulkanImage() const noexcept;

public:
    Image(const Image &) = delete;
    Image(Image &&) = delete;

    Image &operator=(const Image &) = delete;
    Image &operator=(Image &&) = delete;

};

#endif /* IMAGE_H_ */
