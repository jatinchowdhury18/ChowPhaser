# %%
import numpy as np
import scipy.signal as signal
import matplotlib.pyplot as plt
import audio_dspy as adsp
from IPython.display import display, SVG, Image

# %%[markdown]
# # Under the hood of ChowPhaser
#
# Recently, I released the [ChowPhaser](https://github.com/jatinchowdhury18/ChowPhaser)
# audio plugin, an open-source phaser effect based loosely on the
# Schulte Compact Phasing 'A'. I thought it might be cool to take
# a look under the hood of ChowPhaser and show what sets it apart
# from the average digital phaser effect.
#
# ## The Schulte Phaser
#
# The original Schulte phaser is a pretty rare effect but with a
# somewhat large cult status, owing to its use by several notable
# artists including Kraftwerk, Pink Floyd,
# [Tangerine Dream](https://www.youtube.com/watch?v=URssLwPXkVk),
# and more.  While I won't do a full circuit analysis here, I will
# try to explain the overall function of the phaser at a higher level.
# The general signal processing structure of the Schulte phaser is
# shown below:

# %%
display(SVG('../writing/figures/schulte_arch.svg'))

# %% [markdown]
# The Schulte phaser uses 8 first order phasing allpass sections
# all sharing the same low-frequency oscillator (LFO). The output
# of the first two sections is fed back to the input, the amount of
# feedback being controlled by the feedback gain $G$. The output of
# the feedback loop is then fed to one output channel, and mixed with
# the output of the allpass sections then fed to the second output
# channel.
#
# ## LFO
#
# The typical digital phaser might use an LFO made up of a pure sine
# wave, square wave or other standard digital waveform. While the
# Schulte phaser does use a sine-like oscillator circuit, the way
# this oscillator affects the allpass sections is a little bit more
# complex. The Schulte LFO circuit is used to drive a light bulb,
# which then affects the resistance of a light-dependent resistor
# (LDR)  present in each allpass section. LDR tend to exhibit an
# interesting nonlinear characteristic, of the form:
#
# $$
# R_{LD}(x) = R_0 x^{-0.75}
# $$
#
# where $R_0$ is the maximum resistance of the LDR, and $x$ is
# proportional to the light received by the LDR. When creating
# ChowPhaser, I thought it might be interesting to allow the
# user to "skew" the LFO in a form similar to the response
# of an LDR driven by an oscillating light bulb. Since the
# phaser uses a digital sine wave on the range \[-1, 1\], our
# "skew" equation is written as:
#
# $$
# y(x) = 2 \left( \frac{x+1}{2} \right)^{2^\lambda} - 1
# $$
#
# with $\lambda$ as a skew parameter. The LFO output for
# several different skew parameters is shown below.

# %%
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

# %% [markdown]
# ## Feedback Stage
#
# While the Schulte phaser uses some of the same allpass
# sections for the feedback and modulation processing, separating
# these two processes can be convenient in the digital
# implementation. With that in mind, I developed a dedicated
# feedback stage with the architecture shown below:

# %%
display(Image(filename='../writing/figures/fb_arch.png'))

# %% [markdown]
# At first glance, this structure might seem incomputable, since
# it contains a delay-free loop. That said, we can still find
# closed-form solution, using the process described by Dave
# Berners and Jonathan Abel in their 2016
# [AES paper](http://www.aes.org/e-lib/browse.cfm?elib=18489).
#
# $$
# H(z) = \frac{P_1(z) P_2(z)}{1 - G P_1(z) P_2(z)}
# $$
#
# This stage can then be implemented using a standard biquad
# filter. We can then connect the phasers in the feedback stage
# to our sine LFO, which gives the following frequency responses.

# %%
fs = 44100
C = 12e-9
worN = 2*np.logspace(1, 4, 500)

def lfo2res(lfo):
    max_depth = 20
    light_val = (max_depth + 0.1) - (lfo * max_depth)
    return 100000 * np.power(light_val / 0.1, -0.75)

def calc_fb_coefs(R, G):
    b_s = [(R*C)**2, -2*R*C, 1.0]
    a_s = [b_s[0] * (1 + G), -b_s[1] * (1 - G), 1 + G]

    return signal.bilinear(b_s, a_s, fs=fs)

def new_fr_plot():
    plt.figure()
    plt.xlabel('Frequency [Hz]')
    plt.ylabel('Magnitude [dB]')
    plt.grid()
    plt.xlim(20, 20000)
    
# Feedback gain plot
new_fr_plot()
legend = []
for G in [0.0, 0.25, 0.5, 0.75, 0.9]:
    b, a = calc_fb_coefs(10000, G)
    adsp.plot_magnitude_response(b, a, worN=worN, fs=fs)
    legend.append(f'Feedback = {G}')

plt.title('Feedback Stage Response at various feedback gains')
plt.legend(legend)

