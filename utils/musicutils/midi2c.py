import mido
import sys

NOTE_NAMES = ['C', 'CS', 'D', 'DS', 'E', 'F', 'FS', 'G', 'GS', 'A', 'AS', 'B']

def midi_to_c_by_track(midi_file, track_index=0):
    try:
        mid = mido.MidiFile(midi_file)
    except FileNotFoundError:
        print(f"Error: File '{midi_file}' not found")
        return
    except:
        print("Error: Invalid MIDI file")
        return

    if track_index >= len(mid.tracks):
        print(f"Error: Track index {track_index} out of range")
        return

    ticks_per_beat = mid.ticks_per_beat
    tempo = 500000 #Default (you can scale in the arduino file)
    for track in mid.tracks:
        for msg in track:
            if msg.type == 'set_tempo':
                tempo = msg.tempo
                break

    ticks_to_ms = (tempo / 1000) / ticks_per_beat
    track = mid.tracks[track_index]

    channels = {i: [] for i in range(16)}#cus 16 channels
    active_notes_per_channel = {i: {} for i in range(16)}
    abs_time = 0

    for msg in track:
        abs_time += msg.time

        if msg.type == 'note_on' and msg.velocity > 0:
            ch = msg.channel
            if active_notes_per_channel[ch]:
                print(f"Error: Overlapping notes detected on channel {ch} - invalid for buzzer")
                return
            active_notes_per_channel[ch][msg.note] = abs_time

        elif (msg.type == 'note_off' or (msg.type == 'note_on' and msg.velocity == 0)):
            ch = msg.channel
            if msg.note in active_notes_per_channel[ch]:
                start = active_notes_per_channel[ch][msg.note]
                duration = abs_time - start
                channels[ch].append((msg.note, start, duration))
                del active_notes_per_channel[ch][msg.note]

    output = []

    for ch, note_events in channels.items():
        if not note_events:
            continue  #skip empty ones

        #Sort by start
        note_events.sort(key=lambda x: x[1])
        melody = []
        durations = []
        current_time = 0

        for note, start, duration in note_events:
            gap = start - current_time
            if gap > 0:
                melody.append("REST")
                durations.append(int(round(gap * ticks_to_ms)))

            octave = (note // 12) - 1
            note_name = NOTE_NAMES[note % 12]
            melody.append(f"NOTE_{note_name}{octave}")
            durations.append(int(round(duration * ticks_to_ms)))
            current_time = start + duration

        melody_str = ",\n  ".join(melody)
        durations_str = ",\n  ".join(str(d) for d in durations)

##########################################plz ignore cursed format string
        output.append(f"""
// Track {track_index}, Channel {ch}
float melody_ch{ch}[] = {{
  {melody_str}
}};

int noteDurations_ch{ch}[] = {{
  {durations_str}
}};
""")
##########################################
    return "\n".join(output)


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python3 midi2c.py <midi_file> <track_index>")
        sys.exit(1)

    midi_file = sys.argv[1]
    track_index = int(sys.argv[2]) if len(sys.argv) > 2 else 0

    output = midi_to_c_by_track(midi_file, track_index)
    if output:
        print(output)
        with open(f"track{track_index}.txt", "w") as f:
            f.write(output)
