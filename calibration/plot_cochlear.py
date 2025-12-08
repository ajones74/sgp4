import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

# Function to plot the data from CSV
def plot_spiral_from_csv(csv_file):
    # Read the CSV file
    df = pd.read_csv(csv_file, header=None, names=['theta', 'r'])
    
    # Assume columns are 'theta' and 'r'
    thetas = df['theta'].values
    rs = df['r'].values
    
    # Compute Cartesian coordinates
    x = rs * np.cos(thetas)
    y = rs * np.sin(thetas)
    
    # Create figure with two subplots
    plt.figure(figsize=(10, 5))
    
    # Cartesian plot
    plt.subplot(1, 2, 1)
    plt.scatter(x, y, s=1)
    plt.title('Cartesian Plot')
    plt.xlabel('X')
    plt.ylabel('Y')
    plt.axis('equal')
    plt.grid(True)
    
    # Polar plot
    plt.subplot(1, 2, 2, polar=True)
    plt.plot(thetas, rs, linewidth=1)
    plt.title('Polar Plot')
    
    plt.tight_layout()
    plt.show()

# Example usage (replace 'spiral_data.csv' with your actual file name)
plot_spiral_from_csv('cochlear.csv')