# LFO plot
new_fr_plot()
legend = []
for lfo in [-1, 0, 0.5, 0.9, 1.0]:
    b, a = calc_fb_coefs(lfo2res(lfo), 0.5)
    adsp.plot_magnitude_response(b, a, worN=worN, fs=fs)
    legend.append(f'LFO = {lfo}')

plt.title('Feedback Stage Response at various LFO values')
plt.legend(legend, loc='lower left')

# %% [markdown]
# To make the effect more interesting, we can implement the
# feedback stage using a nonlinear biquad filter, as I've
# described in some previous
# [blog posts](https://medium.com/@jatinchowdhury18/complex-nonlinearities-episode-4-nonlinear-biquad-filters-ae6b3f23cb0e).
# The nonlinear biquad filter allows us to apply a saturating
# nonlinearity to the states of the biquad filter, or to the
# feedback paths of the filter. These nonlinearities cause the
# frequency response of the feedback stage to change depending
# on the operating point of the filter.


# %%
def nl_biquad(b, a, op):
    g = (1.0 / np.cosh(op))**2
    mult = np.array([1.0, g, g*g])
    return mult*b, mult*a

def nl_feedback(b, a, op):
    g = (1.0 / np.cosh(op))**2
    mult = np.array([1.0, g, g])
    return b, mult*a

new_fr_plot()
legend = []
b, a = calc_fb_coefs(36000, 0.5)
for op in [0, 0.05, 0.1, 0.15]:
    b_p, a_p = nl_biquad(b, a, op)
    adsp.plot_magnitude_response(b_p, a_p, worN=worN, fs=fs)
    legend.append(f'A = {op}')

plt.title('Feedback Response with Saturating States')
plt.legend(legend)

new_fr_plot()
legend = []
b, a = calc_fb_coefs(80000, 0.5)
for op in [0, 0.05, 0.1, 0.15]:
    b_p, a_p = nl_feedback(b, a, op)
    adsp.plot_magnitude_response(b_p, a_p, worN=worN, fs=fs)
    legend.append(f'A = {op}')

plt.title('Feedback Response with Saturating Feedback')
plt.legend(legend)

# %% [markdown]
# ## Modulation
#
# With the feedback stage separated out, we can now develop a
# dedicated modulation stage made up of a chain of allpass
# sections modulated by the phaser LFO. While the original
# circuit uses a chain of 8 allpass sections, in the digital
# realm we are free to change the number of allpass sections in
# real-time, even allowing the user to fade between the number of
# stages continuously.

# %%
def calc_mod_coefs(R):
    b_s = [R*C, -1]
    a_s = [R*C, +1]

    return signal.bilinear(b_s, a_s, fs=fs)

def get_sos_from_ba(b, a):
    section = np.zeros(6)
    section[:2] = b
    section[3:5] = a
    return section

def get_sos(R, N):
    sos = []

    b, a = calc_mod_coefs(R)
    s = get_sos_from_ba(b, a)
    for _ in range(N):
        sos.append(s)
    
    return sos

def new_pr_plot():
    plt.figure()
    plt.grid()
    plt.xlim(20, 20000)

new_pr_plot()
legend = []
for lfo in [-1, 0, 0.5, 0.9, 1.0]:
    sos = get_sos(lfo2res(lfo), 2)
    adsp.plot_phase_response_sos(sos, worN=worN, fs=fs)
    legend.append(f'LFO = {lfo}')

plt.title('Modulation Stage Response at various LFO values')
plt.legend(legend, loc='lower left')

new_pr_plot()
legend = []
for N in [2, 4, 6, 8, 10]:
    sos = get_sos(40000, N)
    adsp.plot_phase_response_sos(sos, worN=worN, fs=fs)
    legend.append(f'N = {N}')

plt.title(r'Modulation Stage Response with $N$ Allpass Sections')
plt.legend(legend, loc='lower left')

# %% [markdown]
# ## Putting It All Together
#
# In the end, all these components need to be brought together as
# a cohesive, musical effect. With that in mind, I've made a mono
# version of the effect with a similar signal flow to the original
# Schulte phaser, as well as a stereo version, with a dedicated
# LFO, feedback stage, and modulation stage for each channel.
# There are also "drive", "thrash", and "dirt" controls that
# affect the nonlinearities present in the feedback stage.
# A video demo of the effect can be seen on [YouTube](https://youtu.be/QuDWKLaUi_Q).
# To download the plugin or checkout the source code, check
# out the ChowPhaser [GitHub repository](https://github.com/jatinchowdhury18/ChowPhaser).
#
# Big thanks to everyone reading this, as well as to some folks
# on the KVR forum for inspiring me to check out the Schulte
# phaser in the first place, and in particular Margus for helping
# with the design of the plugin!

# %%
