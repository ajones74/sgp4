import csv
import matplotlib.pyplot as plt

# Function to read CSV and plot time vs azimuth
def plot_azimuth_from_csv(filename):
    time = []
    azimuth = []
    elevation = []

    with open(filename, mode='r') as file:
        reader = csv.reader(file)

        for row in reader:
            if len(row) >= 2:  # Ensure at least two fields
                time.append(float(row[0]))  # First field: time (convert to float)
                azimuth.append(float(row[1]))  # second field: azimuth (convert to float)
                elevation.append(float(row[2]))  # third field: elevation (convert to float)

    if not time:
        print("No data found in CSV.")
        return

    # Plot the data
    #plt.figure(figsize=(20, 10))
    fig = plt.figure( )
    #plt.title('Azimuth, Elevation, vs Time')
    ax1 = fig.add_subplot()
    ax1.plot(time, azimuth, marker='o', linestyle='-', color='b')
    ax1.set_ylabel('Azimuth (degrees)', color='b')
    ax1.set_xlabel('Time (seconds)')
 
    ax2 = ax1.twinx()
    ax2.plot(time, elevation, marker='o', linestyle='-', color='r')
    #fig.set_xlabel('Time (seconds)')  # Adjust label if time unit differs
    ax2.set_ylabel('Elevation (degrees)', color='r')  # Adjust if units differ
    plt.grid(True)
    plt.show()

plot_azimuth_from_csv('O3B_MPOWER_F1')
