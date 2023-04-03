Design and manufacturing files for Talos ES™'s control card, the heart of the machine. The folder 'DMs' contains the programming files for the four instruction Decoding Memories (SST39SF010A, SST39SF020A, or SST39SF040; the PCB is compatible with any of them) on this card. The folder 'PLA' contains the programming files for the ATF16V8C Programmable Logic Array on this card. The folder 'Photos' contains detailed photos of a correctly assembled card that you can use as reference when building your own card. This is a four-layer board.<br>
<p>
<b>UPDATE, 2 April 2023:</b> U9 should have been a 74HC08N chip, not a 74HC32N chip as originally specified. This has now been corrected in the schematics and PCB silkscreen, and all files are now updated. The PCB's traces are unchanged, so if you had the previous version manufactured, it is still perfectly usable without bodges (just the silkscreen markings of U9 will be wrong). The ROM files (.bin files in directory '/DMs') for the Decoding Memories have also been updated, as per the latest iteration of the instruction set.<br>
<p>
<b>LAYER ORDER, from top to bottom, or first to last</b>:<br>
  <p>
<i>
TopLayer<br>
Inner1<br>
Inner2<br>
BottomLayer
</i>
<p><br>
<b>DISCLAIMER:</b> As per the license of this repository, these files are provided as-is. To the best of my knowledge, the board works. However, the version I actually had made and tested was slightly different from the updated version in this repository now. The changes were minimal and I have no reason to believe that this updated version doesn't work; much to the contrary. Having said that, I have not had this updated version manufactured and tested, so I cannot offer any guarantees. Just as everything in all of my Github repositories, you use these files at your own risk.
