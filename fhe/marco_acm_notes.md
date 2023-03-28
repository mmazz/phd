# Some notes

* An_Introduction_To_Mathematical_Cryptography.pdf: https://link.springer.com/book/10.1007/978-0-387-77993-5
  * This book was recommended to me to understand the mathematical background, at least for the lattice-based schemes.
  * Chapter 7 talks about lattice-based crypto and Section 8.1 focuses on homomorphic encryption (but is rather short).
* Microsoft SEAL: https://github.com/microsoft/SEAL
  * I am mainly targeting this C++ library. It implements BFV (for integers) and CKKS (for floats).
  * *(Shameless self-promotion)* I supervised a bachelor student doing his thesis on implementing CKKS-encrypted logistic
  regression: https://scripties.uba.uva.nl/search?id=record_28862;setlang=en (let me know if you want the source code).
  He talks more about the implementation downsides (memory consumption is much higher,
  limited multiplicative depth means you only have a limited number of training rounds).
  * Unfortunately, it seems bootstrapping is not implemented in SEAL.
  * They also have an embedded implementation, focusing on reducing the memory consumption: https://github.com/microsoft/SEAL-Embedded
* Literature on hardware acceleration for HE
  * RACE: RISC-V SoC for En/decryption Acceleration on the Edge for Homomorphic Computation - https://doi.org/10.1145/3531437.3539725
    * Actually, this was what I wanted to do... And I discovered this at the ACM Summer School (was published in August).
  * Cheetah: Optimizing and Accelerating Homomorphic Encryption for Private Inference - https://doi.org/10.1109/HPCA51647.2021.00013
  * HEAX: An Architecture for Computing on Encrypted Data - https://doi.org/10.1145/3373376.3378523
  * F1: A Fast and Programmable Accelerator for Fully Homomorphic Encryption - https://doi.org/10.1145/3466752.3480070
    * In my notes I said I liked their explanation of FHE the most. Decide for yourself if you agree :)
  * HEAWS: An Accelerator for Homomorphic Encryption on the Amazon AWS FPGA - https://doi.org/10.1109/TC.2020.2988765
