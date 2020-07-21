import numpy as np
import scipy.signal as signal
import matplotlib.pyplot as plt
import audio_dspy as adsp

fs = 44100
C = 12e-9

def calc_coefs(R, G):
    b_s = [(R*C)**2, -2*R*C, 1.0]
    a_s = [b_s[0] * (1 + G), -b_s[1] * (1 - G), 1 + G]

    return signal.bilinear(b_s, a_s, fs=fs)

def nl_biquad(b, a, op):
    g = (1.0 / np.cosh(op))**2
    mult = np.array([1.0, g, g*g])
    return mult*b, mult*a

def nl_feedback(b, a, op):
    g = (1.0 / np.cosh(op))**2
    mult = np.array([1.0, g, g])
    return b, mult*a

def plot_fr(b, a, ops, func, title):
    plt.figure()
    legend = []
    for op in ops:
        b_p, a_p = func(b, a, op)
        adsp.plot_magnitude_response(b_p, a_p, worN=2*np.logspace(1, 4, 500), fs=fs)
        legend.append(f'A = {op}')

    plt.grid()
    plt.xlim(20, 20000)
    plt.title(title)
    plt.legend(legend)
    plt.xlabel('Frequency [Hz]')
    plt.ylabel('Magnitude [dB]')

b, a = calc_coefs(36000, 0.5)
plot_fr(b, a, [0, 0.05, 0.1, 0.15], nl_biquad, 'Feedback Response with Saturating States')
plt.savefig('writing/figures/nlbiquad.png')

b, a = calc_coefs(80000, 0.5)
plot_fr(b, a, [0, 0.05, 0.1, 0.15], nl_feedback, 'Feedback Response with Saturating Feedback')
plt.savefig('writing/figures/nlfeedback.png')

plt.show()
