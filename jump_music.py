import serial
import time
import subprocess

# ── CONFIGURATION ──
PORT = '/dev/cu.usbserial-110' # Serial port the RedBoard is connected to
BAUD = 9600

# Full path to the song file on your computer
SONG = '/Users/m/Desktop/song.mp3'

# How many seconds of no landings before the music stops
TIMEOUT = 1.5

# ── SERIAL CONNECTION ──
# Open the serial connection to the Arduino
ser = serial.Serial(PORT, BAUD, timeout=1)

# Wait for the Arduino to finish resetting after serial connection opens
time.sleep(2)

# ── STATE TRACKING ──
player = None          # holds the audio subprocess so it can be stopped later
last_landing = 0       # timestamp of the most recent landing event
playing = False        # whether music is currently playing

# ── FUNCTIONS ──────────────────────────────────────────────────
def start_music():
    # Start playing the song if it isn't already playing.
    global player, playing
    if not playing:
        # afplay is Mac's built-in audio player
        player = subprocess.Popen(['afplay', SONG])
        playing = True
        print("Music ON")

def stop_music():
    # Stop the song by terminating the afplay process.
    global player, playing
    if playing and player:
        player.terminate()
        playing = False
        print("Music OFF")

# ── MAIN LOOP ──────────────────────────────────────────────────
print("Listening... start jumping!")

while True:
    # Check if the Arduino has sent any serial data
    if ser.in_waiting:
        line = ser.readline().decode().strip()

        if line == "LANDED":
            # Foil patches made contact — reset the timer and start music
            last_landing = time.time()
            start_music()

    # If music is playing but no landing has come in within TIMEOUT seconds, stop
    if playing and (time.time() - last_landing > TIMEOUT):
        stop_music()

    # Small sleep to avoid hammering the CPU in the loop
    time.sleep(0.02)