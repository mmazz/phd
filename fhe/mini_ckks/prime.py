import numpy as np
from modArithmetic import *



# La gracia viene que si d divide a A y B, tambien va a dividir a A-B.
# Esto viene por que si A=kd y B=md, A-B=d(k-m) que obviamente es divisible por d
# Entonces se hace de forma recursiva hasta que uno es cero y el otro es el resultado del gcd.
# Para hacer aun mas rapido y garantizar que siempre resto el mas grande menos el mas chico,
# se hace B = A%B, que garantiza todo.
def GreatestCommonDivisor(a, b):
    m_a = a
    if isinstance(a, Integer):
        m_a = a._val
    m_b = b
    if isinstance(b, Integer):
        m_b = b._val
    while (m_b != 0):
        tmp = m_b
        m_b = m_a % m_b
        m_a = tmp
    return Integer(m_a)

#std::vector<IntType>
def GetTotientList(n):
    result = []
    for i in range(1, n.Get()):
        if (GreatestCommonDivisor(i, n) == 1):
            result.append(i)
    return np.array(result)

# calcula b0 =  a**d mod p
# si da +1 calcula b1 = b0**2 mod p y asi hasta que bi = bi-1**d mod p de -1
def WitnessFunction(a,  d,  s,  p):
    b  = a.ModExp(d, p)
    prev_b = False
    for i in range(0, s):
        # si b no es ni 1 ni numero-1 entonces 1
        prev_b = ((b != 1) and (b != p - 1))
        # hago b**2 mod p y ese es mi nuevo valor
        b.ModMulFastEq(b, p)
        if ((b == 1) and prev_b):
            return True
    res = (b != 1)
    return res


def MillerRabinPrimalityTest(p, niter=40):
    ZERO = Integer(0)
    ONE = Integer(1)
    TWO = Integer(2)
    THREE = Integer(3)
    FIVE = Integer(5)
    if (p == TWO or p == THREE or p == FIVE):
        return True
    if (p < TWO or (p&1 == ZERO)):
        return False

    # Hasta aca ya sabemos que nuestro p es impar y no es de los triviales
    # Queremos que p-1 = ((2**s) * d))
    # basicamente esto va dividiendo d por 2 hasta que le da un numero no entero, y se queda con el d anterior
    d = p - 1
    s = 0
    while (d.Mod(TWO) == ZERO):
        # divido por 2...
        d.RShiftEq(ONE)
        s = s + 1

    # aca ya tengo los valores que cumplen lo anterior
    for i in range(0, niter):
        rng = (p-THREE).RNG()
        # La witness me dice si es un numero compesto o no. Si lo es ya esta me garantizo que lo es
        # si me devuelve False, es que puede ser que sea primo, y entocnes tengo que probar niter veces la prueba.
        if (WitnessFunction(rng.ModAdd(TWO, p), d, s, p)):
            return False
    return True

def FirstPrime(nBits, M):
    q = Integer(1 << nBits._val)

    r = q.Mod(M)
    qNew = q + 1 - r
    if (r > 0):
        qNew = qNew + M
    while (not MillerRabinPrimalityTest(qNew)):
        qNew = qNew + M
        if (qNew < q):
            print("FirstPrime: overflow growing candidate")
    return qNew

def LastPrime(nBits, M):
    q = Integer(1 << nBits._val)
    r = q.Mod(M)
    qNew = q + 1 - r
    if (r < 2):
        qNew -= M
    while (not MillerRabinPrimalityTest(qNew)):
        qNew -= M
        if (qNew  > q):
            print("LastPrime: overflow shrinking candidate")
    #if (GetMSB(qNew) != nBits):
    #    print("LastPrime: Requested " + str(nBits) + " bits, but returned " +
    #                  str(GetMSB(qNew)) + ". Please adjust parameters.")
    return qNew


def NextPrime(q, M):
    qNew = q + M
    while (not MillerRabinPrimalityTest(qNew)):
        qNew += M
        if (qNew < q):
            print("NextPrime: overflow growing candidate")
    return qNew

def PreviousPrime(q, M):
    qNew = q - M
    while (not MillerRabinPrimalityTest(qNew)):
        qNew -= M
        if (qNew > q):
            print("PreviousPrime: overflow shrinking candidate")
    return qNew

