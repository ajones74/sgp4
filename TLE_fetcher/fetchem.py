import requests
import time
from pathlib import Path

norad_ids = [39476, 40882, 42698, 50319]

with open("global_xpress.tle", "w") as outfile:
    for catnr in norad_ids:
        tle = requests.get(f"https://celestrak.org/NORAD/elements/gp.php?CATNR={catnr}&FORMAT=TLE").text.strip()
        if tle and "No TLE found" not in tle:
            outfile.write(tle + "\n\n")
        time.sleep(0.5)

# Strip the DOS-style line-endings -- use this, or comment-out, as you need...
Path("global_xpress.tle").write_text(Path("global_xpress.tle").read_text().replace("\r\n", "\n"))
