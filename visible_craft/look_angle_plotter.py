import csv
import matplotlib.pyplot as plt

# Function to read CSV and plot time vs azimuth
def plot_azimuth_from_csv(filename):
    time = []
    azimuth = []

    with open(filename, mode='r') as file:
        reader = csv.reader(file)
        # Skip header if present (assume first row is data; uncomment and adjust if there's a header)
        # next(reader)  # Skip header row

        for row in reader:
            if len(row) >= 2:  # Ensure at least two fields
                time.append((float(row[0]) / 1_000_000) - 6213559680 )  # First field: time (convert to float)
                azimuth.append(float(row[2]))  # Second field: elevation (convert to float)

    if not time:
        print("No data found in CSV.")
        return

    # Plot the data
    plt.figure(figsize=(10, 6))
    plt.plot(time, azimuth, marker='o', linestyle='-', color='b')
    plt.title('Azimuth vs Time')
    plt.xlabel('Time (seconds)')  # Adjust label if time unit differs
    plt.ylabel('Azimuth (degrees)')  # Adjust if units differ
    plt.grid(True)
    plt.show()

# Example usage: replace 'data.csv' with your file path
plot_azimuth_from_csv('O3B_MPOWER_F6')
