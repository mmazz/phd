all:  run_encode run_encode_multiRNS run_encode_nonNTT run_encode_nonNTT_multiRNS  run_encode_nonRNS run_encode_nonOps

run_encode:
	../bin/encode_bitflip 0

run_encode_multiRNS:
	../bin/encode_bitflip 0 5

run_encode_nonNTT:
	../bin/encode_bitflip 1

run_encode_nonNTT_multiRNS:
	../bin/encode_bitflip 1 5

run_encode_nonRNS:
	../bin/encode_bitflip 2

run_encode_nonOps:
	../bin/encode_bitflip 3

clean:
	rm -rf ../bin/*
