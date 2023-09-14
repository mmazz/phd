# Profiling

# Install

- googletest: https://github.com/google/googletest
    - git clone https://github.com/google/googletest.git
    - cd googletest
    - mkdir build
    - cd build
    - cmake ..
    - make
    - sudo make install

## Bit flip

### Encryption

- Ahora hay un unico archivo para hacer las corridas, que puede ser o no con RNS y/o  NTT.

### Encoding

- Modifico de a un bit la entrada y veo cuanto cambia su codificacion con la original.
- Como miro el input?
    - Tengo 4096 coefientes de 64 bits, es decir 262144 bits por codificacion.
    - Pero en realidad los coeficientes tienen la cantidad de bits que el modulo y el resto cero.
    - El input es variable, pero a priori lo inicializo con 1000 valores dobles, es decir 64000 bits.
    - El valor de estos puedo ir modificandolo a gusto.
- Tengo hardcodeado la cantidad de coeficiente del polinomio de la codificacion, coeff_count me devuelve
algo 3 veces mas grande. Porque?

## Preguntas

- Se comporta igual todos los valores del vector inputs al ir modificandoles los bits en relacion a que posicion
del vector input se encuentra?

## Ideas

- Example code that can be run in different parameters with the same example case.
- Example code that scale up the depth of multiplication with the parameters.



- Tengo un script que su primer argumento es la cantidad de inputs que va a correr y el
segundo es una seed.
- Crear directorios:
    - log_nonRNS_nonNTT
    - log_nonRNS_nonNTT_elem
