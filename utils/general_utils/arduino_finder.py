"""
A utility script to search for use of the Arduino Library so we can seek and destroy it.
"""


import os
import re

print(f"Current directory:{os.getcwd()}")

wordlist = [
    #"arduino",
    "Serial",
    "Analog",
    "Analogue",
    "Digital",
    #"Pin",
    "PWM",
    "I2C",
    "SPI",
    "Wire",
    "Servo",
    "Stepper",
    "UART",
    "DigitalWrite",
    "DigitalRead",
    "AnalogRead",
    "AnalogWrite",
    "pinMode",
    #"millis",
    "delay",
    #"setup"
]

excluded_dirs = ["LCDWIKI_GUI", "LCDWIKI_SPI", "LCDWIKI_TOUCH", ".pio", ".venv", ".git", "old"]

def grep_for_word(word: str, case_sensitive: bool = False, excluded_directories: list[str]|tuple[str] = ()):
    """
    Searches for all instances of a word in the files of the current directory and subdirectories. and prints the file paths and line numbers where the word is found.
    Excludes specified directories from the search.
    """
    flags = 0 if case_sensitive else re.IGNORECASE
    pattern = re.compile(r'\b' + re.escape(word) + r'\b',
                            flags=flags)
    for root, dirs, files in os.walk('.'):
        # Remove excluded directories from dirs list to prevent os.walk from descending into them
        dirs[:] = [d for d in dirs if d not in excluded_directories]
        
        for file in files:
            if file.endswith(('.cpp', '.h', '.ino', '.c')):
                file_path = os.path.join(root, file)
                try:
                    with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                        for line_number, line in enumerate(f, start=1):
                            if pattern.search(line):
                                print(f"Found '{word}' in {file_path} on line {line_number}")
                except Exception as e:
                    print(f"Could not read file {file_path}: {e}")

def grep_for_list(keywords: list[str]|tuple[str], case_sensitive: bool = False, excluded_directories: list[str]|tuple[str] = ()):
    "Searches for all instances of a list of words in the files of the current directory and subdirectories."
    for word in keywords:
        grep_for_word(word, case_sensitive=case_sensitive, excluded_directories=excluded_directories)

if __name__ == "__main__":
    print("Searching for Arduino library usage...\n")
    grep_for_list(wordlist, case_sensitive=False, excluded_directories=excluded_dirs)