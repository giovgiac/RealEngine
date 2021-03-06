/**
 * Allocator.h
 *
 * Todos os direitos reservados.
 *
 */

#ifndef ALLOCATOR_H_
#define ALLOCATOR_H_

#include "Result.h"

/**
 * A classe abstrata Allocator tem como objetivo fornecer um conjunto de métodos simples que todos os alocadores
 * precisamter, sejam eles StackAllocators, PoolAllocators ou outros. Através desta interface, outros objetos poderão
 * armazenar ou receber alocadores sem necessitarem saber qual o tipo específico que lhes foi encaminhado.
 *
 */
class Allocator {
protected:
    /**
     * Um construtor padrão da interface que serve apenas para permitir a criação correta de seus descendentes. Um
     * objeto do tipo Allocator não poderá ser criado devido a virtualidade pura de seus métodos.
     *
     */
    explicit Allocator() = default;

public:
    /**
     * O método que os alocadores necessitam para que outros objetos possam efetuar requisições de alocações de espaços
     * de memória de vídeo.
     *
     * A memória é retornada na forma de um unique_ptr de forma a manter garantida a exclusividade do objeto Memory e
     * de evitar leaks de memória de vídeo.
     *
     */
    virtual Result<std::unique_ptr<class Memory>> allocate(uint64 siz) = 0;

    /**
     * O método que os alocadores necessitam para que outros objetos possam liberar alocações de memória de vídeo que
     * haviam feito anteriormente.
     *
     * A memória é tomada na forma de uma referência de unique_ptr e deverá nulificar esse ponteiro além de retornar o
     * objeto Memory para a propriedade do alocador.
     *
     */
    virtual void free(std::unique_ptr<class Memory> &mem) = 0;

public:
    Allocator(const Allocator &) = delete;
    Allocator(Allocator &&) = delete;

    Allocator &operator=(const Allocator &) = delete;
    Allocator &operator=(Allocator &&) = delete;
};

#endif /* ALLOCATOR_H_ */
