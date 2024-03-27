import numpy as np

'''
El Eq es que lo hace inplace


'''



class Integer():

    def __init__(self, val=0):
            self._val = np.int64(val)

    def __add__(self, other):
        if isinstance(other, Integer):
            res = Integer(self._val + other._val)
        else:
            res = Integer(self._val + np.int64(other))
        return res

    def __sub__(self, other):
        if isinstance(other, Integer):
            res = Integer(self._val - other._val)
        else:
            res = Integer(self._val - np.int64(other))
        return res

    def __mul__(self, other):
        if isinstance(other, Integer):
            res = Integer(self._val * other._val)
        else:
            res = Integer(self._val * np.int64(other))
        return res

    def __floordiv__(self, other):
        if isinstance(other, Integer):
            res = Integer(self._val // other._val)
        else:
            res = Integer(np.floor_divide(self._val, np.int64(other)))
        return res

    def __truediv__(self, other):
        if isinstance(other, Integer):
            res = Integer(self._val / other._val)
        else:
            res = Integer(self._val / np.int64(other))
        return res

    def __and__(self, other):
        if isinstance(other, Integer):
            res = Integer(self._val & other._val)
        else:
            res = Integer(self._val & np.int64(other))
        return res

        # este creo que no funca
#    def __isub__(self, other):
#        if isinstance(other, Integer):
#            self._val -= other._val
#        else:
#            self._val -= np.int64(other)
#
#        # este creo que no funca
#    def __iadd__(self, other):
#        if isinstance(other, Integer):
#            self._val += other._val
#        else:
#            self._val += np.int64(other)

    def __str__(self):
        return f"{self._val}"

    def __lt__(self, other):
        if isinstance(other, Integer):
            if(self._val < other._val):
                return True
            else:
                return False
        else:
            if(self._val < other):
                return True
            else:
                return False

    def __le__(self, other):
        if isinstance(other, Integer):
            if(self._val <= other._val):
                return True
            else:
                return False
        else:
            if(self._val <= other):
                return True
            else:
                return False

    def __gt__(self, other):
        if isinstance(other, Integer):
            if(self._val > other._val):
                return True
            else:
                return False
        else:
            if(self._val > other):
                return True
            else:
                return False

    def __ge__(self, other):
        if isinstance(other, Integer):
            if(self._val >= other._val):
                return True
            else:
                return False
        else:
            if(self._val >= other):
                return True
            else:
                return False
    def __eq__(self, other):
        if isinstance(other, Integer):
            if(self._val == other._val):
                return True
            else:
                return False
        else:
            if(self._val == other):
                return True
            else:
                return False

    def __ne__(self, other):
        if isinstance(other, Integer):
            if(self._val != other._val):
                return True
            else:
                return False
        else:
            if(self._val != other):
                return True
            else:
                return False

    def Get(self):
        return self._val

    def GetMSB(self):
        """Returns the index, counting from 0, of the
        least significant set bit in `x`.
        """
        return (1 + (self._val ^ (self._val-1))) >> 1

    def RNG(self):
        if (self._val <= 2):
            print(f"Error in randomg generator, high number is: {self._val}")
            return Integer(2)
        else:
            return Integer(np.random.randint(2, self._val))

    def RShiftEq(self, m):
        mv = m._val
        self._val = self._val >> mv

    def RShift(self, m):
        mv = m._val
        res = self._val >> mv
        return Integer(res)

    def Add(self,b):
        bv = b._val
        return Integer(self._val + bv)

    def AddEq(self,b):
        bv = b._val
        self._val = self._val + bv

    def Sub(self,b):
        bv = b._val
        return Integer(self._val - bv)

    def SubEq(self, b):
        bv = b
        self._val = self._val - bv

    def Mul(self,b):
        bv = b._val
        return Integer(self._val * bv)

    def MulEq(self,b):
        bv = b._val
        self._val = self._val * bv

    def DividedBy(self,b):
        bv = b._val
        return Integer(self._val / bv)

    def Mod(self,m):
        mv = m._val
        return Integer(self._val % mv)

    def ModEq(self,m):
        self._val = self._val % m

    def ModAdd(self, b, m):
        bv = b._val
        mv = m._val
        return Integer((self._val+bv) % mv)

    def ModMul(self, b, m):
        bv = b._val
        mv = m._val
        return Integer((self._val*bv) % mv)

    def ModMulEq(self, b, m):
        bv = b._val
        mv = m._val
        self._val = (self._val*bv) % mv

        # falta implementarla
    def ModMulFastEq(self, b, m):
        self.ModMulEq(b, m)

    def ModExp(self, b, m):
        return Integer(np.power(self._val, b._val) % m._val)

    def ComputeMu(self):
        if (self._val == 0):
            print("ComputeMu: Divide by zero")
            return 0
        temp = Integer((1 << (2 * self.GetMSB() + 3)))
        return temp.DividedBy(self)

# Por el momento implemento sin barrett
    def ModBarrett(self, modulus,  mu):
        return self.Mod(modulus)

    def ModEqBarrett(self, modulus, mu):
        self.ModEq(modulus)

    def ModAddBarrett(self, b, modulus, mu):
        return self.ModAdd(b, modulus);

    def ModMulBarrett(self, b, modulus,mu):
        return self.ModMul(b, modulus);

    def ModMulEqBarrett(self, b, modulus, mu):
        self.ModMulEq(b, modulus)

