# Simon-Speck

# C++
Implement on VS2010<br>
C++_2: Uses "<<" and ">>" operation to implement rotate shift left and rotate shift right.<br>
For example, we can use "(X<<n)|(X>>(32-n))" to implement 'rotate shift left X with n bits' on the assumption that X is an unsigned int.<br>
C++_3: More efficient and simpler implementation of Simon. Speck has not been implemented yet. <i>Although the type of input parameters are different, they can be put together, such as setSimonKey32 and setSimonKey64.</i><br>

# AVR
Implement on ATmega128, IDE is Atmel Studio 6.2.<br>
Simon64/128 and Speck64/128 are implemented for Minimal RAM and High-Throughput.<sup>[2]</sup>.<br>
Some instructions such as <b>adiw</b> and <b>cpi</b> can be used to reduce the number of code.<br>

# Triathlon-Only-AVR
Implement on ATmega128, IDE is Atmel Studio 6.2.<br>
Two Scenarios: 1)128 bytes in CBC mode; 2) 128 bits in CTR mode<sup>[3,4]</sup><br>

# Triathlon-C
Two scenarios, each with 3 platfrom.

# References
[1] <a href="https://eprint.iacr.org/2013/404.pdf">The Simon and Speck Families of Lightweight Block Ciphers.<a/><br>
[2] The Simon and Speck Block Ciphers on AVR 8-bit<br>
[2] Triathlon of Lightweight Block Ciphers for the Internet of Things<br>
[3] FELICS - Fair Evaluation of Lightweight Cryptographic Systems<br>
[4] <a href="http://perso.uclouvain.be/fstandae/lightweight_ciphers/">http://perso.uclouvain.be/fstandae/lightweight_ciphers/</a><br>
