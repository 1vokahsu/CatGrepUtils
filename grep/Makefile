all: clean s21_grep

s21_grep:
	gcc -Wall -Werror -Wextra -std=c11 s21_grep.c -o s21_grep
# -Wno-obsolete -Wno-legacy -g
clean:
	rm -rf ./*.o s21_grep ./*.out

clean_caches:
	rm -rf ~/Library/Caches/*
	rm -rf ~/Library/42_cache
	rm -rf ~/Library/Application\ Support/Slack/Service\ Worker/CacheStorage/
	rm -rf ~/Library/Application\ Support/Slack/Cache/
	rm -rf ~/Library/Application\ Support/Slack/Code\ Cache/