import sys
import configparser
from pathlib import Path
import hashlib
import itertools

def main():
    if len(sys.argv) < 3:
        print("Usage: python songlist_gen.py <output> <songs_directory_1> [songs_directory_2] ...")
        return

    out_path = Path(sys.argv[1])
    song_dirs = sys.argv[2:]
    song_list = []
    
    print(song_dirs)

    for song_dir in song_dirs:
        song_dir = Path(song_dir)
        for notes in itertools.chain(song_dir.glob("**/notes.mid"), song_dir.glob("**/notes.chart")):
            song_ini = notes.parent / "song.ini"
            if song_ini.is_file():
                config = configparser.ConfigParser(strict=False)
                try:
                    config.read(song_ini)
                except Exception:
                    print(f"Invalid song.ini: {song_ini}")
                    continue
                if not config.has_section("song"):
                    if not config.has_section("Song"):
                        print(f"Invalid song.ini: {song_ini}")
                        continue
                    song_name = config["Song"]["name"]
                    artist_name = config["Song"]["artist"]
                else:
                    song_name = config["song"]["name"]
                    artist_name = config["song"]["artist"]
                md5 = hashlib.md5(notes.open('rb').read()).hexdigest().upper()
                song_list.append((song_name, artist_name, md5))
    
    with out_path.open('w') as f:
        f.write(f"{len(song_list)}\n")
        for song_name, artist_name, md5 in song_list:
            f.write(f"{song_name}\n{artist_name}\n{md5}\n")

    

if __name__ == '__main__':
    main()