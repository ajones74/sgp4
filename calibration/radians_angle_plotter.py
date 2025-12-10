import pandas as pd
import matplotlib.pyplot as plt

# Replace 'data.csv' with your actual CSV file name
file_name = 'cochlear.csv'

# Read the CSV file without headers, using only the first two columns
df = pd.read_csv(file_name, header=None, usecols=[0, 1], names=['radius', 'angle'])

# Create a polar plot
plt.figure(figsize=(8, 8))
plt.polar(df['angle'], df['radius'], marker='o', linestyle='-', color='b')

# Add title and labels (optional)
plt.title('Polar Plot of Radius vs Angle')
plt.xlabel('Angle (radians)')
plt.ylabel('Radius')

# Show the plot
plt.show()
