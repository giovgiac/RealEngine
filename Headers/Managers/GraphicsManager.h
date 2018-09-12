/**
 * GraphicsManager.h
 *
 * Todos os direitos reservados.
 *
 */

#ifndef GRAPHICSMANAGER_H_
#define GRAPHICSMANAGER_H_

#include "Result.h"

/**
 * O GraphicsManager é a classe que gerencia o subsistema gráfico da Real Engine. Sua função é manter ponteiros
 * exclusivos para os objetos fundamentais da API Vulkan que são necessários para a execução da aplicação e
 * o funcionamento da engine.
 *
 * Quando outros subsistemas ou objetos necessitarem acessar os objetos da API Vulkan, poderão requisitá-los facilmente
 * ao obter uma referência a única instância existente do GraphicsManager.
 *
 * A classe GraphicsManager necessita aplicar a regra dos 5 em C++, efetuando a deletação dos seguintes métodos:
 *      1. O construtor padrão que permite a criação de objetos resetados;
 *      2. O construtor de cópia que permite copiar outros objetos do mesmo tipo;
 *      3. O construtor de movimento que permite incorporar outros objetos através da std::move;
 *      4. O operador de atribuição que permite copiar outros objetos do mesmo tipo;
 *      5. O operador de atribuição que permite incorporar outros objetos através da std::move.
 *
 */
class GraphicsManager final {
private:
    /* O atributo responsável por guardar a instância única do objeto de tipo Device que é inicializado pelo próprio
     * objeto no método startup. */
    std::shared_ptr<class Device> device;

    /* O atributo responsável por guardar a instância compartilhada do objeto de tipo Instance que é inicializado pelo
     * próprio objeto no método startup. */
    std::shared_ptr<class Instance> instance;

private:
    /**
     * O construtor padrão de GraphicsManager e que não pode ser utilizado. O seu único objetivo é resetar os valores
     * dos atributos de tal maneira que eles possam ser setados apropriadamente no método startup.
     *
     */
    explicit GraphicsManager();

    ~GraphicsManager();

public:
    /**
     * O método getGraphicsDevice serve para retornar uma referência, que não pode ser modificada, ao objeto de tipo
     * Device, permitindo que outros objetos possam, por exemplo, adquirir sua handle e utilizá-lo em outras chamadas
     * da API Vulkan que necessitem da especificação do parâmetro VkDevice ou VkPhysicalDevice.
     *
     */
    Result<std::weak_ptr<const class Device>> getGraphicsDevice() const noexcept;

    /**
     * O método getGraphicsInstance serve para retornar uma referência, que não pode ser modificada, ao objeto de tipo
     * Instance, permitindo que outros objetos possam, por exemplo, adquirir sua handle e utilizá-lo em outras chamadas
     * da API Vulkan que necessitem da especificação do parâmetro VkInstance.
     *
     */
    Result<std::weak_ptr<const class Instance>> getGraphicsInstance() const noexcept;

    /**
     * O método getManager tem como objetivo retornar uma referência para a instância única do GraphicsManager, com a
     * finalidade de que os seus métodos possam ser invocados de qualquer posição do código, até mesmo dentro de
     * outros Managers.
     *
     * Se a ocasião for a primeira vez em que o método getManager está sendo invocado, então o objeto de tipo
     * GraphicsManager será criado uma única vez e, antes de ser utilizado, deve ser corretamente inicializado
     * através do método startup.
     *
     */
    inline static GraphicsManager &getManager() noexcept {
        static GraphicsManager inst;
        return inst;
    }

    /**
     * O método startup é fundamental e deve ser utilizado para inicializar o objeto do tipo GraphicsManager antes de
     * qualquer tentativa de utilizar funções da API Vulkan.
     *
     * Ao invocar o método, ele criará novas instâncias únicas de Instance e Device e, logo depois, irá inicializá-los,
     * chamando seus próprios métodos startup.
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
    GraphicsManager(const GraphicsManager &) = delete;
    GraphicsManager(GraphicsManager &&) = delete;

    GraphicsManager &operator=(const GraphicsManager &) = delete;
    GraphicsManager &operator=(GraphicsManager &&) = delete;
};

#endif /* GRAPHICSMANAGER_H_ */
