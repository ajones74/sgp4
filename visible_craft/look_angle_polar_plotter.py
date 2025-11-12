import csv
import matplotlib.pyplot as plt

# Function to read CSV and plot azimuth/elevation in polar plot
def plot_polar_from_csv(filename):
    azimuth = []
    elevation = []

    with open(filename, mode='r') as file:
        reader = csv.reader(file)
        # Skip header if present (assume first row is data; uncomment and adjust if there's a header)
        # next(reader)  # Skip header row

        for row in reader:
            if len(row) >= 3:  # Ensure at least three fields
                azimuth.append(float(row[1]))  # Second field: azimuth (degrees)
                elevation.append(float(row[2]))  # Third field: elevation (degrees)

    if not azimuth:
        print("No data found in CSV.")
        return

    # Convert to polar: theta = azimuth (radians), r = 90 - elevation (zenith at center)
    theta = [az * (2 * 3.141592653589793 / 360) for az in azimuth]  # Degrees to radians
    r = [90 - el for el in elevation]  # Invert elevation for sky plot

    # Plot the data
    fig, ax = plt.subplots(subplot_kw={'projection': 'polar'})
    ax.plot(theta, r, marker='o', linestyle='-', color='b')
    ax.set_title('Azimuth/Elevation Polar Plot (Zenith at Center)')
    ax.set_theta_zero_location('N')  # 0° at North
    ax.set_theta_direction(-1)  # Clockwise (standard for az)
    ax.set_rlabel_position(90)  # Labels on left
    ax.set_ylim(0, 90)  # Horizon at edge, zenith at 0
    plt.show()

plot_polar_from_csv('O3B_MPOWER_F1')