def PollardRhoFactorization(n):
    ZERO = Integer(0)
    ONE = Integer(1)
    TWO = Integer(2)
    if (n.Mod(TWO) == ZERO):
        return TWO
    divisor = ONE
    c = n.RNG()
    x = n.RNG()
    xx = x
    mu = n.ComputeMu()
    xtemp = x.ModMulBarrett(x, n, mu)
    x       = xtemp.ModAddBarrett(c, n, mu)
    xxtemp = xx.ModMulBarrett(xx, n, mu)
    xx      = xxtemp.ModAddBarrett( c, n, mu)
    xxtemp = xx.ModMulBarrett(xx, n, mu)
    xx      = xxtemp.ModAddBarrett( c, n, mu)
    if (x > xx):
        divisor = GreatestCommonDivisor( x - xx, n)
    else:
        divisor = GreatestCommonDivisor( xx - x, n)

    while (divisor == ONE):
        xtemp = x.ModMulBarrett(x, n, mu)
        x       = xtemp.ModAddBarrett(c, n, mu)
        xxtemp = xx.ModMulBarrett(xx, n, mu)
        xx      = xxtemp.ModAddBarrett( c, n, mu)
        xxtemp = xx.ModMulBarrett(xx, n, mu)
        xx      = xxtemp.ModAddBarrett( c, n, mu)
        if (x > xx):
            divisor = GreatestCommonDivisor( x - xx, n)
        else:
            divisor = GreatestCommonDivisor( xx - x, n)
    return divisor;


def PrimeFactorize(n, primeFactors):
    ZERO = Integer(0)
    ONE = Integer(1)
    if ((n == ZERO) or (n == ONE)):
        return
    elif (MillerRabinPrimalityTest(n)):
        primeFactors.add(n.Get())
        return

    divisor = PollardRhoFactorization(n)
    PrimeFactorize(divisor, primeFactors)
    PrimeFactorize(n / divisor, primeFactors)

def FindGenerator(q):
    ZERO = Integer(0)
    ONE = Integer(1)
    TWO = Integer(2)
    qm1 = q - ONE
    qm2 = q - TWO
    primeFactors = set()
    PrimeFactorize(qm1, primeFactors)
    cnt = ZERO
    gen = qm2.RNG() + ONE
    for it in primeFactors:
        cnt = cnt + ONE
        temp = qm1 // it
        gen = gen.ModExp(temp, q)
        if (gen == ONE):
            break
    while (cnt != len(primeFactors)):
        cnt = ZERO
        gen = qm2.RNG() + ONE
        for it in primeFactors:
            cnt = cnt + ONE
            temp = qm1 // it
            gen = gen.ModExp(temp, q)
            if (gen == ONE):
                break
    return gen

def RootOfUnityIndv(M, modulo):
    ZERO = Integer(0)
    ONE = Integer(1)
    if ((modulo - ONE).Mod( M) != ZERO):
        print("Please provide a primeModulus(q) and a cyclotomic number(m) satisfying the condition: (q-1)/m is an integer. The values of primeModulus = " + str(modulo) + " and m = " + str(M) + " do not satisfy this condition")

    #
    gen    = FindGenerator(modulo)
    result = gen.ModExp((modulo - ONE).DividedBy(M), modulo);
    if (result == ONE):
        result = RootOfUnity(M, modulo)
    mu = modulo.ComputeMu()
    x = ONE
    x.ModMulEqBarrett(result, modulo, mu)

    coprimes = GetTotientList(M)
    minRU = x
    curPowIdx = 1
    for i in range(len(coprimes)):
        nextPowIdx = coprimes[i]
        diffPow = nextPowIdx - curPowIdx
        for j in range(diffPow):
            x.ModMulEqBarrett(result, modulo, mu)
        if ((x < minRU) and (x != 1)):
            minRU = x
        curPowIdx = nextPowIdx
    return minRU

def RootOfUnity(M, moduli):
    rootsOfUnity = np.zeros(moduli.size)
    for i in range(moduli.size):
        print(i)
        temp = RootOfUnityIndv(M, moduli[i])
        rootsOfUnity[i] = temp._val
    return rootsOfUnity
