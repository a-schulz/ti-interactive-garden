# Interactive Garden

An interactive learning toy for first and second grade children featuring RFID technology and colorful LED feedback. This project creates a game board where children can place objects with embedded RFID tags on specific positions, with the system providing visual feedback through LED rings.

## Project Overview

The Interactive Garden uses multiple RFID readers positioned under a game board to detect objects placed on different positions. Each object contains a unique RFID tag that identifies it to the system. When a tag is detected, an LED ring lights up with a specific color corresponding to that tag.

## Plant implementation
are there objects or mappings in c as a data straucture?

Suppose i have multiple objects  
example  
tomato.tags = [\<id1>, \<id2>,...]  
tomato.likes = ["carrot"]  
tomato.hates = ["potato"]  
tomato.moisture = [DRY]  
potato.tags...  
potato.likes  
potato.hates = ["tomato"]

Maybe you even know a better war with less redundancy


## Board implementation

Create a modular and maintainable implementation for the board. There is always a combination of led + reader.
Meaning i have led_ring1 + reader1, led_ring2+reader2.
On each chain i have 4 rings.

Later i want to use the implementation of the board to detect which vegetable was placed on top of the reader and then i want to give feedback if the vegetables like or dislike the position (based on the sunlight and moisture) and based on the neighboring plants (also include diagonal neighbors).

The Reader matrix is not filled. It contains some empty spaces 



