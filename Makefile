BULIDDIR=build

sltool: $(BULIDDIR)/sltool.o $(BULIDDIR)/setlist.o $(BULIDDIR)/sltool.o $(BULIDDIR)/songlist.o $(BULIDDIR)/utils.o $(BULIDDIR)/sc_map.o
	cc $(BULIDDIR)/setlist.o $(BULIDDIR)/sltool.o $(BULIDDIR)/songlist.o $(BULIDDIR)/utils.o $(BULIDDIR)/sc_map.o -o sltool -lncurses

$(BULIDDIR)/sltool.o : src/sltool.c | $(BULIDDIR)
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@ -I include -I lib/sc_map

$(BULIDDIR)/setlist.o: src/setlist.c | $(BULIDDIR)
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@ -I include -I lib/sc_map

$(BULIDDIR)/songlist.o: src/songlist.c | $(BULIDDIR)
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@ -I include -I lib/sc_map

$(BULIDDIR)/utils.o: src/utils.c | $(BULIDDIR)
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@ -I include -I lib/sc_map

$(BULIDDIR)/sc_map.o: lib/sc_map/sc_map.c | $(BULIDDIR)
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@ -I lib/sc_map

$(BULIDDIR):
	mkdir $(BULIDDIR)

clean:
	rm $(BULIDDIR)/* sltool