/**
 * Result.h
 *
 * Todos os direitos reservados.
 *
 */

#ifndef RESULT_H_
#define RESULT_H_

#include "Error.h"

#include <stdexcept>

/**
 * A classe Result existe para permitir o retorno de valores de métodos e funções que nem sempre estarão presentes
 * devido a erros na execução destas. Logo, objetos do tipo Result devem ser verificados pela presença de erros, caso
 * estes não estejam presentes então deve-se proceder com a leitura do valor retornado da função, do contrário deve-se
 * realizar os passos corretos para prosseguir com a execução sem aquele valor.
 *
 * É importante notar que caso seja efetuada uma tentativa de acessar um valor em um objeto do tipo Result que esteja
 * armazenando um erro proveniente da função ou método invocado, a aplicação levantará uma exception.
 *
 */
template<typename T>
class Result final {
private:
    /* O atributo que indica se há um erro ou não armazenado no objeto do tipo Result. */
    bool bHasError = false;

    /* O atributo que indica qual o erro que está armazenado, se tiver algum. */
    Error error = Error::None;

    /* O atributo que armazena o valor retornado de algum método se este retornar corretamente, deve ser ignorado se
     * houver erro no objeto. */
    T value;

private:
    /**
     * Um construtor privado que permite criar um objeto do tipo Result a partir de um código de erro, esse construtor
     * é utilizado pelo método error que deve ser utilizado pelos métodos que retornarem objetos do tipo Result para
     * gerarem erros com o código apropriado.
     *
     */
    explicit Result(Error err) : bHasError(true), error(err), value() {}

public:
    /**
     * Um construtor que permite criar um objeto do tipo Result a partir do valor correto a ser retornado, neste caso
     * não haverá presença de erro e o objeto armazenado será retornado corretamente.
     *
     */
    explicit Result(T val) : bHasError(false), error(Error::None), value(std::move(val)) {}

    /**
     * Um método estático que permite criar um objeto do tipo Result com um código de erro especificado. Deve ser
     * utilizado pelos métodos para suas condições de erro.
     *
     */
    inline static Result createError(Error err) noexcept { return Result(err); }

    /**
     * O método cujo objetivo é retornar o código de erro que está armazenado no objeto.
     *
     */
    inline Error getError() const noexcept {
        if (hasError())
            return error;

        return Error::None;
    }

    /**
     * O método que é utilizado para verificar se há um erro no objeto do tipo Result. O seu retorno deve ser verificado
     * antes de se tentar extrair o objeto retornado do método e será verdadeiro se houver erro.
     *
     */
    inline bool hasError() const noexcept { return bHasError; }

    inline T unwrap() {
        if (hasError())
            throw std::invalid_argument("Requested value of Result with errors...");

        return std::move(value);
    }

public:
    /**
     * O operador que permite a criação de objetos do tipo Result a partir do operador de atribuição sendo igualado a
     * algum valor desejado de retorno e, portanto, sem erro.
     *
     */
    inline Result<T>& operator=(const T& val) noexcept {
        bHasError = false;
        value = val;

        return *this;
    }

    /**
     * O operador que permite a conversão automática entre o tipo de retorno e o objeto Result. Se não houver erro
     * retornará corretamente o objeto armazenado, caso contrário, irá jogar uma exception reclamando sobre a tentativa
     * de extrair valores de uma Result com erros.
     *
     */
    inline explicit operator T() {
        if (hasError())
            throw std::invalid_argument("Requested value of Result with errors...");

        return std::move(value);
    }
};

template <>
class Result<void> final {
private:
    /* O atributo que indica se há um erro ou não armazenado no objeto do tipo Result. */
    bool bHasError = false;

    /* O atributo que indica qual o erro que está armazenado, se tiver algum. */
    Error error = Error::None;

public:
    /**
     * Um construtor privado que permite criar um objeto do tipo Result a partir de um código de erro, esse construtor
     * é utilizado pelo método error que deve ser utilizado pelos métodos que retornarem objetos do tipo Result para
     * gerarem erros com o código apropriado.
     *
     */
    explicit Result(Error err) : bHasError(err != Error::None), error(err) {}

    /**
     * Um método estático que permite criar um objeto do tipo Result com um código de erro especificado. Deve ser
     * utilizado pelos métodos para suas condições de erro.
     *
     */
    inline static Result createError(Error err) noexcept { return Result(err); }

    /**
     * O método cujo objetivo é retornar o código de erro que está armazenado no objeto.
     *
     */
    inline Error getError() const noexcept {
        if (hasError())
            return error;

        return Error::None;
    }

    /**
     * O método que é utilizado para verificar se há um erro no objeto do tipo Result. O seu retorno deve ser verificado
     * antes de se tentar extrair o objeto retornado do método e será verdadeiro se houver erro.
     *
     */
    inline bool hasError() const noexcept { return bHasError; }
};

#endif /* RESULT_H_ */
