// #include <stdio.h>
// #include <stdlib.h>
// #include <dirent.h>

// void list_files(const char *dir_path)
// {
//     DIR *dir = opendir(dir_path);  // Open the directory

//     if (dir == NULL) {
//         perror("opendir failed");  // If opendir fails, print an error message
//         return;
//     }

//     struct dirent *entry;
//     // Read each directory entry and print the name
//     while ((entry = readdir(dir)) != NULL) {
//         printf("%s\n", entry->d_name);  // Print the name of the entry
//     }

//     closedir(dir);  // Close the directory when done
// }

// int main()
// {
//     const char *dir_path = ".";  // Current directory
//     list_files(dir_path);        // List files in the current directory
//     return 0;
// }
		
#include <unistd.h>
#include <stdio.h>

int main(int argc, char **argv) {

	char cwd[1024];

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <directory>\n", argv[0]);
		return 1;
	}

	if (chdir(argv[1]) == 0) {
		printf("Changed directory to %s successfully.\n", argv[1]);
	} else {
		perror("chdir failed");
	}
	printf("pwd: [%s]\n", getcwd(cwd, sizeof(cwd)));
	return 0;
}