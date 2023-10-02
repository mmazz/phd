#

## Hardcoded OpenFHE

- openfhe-development/src/pke/include/encoding/ckkspackedencoding.h
    - Saque lo extra que imprime, asi me lo llevo directo a un archivo.


## Entendiendo OpenbFHE

Dando un plain text, puedo obtener sus coeficientes conm GetAllElements().

Esto me da un vector de polynomios.
Cada uno del RNS.

Para cambiar su contenido tengo que directamente indexar con GetAllElements()[i][j]
ya que la variable que inicializo del estilo:

auto elems = plaintext->GetElement<DCRTPoly>().GetAllElements();

Queda medio como una copia, entonces si la modifico en realidad no lo estoy haciendo
al verdadero.
