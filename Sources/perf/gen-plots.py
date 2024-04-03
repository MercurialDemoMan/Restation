import matplotlib.pyplot as plt
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
    ax_width = int(math.ceil(math.sqrt(data_len)))

    fig, axs = plt.subplots(2, 3)
    
    for i, key in enumerate(sorted(data.keys())):
        axs[i // 3, i % 3].set_title(key)
        axs[i // 3, i % 3].plot([1] * len(data[key]), color='blue', linestyle='dashed')
        axs[i // 3, i % 3].plot(data[key], color='red' if i < 3 else 'green')

    plt.show()


if __name__ == "__main__":
    main(sys.argv)