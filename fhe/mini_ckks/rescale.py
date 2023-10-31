from ckks import *

def test_rescale(M, h, scale, P, level, q0):
    encoder = CKKSScheme(M=M, h=h, scale=scale, P=P, level=level, q0=q0)
    secret = encoder.keyGen()
    z1 = scale*np.array([20, 40])
    m1 = encoder.encode(z1, scale)
    c1 = encoder.encrypt(m1)
    m = encoder.decrypt(c1, secret)
    z = encoder.decode(m, scale)
    zReal = np.rint(z.real)
    c1ReScale = encoder.reScale(c1)
    mReScale = encoder.decrypt(c1ReScale, secret)
    zReScale = encoder.decode(mReScale, scale)
    zReScaleReal = np.rint(zReScale.real)
    if(zReal[0]==zReScaleReal[0] and zReal[1]==zReScaleReal[1]):
        print("q0=", q0)
        print("scale=", scale)
        print("P=", P)
        print("Decript:")
        print("Original: ", z1)
        print("CKKS: ",zReal)
        print("CKKS Rescale: ", zReScaleReal)
        print()
    else:
        if VERBOSE:
            print(zReal[0], "!=", zReScaleReal[0], " and ", zReal[1], "!=",zReScaleReal[1])

M = 8
q0s = [11,12,13]
scales = [5,6,8,9]
count = len(q0s)*len(scales)
BARRIDO = False
VERBOSE = True
scale = 2**10
level = 2
q0 = 2**10
P = q0
h = 2

if BARRIDO:
    for i in range(len(q0s)):
        for j in range(len(scales)):
            print(count)
            test_rescale(M, h, 2**scales[j], scales[j]**2*q0s[i], 1, 2**q0s[i])
            count -= 1

else:
    test_rescale(M=M, h=h, scale=scale, P=P, level=level, q0=q0)


"""
encoder = CKKSScheme(M=M, scale=scale,P=P, level=level, q0=q0)
secret, evk = encoder.keyGen()


z2 = np.array([4, 5])
m2 = encoder.encode(z2, scale)
c2 = encoder.encrypt(m2)
cmuli = encoder.CmulI(c1, c1, evk)
cmuliR = encoder.relin(cmuli, evk)
cMulReScale = encoder.reScale(cmuliR)

mMulReScale = encoder.decrypt(cMulReScale, secret)
zMulReScale = encoder.decode(mMulReScale, scale)
zMulReScaleReal = zMulReScale.real
print("Mult:")
print("Original: [",z1[0]*z2[0], z1[1]*z2[1],"]")
print("CKKS: ", np.rint(zMulReScaleReal))

"""
