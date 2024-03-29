CC = clang
flags = -std=c17 -Wall -Wextra -Wconversion -Wunreachable-code -Wnewline-eof -Wno-error=unused-variable -Wshadow -Wfloat-equal -Wcovered-switch-default -Wunreachable-code-break

object_files = main.o dstring.o
allocation_metrics_dir = ../C_Allocation_Metrics/
prompt_dir = ../C_Prompt/
name_of_executable = program

allocation_metrics_lib = -Wl,-rpath,$(allocation_metrics_dir) -L$(allocation_metrics_dir) -lallocation_metrics
prompt_lib = -Wl,-rpath,$(prompt_dir) -L$(prompt_dir) -lprompt

$(name_of_executable): $(object_files)
	$(CC) $^ $(allocation_metrics_lib) $(prompt_lib) -o $@

main.o: main.c
	$(CC) $(flags) -c $^ -o $@

dstring.o: dstring.c dstring.h
	$(CC) $(flags) -c dstring.c -o $@

clean:
	rm *.o $(name_of_executable)