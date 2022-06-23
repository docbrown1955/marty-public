import numpy as np
import matplotlib.pyplot as plt

m = np.arange(100, 1000)
c7, c7t, c8, c8t, c9, c9t, c9b, c9bt, c9a, c9at = np.loadtxt("data.txt").T
c7t /= 2
c8t /= 2
c9t /= 2
c9at /= 2
c9bt /= 4

errc7 = np.nanmean(np.abs((c7 - c7t) / c7t))
errc8 = np.nanmean(np.abs((c8 - c8t) / c8t))
errc9 = np.nanmean(np.abs((c9 - c9t) / c9t))
errc9a = np.nanmean(np.abs((c9a - c9at) / c9at))
errc9b = np.nanmean(np.abs((c9b - c9bt) / c9bt))

ratio = 10
mm = m[::ratio]
c7 = c7[::ratio]
c8 = c8[::ratio]
c9 = c9[::ratio]
c9a = c9a[::ratio]
c9b = c9b[::ratio]

plt.figure(figsize=(15, 12))

plt.subplot(231)
plt.title("C7 D_rel={x:.3e}".format(x=errc7))
plt.plot(mm, c7,  'r--+', label="MARTY")
plt.plot(m, c7t, label="Theo")
plt.legend()
plt.subplot(232)
plt.title("C8 D_rel={x:.3e}".format(x=errc8))
plt.plot(mm, c8,  'r--+', label="MARTY")
plt.plot(m, c8t, label="Theo")
plt.legend()
plt.subplot(233)
plt.title("C9 D_rel={x:.3e}".format(x=errc9))
plt.plot(mm, c9,  'r--+', label="MARTY")
plt.plot(m, c9t, label="Theo")
plt.legend()
plt.subplot(234)
plt.title("C9a D_rel={x:.3e}".format(x=errc9a))
plt.plot(mm, c9a,  'r--+', label="MARTY")
plt.plot(m, c9at, label="Theo")
plt.legend()
plt.subplot(235)
plt.title("C9b D_rel={x:.3e}".format(x=errc9b))
plt.plot(mm, c9b,  'r--+', label="MARTY")
plt.plot(m, c9bt, label="Theo")
plt.legend()

plt.show()
