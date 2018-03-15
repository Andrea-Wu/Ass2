# Ass2
ass


step 1) build tokenizer to parse through tokens in a file
            -> this should be easy, just fscanf? Unless we're not allowed to use it
            -> nvm, professor says we have to use READ( ) function b/c we should get used to it early
            -> the tokenizer should be a function (can pass into nftw). 
            -> returns a sorted linked list of words and their frequencies (i guess we can make a bst or something if we hate ourselves)
            
            hmmm.....
            
How to traverse thru a filesystem
            ->it's literally in his notes

How to store words...
            there will be more different tokens than different filenames, so it makes sense to use the different tokens as the "keys" in the hashtable. Each "value" will be a pointer to a linked list that contains sorted filenames/word count. We can deal with collisions using "chaining". If we care about the Big O...(it might be that a hash table is stupid because there is no need for O(1) access time? debate below) 
every time we insert a word into the hash table, we have to keep it on some sort of list to make sure it's in order for when we need it, and that's o(n) . -> hash table unnecessary. Thus the other option would be to make some 2d linked list or something. However, that is less efficient than the hash table b/c worst case you would do a O(n) lookup for EVERY insertion, even after the first instance of the word. 
Thus we can use amortization. A new word will be O(n) insertion, but every subsequent instance of that word will be O(1). The final printout will be nm, n= number of words and m=number of file names. 

