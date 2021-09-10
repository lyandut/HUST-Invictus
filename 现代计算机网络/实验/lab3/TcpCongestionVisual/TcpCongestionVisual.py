# coding=utf-8
import matplotlib.pyplot as plt
import csv
import os

fig_dir = 'fig/'


def run(data_dir):
    plt.figure(figsize=(20, 20))
    for each_file in os.listdir(data_dir):
        file_name, file_suffix = os.path.splitext(each_file)
        if file_suffix == '.csv':
            timestamp = []
            snd_cwnd = []
            ssthresh = []
            with open(data_dir + '/' + each_file) as csv_file:
                csv_reader = csv.reader(csv_file)
                for row in csv_reader:
                    timestamp.append(float(row[0]))  # 时间戳
                    snd_cwnd.append(int(row[6]))  # 拥塞窗口大小
                    ssthresh.append(int(row[7]))  # 慢启动阈值

            if 'default' in file_name:
                plt.subplot(411)
            elif 'loss_2.5' in file_name:
                plt.subplot(423)
            elif 'loss_5.0' in file_name:
                plt.subplot(425)
            elif 'loss_10.0' in file_name:
                plt.subplot(427)
            elif 'delay_15' in file_name:
                plt.subplot(424)
            elif 'delay_30' in file_name:
                plt.subplot(426)
            elif 'delay_60' in file_name:
                plt.subplot(428)
            else:
                print('Error file_name: ', file_name)

            plt.plot(timestamp, snd_cwnd, label='snd_cwnd')
            plt.plot(timestamp, ssthresh, label='ssthresh')
            plt.legend()
            plt.title(file_name)

    plt.savefig(fig_dir + data_dir + '.png')
    plt.show()


def run2(data_dir):
    for each_file in os.listdir(data_dir):
        try:
            file_name, file_suffix = os.path.splitext(each_file)
            if file_suffix == '.csv':
                timestamp = []
                snd_cwnd = []
                ssthresh = []
                with open(data_dir + '/' + each_file) as csv_file:
                    csv_reader = csv.reader(csv_file)
                    for row in csv_reader:
                        timestamp.append(float(row[0]))  # 时间戳
                        snd_cwnd.append(int(row[6]))  # 拥塞窗口大小
                        ssthresh.append(int(row[7]))  # 慢启动阈值

                plt.plot(timestamp, snd_cwnd, label='snd_cwnd')
                plt.plot(timestamp, ssthresh, label='ssthresh')
                plt.legend()
                plt.title(file_name)
                plt.savefig(fig_dir + file_name + '.png')
                plt.show()
        except Exception as e:
                print(file_name)


if __name__ == '__main__':
    # run('cubic')
    # dir_list = ['cubic', 'reno', 'vegas']
    # for each_dir in dir_list:
    #     run(each_dir)

    run2('data')
