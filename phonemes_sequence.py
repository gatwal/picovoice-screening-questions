# Filename: phonemes_sequence.c
# Author: Gary Atwal
# Project: Picovoice Screening Questions
#
# Description: 
# A phoneme is a sound unit. We have an extensive pronounciation dictionary.
# Below is a snippet:
# 
# ABACUS: AE B AH K AH S
# BOOK: B UH K
# THEIR: DH EH R
# THERE: DH EH R
# TOMATO: T AH M AA T OW
# TOMATO: T AH M EY T OW
# 
# Given a sequence of phonemes as input, find all the combinations of the words
# that can produce this sequence. You can preprocess the dictionary into a
# different data structure if needed.
#
# Example:
#   input: ["DH", "EH", "R", "DH", "EH", "R"]
#   output: [["THEIR", "THEIR"], 
#            ["THEIR", "THERE"],
#            ["THERE", "THEIR"],
#            ["THERE", "THERE"]]
#
# Assumptions:
#   1) dictionary that is provided is a list of tuples 
#      [(str,[str,...])] = [(word, phonemes)]
#
#   2) any unmatched phonemes will be ignored, solution requires exact matches
#      between phoneme sequences in the input and those in the dictionary
# 
# Solution:
# 1) Preprocess dictionary into a dictionary data stucture where the 
#    key = phoneme represented as a tuple of strings
#    value = list of words that sound like that phoneme
#
# 2) Iterate through the input phonemes one sound at a time and check what words
#    can be used to match the phonemes so far
#   i) For each position in sequence, look back to see if any previous segment of
#      the sequence matches a word in the dictionary
#  ii) if it matches, combine this word with any valid combinations found in earlier
#      part of the sequence before this segment of this word started
# iii) Store all valid word combinations thus found in the sequence in a data array
#      such that index = # of phonemes, value = all valid word combinations upto that
#      point so that results can be reused in the iterative process
#  iv) By end of iteration, all possible combinations words that match the entire
#      phoneme sequence will be stored at the last index = len(phonemes)
#     

from typing import List, Sequence, Tuple, Dict

# Define the pronunciation dictionary as a global variable
PRONUNCIATION_DICT = [
    ("ABACUS", ["AE", "B", "AH", "K", "AH", "S"]),
    ("BOOK", ["B", "UH", "K"]),
    ("THEIR", ["DH", "EH", "R"]),
    ("THERE", ["DH", "EH", "R"]),
    ("TOMATO", ["T", "AH", "M", "AA", "T", "OW"]),
    ("TOMATO", ["T", "AH", "M", "EY", "T", "OW"])
]

def preprocess_dictionary_phoneme_as_key() -> Dict[Tuple[str,...], List[str]]:
    phoneme_to_words = {} 
    # key = tuple of phonemes
    # value = list of words

    for word, phonemes in PRONUNCIATION_DICT:
        phoneme_tuple = tuple(phonemes)
        if phoneme_tuple not in phoneme_to_words:
            phoneme_to_words[phoneme_tuple] = []
        phoneme_to_words[phoneme_tuple].append(word)
    return phoneme_to_words

def find_word_combos_with_pronunciation(phonemes: Sequence[str]) -> Sequence[Sequence[str]]:
    phoneme_to_words = preprocess_dictionary_phoneme_as_key();
    # key = phoneme sequence as a tuple
    # value = list of words equivalent to the phoneme sequence key

    n = len(phonemes)
    results = [[] for _ in range(n+1)]
    results[0] = [[]] # base case: 0 phonemes gives empty word combinations

    for i in range(1,n+1): # iterate through phoneme sequence one sound at a time
        for j in range (i): # look back to see if any previous segment of sequence matches
            phoneme_segment = tuple(phonemes[j:i])
            if phoneme_segment in phoneme_to_words:
                words = phoneme_to_words[phoneme_segment]
                for word in words:
                    for prev_word_combo in results[j]:
                        # concatenate prev word combos with new next word
                        results[i].append(prev_word_combo + [word])

        # if no results found for up to ith phoneme, set current result = to prev result
        if results[i] == []:
            results[i] = results[i-1]
            # this is to ensure unmatched phonemes are ignored

    
    # result is stored at last non empty index in results array
    return results[n]

##################################################################################
# MAIN FUNCTION
##################################################################################
def main():
    # Test code with example phoneme sequence provided
    phonemes = ["DH", "EH", "R", "DH", "EH", "R"]
    result = find_word_combos_with_pronunciation(phonemes)
    print("input: ", phonemes)
    print("output: ", result)

if __name__ == "__main__":
    main()