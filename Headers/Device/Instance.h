/**
 * Instance.h
 *
 * Todos os direitos reservados.
 *
 */

#ifndef INSTANCE_H_
#define INSTANCE_H_

#include "Result.h"

/**
 * A classe Instance é responsável por definir uma fina camada de abstração sobre a sua equivalente definida pela
 * especificação Vulkan, VkInstance, permitindo uma criação simplificada do objeto que preenche por padrão informações
 * relacionadas à Real Engine e seu funcionamento interno.
 *
 * Essa classe deve ser inicializada e encerrada através dos metódos startup e shutdown, respectivamente, ou não irá
 * efetuar a criação do objeto subliminar através das chamadas da API Vulkan, se houver tentativa de utilização do
 * objeto neste estado, então um erro será propagado através de um objeto do tipo Result e que, portanto, deverá ser
 * verificado pela presença de erros.
 *
 * A classe Instance necessita aplicar a regra dos 5 em C++, efetuando a deletação dos seguintes métodos:
 *      1. O construtor padrão que permite a criação de objetos resetados;
 *      2. O construtor de cópia que permite copiar outros objetos do mesmo tipo;
 *      3. O construtor de movimento que permite incorporar outros objetos através da std::move;
 *      4. O operador de atribuição que permite copiar outros objetos do mesmo tipo;
 *      5. O operador de atribuição que permite incorporar outros objetos através da std::move.
 *
 */
class Instance final {
private:
    /* O atributo que determina se haverá a ativação das camadas Vulkan que são responsáveis por funções de logging,
     * perfilamento e depuração. */
    bool bIsDebug;

    /* O atributo que define o nome da aplicação, este atributo é importante e deve corretamente identificar a família
     * de aplicações ao qual pertence, visto que será usado pela API Vulkan para identificar melhorias presentes
     * no driver. */
    const utf8 *applicationName;

    /* A versão da aplicação que será passada a API Vulkan, este campo também será utilizado para detectar melhorias
     * disponíveis nos drivers gráficos e recomenda-se que se gere esse atributo através da macro: VK_MAKE_VERSION. */
    uint32 applicationVersion;

    /* O atributo que armazena a instância Vulkan criada através da chamada para a API. Este atributo só será setado
     * após a execução do método startup e será limpo após a chamada do método shutdown. */
    struct VkInstance_T *instance;

    struct VkDebugUtilsMessengerEXT_T *callback;

private:
    /**
     * O método cujo objetivo é criar e completar a estrutura VkApplicationInfo que contém informações sobre a engine
     * utilizada, a aplicação e a versão da API Vulkan que está sendo utilizada.
     *
     * Especificamente, as informações que serão preenchidas em sua totalidade são:
     *      1. Nome da aplicação;
     *      2. Versão da aplicação;
     *      3. Nome da engine;
     *      4. Versão da engine;
     *      5. Versão da API Vulkan.
     *
     * As versões devem ser geradas utilizando a macro da API Vulkan: VK_MAKE_VERSION, enquanto os nomes devem ser
     * passados como arrays fixos de caracteres.
     *
     */
    struct VkApplicationInfo getApplicationInfo() const noexcept;

    /**
     * O metódo responsável por preencher a estrutura VkInstanceCreateInfo que determina as propriedades da instância
     * Vulkan que será criada. Este método precisa receber a estrutura VkApplicationInfo preenchida através de um
     * parâmetro referencial para que seu endereço possa ser alimentado na estrutura.
     *
     * A estrutura VkInstanceCreateInfo especifica para a API Vulkan as extensões e camadas a serem habilitadas para a
     * instância, além das informações sobre a aplicação contidas em sua estrutura complementar VkApplicationInfo.
     *
     * As camadas a serem habilitadas por esse método irão divergir com base no atributo isDebug, onde se este estiver
     * avaliado como verdadeiro, serão ligadas as camadas de logging, perfilamento e depuração da API Vulkan.
     *
     * As extensões dependerão única e exclusivamente das capacidades mínimas da GPU e das necessidades mínimas da Real
     * Engine, caso um meio termo não seja encontrado a aplicação não será executado e retornará um erro.
     *
     */
    struct VkInstanceCreateInfo getInstanceCreateInfo(struct VkApplicationInfo *applicationInfo) const noexcept;

    void setupDebug();

public:
    /**
     * O construtor principal de objetos do tipo Instance. Seu objetivo é armazenar as informações requisitadas nos
     * parâmetros que são:
     *      1. O nome da aplicação;
     *      2. A versão atual da aplicação;
     *      3. Se a aplicação está em modo de depuração ou não.
     *
     *  Observação Importante: Esse construtor não cria o objeto Vulkan do tipo VkInstance e, portanto, o objeto do
     *  tipo Instance não deve ser utilizado diretamente sem antes ser invocado o metódo startup. Além disso, ao
     *  terminar de utilizar este objeto, deve-se chamar o método shutdown para efetuar a limpeza do objeto na memória
     *  e na API Vulkan.
     *
     *  Também é importante manter em mente ao utilizar este objeto que o método shutdown deve ser invocado após o
     *  desligamento de outros objetos de nível de abstração mais elevado, como o Device. Mais especificamente, deve-se
     *  invocar o método shutdown do objeto Instance como o último dentre os objetos que abstraem a API Vulkan.
     *
     */
    explicit Instance(const utf8 *appName, uint32 appVersion, bool bDebug = false);

    ~Instance();

    /**
     * O método cujo objetivo é retornar a handle do objeto VkInstance para que ele possa ser utilizado em outros
     * objetos e em chamadas para a API Vulkan. Irá retornar um erro caso o método startup não tiver sido invocado
     * anteriormente ou se o mesmo tiver retornado um erro que não foi tratado.
     *
     */
    Result<struct VkInstance_T*> getVulkanInstance() const noexcept;

    /**
     * O metódo responsável por criar a instância Vulkan propriamente dita. Construindo-se em cima dos métodos
     * auxiliares getApplicationInfo e getInstanceCreateInfo, este método fornece todas as informações que a API Vulkan
     * necessita para inicializar um objeto do tipo VkInstance e por fim, o método armazena a handle retornada no
     * atributo instance do objeto de tipo Instance.
     *
     */
    Result<void> startup();

    /**
     * O metódo com o propósito de destruir o objeto de tipo VkInstance na API Vulkan e limpar a memória. Esse método
     * deve ser chamado e na ordem correta ou irá acarretar em problemas devido à destruição aleatória de objetos
     * por parte do C++.
     *
     */
    void shutdown();

public:
    Instance(const Instance&) = delete;
    Instance(Instance&&) = delete;

    Instance& operator=(const Instance&) = delete;
    Instance& operator=(Instance&&) = delete;
};

#endif /* INSTANCE_H_ */
