import matplotlib.pyplot as plt
import matplotlib
import math
import sys
import os


def read_data(folder_path: str) -> dict:
    
    if folder_path[-1] != "/":
        folder_path += "/"

    result = dict()
    file_data_paths = os.listdir(folder_path)

    for file_data_path in file_data_paths:
        
        data = []
        with open(folder_path + file_data_path, "r") as f:
            for line in f.readlines():
                data.append(float(line.strip()))
        result[file_data_path[:-4]] = data

    return result


def main(args):

    data = read_data("../../Data")
    data_len = len(data.keys())
    font = {'family' : 'normal',
        'weight' : 'bold',
        'size'   : 18}

    matplotlib.rc('font', **font)

    fig, axs = plt.subplots(2, 3)
    
    for i, key in enumerate(sorted(data.keys())):
        axs[i // 3, i % 3].set_title(key)
        axs[i // 3, i % 3].plot([1] * len(data[key]), color='blue', linestyle='dashed')
        axs[i // 3, i % 3].plot(data[key], color='red' if i < 3 else 'green')

    fig.text(0.5, 0.02, "1 unit = average of 100 drawn frames", ha="center", va="center")
    fig.text(0.04, 0.5, "Multiple of the ideal speed", ha="center", va="center", rotation="vertical")

    plt.tight_layout()
    plt.show()


if __name__ == "__main__":
    main(sys.argv)
