import shutil
import sys

SAVE_A = 0x000000 
SAVE_B = 0x00E000
SEED_O = 0x0001E004

SECTION_SIZE = 4096

BlockA = { SAVE_A: 57344}
BlockB = { SAVE_B: 57344}

Section = {
    0x0:    3968,   # Max data size
    0x0FF4: 2,      # Section ID
    0x0FF6: 2,      # Checksum  
    0x0FF8: 4,      # Signature
    0x0FFC: 4,      # Save index   
}

Trainer = {
    0x00: 7, # Player name
    0x08: 1, # Player gender
    0x09: 1, # Unused
    0x0A: 4, # Trainer ID
    0x0E: 5, # Time Played
    0x13: 3  # Options
}

Audio = { 
    0: "MONO", 
    1: "STEREO" 
}

Button = {
    0: "HELP",
    1: "LR",
    2: "LA",
}

def get_sav_buffer(path: str) -> bytes:
    with open(path, "rb") as f:
        return f.read()
            
def get_byte_slice(data: bytes, offset: bytes, size: int) -> bytes:
    return data[offset:offset + size]

def get_active_block(data: bytes) -> bytes:
    save_index = Section[0x0FFC]

    # Get to end of blocks
    off_a = BlockA[SAVE_A] + SAVE_A - save_index
    off_b = BlockB[SAVE_B] + SAVE_B - save_index

    buff_a = get_byte_slice(data, off_a, save_index)
    buff_b = get_byte_slice(data, off_b, save_index)

    if (int.from_bytes(buff_a, byteorder="little") > 
        int.from_bytes(buff_b, byteorder="little")):
        
        return get_byte_slice(data, SAVE_A, BlockA[SAVE_A])
    
    return get_byte_slice(data, SAVE_B, BlockB[SAVE_B])

def get_trainer_section(data: bytes) -> bytes:
    sec = int.from_bytes(get_byte_slice(data, 0x0FF4, Section[0x0FF4]), byteorder="little")
    return get_byte_slice(data, ((14 - sec) * SECTION_SIZE), SECTION_SIZE)

def get_game_options(data: bytes) -> tuple[str, str]:
    b = data[0x13]
    s = data[0x15] & 1
    return (Button[b], Audio[s])

def calc_stored_seeds(data: bytes) -> int:
    return int.from_bytes(data[SEED_O: SEED_O + 2], byteorder="little")
        

def main(args: list[str]) -> None:
    data = get_sav_buffer(args[1])
    count = calc_stored_seeds(data)

    btn, snd = get_game_options(get_trainer_section(get_active_block(data)))

    shutil.copyfile(
        args[1], 
        f"{args[2]}/{args[3]}_{count}_FR_{snd}_{btn}_{args[4]}.sav",
    )
    
if __name__ == "__main__":
    # Input PATH, Output PATH, Start Value, Button Val
    main(sys.argv)

            