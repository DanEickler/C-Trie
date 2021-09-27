#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>

#define MAX_FILENAME 100

struct node {
    char letter;                //the letter that the node represents
    int generation;             //which generation of nodes this node is a part of
    struct node *child[128];    //an array of pointers to 128 child nodes, to represent 0-127 ascii characters
};

struct node *nodeInit(char letter, struct node *parent) {
    struct node *thisNode = malloc(sizeof(struct node));
    thisNode->letter = letter;                                  //set the passed char to this node's letter value
    thisNode->generation = parent->generation + 1;              //each node's generation will always be one higher than its parent

    return thisNode;
}

struct node *rootInit() {
    struct node *thisNode = malloc(sizeof(struct node));        //initialize the root node
    thisNode->generation = -1;                                  //start at -1 for the generation so the first characters will be 0th
    return thisNode;
}

char *newlineScrubber(char *string) {
    for (int i = 0; i < strlen(string); i++) {
        if (string[i] == '\n' || string[i] == '\r') {
            string[i] = '\0';
        }
    }
    return string;
}

char addChild(struct node *this, const char *string) {
    //the current letter is the generation + 1 character in the string
    char letter = string[this->generation +1];

    //if the child node for the current letter doesn't exist, make it
    if (this->child[letter] == NULL) {
        this->child[letter] = nodeInit(letter, this);

        //\0 means it's the end of the string, so the recursion ends and it returns to the next function down
        if (letter == '\0') {
            return letter;
        }
    }

    //recursively call the addChild function for the next letter in the string
    addChild(this->child[letter], string);

    return letter;
}

//prompt for and return user input for the search term
char *traversePrompt() {
    char *input = malloc(MAX_FILENAME);

    printf("\n> ");
    fgets(input, MAX_FILENAME - 1, stdin);

    return newlineScrubber(input);
}


void traverse(struct node *node, char *path) {
    // Recursively searches through nodes, building a temporary string representing the path that has been traversed.
    // When \0 is encountered it means that a valid word has been found, so it is printed out and then continues to traverse.
    // Nodes are visited starting with the parent, then each child that exists from 0-127.

    if ((path[node->generation] = node->letter) == '\0') {
        printf("%s\n", path);
    }

    for (int i = 0; i < 128; i++) {
        if (node->child[i] != NULL) {
            traverse(node->child[i], path);
        }
    }
}

int matchCheck(struct node *node, const char *stub) {
    struct node *currentNode = node;
    char *path = malloc(MAX_FILENAME);

    //Look through the nodes until arriving at the node that represents the end of the input string stub
    for (int i = 0; i < strlen(stub); i++) {
        if (currentNode->child[stub[i]] != NULL) {
            path[i] = stub[i];
            currentNode = currentNode->child[stub[i]];

            // indicates that there were no matches for the search term
        } else return 1;
    }

    traverse(currentNode, path);

    return 0; //at least one match was found
}

int main() {
    DIR *dirp;
    struct dirent *entry;
    struct node *root = rootInit();
    char directory[PATH_MAX] = "";
    char input[MAX_FILENAME];

    //prompt for a directory path until a valid one is entered
    do {
        printf("\nEnter a valid directory path: ");
        fgets(directory, MAX_FILENAME, stdin);
        strncpy(newlineScrubber(directory), directory, MAX_FILENAME);

    } while (opendir(directory) == NULL);

    dirp = opendir(directory);

    //continue to loop until there are no more entries in the directory
    while ((entry = readdir(dirp)) != NULL) {
        //pass the filename of each regular file to the addChild function
        if (entry->d_type == DT_REG) {
            addChild(root, entry->d_name);
        }
    }
    closedir(dirp);

    while (strcmp(strcpy(input, traversePrompt()), "") != 0) {
        printf("\n\nFiles starting with \"%s\" in \"%s\":\n", input, directory);
        if (matchCheck(root, input)) {
            printf("No matches for \"%s\" in directory \"%s\"\n", input, directory);
        }
    }

    printf("Goodbye!");
    return 0;
}
