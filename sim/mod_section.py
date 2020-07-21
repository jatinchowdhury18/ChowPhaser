import numpy as np
import scipy.signal as signal
import matplotlib.pyplot as plt
import audio_dspy as adsp

fs = 44100
C = 12e-9
worN = 2*np.logspace(1, 4, 500)

def calc_coefs(R):
    b_s = [R*C, -1]
    a_s = [R*C, +1]

    return signal.bilinear(b_s, a_s, fs=fs)

def calc_coefs_frac(R, frac):
    b, a = calc_coefs(R)
    b *= frac
    b[0] += 1.0 - frac

    return b, a

def get_sos_from_ba(b, a):
    section = np.zeros(6)
    section[:2] = b
    section[3:5] = a
    return section

def get_sos(R, N):
    sos = []

    num = int(N)
    frac = N - num

    b, a = calc_coefs(R)
    s = get_sos_from_ba(b, a)
    for _ in range(num):
        sos.append(s)

    b, a = calc_coefs_frac(R, frac)
    sos.append(get_sos_from_ba(b, a))
    
    return sos

R = 40000

def plot_phases(Ns, title):
    plt.figure()
    legend = []
    for N in Ns:
        sos = get_sos(R, N)
        adsp.plot_phase_response_sos(sos,  worN=worN, fs=fs)
        legend.append(f'Mod stages: {N}')

    plt.legend(legend)
    plt.grid()
    plt.xlim(20, 20000)
    plt.title(title)

plot_phases([2, 4, 6, 8, 10], 'Phase Response of Modulation Stage')
plt.savefig('writing/figures/mod_phase.png')
# plot_phases([4, 4.9, 5, 5.9999, 6], 'Phase Responseof Fractional Modulation Sections')

plt.show()
