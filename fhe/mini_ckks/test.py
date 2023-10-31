from ckks import *
import time
# q0=scale=P=1024, level=2
M = 8
q0 = 2**9
scale = 2**9
roundCS = 0
level = 2
P = scale
h = 2
z0 = np.array([1, 2])
z1 = np.array([-3, 4])
def test(z0=z0, z1=z1, M=M, h=h, scale=scale, P=P, level=level, q0=q0, VERBOSE=True):

    encoder = CKKSScheme(M=M, h=h, scale=scale, P=P, level=level, q0=q0)
    secret = encoder.keyGen()
    m0 = encoder.encode(z0, scale)
    c0 = encoder.encrypt(m0)
    d0 = encoder.decrypt(c0, secret)
    z0dec = encoder.decode(d0, scale)
    z0real = np.round(z0dec.real, roundCS ) # np.rint(z0dec.real)
    if (z0real[0]==z0[0]) and (z0real[1]==z0[1]):
        if VERBOSE:
            print("Decription Works")
        m1 = encoder.encode(z1, scale)
        c1 = encoder.encrypt(m1)
        cadd = encoder.Cadd(c0,c1)
        madd = encoder.decrypt(cadd, secret)
        zadd = encoder.decode(madd, scale)
        zaddreal = np.round(zadd.real, roundCS)# np.rint(zadd.real)

        if (zaddreal[0]==np.round(z0[0]+z1[0], roundCS)) and (zaddreal[1]==np.round(z0[1]+z1[1], roundCS)):
            if VERBOSE:
                print("Addition Works")
            cmuli_cte = encoder.Cmul_cte(c0, m1)
            cmuli_cte = encoder.reScale(cmuli_cte)
            mmuli_cte = encoder.decrypt(cmuli_cte, secret)
            zmuli_cte = encoder.decode(mmuli_cte, scale)
            zmuliReal_cte = np.round(zmuli_cte.real, roundCS)
            z0z1 = [np.round(z0[0]*z1[0], roundCS), np.round(z0[1]*z1[1], roundCS)]

            if (zmuliReal_cte[0]==z0z1[0]) and (zmuliReal_cte[1]==z0z1[1]):
                print()
                print("Multiplication with plainText Works!!!")
                print("q0: ", q0, " P: ", P, " scale: ", scale)
            else:
                if VERBOSE:
                    print("Multiplication with plainText Failed")
                    print(zmuliReal_cte[0], "!=", z0z1[0], "    and    ", zmuliReal_cte[1], "!=", z0z1[1])

            cmuli = encoder.Cmul(c0, c1)
            cmuliRescale = encoder.reScale(cmuli)
            mmuliRescale = encoder.decrypt(cmuliRescale, secret)
            zmuliRescale = encoder.decode(mmuliRescale, scale)
            zmuliReScaleReal = zmuliRescale.real
            if (np.rint(zmuliReScaleReal[0])==z0z1[0]) and (np.rint(zmuliReScaleReal[1])==z0z1[1]):
                print("Multiplication with ReScaling Works")
                print("q0: ", q0, " scale: ", scale, " P: ", P)
            else:
                if VERBOSE:
                    print("Multiplication with ReScaling Failed")
                    print(round(zmuliReScaleReal[0], roundCS), "!=", z0z1[0], "    and    ",
                          round(zmuliReScaleReal[1], roundCS), "!=", z0z1[1])

        else:
            if VERBOSE:
                print("Addition Failed")
                print(zaddreal[0], "!=", z0[0]+z1[0], "    and    ", zaddreal[1], "!=", z0[1]+z1[1])
    else:
        if VERBOSE:
            print("Decription Failed")
            print(z0[0], "!=", z0real[0], "    and    ", z0[1], "!=", z0real[1])

print("vec 1: ", z0)
print("vec 2: ", z1)
BARRIDO = True
VERBOSE = True

if BARRIDO:
    q0s = [10,11,12,13,14,15]
    scales = q0s  #[5,6,8,9]
    Ps = q0s
    count = len(q0s)*len(scales)*len(Ps)
    for i in range(len(q0s)):
        for j in range(len(scales)):
            for k in range(len(Ps)):
                print(count)
                test(z0,z1, M, h, scale=2**scales[j], P=2**Ps[k], level=level, q0=2**q0s[i], VERBOSE=VERBOSE)
                count -= 1
else:
    scale = 2**9
    level = 2
    q0 = 2**4
    P = scale
    st = time.time()
    test(z0,z1, M=M, h=h, scale=scale, P=P, level=level, q0=q0,VERBOSE=VERBOSE)
    et = time.time()
    elapsed_time = round(et - st,5)
    print('Execution time:', elapsed_time, 'seconds')






