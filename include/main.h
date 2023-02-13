int main(int argc, char *argv[]);
int parse_args(int argc, char *argv[], char **setlist_path, char **output_path, char **songlist_path);
void usage();
void display_menu(char *setlist_path, char *songlist_path, char *output_path);
void save(char *output_path, int n_choices);
int list_songs(char *setlist_path, int swap_index);
void remove_duplicates(int n_choices);
void shuffle(int n_choices);
void remove_song(int n_choices, char *setlist_path);
