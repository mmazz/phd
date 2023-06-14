import pandas as pd

dir0 = "../Random_data/log_encode_NTT/"
print("Encoding using NTT")
print(" ")
print("Using RNS ")
df0_0 = pd.read_csv(dir0+'encoding_elemDiff_withRNS.txt', header=None,  skip_blank_lines=False)
df0_1 = pd.read_csv(dir0+'encoding_withRNS.txt', header=None,  skip_blank_lines=False)
df0_0 = df0_0.iloc[1:,:]
df0_1 = df0_1.iloc[1:,:]
encoding = df0_0[df0_0.columns[0]].to_numpy(dtype=int) .sum()
print("Element diference             ", encoding)
encoding = df0_1[df0_1.columns[0]].to_numpy(dtype=int) .sum()
print("Element diference with norm 2 ", encoding)


print("Without RNS ")
df0_0 = pd.read_csv(dir0+'encoding_elemDiff_nonRNS.txt', header=None,  skip_blank_lines=False)
df0_1 = pd.read_csv(dir0+'encoding_nonRNS.txt', header=None,  skip_blank_lines=False)
df0_0 = df0_0.iloc[1:,:]
df0_1 = df0_1.iloc[1:,:]
encoding = df0_0[df0_0.columns[0]].to_numpy(dtype=int) .sum()
print("Element diference             ", encoding)
encoding = df0_1[df0_1.columns[0]].to_numpy(dtype=int) .sum()
print("Element diference with norm 2 ", encoding)

print(" ")
print(" ")

dir0 = "../Random_data/log_encode_nonNTT/"
print("Encoding without NTT")
print(" ")
print("Using RNS ")
df0_0 = pd.read_csv(dir0+'encoding_elemDiff_nonNTT_withRNS.txt', header=None,  skip_blank_lines=False)
df0_1 = pd.read_csv(dir0+'encoding_nonNTT_withRNS.txt', header=None,  skip_blank_lines=False)
df0_0 = df0_0.iloc[1:,:]
df0_1 = df0_1.iloc[1:,:]
encoding = df0_0[df0_0.columns[0]].to_numpy(dtype=int) .sum()
print("Element diference             ", encoding)
encoding = df0_1[df0_1.columns[0]].to_numpy(dtype=int) .sum()
print("Element diference with norm 2 ", encoding)


print("Without RNS ")
df0_0 = pd.read_csv(dir0+'encoding_elemDiff_nonNTT_nonRNS.txt', header=None,  skip_blank_lines=False)
df0_1 = pd.read_csv(dir0+'encoding_nonNTT_nonRNS.txt', header=None,  skip_blank_lines=False)
df0_0 = df0_0.iloc[1:,:]
df0_1 = df0_1.iloc[1:,:]
encoding = df0_0[df0_0.columns[0]].to_numpy(dtype=int) .sum()
print("Element diference             ", encoding)
encoding = df0_1[df0_1.columns[0]].to_numpy(dtype=int) .sum()
print("Element diference with norm 2 ", encoding)



print(" ")
print(" ")


dir0 = "../Random_data/log_encrypt_NTT/"
print("Encryption using NTT")
print(" ")
print("Using RNS ")
df0_0 = pd.read_csv(dir0+'encryption_c_elemDiff_withRNS.txt', header=None,  skip_blank_lines=False)
df0_1 = pd.read_csv(dir0+'encryption_c_withRNS.txt', header=None,  skip_blank_lines=False)
df0_0 = df0_0.iloc[1:,:]
df0_1 = df0_1.iloc[1:,:]
encoding = df0_0[df0_0.columns[0]].to_numpy(dtype=int) .sum()
print("Element diference             ", encoding)
encoding = df0_1[df0_1.columns[0]].to_numpy(dtype=int) .sum()
print("Element diference with norm 2 ", encoding)


print("Without RNS ")
df0_0 = pd.read_csv(dir0+'encryption_c_elemDiff_nonRNS.txt', header=None,  skip_blank_lines=False)
df0_1 = pd.read_csv(dir0+'encryption_c_nonRNS.txt', header=None,  skip_blank_lines=False)
df0_0 = df0_0.iloc[1:,:]
df0_1 = df0_1.iloc[1:,:]
encoding = df0_0[df0_0.columns[0]].to_numpy(dtype=int) .sum()
print("Element diference             ", encoding)
encoding = df0_1[df0_1.columns[0]].to_numpy(dtype=int) .sum()
print("Element diference with norm 2 ", encoding)




print(" ")
print(" ")

dir0 = "../Random_data/log_encrypt_nonNTT/"
print("Encryption without NTT")
print(" ")
print("Using RNS ")
df0_0 = pd.read_csv(dir0+'encryption_c_elemDiff_nonNTT_withRNS.txt', header=None,  skip_blank_lines=False)
df0_1 = pd.read_csv(dir0+'encryption_c_nonNTT_withRNS.txt', header=None,  skip_blank_lines=False)
df0_0 = df0_0.iloc[1:,:]
df0_1 = df0_1.iloc[1:,:]
encoding = df0_0[df0_0.columns[0]].to_numpy(dtype=int) .sum()
print("Element diference             ", encoding)
encoding = df0_1[df0_1.columns[0]].to_numpy(dtype=int) .sum()
print("Element diference with norm 2 ", encoding)


print("Without RNS ")
df0_0 = pd.read_csv(dir0+'encryption_c_elemDiff_nonNTT_nonRNS.txt', header=None,  skip_blank_lines=False)
df0_1 = pd.read_csv(dir0+'encryption_c_nonNTT_nonRNS.txt', header=None,  skip_blank_lines=False)
df0_0 = df0_0.iloc[1:,:]
df0_1 = df0_1.iloc[1:,:]
encoding = df0_0[df0_0.columns[0]].to_numpy(dtype=int) .sum()
print("Element diference             ", encoding)
encoding = df0_1[df0_1.columns[0]].to_numpy(dtype=int) .sum()
print("Element diference with norm 2 ", encoding)


