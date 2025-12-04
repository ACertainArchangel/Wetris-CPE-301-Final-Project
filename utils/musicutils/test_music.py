import re
import numpy as np
import simpleaudio as sa

SAMPLE_RATE = 44100

NOTE_FREQS = {
    'NOTE_C0': 16, 'NOTE_CS0': 17, 'NOTE_D0': 18, 'NOTE_DS0': 19, 'NOTE_E0': 21,
    'NOTE_F0': 22, 'NOTE_FS0': 23, 'NOTE_G0': 25, 'NOTE_GS0': 26, 'NOTE_A0': 28,
    'NOTE_AS0': 29, 'NOTE_B0': 31,
    'NOTE_C1': 33, 'NOTE_CS1': 35, 'NOTE_D1': 37, 'NOTE_DS1': 39, 'NOTE_E1': 41,
    'NOTE_F1': 44, 'NOTE_FS1': 46, 'NOTE_G1': 49, 'NOTE_GS1': 52, 'NOTE_A1': 55,
    'NOTE_AS1': 58, 'NOTE_B1': 62,
    'NOTE_C2': 65, 'NOTE_CS2': 69, 'NOTE_D2': 73, 'NOTE_DS2': 78, 'NOTE_E2': 82,
    'NOTE_F2': 87, 'NOTE_FS2': 93, 'NOTE_G2': 98, 'NOTE_GS2': 104, 'NOTE_A2': 110,
    'NOTE_AS2': 117, 'NOTE_B2': 123,
    'NOTE_C3': 131, 'NOTE_CS3': 139, 'NOTE_D3': 147, 'NOTE_DS3': 156, 'NOTE_E3': 165,
    'NOTE_F3': 175, 'NOTE_FS3': 185, 'NOTE_G3': 196, 'NOTE_GS3': 208, 'NOTE_A3': 220,
    'NOTE_AS3': 233, 'NOTE_B3': 247,
    'NOTE_C4': 262, 'NOTE_CS4': 277, 'NOTE_D4': 294, 'NOTE_DS4': 311, 'NOTE_E4': 330,
    'NOTE_F4': 349, 'NOTE_FS4': 370, 'NOTE_G4': 392, 'NOTE_GS4': 415, 'NOTE_A4': 440,
    'NOTE_AS4': 466, 'NOTE_B4': 494,
    'NOTE_C5': 523, 'NOTE_CS5': 554, 'NOTE_D5': 587, 'NOTE_DS5': 622, 'NOTE_E5': 659,
    'NOTE_F5': 698, 'NOTE_FS5': 740, 'NOTE_G5': 784, 'NOTE_GS5': 831, 'NOTE_A5': 880,
    'NOTE_AS5': 932, 'NOTE_B5': 988,
    'NOTE_C6': 1047, 'NOTE_CS6': 1109, 'NOTE_D6': 1175, 'NOTE_DS6': 1245, 'NOTE_E6': 1319,
    'NOTE_F6': 1397, 'NOTE_FS6': 1480, 'NOTE_G6': 1568, 'NOTE_GS6': 1661, 'NOTE_A6': 1760,
    'NOTE_AS6': 1865, 'NOTE_B6': 1976,
    'NOTE_C7': 2093, 'NOTE_CS7': 2217, 'NOTE_D7': 2349, 'NOTE_DS7': 2489, 'NOTE_E7': 2637,
    'NOTE_F7': 2794, 'NOTE_FS7': 2960, 'NOTE_G7': 3136, 'NOTE_GS7': 3322, 'NOTE_A7': 3520,
    'NOTE_AS7': 3729, 'NOTE_B7': 3951,
    'NOTE_C8': 4186, 'NOTE_CS8': 4435, 'NOTE_D8': 4699, 'NOTE_DS8': 4978
}

def parse_track_file(filename):
    """Parse melody and noteDurations arrays from Arduino C-format file"""
    with open(filename) as f:
        text = f.read()

    array_matches = re.findall(r"\w+\s+(\w+)\s*\[\]\s*=\s*{([^}]+)};", text, re.DOTALL)

    if len(array_matches) < 2:
        raise ValueError(f"Could not parse arrays in {filename}")

    melody_str = array_matches[0][1]
    durations_str = array_matches[1][1]

    melody = [note.strip() for note in melody_str.split(',') if note.strip()]
    durations = [int(d.strip()) for d in durations_str.split(',') if d.strip()]
    return melody, durations

def generate_wave(note, duration_ms):
    """Generate a waveform for a single note with tiny fade-in/out"""
    samples = int(SAMPLE_RATE * duration_ms / 1000)
    if note == "REST":
        return np.zeros(samples, dtype=np.float32)
    freq = NOTE_FREQS.get(note, 0)
    t = np.linspace(0, duration_ms / 1000, samples, False)
    wave = np.sin(freq * t * 2 * np.pi)

    # Apply 5 ms linear fade in/out
    fade_samples = int(0.005 * SAMPLE_RATE)
    fade_samples = min(fade_samples, samples // 2)  # Don't exceed half the note
    fade_in = np.linspace(0, 1, fade_samples)
    fade_out = np.linspace(1, 0, fade_samples)
    wave[:fade_samples] *= fade_in
    wave[-fade_samples:] *= fade_out

    return wave.astype(np.float32)

def generate_wave_trap_beat(note, duration_ms):
    """Generate a waveform for a single note"""
    samples = int(SAMPLE_RATE * duration_ms / 1000)
    if note == "REST":
        return np.zeros(samples, dtype=np.float32)
    freq = NOTE_FREQS.get(note, 0)
    t = np.linspace(0, duration_ms / 1000, samples, False)
    wave = np.sin(freq * t * 2 * np.pi)
    return wave.astype(np.float32)

def build_track_waveform(melody, durations):
    """Generate full waveform for a track"""
    track_wave = np.concatenate([generate_wave(n, d) for n, d in zip(melody, durations)])
    return track_wave

def mix_tracks_like_theyre_milkshakes_skrrrtskrrtbrr(track_waves):
    """Mix multiple tracks into a single waveform"""
    max_len = max(len(w) for w in track_waves)
    # Pad tracks to equal length
    padded_tracks = [np.pad(w, (0, max_len - len(w))) for w in track_waves]
    # SIGMA tracks
    mixed = np.sum(padded_tracks, axis=0)
    # Normalize (you're not normal)
    mixed /= np.max(np.abs(mixed)) + 1e-6
    return mixed

def play_waveform(waveform):
    """Play a numpy waveform"""
    audio = (waveform * 32767).astype(np.int16)
    sa.play_buffer(audio, 1, 2, SAMPLE_RATE).wait_done()

def main():
    track_files = ["sad1.txt", "sad2.txt", "sad3.txt"]#["track1.txt", "track2.txt", "track3.txt"]
    track_waves = []

    for tf in track_files:
        try:
            melody, durations = parse_track_file(tf)
            wave = build_track_waveform(melody, durations)
            track_waves.append(wave)
        except Exception as e:
            print(f"Error parsing {tf}: {e}")

    if not track_waves:
        print("No tracks loaded")
        return

    mixed_wave = mix_tracks_like_theyre_milkshakes_skrrrtskrrtbrr(track_waves)
    print("Playing mixed track...")
    play_waveform(mixed_wave)
    print("Playback finished.")

if __name__ == "__main__":
    main()
