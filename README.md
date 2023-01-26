# CloneHero Setlist Tool

This tool can edit CloneHero setlist files, allowing you to have large setlists while still being able to easily edit them.

CloneHero's built-in editing of setlists is limited to appending a song to the end of the list and removing the last song of the list, which is not very useful for large setlists. When you have 1000+ songs in CloneHero it is nice to have a setlist with your favourite songs.

## Functions

This tool provides the following functions:

* Analyze setlist: prints out the number of songs in the setlist and the number of duplicates
* Remove song: removes the song at the provided index
* Swap songs: swaps position of two songs
* Remove duplicates: resolves all duplicates within the setlist (an MD5 hash of the chart file is compared, so if you have two versions of the same song it will keep them both)
* List all songs: prints a nice list of all songs in the setlist, with index (note: this requires a provided songlist file which can be generated by songlist_gen.py)
* Shuffle: shuffles the setlist into a random order

## Compiling

The source can be compiled by running the `make.bat` or `make.sh` scripts.

## Usage

```bash
sltool <setlist_path> [songlist_path]
```

Providing a songlist file is not required, but will allow you to see the song and artist names when using the tool.

## Songlist File

The setlist files only contain MD5 hashes of the notes.chart/notes.mid files. The other information is stored by CloneHero in `%userprofile%\appdata\locallow\srylain Inc_\Clone Hero\songcache.bin`, but I was not able to uncover entirely how this file is formatted.

To see the song and artist names when deleting and swapping songs, or printing the setlist, you will need to generate a songlist with [songlist_gen.py](TODO).

```bash
python songlist_gen.py <output> <songs_directory_1> [songs_directory_2] ...
```

## Note

If someone knows or figures out how the songcache.bin file is formatted, please let me know! I know how it is formatted up until the array with the file path structs starts.

Yes, the code is very very messy.