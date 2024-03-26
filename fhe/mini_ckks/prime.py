import numpy as np
import random


def GetMSB(x):
    """Returns the index, counting from 0, of the
    least significant set bit in `x`.
    """
    return (1 + (x ^ (x-1))) >> 1

def RShiftEq(d, m):
    d = d >> m
    return d

def RShift(d, m):
    d = d >> m
    return d

def Add(a,b):
    return a + b

def Sub(a,b):
    return a - b

def Mul(a,b):
    return a * b
def Mod(a,m):
    return a % m

def ModMu(a, modulus,  mu):
    if (a < modulus):
        return a
    n = GetMSB(modulus)
    alpha = (n + 3)
    beta = -2
    q = mu * RShift(a, n + beta)
    q >>= (alpha - beta)
    z = Sub(a, q * modulus)
    if (z >= modulus):
        return Sub(z, modulus)
    return z;

def DividedBy(a, b):
    return int(a/b)



def RNG(p):
    return random.randint(2, p)

def SubEq(a, b, modulus, mu):
    bv = b
    if (bv >= modulus):
        bv = ModEq(bv, modulus, mu);
    if (a >= modulus):
        a = ModEq(a, modulus, mu);
    if (a < bv):
        a = Add(modulus, a);
    return SubEq(a, bv);

def ModAdd(a, b, m):
    return (a+b) % m

def ModAddMu(a, b, modulus, mu):
    return ModMu(Add(b, a),modulus, mu);

def ModMul(a, b, m):
    return (a*b) % m

def ModMulMu(a, b, modulus,mu):
    bv = b
    av = a
    if (bv >= modulus):
        bv = ModEq(bv, modulus, mu)
    if (av >= modulus):
        av = ModEq(av, modulus, mu);
    return ModMu(Mul(av,bv),modulus, mu);

def ModExp(a, b, m):
    return pow(a,b, m)

def ModMulFastEq(a, b, m):
    return (a*b)%m

def ModEq(a, b, modulus, mu):
    if (a < modulus):
        return a
    n = GetMSB(modulus)
    alpha = n + 3
    beta = -2
    q = mu * RShift(a, n + beta)
    q >>= alpha - beta
    a = SubEq(a, q * modulus)
    if (a >= modulus):
        return SubEq(a, modulus)
    return a

def ModMulEq(a, b, modulus, mu):
    bv = b
    if (bv >= modulus):
        bv = ModEq(bv, modulus, mu)
    if (a >= modulus):
        a= ModEq(a, modulus, mu)

    a = ModMu(Mul(bv,a), modulus, mu)
    return a

def ComputeMu(x):
    a = (1 << (2 * GetMSB(x) + 3))
    return DividedBy(a, x);

def GreatestCommonDivisor(a, b):
    m_a = a
    m_b = b
    while (m_b != 0):
        tmp = m_b
        m_b = m_a % m_b
        m_a = tmp
    return m_a

#std::vector<IntType>
def GetTotientList(n):
    result = []
    for i in range(1, n):
        if (GreatestCommonDivisor(i, n) == 1):
            result.append(i)
    return np.array(result)

# calcula b0 =  a**d mod p
# si da +1 calcula b1 = b0**2 mod p y asi hasta que bi = bi-1**d mod p de -1
def WitnessFunction(a,  d,  s,  p):
    b  = ModExp(a, d, p)
    prev_b = False
    for i in range(0, s):
        # si b no es ni 1 ni numero-1 entonces 1
        prev_b = ((b != 1) and (b != p - 1))
        # hago b**2 mod p y ese es mi nuevo valor
        b = ModMulFastEq(b, b, p)
        if ((b == 1) and prev_b):
            return True
    res = (b != 1)
    return res


def MillerRabinPrimalityTest(p, niter=40):
    ZERO = 0
    TWO = 2
    THREE = 3
    FIVE = 5
    if (p == TWO or p == THREE or p == FIVE):
        return True
    if (p < TWO or (p&1 == ZERO)):
        return False

    # Hasta aca ya sabemos que nuestro p es impar y no es de los triviales
    # Queremos que p-1 = ((2**s) * d))
    # basicamente esto va dividiendo d por 2 hasta que le da un numero no entero, y se queda con el d anterior
    d = p - 1
    s = 0
    while (Mod(d, TWO) == ZERO):
        # divido por 2...
        d = RShiftEq(d, 1)
        s += 1

    # aca ya tengo los valores que cumplen lo anterior
    for i in range(0, niter):
        rng = RNG(p - THREE)
        # La witness me dice si es un numero compesto o no. Si lo es ya esta me garantizo que lo es
        # si me devuelve False, es que puede ser que sea primo, y entocnes tengo que probar niter veces la prueba.
        if (WitnessFunction(ModAdd(rng, TWO, p), d, s, p)):
            return False
    return True

