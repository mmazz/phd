#

## Setup

- $ clang-format -style=Microsoft --dump-config > .clang-format



## Hardcoded OpenFHE

- src/pke/lib/encoding/ckkspackedencoding.cpp: 535. Comente el if real para que nunca entre. Puse un logstd = 1, if (logstd < 0)
- src/core/include/math/distributiongenerator.h descomente el define FIXED_SEED
- Para lo anterior funcione instalar OpenFHE con $cmake -DCMAKE_BUILD_TYPE=Debug -DWITH_OPENMP=OFF .., en la carpeta build.
- Para asegurarse que funcione borrar el CMakeCache.txt de la carpeta donde tenemos los files nuestros.
- src/core/include/math/distributiongenerator.h: 120,  Comente el cerr que avisa que estoy usando fixed_seed
- src/pke/lib/schemerns/rns-pke.cpp: Aca esta implementado el Encryption
- src/core/include/lattice/hal/default/dcrtpoly-impl.h:
- src/core/include/math/discretegaussiangenerator-impl.h: En diferentes lados agregue lo siguiente para fijar el random:
    - \#if defined(FIXED_SEED)
        -   PseudoRandomNumberGenerator::GetPRNG().setCounter(0);
    - \#endif
    - Tambien a los siguientes archivos
        - src/core/include/math/binaryuniformgenerator-impl.h
        - src/core/include/math/discreteuniformgenerator-impl.h
        - src/core/include/math/ternaryuniformgenerator-impl.h
        - src/core/include/math/nbtheory-impl.h: Aca si lo pongo hace que quede loopeando de forma infinita...
- Para que lo anterior funcione agregue a la parte publica src/core/include/utils/prng/blake2engine.h
    - void setCounter(uint64_t val)
    - {
        -   m_counter = val;
        - m_bufferIndex = val;
        - for(size_t i=0; i<16; i++)
            -  m_seed[i] = val;
        - for(size_t i=0; i<PRNG_BUFFER_SIZE; i++)
            - m_buffer[i] = val;
    - }

- src/core/lib/math/hal/intnat/mubintvecnat.cpp: para entender como hacen el mod

## Entendiendo OpenFHE

Dando un plain text, puedo obtener sus coeficientes con GetAllElements().

Esto me da un vector de polinomios.
Cada uno del RNS.

Para cambiar su contenido tengo que directamente indexar con GetAllElements()[i][j]
ya que la variable que inicializo del estilo:

auto elems = plaintext->GetElement<DCRTPoly>().GetAllElements();

Queda medio como una copia, entonces si la modifico en realidad no lo estoy haciendo
al verdadero.

La suma que hace a la hora de encriptar es AddModFast, que asume que ambos argumentos
son menores que el modulo y si la suma da mayor al modulo simplemente le resta el modulo.
Por eso hay veces que si cambio bits grandes me queda mucho mas grande que el modulo.

La función que usan de conjugar es rara…. hay veces que no hace nada…

# Encriptacion

Es muy similar pero esta vez hay que contemplar que estan los polinomios c0 y c1.
