# CloneHero Setlist Tool

This tool can edit CloneHero setlist files, allowing you to have large setlists while still being able to easily edit them.

CloneHero's built-in editing of setlists is limited to appending a song to the end of the list and removing the last song of the list, which is not very useful for large setlists. When you have 1000+ songs in CloneHero it is nice to have a setlist with your favourite songs.

Supports Linux and Windows. The Windows version has not been tested thoroughly but should work.

## Functions

This tool provides the following functions:

* Song removal: Remove a specific song in the setlist
* Song swap: Swap the positions of two songs
* Shuffle: Shuffle all the songs in the setlist
* Duplicate removal: Removes all duplicates in the setlist

## Usage

```bash
sltool -s <path> [-o output_path] [-S songlist_path]
```

If no output path is provided, the setlist file will be overwritten. I do not recommend overwriting files. An error in the code can theoretically cause your setlist to become unreadable, although I have not encountered such bugs myself.

Providing a songlist file is not required, but will allow you to see the song and artist names when using the tool.

**Example of a setlist being shuffled and saved to a new file**

https://github.com/larsjuhw/clonehero-setlist-tool/assets/39745476/f84aec72-986c-4fd9-b535-a506387ec82d

## Songlist File

The setlist files only contain MD5 hashes of the notes.chart/notes.mid files. The other information is stored by CloneHero in `%userprofile%\appdata\locallow\srylain Inc_\Clone Hero\songcache.bin`, but I was not able to uncover entirely how this file is formatted.

To see the song and artist names when deleting and swapping songs, or printing the setlist, you will need to generate a songlist with [songlist_gen.py].

```
python songlist_gen.py <output> <songs_directory_1> [songs_directory_2] ...
```

## Compiling

The code can be compiled by running `make`. Use MinGW to compile for Windows.

Requires libncurses.

## Note

If someone knows or figures out how the songcache.bin file is formatted, please let me know! I know how it is formatted up until the array with the file path structs starts.