def FirstPrime(nBits, M):
    q = (1 << nBits)

    r = Mod(q, M)
    qNew = q + 1 - r
    if (r > 0):
        qNew += M
    while (not MillerRabinPrimalityTest(qNew)):
        qNew += M
        if (qNew < q):
            print("FirstPrime: overflow growing candidate")
    return qNew

def LastPrime(nBits, M):
    q = (1 << nBits)
    r = Mod(q, M)
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
    if (Mod(n,2) == 0):
        return 2
    divisor = 1
    c = RNG(n)
    x = RNG(n)
    xx = x
    mu = ComputeMu(n)
    xtemp = ModMulMu(x, x, n, mu)
    x       = ModAddMu(xtemp, c, n, mu)
    xxtemp = ModMulMu(xx, xx, n, mu)
    xx      = ModAddMu(xxtemp, c, n, mu)
    xxtemp = ModMulMu(xx, xx, n, mu)
    xx      = ModAddMu(xxtemp, c, n, mu)
    if (x > xx):
        divisor = GreatestCommonDivisor( x - xx, n)
    else:
        divisor = GreatestCommonDivisor( xx - x, n)

    while (divisor == 1):
        xtemp = ModMulMu(x, x, n, mu)
        x       = ModAddMu(xtemp, c, n, mu)
        xxtemp = ModMulMu(xx, xx, n, mu)
        xx      = ModAddMu(xxtemp, c, n, mu)
        xxtemp = ModMulMu(xx, xx, n, mu)
        xx      = ModAddMu(xxtemp, c, n, mu)
        if (x > xx):
            divisor = GreatestCommonDivisor( x - xx, n)
        else:
            divisor = GreatestCommonDivisor( xx - x, n)
    return divisor;


def PrimeFactorize(n, primeFactors):
    if ((n == 0) or (n == 1)):
        return
    elif (MillerRabinPrimalityTest(n)):
        primeFactors.add(n)
        return

    divisor = PollardRhoFactorization(n)
    PrimeFactorize(divisor, primeFactors)
    PrimeFactorize(n / divisor, primeFactors)

def FindGenerator(q):
    qm1 = q - 1
    qm2 = q - 2
    primeFactors = set()
    PrimeFactorize(qm1, primeFactors)
    cnt = 0
    gen = RNG(qm2) + 1
    for it in primeFactors:
        cnt+=1
        gen = ModExp(gen, int(qm1 / it), q)
        if (gen == 1):
            break
    while (cnt != len(primeFactors)):
        cnt = 0
        gen = RNG(qm2) + 1
        for it in primeFactors:
            cnt+=1
            gen = ModExp(gen, int(qm1 / it), q)
            if (gen == 1):
                break
    return gen

def RootOfUnityIndv(M, modulo):
    if (Mod((modulo - 1), M) != 0):
        print("Please provide a primeModulus(q) and a cyclotomic number(m) satisfying the condition: (q-1)/m is an integer. The values of primeModulus = " + str(modulo) + " and m = " + str(M) + " do not satisfy this condition")
    gen    = FindGenerator(modulo)
    result = ModExp(gen, DividedBy((modulo - 1), M), modulo)
    if (result == 1):
        result = RootOfUnity(M, modulo)
    mu = ComputeMu(modulo)
    x = 1
    x = ModMulEq(x, result, modulo, mu)

    coprimes = GetTotientList(M)
    minRU = x
    curPowIdx = 1
    for i in range(len(coprimes)):
        nextPowIdx = coprimes[i]
        diffPow = nextPowIdx - curPowIdx
        for j in range(diffPow):
            x = ModMulEq(x, result, modulo, mu)
        if ((x < minRU) and (x != 1)):
            minRU = x
        curPowIdx = nextPowIdx

    return minRU

def RootOfUnity(M, moduli):
    rootsOfUnity = np.zeros(moduli.size)
    for i in range(moduli.size):
        rootsOfUnity[i] = RootOfUnityIndv(m, moduli[i])
    return rootsOfUnity
