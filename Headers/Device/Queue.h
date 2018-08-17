/**
 * Queue.h
 *
 * Todos os direitos reservados.
 *
 */

#ifndef QUEUE_H_
#define QUEUE_H_

#include "Result.h"

/**
 * A classe Queue é uma abstração sobre o objeto VkQueue da API Vulkan. Sua funcionalidade básica é submeter comandos
 * à respectiva fila que representa e, portanto, tais objetos serão usados frequentemente em conjunto com o
 * Escalonador (Renderer) enquanto este decide em qual das filas é apropriado realizar as operações
 * a ele requisitadas.
 *
 * A classe Queue necessita aplicar a regra dos 5 em C++, efetuando a deletação dos seguintes métodos:
 *      1. O construtor padrão que permite a criação de objetos resetados;
 *      2. O construtor de cópia que permite copiar outros objetos do mesmo tipo;
 *      3. O construtor de movimento que permite incorporar outros objetos através da std::move;
 *      4. O operador de atribuição que permite copiar outros objetos do mesmo tipo;
 *      5. O operador de atribuição que permite incorporar outros objetos através da std::move.
 *
 */
class Queue {

};

#endif /* QUEUE_H_ */
