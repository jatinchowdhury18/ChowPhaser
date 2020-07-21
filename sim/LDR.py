import numpy as np
import matplotlib.pyplot as plt

def light_shape(x, skew):
    return (np.power((x + 1) / 2.0, skew) * 2) - 1

freq = 10
fs = 44100
N = 4096*2.75
sin = np.sin(2 * np.pi * np.arange(N) * freq / fs)

for s in [-1.5, 0.0, 1.5]:
    skew_val = 2**s
    lfo = light_shape(sin, skew_val)
    plt.plot(np.arange(N) / fs, lfo, label=f'skew = {s}')

plt.xlabel('Time [s]')
plt.grid()
plt.legend(loc='lower left')
plt.title('LFO with LDR Skew')
plt.savefig('writing/figures/ldr.png')
plt.show()
