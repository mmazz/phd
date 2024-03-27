from prime import *



mod = Integer(4)
a = Integer(3)
b = Integer(4)
print(7, a+b)
print(1, b-a)
c = a + 3
print(6, c)
print("True", a>1)
print("False", a<1)

print("True", a>=1)
print("False", a<=1)

print(f"{b} es par", 0, b&1)
print(f"{a} es impar", 1, a&1)

print(MillerRabinPrimalityTest(a))
print(MillerRabinPrimalityTest(b))

nBits = Integer(16)
cycleOrder = Integer(16)
q = FirstPrime(nBits, cycleOrder)
print(q)
q = LastPrime(nBits, cycleOrder)
print(q)
q = NextPrime(q, cycleOrder)
print(q)
q = PreviousPrime(q, cycleOrder)
print(q)

p = PollardRhoFactorization(q)
print(q, p)
p = PollardRhoFactorization(Integer(6))
print(Integer(6), p)


mod = Integer(33)
pa = RootOfUnityIndv(cycleOrder, mod)
print(pa)

moduli = np.array([Integer(33), Integer(65), Integer(97)])
roots = RootOfUnity(cycleOrder, moduli)
for i in range(roots.size):
    print(roots[i])
