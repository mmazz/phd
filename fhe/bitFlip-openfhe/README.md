#

## Hardcoded OpenFHE

- openfhe-develpment/src/pke/lib/encoding/ckkspackedencoding.cpp: 535. Comente el if real para que nunca entre. Puse un logstd = 1, if (logstd < 0)
- openfhe-development/src/core/include/math/distributiongenerator.h descomente el define FIXED_SEED
- Para lo anterior funcione instalar OpenFHE con $cmake -DCMAKE_BUILD_TYPE=Debug -DWITH_OPENMP=OFF .., en la carpeta build.
- Para asegurarse que funcione borrar el CMakeCache.txt de la carpeta donde tenemos los files nuestros.
- src/core/include/math/distributiongenerator.h: 120,  Comente el cerr que avisa que estoy usando fixed_seed
- src/core/include/lattice/hal/default/dcrtpoly-impl.h: 139 y 202, k=1


## Entendiendo OpenbFHE

Dando un plain text, puedo obtener sus coeficientes conm GetAllElements().

Esto me da un vector de polynomios.
Cada uno del RNS.

Para cambiar su contenido tengo que directamente indexar con GetAllElements()[i][j]
ya que la variable que inicializo del estilo:

auto elems = plaintext->GetElement<DCRTPoly>().GetAllElements();

Queda medio como una copia, entonces si la modifico en realidad no lo estoy haciendo
al verdadero.
