import numpy as np
import matplotlib.pyplot as plt
from matplotlib.widgets import Button, TextBox

ppg = np.load('./ppg_set.npy')

ar_labels = np.full(len(ppg), 0)

def update_plot(signal, idx):
    global ar_labels
    ax.clear()
    ax.plot(signal[idx])
    ax.set_xlabel('Time')
    ax.set_ylabel('Amplitude')
    ax.set_title('ID: ' + str(idx) + ' Label: ' + str(ar_labels[idx]))
    ax.grid()
    plt.draw()

def on_key(event):
    if event.key == 'left':
        prev(event)
    elif event.key == 'right':
        next(event)
    elif event.key == '0':
        normal(event)
    elif event.key == '1':
        anomalies(event)

def next(event):
    global idx
    idx = (idx + 1) % len(ppg)
    update_plot(ppg, idx)

def prev(event):
    global idx
    idx = (idx - 1) % len(ppg)
    update_plot(ppg, idx)

def normal(event):
    global idx, ar_labels
    ar_labels[idx] = 0
    update_plot(ppg, idx)

def anomalies(event):
    global idx, ar_labels
    ar_labels[idx] = 1
    update_plot(ppg, idx)

def submit(text):
    global idx
    try:
        idx = int(text)
        if 0 <= idx < len(ppg):
            update_plot(ppg, idx)
        else:
            print('Invalid idx. Please enter a number between 0 and', len(ppg) - 1)
    except ValueError:
        print('Invalid input. Please enter a number.')

def save(event):
    normal_data = ppg[ar_labels == 0]
    anomalies_data = ppg[ar_labels == 1]
    np.save('./normal_data.npy', normal_data)
    np.save('./anomalies_data.npy', anomalies_data)
    print('Data saved successfully')

idx = 0
fig, ax = plt.subplots(figsize=(10, 6))
plt.subplots_adjust(bottom=0.3)

update_plot(ppg, idx)

axtext = plt.axes([0.1, 0.1, 0.2, 0.075])
axprev = plt.axes([0.32, 0.1, 0.1, 0.075])
axnext = plt.axes([0.43, 0.1, 0.1, 0.075])
axnormal = plt.axes([0.54, 0.1, 0.1, 0.075])
axanomalies = plt.axes([0.65, 0.1, 0.1, 0.075])
axsave = plt.axes([0.76, 0.1, 0.1, 0.075])

btext = TextBox(axtext, 'Index: ', initial='0')
bprev = Button(axprev, 'Previous')
bnext = Button(axnext, 'Next')
bnormal = Button(axnormal, 'Normal')
banomalies = Button(axanomalies, 'Anomalies')
bsave = Button(axsave, 'Save')

btext.on_submit(submit)
bprev.on_clicked(prev)
bnext.on_clicked(next)
bnormal.on_clicked(normal)
banomalies.on_clicked(anomalies)
bsave.on_clicked(save)

fig.canvas.mpl_connect('key_press_event', on_key)

plt.show()
