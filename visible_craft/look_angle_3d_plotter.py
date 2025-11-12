import csv
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import numpy as np

# Function to read CSV and plot azimuth/elevation in 3D sky plot
def plot_3d_sky_from_csv(filename):
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

    # Convert to spherical coordinates for 3D: theta = azimuth (rad), phi = 90 - elevation (rad), r = 1 (unit sphere)
    theta = np.deg2rad(azimuth)  # Azimuth to radians
    phi = np.deg2rad(90 - np.array(elevation))  # Elevation to polar angle (0 at zenith, pi/2 at horizon)
    r = np.ones_like(theta)  # Unit radius for sky dome

    # Convert to Cartesian for 3D plotting
    x = r * np.sin(phi) * np.cos(theta)
    y = r * np.sin(phi) * np.sin(theta)
    z = r * np.cos(phi)

    # Plot the data
    fig = plt.figure(figsize=(10, 8))
    ax = fig.add_subplot(111, projection='3d')
    ax.plot(x, y, z, marker='o', linestyle='-', color='b', label='Satellite Track')

    # Set limits for unit sphere (zoom out slightly if needed)
    ax.set_xlim(-1.2, 1.2)
    ax.set_ylim(-1.2, 1.2)
    ax.set_zlim(0, 1.2)  # Zenith up, horizon at z=0

    ax.set_xlabel('X (East)')
    ax.set_ylabel('Y (North)')
    ax.set_zlabel('Z (Up/Zenith)')
    ax.set_title('3D Sky Plot of Azimuth/Elevation Track (Zenith at Top)')
    ax.legend()
    plt.show()

plot_3d_sky_from_csv('O3B_MPOWER_F1')
