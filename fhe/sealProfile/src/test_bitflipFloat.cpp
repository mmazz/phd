/*
Viendo como cambia modificando cada bit, https://baseconvert.com/ieee-754-floating-point, vemos
que es correcto lo que se genera.
Tambien vemos que el chequeador tambien funciona.
*/
#include "../src/examples.h"
#include "../src/utils_mati.h"

double value = 1.0;
uint64_t int_value;
uint64_t int_target;

void* int_value_p = memcpy(&int_value, &value, sizeof(int_value));

int main(int argc, char * argv[])
{
    double target = 1.0;
    int bit_changed = 0;
    for (short bit_change=0; bit_change<64; bit_change++)
    {
        target = bit_flip(value, bit_change);

        void* int_target_p = memcpy(&int_target, &target, sizeof(int_target));
        bit_changed = bit_check( int_target, int_value, 64);
        if (bit_changed == bit_change)
            std::cout<< bit_change << ": "<< std::setprecision (64)<< target << " bit_check is correct!" << std::endl;
        else
        {
            std::cout<< bit_change << ": "<< std::setprecision (64)<< target << " bit_check is WROOOONGGG!" << std::endl;
            std::cout<< std::setprecision (64) <<bit_change << " " << bit_changed << std::endl;
            std::cout<< " " << std::endl;
        }
    }
   return 0;
}
